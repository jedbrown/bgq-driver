/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2011                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#include "Job.h"

#include "Database.h"
#include "SecurityKey.h"

#include "server/BCNodeInfo.h"
#include "server/BlockControllerBase.h"
#include "server/CNBlockController.h"
#include "server/IOBlockController.h"

#include "common/Properties.h"

#include <bgq_util/include/Location.h>

#include <ramdisk/include/services/MessageUtility.h>

#include <utility/include/Log.h>
#include <utility/include/ScopeGuard.h>
#include <utility/include/UserId.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>


using namespace mmcs::server;

using mmcs::common::Properties;


LOG_DECLARE_FILE( "mmcs.lite" );


namespace mmcs {
namespace lite {


Job::Job(
        const JobInfo& info,
        const server::BlockPtr& block
        ) :
    _io_service( new boost::asio::io_service() ),
    _timer( *_io_service ),
    _strand( *_io_service ),
    _control(),
    _data(),
    _info( info ),
    _block( block ),
    _classRoute( _info, _block ),
    _size( 0 ),
    _sim_id( -1 ),
    _control_port(),
    _data_port(),
    _status(),
    _exit_status( -1 ),
    _error_text(),
    _stdout(),
    _stderr(),
    _start_time( boost::posix_time::second_clock::local_time() )

{

    BOOST_FOREACH( const BCNodeInfo* node, block->getNodes() ) {
        // only count nodes that are included in this target
        // and I/O nodes that aren't used just for link training
        if ( node->_open && !node->_iopos.trainOnly() ) {
            ++_size;
        }
    }

    LOG_TRACE_MSG( "creating job " << _info.getId() << " with " << _size << " nodes" );
    // look for jobsim key in properties file
    if ( !Properties::getProperty( "jobsim" ).empty() ) {
        // set IP to localhost
        _info.setIp( "127.0.0.1" );

        // get job sim ID
        try {
            _sim_id = boost::lexical_cast<uint32_t>( Properties::getProperty( "jobsim" ) );
            LOG_TRACE_MSG( "using job simulation ID " << _sim_id );

            // get control and data port numbers
            _control_port = this->getPort( "jobcontrol.port" );
            _data_port = this->getPort( "standardio.port" );

            // ensure both are sane
            if ( _control_port == 0 || _data_port == 0 ) {
                std::ostringstream msg;
                msg << "could not find both control and data port numbers using simulation ID " << _sim_id;
                BOOST_THROW_EXCEPTION( std::runtime_error(msg.str()) );
            }
        } catch ( const boost::bad_lexical_cast& e ) {
            std::ostringstream msg;
            msg << "bad jobsim value in properties file: " << Properties::getProperty( "jobsim" );
            BOOST_THROW_EXCEPTION( std::runtime_error(msg.str()) );
        }
    } else {
        // not simulating, set control and data ports from the properties file
        try {
            bgq::utility::Properties::ConstPtr properties( Properties::getProperties() );
            _control_port = boost::lexical_cast<uint16_t>(
                    properties->getValue( "cios.jobctld", "listenPort" )
                    );
            _data_port = boost::lexical_cast<uint16_t>(
                    properties->getValue( "cios.stdiod", "listenPort" )
                    );
        } catch ( const boost::bad_lexical_cast& e ) {
            LOG_ERROR_MSG( e.what() );
            throw;
        } catch ( const std::exception& e ) {
            LOG_ERROR_MSG( e.what() );
            throw;
        }
    }

    // open file for stdout
    if ( !_info.getInline() ) {
        std::ostringstream file;
        file << _info.getId() << ".out";
        _stdout.open( file.str().c_str(), std::ios_base::trunc | std::ios_base::out );
        if ( !_stdout) {
            std::ostringstream msg;
            msg << "could not open file for stdout: " << file.str();
            BOOST_THROW_EXCEPTION( std::runtime_error( msg.str() ) );
        }

        // open file for stderr
        file.str("");
        file << _info.getId() << ".err";
        _stderr.open( file.str().c_str(), std::ios_base::trunc | std::ios_base::out );
        if ( !_stderr) {
            std::ostringstream msg;
            msg << "could not open file for stderr: " << file.str();
            BOOST_THROW_EXCEPTION( std::runtime_error( msg.str() ) );
        }
    }
}

void
Job::setStatus(
        Status flag
        )
{
    LOG_TRACE_MSG( "setting " << this->toString(flag) << " flag" );
    _status.set( flag );
    this->startTimer( flag );
}

uint16_t
Job::getPort(
        const char* name
        )
{
    std::ostringstream path;
    path << "/tmp/cios" << _sim_id << "/" << name;
    std::ifstream file( path.str().c_str() );

    // ensure we opened the file
    if ( !file ) {
        LOG_ERROR_MSG( "could not open " << path.str() );
        return 0;
    }

    // read line
    std::string line;
    std::getline( file, line );
    LOG_TRACE_MSG( name << ": port=" << line );

    // cast to integer
    try {
        uint16_t result = boost::lexical_cast<uint16_t>( line );
        return result;
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( "bad port value in " << name );
        LOG_WARN_MSG( e.what() );
        return 0;
    }
}

void
Job::start()
{
    // create control endpoint
    boost::asio::ip::tcp::endpoint control_ep;
    control_ep.address(
            boost::asio::ip::address::from_string( _info.getIp() )
            );
    control_ep.port( _control_port );

    // create data endpoint
    boost::asio::ip::tcp::endpoint data_ep;
    data_ep.address(
            boost::asio::ip::address::from_string( _info.getIp() )
            );
    data_ep.port( _data_port );

    // update status
    this->setStatus( Connecting );

    // start connecting control
    _control.reset(
            new Connection( *_io_service )
            );
    LOG_TRACE_MSG( "connecting control connetion to " << control_ep );
    _control->start(
            control_ep,
            _strand.wrap(
                boost::bind(
                    &Job::handleConnect,
                    shared_from_this(),
                    _control,
                    control_ep,
                    boost::asio::placeholders::error
                    )
                )
            );

    // start connecting data
    _data.reset(
            new Connection( *_io_service )
            );
    LOG_TRACE_MSG( "connecting data connetion to " << data_ep );
    _data->start(
            data_ep,
            _strand.wrap(
                boost::bind(
                    &Job::handleConnect,
                    shared_from_this(),
                    _data,
                    data_ep,
                    boost::asio::placeholders::error
                    )
                )
            );

    // run I/O service
    boost::thread_group threads;
    (void)threads.create_thread(
            boost::bind(
                &boost::asio::io_service::run,
                _io_service
                )
            );
}

void
Job::kill(
        int signal
        )
{
    _strand.post(
            boost::bind(
                &Job::killImpl,
                shared_from_this(),
                signal
                )
            );
}

void
Job::killImpl(
        int signal
        )
{
    if ( _status.test(Running) ) {
        LOG_TRACE_MSG( "sending signal " << signal << " to job " << _info.getId() );

        // create message
        boost::shared_ptr<bgcios::jobctl::SignalJobMessage> msg( new bgcios::jobctl::SignalJobMessage );
        bzero( msg.get(), sizeof(bgcios::jobctl::SignalJobMessage) );
        msg->header.jobId = _info.getId();
        msg->header.service = bgcios::JobctlService;
        msg->header.version = bgcios::jobctl::ProtocolVersion;
        msg->header.type = bgcios::jobctl::SignalJob;
        msg->header.length = sizeof( bgcios::jobctl::SignalJobMessage );
        msg->signo = signal;

        // send message
        this->write( _control, msg, msg->header.length );
    } else {
        LOG_TRACE_MSG( "job " << _info.getId() << " is not running" );
    }
}

void
Job::read(
        Connection::Ptr connection
        )
{
    connection->readMessage(
            _strand.wrap(
                boost::bind(
                    &Job::readMessageHandler,
                    shared_from_this(),
                    connection,
                    connection->remote_endpoint(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                    )
                )
            );
}

void
Job::readHeader(
        Connection::Ptr connection
        )
{
    connection->readHeader(
            _strand.wrap(
                boost::bind(
                    &Job::readHeaderHandler,
                    shared_from_this(),
                    connection,
                    connection->remote_endpoint(),
                    boost::asio::placeholders::error
                    )
                )
            );
}

void
Job::write(
        Connection::Ptr connection,
        boost::shared_ptr<void> msg,
        size_t length
        )
{
    connection->write(
            msg.get(),
            length,
            _strand.wrap(
                boost::bind(
                    &Job::writeHandler,
                    shared_from_this(),
                    connection,
                    connection->remote_endpoint(),
                    msg,
                    boost::asio::placeholders::error
                    )
                )
            );
}

Job::~Job()
{
    LOG_TRACE_MSG( "cancelling I/O service" );
    _io_service->stop();

    // send this guy to stdout so the user knows the job is done
    if ( WIFEXITED(_exit_status) ) {
        std::cout << "job " << _info.getId() << " exited with status " << WEXITSTATUS(_exit_status) << std::endl;
    } else if ( WIFSIGNALED(_exit_status) ) {
        _error_text << "terminated by signal " << WTERMSIG(_exit_status);
        std::cout << "job " << _info.getId() << " " << _error_text.str() << std::endl;
    } else {
        std::cout << "job " << _info.getId() << " " << _error_text.str() << std::endl;
    }

    // prepare query
    LOG_TRACE_MSG( "updating exit status and end time in database" );
    std::ostringstream sql;
    sql << "update jobs set exitstatus=" << _exit_status << ",";
    sql << "endtime=datetime('now'),";
    sql << "errortext='" << _error_text.str() << "'";
    sql << " ";
    sql << "where jobid=" << _info.getId();
    LOG_TRACE_MSG( sql.str() );

    // execute quer
    lite::Database db;
    char* error_message = NULL;
    bgq::utility::ScopeGuard guard( boost::bind( sqlite3_free, boost::ref(error_message) ) );
    int rc = sqlite3_exec( db.getHandle(), sql.str().c_str(), NULL, 0 , &error_message);
    if ( rc != SQLITE_OK ) {
        LOG_TRACE_MSG( "could not update job " << _info.getId() << " exit status" );
        LOG_TRACE_MSG( error_message );
    }
}

void
Job::handleConnect(
        Connection::Ptr connection,
        const boost::asio::ip::tcp::endpoint& ep,
        const boost::system::error_code& error
        )
{
    if ( error ) {
        _error_text << "could not connect to " << ep << ". ";
        return;
    }
    LOG_TRACE_MSG( "connected to " << ep );

    // set control connection
    if ( ep.port() == _control->remote_endpoint().port() ) {
        LOG_TRACE_MSG( "connected to control daemon at " << _control->remote_endpoint() );
        _control->setConnected();
    } else if ( ep.port() == _data->remote_endpoint().port() ) {
        LOG_TRACE_MSG( "connected to data daemon at " << _data->remote_endpoint() );
        _data->setConnected();
    }

    // start job if we're connected to both
    if ( _control->getConnected() && _data->getConnected() ) {
        this->authenticateControl();
    } else {
        LOG_TRACE_MSG( "not connected to all nodes yet" );
    }
}

void
Job::authenticateControl()
{
    this->setStatus( AuthenticateControl );

    const BGQBlockNodeConfig* config = _block->getBlockNodeConfig();
    SecurityKey key(
            config->securityKey(),
            _block->getBlockName()
            );

    const boost::shared_ptr<bgcios::jobctl::AuthenticateMessage> msg(
            new bgcios::jobctl::AuthenticateMessage
            );
    bzero( msg.get(), sizeof(bgcios::jobctl::AuthenticateMessage) );
    msg->header.service = bgcios::JobctlService;
    msg->header.version = bgcios::jobctl::ProtocolVersion;
    msg->header.type = bgcios::jobctl::Authenticate;
    msg->header.jobId = 0; /* does not matter */
    msg->header.length = sizeof( bgcios::jobctl::AuthenticateMessage );
    memcpy(
            msg->plainData,
            key.clear().data(),
            key.clear().size()
          );
    memcpy(
            msg->encryptedData,
            key.encrypted().data(),
            key.encrypted().size()
          );

    this->write( _control, msg, msg->header.length );
    this->readHeader( _control );
}

void
Job::authenticateData()
{
    this->setStatus( AuthenticateData );

    const BGQBlockNodeConfig* config = _block->getBlockNodeConfig();
    SecurityKey key(
            config->securityKey(),
            _block->getBlockName()
            );

    const boost::shared_ptr<bgcios::stdio::AuthenticateMessage> msg(
            new bgcios::stdio::AuthenticateMessage
            );
    bzero( msg.get(), sizeof(bgcios::stdio::AuthenticateMessage) );
    msg->header.service = bgcios::StdioService;
    msg->header.version = bgcios::stdio::ProtocolVersion;
    msg->header.type = bgcios::stdio::Authenticate;
    msg->header.jobId = 0; /* does not matter */
    msg->header.length = sizeof( bgcios::stdio::AuthenticateMessage );
    memcpy(
            msg->plainData,
            key.clear().data(),
            key.clear().size()
          );
    memcpy(
            msg->encryptedData,
            key.encrypted().data(),
            key.encrypted().size()
          );

    this->write( _data, msg, msg->header.length );
    this->readHeader( _data );
}

void
Job::setup()
{
    this->setStatus( Setup );

    // create message
    const boost::shared_ptr<bgcios::jobctl::SetupJobMessage> msg( new bgcios::jobctl::SetupJobMessage );
    bzero( msg.get(), sizeof(bgcios::jobctl::SetupJobMessage) );
    msg->header.service = bgcios::JobctlService;
    msg->header.version = bgcios::jobctl::ProtocolVersion;
    msg->header.type = bgcios::jobctl::SetupJob;
    msg->header.jobId = _info.getId();
    msg->header.length = sizeof( bgcios::jobctl::SetupJobMessage );
    LOG_TRACE_MSG( "setting up job " << _info.getId() );

    // corner is always 0,0,0,0,0 so don't set it

    // default to ABCDET mapping
    snprintf( msg->mapping, sizeof(msg->mapping), "%s", "ABCDET" );

    // set shape
    msg->shape.core = bgq::util::Location::ComputeCardCoresOnBoard - 1; // compute nodes only support 16 cores
    if ( boost::dynamic_pointer_cast<CNBlockController>(_block) ) {
        msg->shape.aCoord = _block->getBlockNodeConfig()->aNodeSize();
        msg->shape.bCoord = _block->getBlockNodeConfig()->bNodeSize();
        msg->shape.cCoord = _block->getBlockNodeConfig()->cNodeSize();
        msg->shape.dCoord = _block->getBlockNodeConfig()->dNodeSize();
        msg->shape.eCoord = _block->getBlockNodeConfig()->eNodeSize();
    } else if ( boost::dynamic_pointer_cast<IOBlockController>(_block) ) {
        msg->shape.aCoord = _block->getBlockNodeConfig()->aIONodeSize();
        msg->shape.bCoord = _block->getBlockNodeConfig()->bIONodeSize();
        msg->shape.cCoord = _block->getBlockNodeConfig()->cIONodeSize();
        msg->shape.dCoord = _block->getBlockNodeConfig()->dIONodeSize();
        msg->shape.eCoord = 1;
    } else {
        BOOST_ASSERT( !"not an I/O or compute block" );
    }

    LOG_TRACE_MSG(
            "shape " <<
            static_cast<unsigned>( msg->shape.aCoord ) << "x" <<
            static_cast<unsigned>( msg->shape.bCoord ) << "x" <<
            static_cast<unsigned>( msg->shape.cCoord ) << "x" <<
            static_cast<unsigned>( msg->shape.dCoord ) << "x" <<
            static_cast<unsigned>( msg->shape.eCoord )
            );

    // if --np is not provided, it is always the entire block
    msg->numRanks = _info.getNp() ? _info.getNp() : _size * _info.getRanksPerNode();
    LOG_TRACE_MSG( "numRanks: " << static_cast<unsigned>(msg->numRanks) );

    // ranks per node
    msg->numRanksPerNode = _info.getRanksPerNode();

    // set each compute node's class route
    msg->numNodes = _classRoute.container().size();
    LOG_TRACE_MSG( "numNodes: " << static_cast<unsigned>(msg->numNodes) );
    unsigned index = 0;
    BOOST_FOREACH( const bgcios::jobctl::NodeClassRoute& i, _classRoute.container() ) {
        msg->nodesInJob[ index++ ] = i;
    }

    // set job leader
    msg->jobLeader.aCoord = _classRoute.leader().coords[0];
    msg->jobLeader.bCoord = _classRoute.leader().coords[1];
    msg->jobLeader.cCoord = _classRoute.leader().coords[2];
    msg->jobLeader.dCoord = _classRoute.leader().coords[3];
    msg->jobLeader.eCoord = _classRoute.leader().coords[4];

    this->write( _control, msg, msg->header.length );
}

void
Job::load()
{
    // set status
    this->setStatus( Loading );

    // create message
    boost::shared_ptr<bgcios::jobctl::LoadJobMessage> msg( new bgcios::jobctl::LoadJobMessage );
    bzero( msg.get(), sizeof(bgcios::jobctl::LoadJobMessage) );
    msg->header.service = bgcios::JobctlService;
    msg->header.version = bgcios::jobctl::ProtocolVersion;
    msg->header.type = bgcios::jobctl::LoadJob;
    msg->header.jobId = _info.getId();
    LOG_TRACE_MSG( "loading job " << _info.getId() );

    // add credentials
    bgq::utility::UserId uid;
    msg->userId = uid.getUid();
    LOG_TRACE_MSG( "set uid to " << msg->userId );
    BOOST_FOREACH( const bgq::utility::UserId::Group& group, uid.getGroups() ) {
        if ( msg->groupId == 0 ) {
            msg->groupId = group.first;
            LOG_TRACE_MSG( "set gid to " << msg->userId );
        } else if ( msg->numSecondaryGroups < bgcios::jobctl::MaxGroups ) {
            msg->secondaryGroups[ msg->numSecondaryGroups ] = group.first;
            msg->numSecondaryGroups++;
            LOG_TRACE_MSG( "added group " << group.first );
        } else {
            LOG_WARN_MSG( "ignoring group " << group.second );
        }
    }

    // add umask
    mode_t current_umask = umask(0);
    bgq::utility::ScopeGuard umask_guard(boost::bind( &umask, current_umask) );
    msg->umask = current_umask;
    LOG_TRACE_MSG( "umask " << std::setfill('0') << std::setw(4) << std::oct << msg->umask );

    // add executable
    int offset = snprintf( msg->arguments, sizeof(msg->arguments), "%s%c", _info.getExe().c_str(), '\0' );
    msg->numArguments = 1;

    // add arguments
    LOG_TRACE_MSG( _info.getArgs().size() << " arguments" );
    BOOST_FOREACH( const std::string& arg, _info.getArgs() ) {
        offset += snprintf( msg->arguments + offset, sizeof(msg->arguments) - offset, "%s%c", arg.c_str(), '\0' );
        LOG_TRACE_MSG( "adding arg " << arg );
        msg->numArguments++;
    }
    msg->argumentSize = offset;
    LOG_TRACE_MSG( "arg size: " << offset );

    // add environment variables
    LOG_TRACE_MSG( _info.getEnvs().size() << " environment variables" );
    offset = 0;
    BOOST_FOREACH( const std::string& env, _info.getEnvs() ) {
        offset += snprintf( msg->variables + offset, sizeof(msg->variables) - offset, "%s%c", env.c_str(), '\0' );
        LOG_TRACE_MSG( "adding env " << env );
        msg->numVariables++;
    }
    msg->variableSize = offset;
    LOG_TRACE_MSG( "env size: " << offset );

    // set cwd
    strncpy( msg->workingDirectoryPath, _info.getCwd().c_str(), sizeof(msg->workingDirectoryPath) );

    // set length
    msg->header.length = sizeof( bgcios::jobctl::LoadJobMessage );

    // set strace
    msg->rankForStrace = bgcios::jobctl::DisableStrace;

    // send
    this->write( _control, msg, msg->header.length );
}

void
Job::startTimer(
        Status status
        )
{
    // cancel old timer
    _timer.cancel();

    // start new timer
    if ( status == Running ) {
        // check running jobs after timeout value
        if ( _info.getTimeout() > 0 ) {
            LOG_TRACE_MSG( "timer expires " << _info.getTimeout() << " seconds from now" );
            _timer.expires_from_now( boost::posix_time::seconds(_info.getTimeout()) );
        } else {
            // no timer needed for infinity timeout
            return;
        }
    } else {
        LOG_TRACE_MSG( "timer expires " << _info.getTransitionTimeout() << " seconds from now" );
        _timer.expires_from_now(
                boost::posix_time::seconds(
                    _info.getTransitionTimeout()
                    )
                );
    }

    // do the wait
    _timer.async_wait(
            _strand.wrap(
                boost::bind(
                    &Job::handleTimer,
                    shared_from_this(),
                    status,
                    boost::asio::placeholders::error
                    )
                )
            );
}

void
Job::readHeaderHandler(
        Connection::Ptr connection,
        const boost::asio::ip::tcp::endpoint& ep,
        const boost::system::error_code& error
        )
{
    LOG_TRACE_MSG( "read header handler" );
    if ( error ) {
        if ( error == boost::asio::error::operation_aborted ) {
            // this will happen when stopping the io_service
            LOG_TRACE_MSG( "read canceled" );
        } else {
            _error_text << "could not read from " << ep << ": " << error.message() << ". ";
        }

        return;
    }

    // create buffer for message
    if ( connection == _control && _control->getHeader().service == bgcios::JobctlService ) {
        LOG_TRACE_MSG( "control " << bgcios::jobctl::toString(_control->getHeader().type) );
        LOG_TRACE_MSG( bgcios::printHeader(_control->getHeader()) );
        this->read( connection );
    } else if ( connection == _data && _data->getHeader().service == bgcios::StdioService ) {
        LOG_TRACE_MSG( "data " << bgcios::stdio::toString(_data->getHeader().type) );
        LOG_TRACE_MSG( bgcios::printHeader(_data->getHeader()) );
        this->read( connection );
    } else {
        LOG_FATAL_MSG( bgcios::printHeader(_data->getHeader()) );
        BOOST_ASSERT( !"unhandled service type" );
    }
}

void
Job::readMessageHandler(
        Connection::Ptr connection,
        const boost::asio::ip::tcp::endpoint& ep,
        const boost::system::error_code& error,
        size_t bytesTransferred
        )
{
    if ( error ) {
        if ( error == boost::asio::error::operation_aborted ) {
            // this will happen when stopping the io_service
            LOG_TRACE_MSG( "read canceled" );
        } else {
            _error_text << "could not read from " << ep << ": " << error.message() << ". ";
        }

        return;
    }

    // check if this was from a control or data connection
    if ( connection == _control ) {
        LOG_TRACE_MSG(
                "read " << bytesTransferred << " bytes for " <<
                bgcios::jobctl::toString( _control->getHeader().type ) << " message"
                );
        // handle message type
        if ( _control->getHeader().type == bgcios::jobctl::AuthenticateAck && _status.test( AuthenticateControl ) ) {
            this->authenticateData();
        } else if ( _control->getHeader().type == bgcios::jobctl::SetupJobAck && _status.test( Setup ) ) {
            // handle errors
            if ( _control->getHeader().returnCode != bgcios::Success ) {
                _error_text << "setup failed with rc " << _control->getHeader().returnCode << ": " <<
                    bgcios::returnCodeToString( _control->getHeader().returnCode );
                LOG_TRACE_MSG( _error_text.str() );

                // set status
                _status.set( Drained );
                this->setStatus( Ending );

                // end job
                boost::shared_ptr<bgcios::jobctl::CleanupJobMessage> msg( new bgcios::jobctl::CleanupJobMessage );
                bzero( msg.get(), sizeof(bgcios::jobctl::CleanupJobMessage) );
                msg->header.service = bgcios::JobctlService;
                msg->header.version = bgcios::jobctl::ProtocolVersion;
                msg->header.type = bgcios::jobctl::CleanupJob;
                msg->header.jobId = _info.getId();
                msg->header.length = sizeof( bgcios::jobctl::CleanupJobMessage );
                this->write( _control, msg, msg->header.length );

                // cancel data
                _data->stop();
            } else {
                this->load();
            }
        } else if ( _control->getHeader().type == bgcios::jobctl::LoadJobAck && _status.test( Loading ) ) {
            // handle errors
            if ( _control->getHeader().returnCode != bgcios::Success ) {
                _error_text << "load failed with rc " << _control->getHeader().returnCode << ": " <<
                    bgcios::returnCodeToString( _control->getHeader().returnCode );
                LOG_TRACE_MSG( _error_text.str() );

                // set status
                _status.set( Drained );
                this->setStatus( Ending );

                // end job
                boost::shared_ptr<bgcios::jobctl::CleanupJobMessage> msg( new bgcios::jobctl::CleanupJobMessage );
                bzero( msg.get(), sizeof(bgcios::jobctl::CleanupJobMessage) );
                msg->header.service = bgcios::JobctlService;
                msg->header.version = bgcios::jobctl::ProtocolVersion;
                msg->header.type = bgcios::jobctl::CleanupJob;
                msg->header.jobId = _info.getId();
                msg->header.length = sizeof( bgcios::jobctl::CleanupJobMessage );
                this->write( _control, msg, msg->header.length );

                // cancel data
                _data->stop();
            } else {
                // update status
                this->setStatus( Starting );

                // send start job message to jobctld
                boost::shared_ptr<bgcios::jobctl::StartJobMessage> control( new bgcios::jobctl::StartJobMessage );
                bzero( control.get(), sizeof(bgcios::jobctl::StartJobMessage) );
                control->header.jobId = _info.getId();
                control->header.service = bgcios::JobctlService;
                control->header.version = bgcios::jobctl::ProtocolVersion;
                control->header.type = bgcios::jobctl::StartJob;
                control->header.length = sizeof( bgcios::jobctl::StartJobMessage );
                control->numRanksForIONode = _size * _info.getRanksPerNode();
                this->write( _control, control, control->header.length );

                // send start job message to stdiod
                boost::shared_ptr<bgcios::stdio::StartJobMessage> data( new bgcios::stdio::StartJobMessage );
                bzero( data.get(), sizeof(bgcios::stdio::StartJobMessage) );
                data->header.jobId = _info.getId();
                data->header.service = bgcios::StdioService;
                data->header.version = bgcios::stdio::ProtocolVersion;
                data->header.type = bgcios::stdio::StartJob;
                data->header.length = sizeof( bgcios::stdio::StartJobMessage );
                data->numRanksForIONode = _size * _info.getRanksPerNode();
                this->write( _data, data, data->header.length );
            }
        } else if ( _control->getHeader().type == bgcios::jobctl::StartJobAck && _status.test( Starting ) ) {
            this->setStatus( Running );
            LOG_TRACE_MSG( "job " << _info.getId() << " running" );
        } else if ( _control->getHeader().type == bgcios::jobctl::ExitJob && _status.test( Running ) ) {
            // cast buffer to message type
            boost::shared_ptr<const bgcios::jobctl::ExitJobMessage> msg;
            msg = _control->getBuffer<bgcios::jobctl::ExitJobMessage>();
            _exit_status = msg->status;

            // end job if we're drained
            if ( _status.test(Drained) ) {
                this->setStatus( Ending );
                boost::shared_ptr<bgcios::jobctl::CleanupJobMessage> msg( new bgcios::jobctl::CleanupJobMessage );
                bzero( msg.get(), sizeof(bgcios::jobctl::CleanupJobMessage) );
                msg->header.service = bgcios::JobctlService;
                msg->header.version = bgcios::jobctl::ProtocolVersion;
                msg->header.type = bgcios::jobctl::CleanupJob;
                msg->header.jobId = _info.getId();
                msg->header.length = sizeof( bgcios::jobctl::CleanupJobMessage );
                this->write( _control, msg, msg->header.length );
            } else {
                // job will end when it's drained
            }
        } else if ( _control->getHeader().type == bgcios::jobctl::CleanupJobAck && _status.test( Ending ) ) {
            _status.set( Terminating );
        } else if ( _control->getHeader().type == bgcios::jobctl::SignalJobAck ) {
            // nothing to do
        } else {
            LOG_WARN_MSG(
                    "unsupported message type " << bgcios::jobctl::toString( _control->getHeader().type ) << " " <<
                    "for status " << _status
                    )
        };

        // start reading next control header if we haven't terminated
        if ( !_status.test(Terminating) ) {
            this->readHeader( _control );
        } else {
            // cancel timer
            _timer.cancel();
        }
    } else if ( connection == _data ) {
        LOG_TRACE_MSG(
                "read " << bytesTransferred << " bytes for " <<
                bgcios::stdio::toString( _data->getHeader().type ) << " message"
                );
        // handle message type
        if ( _data->getHeader().type == bgcios::stdio::AuthenticateAck ) {
            LOG_TRACE_MSG( "HELLO WORLD" );
            this->setup();
        } else if ( _data->getHeader().type == bgcios::stdio::StartJobAck ) {
            LOG_TRACE_MSG( "start job ack" );
        } else if ( _data->getHeader().type == bgcios::stdio::CloseStdioAck ) {
            LOG_TRACE_MSG( "close stdio ack" );
        } else if (
                _data->getHeader().type == bgcios::stdio::WriteStdout ||
                _data->getHeader().type == bgcios::stdio::WriteStderr
                )
        {
            // cast buffer to message type
            boost::shared_ptr<const bgcios::stdio::WriteStdioMessage> msg;
            msg = _data->getBuffer<bgcios::stdio::WriteStdioMessage>();

            // get output type
            std::ostream& out = ( _info.getInline() ? std::cout : _stdout );
            std::ostream& err = ( _info.getInline() ? std::cerr : _stderr );
            std::ostream& dest = ( _data->getHeader().type == bgcios::stdio::WriteStdout ? out : err );
            const char* label = ( _data->getHeader().type == bgcios::stdio::WriteStdout ? "stdout" : "stderr" );

            // write prefix
            if ( _info.getLabel() ) {
                std::ostringstream prefix;
                prefix << label << "[" << msg->header.rank << "] ";
                dest.write( prefix.str().c_str(), prefix.str().size() );
            }

            // write data
            dest.write( msg->data, msg->header.length - sizeof(msg->header) );
        } else if ( _data->getHeader().type == bgcios::stdio::CloseStdio ) {
            _status.set( Drained );
            // end job if we've ended
            if ( _status.test(Ending) ) {
                this->setStatus( Ending );
                boost::shared_ptr<bgcios::jobctl::CleanupJobMessage> msg( new bgcios::jobctl::CleanupJobMessage );
                bzero( msg.get(), sizeof(bgcios::jobctl::CleanupJobMessage) );
                msg->header.service = bgcios::JobctlService;
                msg->header.version = bgcios::jobctl::ProtocolVersion;
                msg->header.type = bgcios::jobctl::CleanupJob;
                msg->header.jobId = _info.getId();
                msg->header.length = sizeof( bgcios::jobctl::CleanupJobMessage );
                this->write( _control, msg, msg->header.length );
            } else {
                // job will end when it's ended
            }
        } else if ( _data->getHeader().type == bgcios::stdio::ReadStdin ) {
            boost::shared_ptr<bgcios::stdio::ReadStdinAckMessage> msg( new bgcios::stdio::ReadStdinAckMessage );
            bzero( msg.get(), sizeof(bgcios::stdio::ReadStdinAckMessage) );
            msg->header.service = bgcios::StdioService;
            msg->header.version = bgcios::stdio::ProtocolVersion;
            msg->header.type = bgcios::stdio::ReadStdinAck;
            msg->header.jobId = _info.getId();
            msg->header.returnCode = bgcios::RequestFailed; // stdin not supported in mmcs_lite environment
            msg->header.length = sizeof( bgcios::MessageHeader ); // no payload, length is just the header size
            this->write( _data, msg, msg->header.length );
        } else {
            LOG_WARN_MSG(
                "unsupported message type " << bgcios::stdio::toString( _data->getHeader().type ) << " " <<
                "for status " << _status
                );
        }

        // start reading next data header if we're not done
        if ( !_status.test(Drained) ) {
            this->readHeader( _data );
        }
    } else {
        BOOST_ASSERT( !"unhandled connection" );
    }
}

void
Job::writeHandler(
        Connection::Ptr connection,
        const boost::asio::ip::tcp::endpoint& ep,
        boost::shared_ptr<void> message,
        const boost::system::error_code& error
        )
{
    // silence unused parameter
    (void)message;

    // handle errors
    if ( error ) {
        _error_text << "could not write to " << ep << ": " << error.message() << ". ";
        return;
    }
    LOG_TRACE_MSG( "wrote " << (connection == _control ? "control" : "data") << " message successfully" );
}

void
Job::handleTimer(
        Status status,
        const boost::system::error_code& error
        )
{
    if ( error == boost::asio::error::operation_aborted ) {
        // do nothing, we were canceled
    } else if ( _status.test( Running ) ) {
        // job has timed out
        LOG_TRACE_MSG( "job " << _info.getId() << " has timed out" );
        this->kill( SIGKILL );
    } else {
        // some other state, we timed out waiting for an ack from the I/O node
        _control->stop();
        _data->stop();
        _error_text << "timed out in " << this->toString( status ) << " state. ";
        LOG_TRACE_MSG( _error_text.str() );
    }
}

} } // namespace mmcs::lite
