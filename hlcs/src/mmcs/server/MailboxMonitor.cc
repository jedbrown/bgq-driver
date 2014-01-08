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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

/*!
 * \file MailboxMonitor.cc
 */

#include "MailboxMonitor.h"

#include "BlockControllerBase.h"
#include "BlockControllerTarget.h"
#include "BlockHelper.h"

#include "common/Properties.h"

#include <control/include/mcServer/MCServerRef.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>

#include <utility/include/cxxsockets/ListeningSocket.h>
#include <utility/include/cxxsockets/SecureTCPSocket.h>
#include <utility/include/cxxsockets/SockAddr.h>

#include <signal.h>

using namespace MCServerMessageSpec;

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {

MailboxMonitor::MailboxMonitor(
        BlockControllerBase* block
        ) :
    _listener(block),
    _mailboxRegistered(false),
    _block(block)
{

}

MailboxMonitor::~MailboxMonitor()
{
    if (isStarted() == true) {
        stop();
    }
}

void
MailboxMonitor::start(
        mmcs_client::CommandReply& reply
        )
{
    if (isStarted())  { // Already started?
        return;
    }

    // Listen on an ephemeral port for a connection from the mcServer mailbox monitor
    std::ostringstream listenerAddr;

    try {
        CxxSockets::SockAddr saddr;
        _block->getMCServer()->getConnectionSock()->getSockName(saddr);

        const CxxSockets::SockAddr lsa(
                saddr.family(),
                saddr.getHostAddr(),
                "0"
                );

        _listener._listener.reset(
                new CxxSockets::ListeningSocket(lsa, SOMAXCONN)
                );

        CxxSockets::SockAddr sa;
        _listener._listener->getSockName(sa);

        // Start the MailboxListener thread
        _listener.setJoinable(true);
        _listener.start();

        listenerAddr << sa.getHostAddr() << ":" << sa.getServicePort();
        LOG_DEBUG_MSG("Listening on " << listenerAddr.str() << " for mc_server return connection.");
    } catch (const CxxSockets::Error& e) {
        std::ostringstream errmsg;
        errmsg << "MailboxMonitor failed to register listener socket.  " << e.what();
        LOG_ERROR_MSG(errmsg.str());
        reply << mmcs_client::FAIL << errmsg.str() << mmcs_client::DONE;
        return;
    }

    // Send a RegisterEventListener request with our port number to the mcServer
    try {
        const RegisterEventListenerRequest mcRegisterEventListenerRequest(
                _block->getBlockName(),
                listenerAddr.str(),
                false,
                atoi(MCServerMessageSpec::VERSION.c_str())
                );
        RegisterEventListenerReply mcRegisterEventListenerReply;
        if (!_block->hardWareAccessBlocked()) {
            _block->_mcServer->registerEventListener(mcRegisterEventListenerRequest, mcRegisterEventListenerReply);
        } else {
            mcRegisterEventListenerReply._rc = -1;
            mcRegisterEventListenerReply._rt = "SubnetMc temporarily unavailable.";
        }

        if (mcRegisterEventListenerReply._rc) {
            reply << mmcs_client::FAIL << "MailboxMonitor failed to register event listener: "
                << mcRegisterEventListenerReply._rt << mmcs_client::DONE;
            return;
        }
        _mailboxRegistered = true;
        reply << mmcs_client::OK << mmcs_client::DONE;
    } catch (const std::exception& e) {
        reply << mmcs_client::FAIL << e.what() << mmcs_client::DONE;
        return;
    }
}

void
MailboxMonitor::stop()
{
    _listener.stop(SIGUSR1); // Stop the mailbox listener

    // Stop the mailbox listener from reading any more RAS before we send a
    // close request.
    if (_listener._mailbox) {
        _listener._mailbox->Shutdown(CxxSockets::Socket::RECEIVE);
    } else {
        return;
    }

    // Send a CloseEventListener request to the mcServer
    if (_mailboxRegistered) {
        try {
            const std::string blockname = _block->getBlockName();
            CloseEventListenerRequest mcCloseEventListenerRequest;
            mcCloseEventListenerRequest._set = _block->getBlockName();
            CloseEventListenerReply mcCloseEventListenerReply;
            _block->_mcServer->closeEventListener(mcCloseEventListenerRequest, mcCloseEventListenerReply);
            if (mcCloseEventListenerReply._rc) {
                LOG_ERROR_MSG("Failed to close event listener: " << mcCloseEventListenerReply._rt);
            }
        } catch (const std::exception &e) {
            LOG_ERROR_MSG(__FUNCTION__ << "() " << e.what());
        }
        _mailboxRegistered = false;
    }
}

MailboxListener::MailboxListener(
        BlockControllerBase* block
        ) :
    Thread(),
    _block( block )
{

}

void*
MailboxListener::threadStart()
{
    static int signals[] = { SIGHUP, SIGINT, SIGQUIT, SIGTERM, SIGPIPE, SIGXFSZ };
    static int num_signals = sizeof(signals) / sizeof(signals[0]);
    sigset_t newmask;

    // Ignore termination signals -- let the foreground thread handle them
    sigemptyset(&newmask);
    for (int i = 0; i < num_signals; i++) {
        sigaddset(&newmask, signals[i]);
    }
    pthread_sigmask(SIG_BLOCK, &newmask, NULL);

    log4cxx::MDC _blockid_mdc_( "blockId", std::string("{") + _block->getBlockName() + "} " );
    log4cxx::MDC _user_mdc_( "user", std::string("[") + _block->getUserName() + "] " );

    LOG_DEBUG_MSG("Starting mailbox listener thread.");

    // Wait for a connection from mcServer
    const CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
    _listener->AcceptNew(sock);
    bgq::utility::ServerPortConfiguration port_config(0);
    port_config.setProperties(common::Properties::getProperties(), "");
    port_config.notifyComplete();
    _mailbox.reset(
            new CxxSockets::SecureTCPSocket(
                sock,
                port_config
                )
            );
    CxxSockets::SockAddr remote;
    _mailbox->getPeerName( remote );
    LOG_DEBUG_MSG( "Accepted connection from mc_server at " << remote.getHostAddr() << ":" << remote.getServicePort() );

    _listener.reset();

    while (1) {
        if (isThreadStopping()) {
            break;
        }

        // Check for a mailbox message
        CxxSockets::Message classNameMessage;
        std::string messageType;
        try {
            _mailbox->Receive(classNameMessage);
            messageType = classNameMessage.str();
        } catch (const CxxSockets::SoftError& e) {
            LOG_TRACE_MSG( "Connection to mc_server interrupted: " << e.what() );
            continue;
        } catch (const CxxSockets::Error& e) {
            if (e.errcode == 0) { // This would be for a normal connection closed
                LOG_INFO_MSG("Connection to mc_server closed.");
            } else {
                LOG_ERROR_MSG( __FUNCTION__ << "() " << e.what() );
            }
            break;
        }

        // Receive and process a mailbox message
        if (messageType == RASMessage::getClassName()) {
            RASMessage rasMessage;
            CxxSockets::Message msg;
            int recid = 0;
            try {
                _mailbox->Receive(msg);

                std::istringstream is(msg.str());
                rasMessage.read(is);
                recid = processRASMessage(rasMessage);
            } catch (const CxxSockets::Error& e) {
                LOG_ERROR_MSG( "Connection to mc_server failed: " << e.what() );
                break;
            }

            // Responded to the RAS message.  Now we need to do post-processing and barrier acking.
            if (_block->_do_barrier_ack) {
                MCServerRef* temp;
                mmcs_client::CommandReply reply;
                BlockControllerBase::mcserver_connect(temp, _block->_userName, reply);
                const boost::scoped_ptr<MCServerRef> ref( temp );

                if (reply.getStatus()) {
                    // Could not connect to mc_server, nothing else we can do
                } else {
                    LOG_TRACE_MSG("Connected to mc_server.");
                    const BlockPtr p = _block->shared_from_this();
                    const BlockControllerTarget target(p, "{*}", reply);
                    _block->makeAndOpenTargetSet(ref.get(), true, reply, &target);

                    const BarrierAckRequest ackreq(_block->_blockName);
                    BarrierAckReply ackrep;
                    ref->barrierAck(ackreq, ackrep);
                    _block->_do_barrier_ack = false;
                }
            }
            if (recid) {
                _block->_helper->postProcessRASMessage(recid);
            }
        } else if (messageType == ConsoleMessage::getClassName()) {
            ConsoleMessage consoleMessage;
            CxxSockets::Message msg;
            try {
                _mailbox->Receive(msg);

                std::istringstream is(msg.str());
                consoleMessage.read(is);
            } catch (const CxxSockets::Error& e) {
                LOG_ERROR_MSG( "Failure receiving console message, error is: " << e.what() );
                break;
            }
            processConsoleMessage(consoleMessage);
        } else {
            // If it wasn't a RAS or Console message, quit
            LOG_ERROR_MSG("Unsupported message type " << messageType);
            break;
        }
    }

    LOG_DEBUG_MSG("Stopping mailbox listener thread.");
    return NULL;
}

int
MailboxListener::processRASMessage(
        RASMessage& rasMessage
        )
{
    // Build a RasEventImpl object from the rasMessage
    RasEventImpl rasEvent(
            rasMessage._msgId,
            rasMessage._handled,
            rasMessage._time_secs,
            rasMessage._time_usecs,
            rasMessage._keys,
            rasMessage._values
            );

    // Invoke the RAS event handler chain
    RasEventHandlerChain::handle(rasEvent);

    // Log the RAS event in the database
    return _block->_helper->processRASMessage(rasEvent);
}

void
MailboxListener::processConsoleMessage(
        ConsoleMessage& consoleMessage
        )
{
    _block->_helper->processConsoleMessage(consoleMessage);
}

} } // namespace mmcs::server
