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
#include "server/job/Tool.h"

#include "common/logging.h"

#include "server/cios/Message.h"

#include "server/job/IoNode.h"

#include "server/Job.h"

#include <db/include/api/tableapi/gensrc/DBTJobtool.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <hlcs/include/runjob/commands/error.h>

#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/MessageUtility.h>

#include <boost/system/system_error.hpp>

#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

Tool::Tool(
        const unsigned id,
        const Job::Ptr& job,
        const runjob::tool::Daemon& tool,
        runjob::tool::Proctable& proctable,
        const Callback& callback
        ) :
    _job( job ),
    _id( id ),
    _io(),
    _error(),
    _callback( callback )
{
    LOG_DEBUG_MSG( "starting tool " << _id << " with subset " << tool.getSubset() );

    const cios::Message::Ptr message = cios::Message::create(bgcios::jobctl::StartTool, job->id());
    const boost::shared_ptr<bgcios::jobctl::StartToolMessage> startToolMessage = message->as<bgcios::jobctl::StartToolMessage>();
    startToolMessage->toolId = _id;

    // first argument is the tool executable
    startToolMessage->numArguments = 1;
    int offset = snprintf( startToolMessage->arguments, sizeof(startToolMessage->arguments), "%s%c", tool.getExecutable().c_str(), '\0' );

    // add arguments
    LOG_TRACE_MSG(
            tool.getArguments().size() <<  " argument" <<
            (tool.getArguments().size() == 1 ? "" : "s")
            );
    for ( auto i = tool.getArguments().begin(); i != tool.getArguments().end(); ++i ) {
        offset += snprintf( startToolMessage->arguments + offset, sizeof(startToolMessage->arguments) - offset, "%s%c", i->c_str(), '\0' );
        LOG_TRACE_MSG( "adding arg " << *i );
        startToolMessage->numArguments++;
    }
    LOG_TRACE_MSG( 
            "arg size: " << offset << " bytes for " << 
            startToolMessage->numArguments << " argument" <<
            (startToolMessage->numArguments == 1 ? "" : "s")
            );

    // add an entry for each I/O node location in the tool subset
    for ( unsigned i = 0; i < proctable.size(); ++i ) {
        const unsigned rank = proctable[i].rank();
        const Uci& location = proctable[i].io();
        const bool participating = tool.getSubset().match( rank );
        if ( !participating ) {
            LOG_TRACE_MSG( "rank " << rank << " with I/O node " << location << " is not participating" );
            proctable[i].setIo( Uci() );
            continue; 
        }

        // ensure this location is used by the job
        const IoNode::Map::iterator io = job->io().find( location );
        if ( io == job->io().end() ) continue;

        // remember this tool uses this I/O node
        const bool inserted = _io.insert(
                Io::value_type( location, tool::Status::Starting )
                ).second;

        if ( inserted ) {
            io->second.writeControl( message );
        }
        LOG_TRACE_MSG( "added I/O node " << location << " for rank " << rank );
    }

    // ensure we matched at least one rank for this tool
    if ( _io.empty() ) {
        _error +=
            boost::lexical_cast<std::string>( tool.getSubset() ) +
            " does not match any ranks"
            ;
        this->updateDatabase( tool::Status::Error );

        _callback( 
                _id, 
                runjob::commands::error::tool_subset_invalid,
                _error
                );

        BOOST_THROW_EXCEPTION(
                std::runtime_error( _error )
                );
    }
}

Tool::~Tool()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

Tool::Ptr
Tool::update(
        const tool::Status status,
        const Uci& location
        )
{
    const Ptr result = shared_from_this();

    const auto io = _io.find( location );
    if ( io == _io.end() ) {
        LOG_WARN_MSG( "could not find location " << location );
        return result;
    }
   
    LOG_TRACE_MSG( io->first << " " << status );
    io->second = status;

    // count total nodes that have same status
    const std::size_t total = std::count_if(
            _io.begin(),
            _io.end(),
            boost::bind(
                std::equal_to<tool::Status>(),
                boost::bind(
                    &Io::value_type::second,
                    _1
                    ),
                status
                )
            );
    LOG_TRACE_MSG( total << " of " << _io.size() << " nodes " << status );
    if ( total != _io.size() ) return result;

    LOG_INFO_MSG( "tool " << _id << " " << status );

    // special case for tools that failed to start
    if ( _callback && !_error.empty() ) {
        this->updateDatabase( tool::Status::Error );
        _callback( _id, runjob::commands::error::tool_path_invalid, _error );
    } else {
        this->updateDatabase( status );
    }

    if ( status == tool::Status::Running ) {
        _callback( _id, runjob::commands::error::success, _error );

        // clear callback since we are done with it
        _callback.clear();

        return result;
    }

    if ( status == tool::Status::Terminated ) {
        // remove from tool::Container if we ended
        if ( Job::Ptr job = _job.lock() ) {
            job->tools().remove( _id );
        }
    }
        
    return result;
}

