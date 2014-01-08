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
 * \file SecureMailboxMonitor.cc
 */


#include "SecureMailboxMonitor.h"

#include "BlockControllerBase.h"
#include "BlockControllerTarget.h"
#include "BlockHelper.h"

#include "common/Properties.h"

#include <control/include/mcServer/MCServerRef.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>

#include <utility/include/cxxsockets/ListeningSocket.h>
#include <utility/include/cxxsockets/PollingSocketSet.h>
#include <utility/include/cxxsockets/SecureTCPSocket.h>
#include <utility/include/cxxsockets/SockAddr.h>

#include <signal.h>


using namespace MCServerMessageSpec;
using namespace std;


LOG_DECLARE_FILE( "mmcs.server" );


#define LOGGING_DECLARE_MBOX_BLOCK_ID_MDC \
    log4cxx::MDC _blockid_mbox_mdc_( "blockId", std::string("{") + _blockController->getBlockName() + "} " );


namespace mmcs {
namespace server {


SecureMailboxMonitor::SecureMailboxMonitor(
        BlockControllerBase* blockController
        ) :
    _mailboxRegistered(false),
    _blockController(blockController)
{
    // initialize mailbox listener
    _mailboxListener._blockController = _blockController;
}

SecureMailboxMonitor::~SecureMailboxMonitor()
{
    if(isStarted() == true) {
        stop();
    }
}

void
SecureMailboxMonitor::start(
        mmcs_client::CommandReply& reply
        )
{
    if (isStarted()) // already started?
        return;


    // listen on an ephemeral port for a connection from the mcServer mailbox monitor

    std::ostringstream listenerAddr;

    try {
        CxxSockets::SockAddr saddr;
        _blockController->getMCServer()->getConnectionSock()->getSockName(saddr);

        CxxSockets::SockAddr lsa(
                saddr.family(),
                saddr.getHostAddr(),
                "0"
                );

        CxxSockets::ListeningSocketPtr ln(new CxxSockets::ListeningSocket(lsa, SOMAXCONN));

        CxxSockets::SockAddr sa;
        ln->getSockName(sa);

        // start the SecureMailboxListener thread
        _mailboxListener._listener = ln;
        _mailboxListener.setJoinable(true);
        _mailboxListener.start();

        const int port = sa.getServicePort();
        const std::string host = sa.getHostAddr();
        listenerAddr << host << ":" << port;
        LOG_DEBUG_MSG("Listening on " << listenerAddr.str() << " for mc_server return connection.");
    } catch (const CxxSockets::Error& e) {
        std::ostringstream errmsg;
        errmsg << "SecureMailboxMonitor failed to register listener socket.  " << e.what();
        LOG_ERROR_MSG(errmsg.str());
        reply << mmcs_client::FAIL << errmsg.str() << mmcs_client::DONE;
        return;
    }

    // send a RegisterEventListener request with our port number to the mcServer
    try {
        RegisterEventListenerRequest mcRegisterEventListenerRequest(
                _blockController->getBlockName(),
                listenerAddr.str(),
                false,
                atoi(MCServerMessageSpec::VERSION.c_str())
                );
        RegisterEventListenerReply mcRegisterEventListenerReply;
        if(!_blockController->hardWareAccessBlocked())
            _blockController->_mcServer->registerEventListener(mcRegisterEventListenerRequest, mcRegisterEventListenerReply);
        else {
            mcRegisterEventListenerReply._rc = -1;
            mcRegisterEventListenerReply._rt = "SubnetMc temporarily unavailable.";
        }

        if (mcRegisterEventListenerReply._rc) {
            reply << mmcs_client::FAIL << "SecureMailboxMonitor failed to register event listener: "
                << mcRegisterEventListenerReply._rt << mmcs_client::DONE;
            return;
        }
        _mailboxRegistered = true;
        reply << mmcs_client::OK << mmcs_client::DONE;
    } catch (const exception& e) {
        reply << mmcs_client::FAIL << e.what() << mmcs_client::DONE;
        return;
    }
}

void
SecureMailboxMonitor::stop()
{
    _mailboxListener.stop(SIGUSR1); // stop the mailbox listener

    // Stop the mailbox listener from reading any more RAS
    // before we send a close request.  This will allow us
    // to continue to send any ras acks.
    if (_mailboxListener._mailbox) {
        _mailboxListener._mailbox->Shutdown(CxxSockets::Socket::RECEIVE);
    } else return;

    // send a CloseEventListener request to the mcServer
    if (_mailboxRegistered) {
        try {
            string blockname = _blockController->getBlockName();
            CloseEventListenerRequest mcCloseEventListenerRequest;
            mcCloseEventListenerRequest._set = _blockController->getBlockName();
            CloseEventListenerReply mcCloseEventListenerReply;
            _blockController->_mcServer->closeEventListener(mcCloseEventListenerRequest, mcCloseEventListenerReply);
            if (mcCloseEventListenerReply._rc)
                LOG_ERROR_MSG("failed to close event listener: " << mcCloseEventListenerReply._rt);
        } catch (const exception &e) {
            LOG_ERROR_MSG(e.what());
        }
        _mailboxRegistered = false;
    }
}

void*
SecureMailboxListener::threadStart()
{
    LOGGING_DECLARE_MBOX_BLOCK_ID_MDC;
    static int signals[] = { SIGHUP, SIGINT, SIGQUIT, SIGTERM, SIGPIPE, SIGXFSZ };
    static int num_signals = sizeof(signals) / sizeof(signals[0]);
    sigset_t newmask;

    LOG_INFO_MSG("starting");

    // ignore termination signals -- let the foreground thread handle them
    sigemptyset(&newmask);
    for (int i = 0; i < num_signals; i++)
        sigaddset(&newmask, signals[i]);
    pthread_sigmask(SIG_BLOCK, &newmask, NULL);

    // wait for a connection from the mcServer
    CxxSockets::PollingSocketSet listenerset;
    listenerset.AddSock(_listener);

    CxxSockets::SecureTCPSocketPtr secure;
    const unsigned timeout = 30000; // 30 seconds
    if ( !listenerset.Poll(timeout) ) {
        LOG_ERROR_MSG("timed out waiting for connection");
        return NULL;
    }

    CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
    _listener->AcceptNew(sock);
    bgq::utility::ServerPortConfiguration port_config(0);
    port_config.setProperties(common::Properties::getProperties(), "");
    port_config.notifyComplete();
    secure.reset(
            new CxxSockets::SecureTCPSocket(
                sock,
                port_config
                )
            );
    _mailbox = secure;
    LOG_DEBUG_MSG("Accepted new connection from mcserver");

    listenerset.RemoveSock(_listener);
    _listener.reset();

    while (1) {
        if (isThreadStopping()) break;

        // check for a mailbox message
        CxxSockets::Message classNameMessage;
        string messageType;
        try {
            _mailbox->Receive(classNameMessage);
            messageType = classNameMessage.str();
        } catch (const CxxSockets::SoftError& e) {
            LOG_DEBUG_MSG( "connection interrupted: " << e.what() );
            continue;
        } catch (const CxxSockets::Error& e) {
            if (std::string(e.what()).find("Success") == std::string::npos) {
                LOG_ERROR_MSG(e.what());
            } else {
                LOG_INFO_MSG("connection closed");
            }
            break;
        }

        // receive and process a mailbox message
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
                LOG_ERROR_MSG( "connection failed: " << e.what() );
                break;
            }

            // Responded to the RAS message.  Now we need to do post-processing and barrier acking.
            if (_blockController->doBarrierAck()) {
                MCServerRef* temp;
                mmcs_client::CommandReply reply;
                BlockControllerBase::mcserver_connect(temp, _blockController->_userName, reply);
                boost::scoped_ptr<MCServerRef> ref( temp );

                if (reply.getStatus()) {
                    // could not connect to mc_server, nothing else we can do
                } else {
                    LOG_DEBUG_MSG("connected to mcServer");
                    const BlockPtr p = _blockController->shared_from_this();
                    const BlockControllerTarget target(p, "{*}", reply);
                    _blockController->makeAndOpenTargetSet(ref.get(), true, reply, &target);

                    BarrierAckRequest ackreq(_blockController->_blockName);
                    BarrierAckReply ackrep;
                    ref->barrierAck(ackreq, ackrep);
                    _blockController->doBarrierAck(false);
                }
            }
            if (recid) {
                _blockController->getHelper()->postProcessRASMessage(recid);
            }
        } else if (messageType == ConsoleMessage::getClassName()) {
            ConsoleMessage consoleMessage;
            CxxSockets::Message msg;
            try {
                _mailbox->Receive(msg);

                std::istringstream is(msg.str());
                consoleMessage.read(is);
            } catch(const CxxSockets::Error& e) {
                LOG_ERROR_MSG( "failure receiving console message" << e.what() );
                break;
            }
            processConsoleMessage(consoleMessage);
        } else {
            // if it wasn't a RAS or Console message, quit
            LOG_ERROR_MSG("unsupported message type " << messageType);
            break;
        }
    }

    // terminate
    LOG_INFO_MSG("stopping");
    return NULL;
}

int
SecureMailboxListener::processRASMessage(
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
    return _blockController->getHelper()->processRASMessage(rasEvent);
}

void
SecureMailboxListener::processConsoleMessage(
        ConsoleMessage& consoleMessage
        )
{
    _blockController->getHelper()->processConsoleMessage(consoleMessage);
}

} } // namespace mmcs::server
