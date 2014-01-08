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
#include "server/job/tool/End.h"

#include "common/commands/EndTool.h"

#include "common/logging.h"

#include "server/cios/Message.h"

#include "server/CommandConnection.h"
#include "server/Job.h"

#include <ramdisk/include/services/JobctlMessages.h>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {
namespace tool {

void
End::create(
        const Job::Ptr& job,
        const runjob::commands::request::EndTool::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    const Ptr result(
            new End( job, request, connection )
            );

    job->strand().post(
            boost::bind(
                &End::impl,
                result
                )
            );
}

End::End(
        const Job::Ptr& job,
        const runjob::commands::request::EndTool::Ptr& request,
        const CommandConnection::Ptr& connection
        ) :
    _job( job ),
    _request( request ),
    _connection( connection ),
    _status( runjob::commands::error::unknown_failure ),
    _message()
{

}

End::~End()
{
    LOG_TRACE_MSG( __FUNCTION__ );
    try {
        const runjob::commands::response::EndTool::Ptr response(
                new runjob::commands::response::EndTool
                );
        if ( _status ) {
            LOG_INFO_MSG( _message.str() );
        }
        response->setError( _status );
        response->setMessage( _message.str() );
        _connection->write( response );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    } catch ( ... ) {
        LOG_WARN_MSG( "caught some other exception" );
    }
}

void
End::impl()
{
    LOGGING_DECLARE_JOB_MDC( _job->id() );
    LOGGING_DECLARE_BLOCK_MDC( _job->info().getBlock() );
    LOG_TRACE_MSG( "impl" );

    const Container::Tools& tools = _job->tools().get();
    const auto& tool = tools.find( _request->_tool );
    if ( tool == tools.end() ) {
        _status = runjob::commands::error::tool_not_found;
        _message << "tool " << _request->_tool << " not found";
        return;
    }

    // ready to end the tool
    LOG_DEBUG_MSG( "ending tool " << _request->_tool << " with signal " << _request->_signal );

    const cios::Message::Ptr message = cios::Message::create(bgcios::jobctl::EndTool, _job->id());
    const boost::shared_ptr<bgcios::jobctl::EndToolMessage> endToolMessage(
            message->as<bgcios::jobctl::EndToolMessage>()
            );
    endToolMessage->toolId = _request->_tool;
    endToolMessage->signo = _request->_signal;

    BOOST_FOREACH( const auto& i, tool->second->_io ) {
        const Uci& location = i.first;
        const IoNode::Map::iterator io = _job->io().find( location );

        if ( io == _job->io().end() ) continue;
        
        IoNode& node = io->second;
        node.writeControl( message );
        
        LOG_TRACE_MSG( "stopping tool " << _request->_tool << " on I/O node " << location );
    }

    _status = runjob::commands::error::success;
}

} // tool
} // job
} // server 
} // runjob

