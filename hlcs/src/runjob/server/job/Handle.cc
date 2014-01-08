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
#include "server/job/Handle.h"

#include "common/logging.h"

#include "server/block/IoNode.h"

#include "server/cios/Message.h"

#include "server/job/Debug.h"
#include "server/job/IoNode.h"
#include "server/job/Load.h"
#include "server/job/Start.h"
#include "server/job/Status.h"
#include "server/job/SubNodePacing.h"
#include "server/job/Transition.h"

#include "server/Job.h"

#include <ramdisk/include/services/MessageUtility.h>
#include <ramdisk/include/services/StdioMessages.h>


LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

Handle::Handle(
        const Job::Ptr& job
        ) :
    _job( job )
{

}

void
Handle::impl(
        const Uci& location,
        const cios::Message::Ptr& message,
        const Callback& callback
        )
{
    LOGGING_DECLARE_JOB_MDC( _job->id() );
    LOGGING_DECLARE_BLOCK_MDC( _job->info().getBlock() );

    // header is always the first member of each message
    const bgcios::MessageHeader* header = message->header();

    // find node
    const IoNode::Map::iterator iterator = _job->io().find( location );
    if ( iterator == _job->io().end() ) {
        LOG_WARN_MSG( "could not find I/O node" );
        callback();
        return;
    }
    IoNode& node = iterator->second;

    // check service
    if ( header->service == bgcios::JobctlService ) {
        // let the I/O node handle the message
        node.handleControl(
                header,
                message,
                _job
                );
        callback();
    } else if ( header->service == bgcios::StdioService ) {
        node.handleData(
                header,
                message,
                _job,
                callback
                );
    } else {
        BOOST_ASSERT( !"unhandled I/O service" );
    }

    if ( 
            _job->pacing() && (
            header->type == bgcios::jobctl::SetupJobAck ||
            header->type == bgcios::jobctl::LoadJobAck ||
            header->type == bgcios::jobctl::StartJobAck ||
            header->type == bgcios::jobctl::CleanupJobAck ||
            header->type == bgcios::jobctl::SignalJobAck)
       )
    {
        // got an ack from a sub-node job, we can send a message for the next job
        // for this node if one is available
        _job->pacing()->next();
    }

    // skip status transition check for certain messages
    if ( 
            header->type == bgcios::stdio::WriteStdout ||
            header->type == bgcios::stdio::WriteStderr ||
            header->type == bgcios::jobctl::StartToolAck ||
            header->type == bgcios::jobctl::EndToolAck ||
            header->type == bgcios::jobctl::SignalJobAck
       )
    {
        return;
    }

    // check if we can transition
    const Transition transition( _job );
    const Status::Value status = _job->status().get();
    IoNode::Map& io = _job->io();
    const std::size_t count = transition.next( status, io );
    if ( count != io.size() ) {
        return;
    }

    if ( status == Status::Setup ) {
        if ( _job->exitStatus().getError() ) {
            transition.end();
        } else {
            Load::create( _job );
        }
    } else if ( status == Status::Loading ) {
        if ( _job->exitStatus().getError() ) {
            transition.end();
        } else {
            transition.loaded();
        }
    } else if ( status == Status::Running ) {
        transition.end();
    } else if ( status == Status::OutputStarting ) {
        if ( _job->exitStatus().getError() ) {
            transition.end();
        } else {
            Start::create( _job, bgcios::JobctlService );
        }
    } else if ( status == Status::ControlStarting ) {
        if ( _job->exitStatus().getError() ) {
            transition.end();
        } else {
            transition.run();
        }
    } else if ( status == Status::Cleanup ) {
        _job->remove();
    } else {
        BOOST_ASSERT( !"unhandled status" );
    }
}

} // job
} // server
} // runjob