void
Tool::failure(
        const boost::shared_ptr<bgcios::jobctl::StartToolAckMessage>& ack,
        const Uci& location
        )
{
    const Job::Ptr job( _job.lock() );
    if ( !job ) return;

    // header is always the first member of each message
    const bgcios::MessageHeader* header = reinterpret_cast<bgcios::MessageHeader*>( ack.get() );

    // if this is the first failure, retain some information
    if ( _error.empty() ) {
        const boost::system::system_error error(
                boost::system::error_code(
                    header->errorCode,
                    boost::system::get_system_category()
                    )
                );
        _error +=
            "Start tool " + boost::lexical_cast<std::string>(ack->toolId) + " failed on " + boost::lexical_cast<std::string>(location) + ": " +
            bgcios::returnCodeToString( header->returnCode ) + ", errno " +
            boost::lexical_cast<std::string>( error.code().value() ) + " " +
            error.what()
            ;
    }
   
    const Ptr me = this->update( tool::Status::Terminated, location );

    // kill remaining tools on other I/O nodes
    const cios::Message::Ptr message( cios::Message::create(bgcios::jobctl::EndTool, job->id()) );
    const boost::shared_ptr<bgcios::jobctl::EndToolMessage> endToolMessage(
            message->as<bgcios::jobctl::EndToolMessage>()
            );
    endToolMessage->toolId = _id;
    endToolMessage->signo = SIGTERM;

    BOOST_FOREACH( auto& i, _io ) {
        if (
                i.second != tool::Status::Ending ||
                i.second != tool::Status::Terminated
           )
        {
            continue;
        }

        i.second = tool::Status::Ending;

        const auto node = job->io().find( i.first );
        if ( node != job->io().end() ) {
            node->second.writeControl( message );
        }
    }
}

void
Tool::exited(
        const boost::shared_ptr<bgcios::jobctl::ExitToolMessage>& etm,
        const Uci& location
        )
{
    const Job::Ptr job( _job.lock() );
    if ( !job ) return;

    std::ostringstream error;
    error << "Tool " << etm->toolId << ": ";
    if ( WIFEXITED(etm->status) ) {
        error << "exited with status " << WEXITSTATUS(etm->status) << ".";
    } else if ( WIFSIGNALED(etm->status) ) {
        error << "terminated by signal " << WTERMSIG(etm->status) << ".";
    } else {
        error << "terminated with status " << etm->status << ".";
    }

    if ( etm->status ) {
        LOG_INFO_MSG( error.str() );
    }

    // if this is the first abnormal termination, retain some information
    if ( _error.empty() && etm->status ) {
        _error += error.str();
    }

    const Ptr me = this->update( tool::Status::Terminated, location );
}

void
Tool::updateDatabase(
        tool::Status status
        )
{
    if ( 
            status != tool::Status::Running &&
            status != tool::Status::Terminated &&
            status != tool::Status::Error
       )
    {
        LOG_TRACE_MSG( "skipping database update for status " << status );
        return;
    }

    try {
        const cxxdb::ConnectionPtr db(
                BGQDB::DBConnectionPool::Instance().getConnection()
                );
        if ( !db ) {
            LOG_WARN_MSG( "could not get database connection" );
            return;
        }

        const cxxdb::UpdateStatementPtr statement = db->prepareUpdate(
                std::string("UPDATE ") + BGQDB::DBTJobtool().getTableName() + " " +
                "SET " + BGQDB::DBTJobtool::STATUS_COL + "=?, " +
                BGQDB::DBTJobtool::ERRTEXT_COL + "=? " +
                "WHERE " + BGQDB::DBTJobtool::ID_COL + "=?",
                boost::assign::list_of
                ( BGQDB::DBTJobtool::STATUS_COL )
                ( BGQDB::DBTJobtool::ERRTEXT_COL )
                ( BGQDB::DBTJobtool::ID_COL )
                );
        statement->parameters()[ BGQDB::DBTJobtool::STATUS_COL ].set( tool::getDatabaseValue(status) );
        if ( _error.empty() ) {
            statement->parameters()[ BGQDB::DBTJobtool::ERRTEXT_COL ].setNull();
        } else {
            statement->parameters()[ BGQDB::DBTJobtool::ERRTEXT_COL ].set( _error );
        }
        statement->parameters()[ BGQDB::DBTJobtool::ID_COL ].cast( _id );

        statement->execute();
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }
}

} // job
} // server
} // runjob
