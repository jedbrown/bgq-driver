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

#include "Protocol.h"

#include <utility/include/cxxsockets/SockAddrList.h>

#include <boost/foreach.hpp>


#include <unistd.h>

LOG_DECLARE_FILE( "master" );

Protocol::Protocol()
{

}

Protocol::~Protocol()
{
    // Lock and unlock so that we don't destruct
    // in the middle of a send/receive operation.
    // If the op is hung on a blocking send/receive,
    // we can't destruct, but if that ever happens,
    // we've got bigger problems that need to be addressed.
    boost::mutex::scoped_lock scoped_lock(_sr_lock);
    scoped_lock.unlock();
}

void
Protocol::sendOnly(
        const std::string& requestName,
        const XML::Serializable& requestObject
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    boost::mutex::scoped_lock scoped_lock(_sr_lock);
    if (!_requester) {
        throw CxxSockets::Error(-1, "No requester socket.");
    }

    // Send the request
    const CxxSockets::Message requestNameMessage(requestName);
    CxxSockets::Message requestMessage;
    requestObject.write(requestMessage);
    LOG_TRACE_MSG("Sending request name.");
    _requester->Send(requestNameMessage);
    LOG_TRACE_MSG("Sent name, sending message.");
    _requester->Send(requestMessage);
    LOG_TRACE_MSG("Sent message.");
}

void
Protocol::sendReceive(
        const std::string& requestName,
        const XML::Serializable& requestObject,
        const std::string& replyName,
        XML::Serializable& replyObject
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    boost::mutex::scoped_lock scoped_lock(_sr_lock);
    if (!_requester) {
        throw CxxSockets::Error(-1, "No requester socket.");
    }

    // Send the request
    const CxxSockets::Message requestNameMessage(requestName);
    CxxSockets::Message requestMessage;
    requestObject.write(requestMessage);
    LOG_TRACE_MSG("Sending request name.");
    _requester->Send(requestNameMessage);
    LOG_TRACE_MSG("Sent name, sending message.");
    _requester->Send(requestMessage);
    LOG_TRACE_MSG("Sent message.");

    // Receive the reply
    CxxSockets::Message replyClassName;
    CxxSockets::Message replyMessage;

    LOG_TRACE_MSG("Receiving reply class name.");
    _requester->Receive(replyClassName);
    LOG_TRACE_MSG("Reply class name received.");
    if (replyClassName.str() != replyName) {
        LOG_ERROR_MSG("Bad reply: " << replyClassName.str() << "!=" << replyName);
    }

    LOG_TRACE_MSG("Receiving reply.");
    _requester->Receive(replyMessage);
    LOG_TRACE_MSG("Reply received.");

    std::istringstream is(replyMessage.str());
    replyObject.read(is);
}

void
Protocol::initializeRequester(
        const bgq::utility::Properties::ConstPtr& props,
        const int ipv,
        const std::string& host,
        const std::string& port,
        const unsigned attempts
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    const CxxSockets::SockAddrList remote_list(static_cast<unsigned short>(ipv), host, port);

    // Connect to the server.
    // Let the caller decide the exception handling policy
    unsigned retries = 0;
    const bool forever = (attempts == 0); // Zero attempts means try 'forever'
    unsigned timeout = 500;
    const unsigned timeout_max = 3000000;

    // Normally, we pass socket exceptions back to the client.
    // Here, however, we make use of them.
    bool connected = false;
    BOOST_FOREACH(const CxxSockets::SockAddr& remote, remote_list) {
        if (connected) {
            break;
        }
        while (retries < attempts || forever) {
            const CxxSockets::SecureTCPSocketPtr sock(new CxxSockets::SecureTCPSocket(remote.family(), 0));
            try {
                bgq::utility::ClientPortConfiguration port_config(0, bgq::utility::ClientPortConfiguration::ConnectionType::Command);
                port_config.setProperties(props, "");
                port_config.notifyComplete();
                sock->Connect(remote, port_config);
            } catch (const CxxSockets::Error& e) {
                if ( e.errcode == -1 ) {
                    // no point in retrying
                    throw;
                }
                LOG_DEBUG_MSG("Server not available, will retry: " << e.what() );
                if (timeout < timeout_max)
                    timeout *= 10;
                if ( forever || attempts > 1 ) {
                    usleep(timeout);
                }
                ++retries;
                continue;
            }
            _requester = sock;
            connected = true;
            break;
        }
    }

    if (retries >= attempts && !forever) {
        std::ostringstream msg;
        msg << "Retries timed out attempting to connect to " << host << ":" << port;
        throw CxxSockets::HardError(EAGAIN, msg.str());
    }

    // Keepalive.  Going to fire 'em off pretty quick because LNs may go away without warning.
    // 1) Two seconds from the last data packet, we send our first probe.
    // 2) Send a probe every second.
    // 3) Only three probes to consider connection dead.
    // So what this means is that after we send a packet, we wait five
    // seconds.  If we haven't sent another, we start sending probes.
    // We send a probe every second and if the other side doesn't respond
    // in three seconds, we consider the connection dead.
    // So, if the connection is not responsive for five seconds, we consider it bad.
    _requester->setProbe(true, 2, 1, 3);
}

void
Protocol::initializeResponder(
        CxxSockets::SecureTCPSocketPtr sock
        )
{
     LOG_TRACE_MSG(__FUNCTION__);
     _responder = sock;
    // Keepalive.  Going to fire 'em off pretty quick because LNs may go away without warning.
    // 1) Five seconds from the last data packet, we send our first probe.
    // 2) Send a probe every second.
    // 3) Only three probes to consider connection dead.
    // So what this means is that after we send a packet, we wait five
    // seconds.  If we haven't sent another, we start sending probes.
    // We send a probe every second and if the other side doesn't respond
    // in three seconds, we consider the connection dead.
    // So, if the connection is not responsive for five seconds, we consider it bad.
    _responder->setProbe(true, 2, 1, 3);
}

void
Protocol::setRequester(
        CxxSockets::SecureTCPSocketPtr sock
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    _requester = sock;
}

void
Protocol::sendReply(
        const std::string& requestName,
        const XML::Serializable& replyObject
        )
{
    if (!_responder)
        return;
    LOG_DEBUG_MSG("Sending " << requestName << " reply.");
    // First send the class name
    const CxxSockets::Message replyClassName(requestName);
    if (_responder) {
        _responder->Send(replyClassName);
    } else {
        std::ostringstream msg;
        msg << "Responder not yet initialized.";
        LOG_ERROR_MSG(msg.str());
        throw CxxSockets::SoftError(EAGAIN, msg.str());
    }

    // Then send the message
    CxxSockets::Message replyMessage;
    replyObject.write(replyMessage);
    LOG_TRACE_MSG("Sending reply.");
    _responder->Send(replyMessage);
    LOG_TRACE_MSG("Sent reply.");
}

void
Protocol::getName(
        std::string& requestName
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    if (!_responder)
        return;
    CxxSockets::Message classNameMessage;
    LOG_TRACE_MSG("Receiving class name.");
    _responder->Receive(classNameMessage);
    LOG_TRACE_MSG("Received class name.");
    requestName = classNameMessage.str();
}

void
Protocol::getObject(
        XML::Serializable* requestObject
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    if (!_responder)
        return;
    CxxSockets::Message msg;
    LOG_TRACE_MSG("Receiving object.");
    _responder->Receive(msg);
    LOG_TRACE_MSG("Received object.");
    std::istringstream is(msg.str());
    requestObject->read(is);
}
