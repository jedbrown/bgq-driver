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
#include "server/job/Signal.h"
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
Handle::control(
        const Uci& location,
        const cios::Message::Ptr& message
        )
{
    LOGGING_DECLARE_JOB_MDC( _job->id() );
    LOGGING_DECLARE_BLOCK_MDC( _job->info().getBlock() );

    const bgcios::MessageHeader* header = message->header();

    const IoNode::Map::iterator iterator = _job->io().find( location );
    if ( iterator == _job->io().end() ) {
        LOG_WARN_MSG( "could not find I/O node: " << bgcios::printHeader(*header) );
        return;
    }
    IoNode& node = iterator->second;

    BOOST_ASSERT( header->service == bgcios::JobctlService );

    node.handleControl(
            header,
            message,
            _job
            );
    
    this->transition( header );
}

void
Handle::data(
        const Uci& location,
        const cios::Message::Ptr& message,
        const Callback& callback
        )
{
    LOGGING_DECLARE_JOB_MDC( _job->id() );
    LOGGING_DECLARE_BLOCK_MDC( _job->info().getBlock() );

    const bgcios::MessageHeader* header = message->header();

    if ( header->type == bgcios::stdio::WriteStdout || header->type == bgcios::stdio::WriteStderr ) {
        if ( !_job->killTimer().expires().is_not_a_date_time() ) {
            // job is dying, drop output
            callback();
            return;
        }

        const boost::shared_ptr<bgcios::stdio::WriteStdioMessage> msg(
                message->as<bgcios::stdio::WriteStdioMessage>()
                );

        // note: length of stdout message is contained within the header
        const message::StdIo::Ptr outmsg( new message::StdIo() );
        outmsg->setClientId( _job->client() );
        outmsg->setJobId( _job->id() );
        outmsg->setData( msg->data, header->length - sizeof(*header) );
        outmsg->setRank( header->rank );
        outmsg->setType( 
                header->type == bgcios::stdio::WriteStdout ? runjob::Message::StdOut : runjob::Message::StdError
                );

        // add it to our queue
        _job->queue().add( outmsg, callback );

        return;
    }

    const IoNode::Map::iterator iterator = _job->io().find( location );
    if ( iterator == _job->io().end() ) {
        LOG_WARN_MSG( "could not find I/O node: " << bgcios::printHeader(*header) );
        if ( callback ) callback();
        return;
    }
    IoNode& node = iterator->second;

    BOOST_ASSERT( header->service == bgcios::StdioService );

    node.handleData(
            header,
            message,
            _job,
            callback
            );

    this->transition( header );
}

void
Handle::transition(
        const bgcios::MessageHeader* header
        )
{
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
            header->type == bgcios::jobctl::EndToolAck
       )
    {
        return;
    }
            
    if ( header->type == bgcios::jobctl::SignalJobAck ) {
        IoNode::Map& io = _job->io();
        std::size_t count = 0;
        BOOST_FOREACH( IoNode::Map::value_type& i, io ) {
            IoNode& node = i.second;
            if ( !node.signalInFlight() ) ++count;
        }
        if ( count != io.size() ) {
            const std::size_t remaining = io.size() - count;
            LOG_DEBUG_MSG( "waiting for " << remaining << " I/O node" << (remaining == 1 ? "" : "s") << " to acknowledge signal");
            return;
        }

        const int outstanding( _job->_outstandingSignal );
        LOG_INFO_MSG( "signal " << outstanding << " acknowledged" );
        _job->_outstandingSignal = 0;

        // need to deliver KILL signal if it was requested while previous signal
        // was oustanding
        if (
                outstanding != SIGKILL &&
                outstanding != bgcios::jobctl::SIGHARDWAREFAILURE &&
                !_job->killTimer().expires().is_not_a_date_time()
           )
        {
            LOG_INFO_MSG( "delivering delayed KILL signal" );
            Signal::create( _job, SIGKILL );
        }

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
