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
#include "server/job/Input.h"

#include "common/message/StdIo.h"

#include "common/logging.h"

#include "server/cios/Message.h"

#include "server/Job.h"

#include <ramdisk/include/services/StdioMessages.h>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

void
Input::create(
        const Job::Ptr& job,
        const message::StdIo::Ptr& msg
        )
{
    const Input::Ptr input(
            new Input( job )
            );
    job->strand().post(
            boost::bind(
                &Input::impl,
                input,
                msg
                )
            );
}

Input::Input(
        const Job::Ptr& job
        ) :
    _job( job )
{

}

void
Input::impl(
        const message::StdIo::Ptr& msg
        )
{
    LOGGING_DECLARE_JOB_MDC( _job->id() );
    LOGGING_DECLARE_BLOCK_MDC( _job->info().getBlock() );
    LOG_TRACE_MSG( "impl" );

    LOG_TRACE_MSG( "rank " << msg->getRank() );

    // create message
    cios::Message::Ptr message(
            cios::Message::create( bgcios::stdio::ReadStdinAck, _job->id(), msg->getRank() ) 
            );
    const boost::shared_ptr<bgcios::stdio::ReadStdinAckMessage> ack = message->as<bgcios::stdio::ReadStdinAckMessage>();

    // ensure the message fits in the buffer, if not truncate it
    std::size_t size = msg->getLength();
    if ( size > sizeof(ack->data) ) {
        LOG_WARN_MSG( "message truncated to " << sizeof(ack->data) << " bytes from " << size << " bytes" );
        size = sizeof(ack->data);
    }
    memcpy( ack->data, msg->getData().c_str(), size );

    // length is header + length of bytes in buffer
    ack->header.length = static_cast<unsigned>( sizeof(bgcios::MessageHeader) + size );

    LOG_TRACE_MSG( "looking for " << msg->getLocation() );
    const IoNode::Map::iterator node = _job->io().find( msg->getLocation() );
    if ( node == _job->io().end() ) {
        LOG_WARN_MSG( "could not find I/O node " << msg->getLocation() );
        return;
    }

    IoNode& io = node->second;
    io.writeData( message );
}

} // job
} // server
} // runjob

