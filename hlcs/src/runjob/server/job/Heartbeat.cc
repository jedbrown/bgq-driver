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

#include "server/job/Heartbeat.h"

#include "server/block/Compute.h"

#include "server/cios/Message.h"

#include "server/job/IoNode.h"

#include "server/Job.h"
#include "server/Options.h"
#include "server/Server.h"

#include "common/logging.h"
#include "common/properties.h"

#include <boost/bind.hpp>

#include <limits>
#include <numeric>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

Heartbeat::Heartbeat(
        const Server::Ptr& server
        ) :
    _timer( server->getIoService() ),
    _waitTime( 60 ),
    _start(),
    _status(),
    _stopped( false ),
    _cycles( 0 )
{
    // get heartbeat timeout value from properties file
    const std::string key( "control_action_heartbeat" );
    std::string value;

    try {
        const Options& options = server->getOptions();
        const bgq::utility::Properties::ConstPtr& properties = options.getProperties();
        value = properties->getValue( PropertiesSection, key );
        const int waitTime = boost::lexical_cast<int>( value );
        if ( waitTime <= 0 ) {
            LOG_WARN_MSG( "invalid " << key << " value (" << value << ") in section [" << PropertiesSection << "]" );
            LOG_WARN_MSG( "using default value: " << _waitTime << " seconds" );
        } else {
            _waitTime = waitTime;
            LOG_TRACE_MSG( _waitTime << " seconds" );
        }
    } catch ( const std::invalid_argument& e ) {
        LOG_WARN_MSG( "could not find key " << key << " in section [" << PropertiesSection << "]" );
        LOG_WARN_MSG( "using default value: " << _waitTime << " seconds" );
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( "invalid " << key << " value (" << value << ") in section [" << PropertiesSection << "]" );
        LOG_WARN_MSG( "using default value: " << _waitTime << " seconds" );
    }
}

Heartbeat::~Heartbeat()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

void
Heartbeat::start(
        const Job::Ptr& job
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );

    // ensure heartbeat has not started
    if ( !_start.is_not_a_date_time() ) {
        const boost::posix_time::time_duration expires = _timer.expires_from_now();
        BOOST_THROW_EXCEPTION(
                std::logic_error(
                    "heartbeat timeout expires in " + boost::lexical_cast<std::string>( expires.total_seconds() ) + " seconds" )
                );
    }
    _start =  boost::posix_time::microsec_clock::local_time();

    BOOST_FOREACH( IoNode::Map::value_type& i, job->io() ) {
        _status.insert(
                std::make_pair( i.first, std::numeric_limits<unsigned>::max() )
                );
    }

    this->wait( job );
}

void
Heartbeat::stop()
{
    // canceling a deadline_timer requires using a _stopped flag since the handler can already be
    // queued for invoation prior to invoking cancel() see
    // http://www.boost.org/doc/libs/1_42_0/doc/html/boost_asio/reference/basic_deadline_timer/cancel/overload1.html
    _stopped = true;

    LOG_TRACE_MSG( __FUNCTION__ );
    boost::system::error_code error;
    _timer.cancel( error );

    if ( error ) {
        LOG_WARN_MSG( "could not stop: " << boost::system::system_error( error ).what() );
    }
}

void
Heartbeat::wait(
        const Job::Ptr& job
        )
{
    _timer.expires_from_now( boost::posix_time::seconds(_waitTime) );
    _timer.async_wait(
            job->strand().wrap(
                boost::bind(
                    &Heartbeat::handler,
                    this,
                    _1,
                    job
                    )
                )
            );
}

