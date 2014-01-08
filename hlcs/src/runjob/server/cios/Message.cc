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
#include "server/cios/Message.h"

#include "common/logging.h"

#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <ramdisk/include/services/StdioMessages.h>

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace cios {

template <typename T>
boost::shared_ptr<void>
init(
        BGQDB::job::Id job,
        uint32_t rank,
        uint16_t type,
        uint8_t service
    )
{
    const boost::shared_ptr<T> result( boost::make_shared<T>() );
    bzero( result.get(), sizeof(T) );

    result->header.type = type;
    result->header.length = sizeof(T);
    result->header.jobId = job;
    result->header.rank = rank;
    result->header.service = service;
    if ( service == bgcios::JobctlService ) {
        result->header.version = bgcios::jobctl::ProtocolVersion;
    } else if ( service == bgcios::StdioService ) {
        result->header.version = bgcios::stdio::ProtocolVersion;
    } else {
        LOG_FATAL_MSG( service );
        BOOST_ASSERT( !"unhandled service" );
    }
    LOG_TRACE_MSG( bgcios::printHeader(result->header) );

    return result;
}

Message::Message() :
    _message(),
    _unhandled(),
    _type( 0 )
{

}

boost::asio::const_buffers_1
Message::buffer() const
{
    BOOST_ASSERT( _message );

    const bgcios::MessageHeader* header = this->header();
    BOOST_ASSERT( header->length != 0 );

    return boost::asio::const_buffers_1(
            _message.get(),
            header->length
            );
}

Message::Ptr
Message::create()
{
    const Ptr result(
            boost::make_shared<Message>()
            );

    return result;
}

Message::Ptr
Message::create(
        uint16_t type,
        BGQDB::job::Id job,
        uint32_t rank
        )
{
    Ptr result( boost::make_shared<Message>() );
    boost::shared_ptr<void> message;

    switch ( type ) {
        case bgcios::jobctl::ChangeConfig: 
            message = init<bgcios::jobctl::ChangeConfigMessage>( job, rank, type, bgcios::JobctlService );
            break;
        case bgcios::jobctl::CheckToolStatus: 
            message = init<bgcios::jobctl::CheckToolStatusMessage>( job, rank, type, bgcios::JobctlService );
            break;
        case bgcios::jobctl::CleanupJob: 
            message = init<bgcios::jobctl::CleanupJobMessage>( job, rank, type, bgcios::JobctlService );
            break;
        case bgcios::jobctl::LoadJob: 
            message = init<bgcios::jobctl::LoadJobMessage>( job, rank, type, bgcios::JobctlService );
            break;
        case bgcios::jobctl::SetupJob: 
            message = init<bgcios::jobctl::SetupJobMessage>( job, rank, type, bgcios::JobctlService );
            break;
        case bgcios::jobctl::SignalJob: 
            message = init<bgcios::jobctl::SignalJobMessage>( job, rank, type, bgcios::JobctlService );
            break;
        case bgcios::jobctl::StartJob: 
            message = init<bgcios::jobctl::StartJobMessage>( job, rank, type, bgcios::JobctlService );
            break;
        case bgcios::jobctl::StartTool: 
            message = init<bgcios::jobctl::StartToolMessage>( job, rank, type, bgcios::JobctlService );
            break;
        case bgcios::jobctl::EndTool: 
            message = init<bgcios::jobctl::EndToolMessage>( job, rank, type, bgcios::JobctlService );
            break;
        case bgcios::jobctl::ExitToolAck: 
            message = init<bgcios::jobctl::ExitToolAckMessage>( job, rank, type, bgcios::JobctlService );
            break;
        case bgcios::stdio::ChangeConfig: 
            message = init<bgcios::stdio::ChangeConfigMessage>( job, rank, type, bgcios::StdioService );
            break;
        case bgcios::stdio::StartJob: 
            message = init<bgcios::stdio::StartJobMessage>( job, rank, type, bgcios::StdioService );
            break;
        case bgcios::stdio::ReadStdinAck: 
            message = init<bgcios::stdio::ReadStdinAckMessage>( job, rank, type, bgcios::StdioService );
            break;
        case bgcios::jobctl::Authenticate: 
            message = init<bgcios::jobctl::AuthenticateMessage>( job, rank, type, bgcios::JobctlService );
            break;
        case bgcios::stdio::Authenticate: 
            message = init<bgcios::stdio::AuthenticateMessage>( job, rank, type, bgcios::StdioService );
            break;
        case bgcios::stdio::Reconnect: 
            message = init<bgcios::stdio::ReconnectMessage>( job, rank, type, bgcios::StdioService );
            break;
        case bgcios::jobctl::Reconnect: 
            message = init<bgcios::jobctl::ReconnectMessage>( job, rank, type, bgcios::JobctlService );
            break;
        case bgcios::jobctl::Heartbeat: 
            message = init<bgcios::jobctl::HeartbeatMessage>( job, rank, type, bgcios::JobctlService );
            break;
        default:
            BOOST_ASSERT( !"unhandled type" );
    }

    result->_message = message;
    result->_type = type;

    return result;
}

boost::asio::mutable_buffer
Message::prepare(
        const bgcios::MessageHeader& header
        )
{
    // create buffer for message
    if ( header.service == bgcios::JobctlService) {
        LOG_TRACE_MSG(bgcios::jobctl::toString( header.type ) );
        switch ( header.type ) {
            case bgcios::jobctl::LoadJobAck:
                _message = boost::make_shared<bgcios::jobctl::LoadJobAckMessage>();
                break;
            case bgcios::jobctl::StartJobAck:
                _message = boost::make_shared<bgcios::jobctl::StartJobAckMessage>();
                break;
            case bgcios::jobctl::EndToolAck:
                _message = boost::make_shared<bgcios::jobctl::EndToolAckMessage>();
                break;
            case bgcios::jobctl::ExitJob:
                _message = boost::make_shared<bgcios::jobctl::ExitJobMessage>();
                break;
            case bgcios::jobctl::ExitProcess: 
                _message = boost::make_shared<bgcios::jobctl::ExitProcessMessage>();
                break;
            case bgcios::jobctl::ExitTool: 
                _message = boost::make_shared<bgcios::jobctl::ExitToolMessage>();
                break;
            case bgcios::jobctl::ErrorAck:
                _message = boost::make_shared<bgcios::jobctl::ErrorAckMessage>();
                break;
            case bgcios::jobctl::ChangeConfig:
                _message = boost::make_shared<bgcios::jobctl::ChangeConfigMessage>();
                break;
            case bgcios::jobctl::ChangeConfigAck:
                _message = boost::make_shared<bgcios::jobctl::ChangeConfigAckMessage>();
                break;
            case bgcios::jobctl::CheckToolStatusAck:
                _message = boost::make_shared<bgcios::jobctl::CheckToolStatusAckMessage>();
                break;
            case bgcios::jobctl::CleanupJobAck:
                _message = boost::make_shared<bgcios::jobctl::CleanupJobAckMessage>();
                break;
            case bgcios::jobctl::SetupJobAck:
                _message = boost::make_shared<bgcios::jobctl::SetupJobAckMessage>();
                break;
            case bgcios::jobctl::StartToolAck:
                _message = boost::make_shared<bgcios::jobctl::StartToolAckMessage>();
                break;
            case bgcios::jobctl::SignalJobAck:
                _message = boost::make_shared<bgcios::jobctl::SignalJobAckMessage>();
                break;
            case bgcios::jobctl::AuthenticateAck:
                _message = boost::make_shared<bgcios::jobctl::AuthenticateAckMessage>();
                break;
            case bgcios::jobctl::ReconnectAck:
                _message = boost::make_shared<bgcios::jobctl::ReconnectAckMessage>();
                break;
            case bgcios::jobctl::HeartbeatAck:
                _message = boost::make_shared<bgcios::jobctl::HeartbeatAckMessage>();
                break;
        }
    } else if ( header.service == bgcios::StdioService ) {
        LOG_TRACE_MSG(bgcios::stdio::toString( header.type ) );
        switch ( header.type ) {
            case bgcios::stdio::ChangeConfig:
                _message = boost::make_shared<bgcios::stdio::ChangeConfigMessage>();
                break;
            case bgcios::stdio::ChangeConfigAck:
                _message = boost::make_shared<bgcios::stdio::ChangeConfigAckMessage>();
                break;
            case bgcios::stdio::WriteStdout:
            case bgcios::stdio::WriteStderr:
                _message = boost::make_shared<bgcios::stdio::WriteStdioMessage>();
                break;
            case bgcios::stdio::WriteStdoutAck:
            case bgcios::stdio::WriteStderrAck:
                _message = boost::make_shared<bgcios::stdio::WriteStdioAckMessage>();
                break;
            case bgcios::stdio::StartJobAck:
                _message = boost::make_shared<bgcios::stdio::StartJobAckMessage>();
                break;
            case bgcios::stdio::CloseStdio:
                _message = boost::make_shared<bgcios::stdio::CloseStdioMessage>();
                break;
            case bgcios::stdio::ReadStdin:
                _message = boost::make_shared<bgcios::stdio::ReadStdinMessage>();
                break;
            case bgcios::stdio::AuthenticateAck:
                _message = boost::make_shared<bgcios::stdio::AuthenticateAckMessage>();
                break;
            case bgcios::stdio::ReconnectAck:
                _message = boost::make_shared<bgcios::stdio::ReconnectAckMessage>();
                break;
        }
    } else {
        BOOST_ASSERT(!"unhandled service type");
    }

    boost::asio::mutable_buffer result;

    if ( !_message ) {
        // This can happen if there's a new message type added to the cios protocol and this
        // code has not yet been updated to handle it.
        // We will gracefully handle this by creating an opaque buffer to read the message,
        // then later discard it because there's no way to handle a message that we
        // don't know about
        LOG_WARN_MSG( "unknown message type: " << bgcios::printHeader(header) );
        _unhandled.reset( new char[header.length - sizeof(header)] );
        result = boost::asio::mutable_buffer( _unhandled.get(), header.length - sizeof(header) );
    } else {
        result = boost::asio::mutable_buffer( _message.get(), header.length );

        // copy header into buffer
        memcpy( boost::asio::buffer_cast<void*>(result), &header, sizeof(header) );

        // create mutable buffer that is offset since we've already read the header
        result = result + sizeof(header);
    }

    return result;
}

} // cios
} // server
} // runjob