void
Heartbeat::pulse(
        const Job::Ptr& job,
        const Uci& node,
        const unsigned count
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );
    const ToolCount::iterator i = _status.find( node );
    if ( i == _status.end() ) {
        LOG_WARN_MSG( "could not find node " << node );
        return;
    }

    i->second = count;
    LOG_TRACE_MSG(
            node << " has " << count << " tool" <<
            ( count == 1 ? "" : "s" ) <<
            " running"
            );

    const unsigned sum = this->sum();

    if ( sum == std::numeric_limits<unsigned>::max() ) {
        // not all nodes have reported in yet
        return;
    }

    if ( sum ) {
        // some tools still active
        return;
    }

    // getting here means no tools are active
    this->stop();

    const boost::posix_time::ptime now( boost::posix_time::microsec_clock::local_time() );
    const boost::posix_time::time_duration elapsed( now - _start );
    LOG_WARN_MSG( "no progress made in last " << _waitTime << " seconds" );
    job->setError( 
            "END_JOB control action heartbeat timed out after " + boost::lexical_cast<std::string>( elapsed.total_seconds() ) +
            " seconds",
            error_code::kill_timeout
            );

    // mark the compute nodes in use by this job as unavailable for future jobs
    job->block()->unavailable( job );

    job->remove();
}

unsigned
Heartbeat::sum() const
{
    // if any I/O nodes have a tool count of std::numeric_limits<unsigned>::max it means
    // they haven't reported yet
    const ToolCount::const_iterator i = std::find_if(
            _status.begin(),
            _status.end(),
            boost::bind(
                std::equal_to<unsigned>(),
                std::numeric_limits<unsigned>::max(),
                boost::bind(
                    &ToolCount::value_type::second,
                    _1
                    )
                )
            );

    if ( i != _status.end() ) {
        LOG_TRACE_MSG( i->first << " has not reported" );
        return std::numeric_limits<unsigned>::max();
    }

    const unsigned sum = std::accumulate(
            _status.begin(),
            _status.end(),
            0,
            boost::bind(
                std::plus<unsigned>(),
                _1,
                boost::bind(
                    &ToolCount::value_type::second,
                    _2
                    )
                )
            );

    return sum;
}

void
Heartbeat::handler(
        const boost::system::error_code& error,
        const boost::shared_ptr<Job>& job
        )
{
    LOGGING_DECLARE_JOB_MDC( job->id() );
    ++_cycles;
    LOG_TRACE_MSG( __FUNCTION__ << " " << _cycles );

    if ( error == boost::asio::error::operation_aborted || _stopped) {
        LOG_TRACE_MSG( "aborted" );
        return;
    } else if ( error ) {
        LOG_ERROR_MSG( "could not wait: " << boost::system::system_error(error).what() );
        return;
    }

    // look for any tools that haven't reported
    const ToolCount::const_iterator i = std::find_if(
            _status.begin(),
            _status.end(),
            boost::bind(
                std::equal_to<unsigned>(),
                std::numeric_limits<unsigned>::max(),
                boost::bind(
                    &ToolCount::value_type::second,
                    _1
                    )
                )
            );

    // we only care if a tool hansn't made progress after the first cycle
    if ( _cycles > 1 && i != _status.end() ) {
        LOG_INFO_MSG( i->first << " has not reported" );

        const boost::posix_time::ptime now( boost::posix_time::microsec_clock::local_time() );
        const boost::posix_time::time_duration elapsed( now - _start );
        job->setError( 
                "END_JOB control action heartbeat timed out after " + boost::lexical_cast<std::string>( elapsed.total_seconds() ) +
                " seconds",
                error_code::kill_timeout
                );

        // mark the compute nodes in use by this job as unavailable for future jobs
        job->block()->unavailable( job );

        job->remove();

        return;
    } else if ( _cycles > 1 ) {
        const unsigned sum = this->sum();
        LOG_INFO_MSG( this->sum() << " active tool" << (sum == 1 ? "" : "s") );
    }

    LOG_INFO_MSG( 
            "checking for tool progress in " << job->io().size() << " I/O node" << 
            (job->io().size() == 1 ? "" : "s") << " after " << _cycles << " cycles"
            );

    // send tool status to each I/O node
    const cios::Message::Ptr msg(
            cios::Message::create( bgcios::jobctl::CheckToolStatus, job->id() )
            );
    msg->as<bgcios::jobctl::CheckToolStatusMessage>()->seconds = _waitTime;

    BOOST_FOREACH( IoNode::Map::value_type& i, job->io() ) {
        IoNode& node = i.second;
        node.writeControl( msg );
    }

    this->wait( job );
}

} // job
} // server
} // runjob
