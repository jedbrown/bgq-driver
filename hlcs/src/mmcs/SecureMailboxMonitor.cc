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

#include "BlockControllerBase.h"
#include "MMCSCommandReply.h"
#include "MMCSProperties.h"
#include "SecureMailboxMonitor.h"
#include <signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <errno.h>

#include <bgq_util/include/TCPlisten.h>

#include <control/include/mcServer/MCServerRef.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>

using namespace std;
using namespace MCServerMessageSpec;


LOG_DECLARE_FILE( "mmcs" );
#define LOGGING_DECLARE_MBOX_BLOCK_ID_MDC \
    log4cxx::MDC _blockid_mbox_mdc_( "blockId", std::string("{") + _blockController->getBlockName() + "} " );

SecureMailboxMonitor::SecureMailboxMonitor(BlockControllerBase* blockController)
    : _mailboxRegistered(false),
      _blockController(blockController)
{
    // initialize mailbox listener
    _mailboxListener._blockController = _blockController;
    _mailboxListener._mailboxFd = -1;
    _mailboxListener._listenerFd = -1;
}

SecureMailboxMonitor::~SecureMailboxMonitor()
{
    if(isStarted() == true) {
        stop();
    }
}

void SecureMailboxMonitor::start(MMCSCommandReply& reply)
{
    LOGGING_DECLARE_MBOX_BLOCK_ID_MDC;
    if (isStarted())	// already started?
	return;

    //    static const size_t thread_stacksize = 2 * 1024 * 1024; // thread stack size for SecureMailbox thread

    //
    // listen on an ephemeral port for a connection from the mcServer mailbox monitor
    //

    using namespace CxxSockets;
    std::ostringstream listenerAddr;

    try {
        CxxSockets::SockAddr lsa(BlockControllerBase::_local_saddr.family(), 
                                 BlockControllerBase::_local_saddr.getHostAddr(), "0"); 
        
        ListeningSocketPtr ln(new ListeningSocket(lsa, SOMAXCONN, SECURE));

        SockAddr sa;
        ln->getSockName(sa);

        // start the SecureMailboxListener thread
        _mailboxListener._listener = ln;
        //    _mailboxListener.setStacksize(thread_stacksize);
        _mailboxListener.setJoinable(true);
        _mailboxListener.start();

        int port = sa.getServicePort();
        std::string host = sa.getHostAddr();
        listenerAddr << host << ":" << port;
        LOG_DEBUG_MSG("Listening on " << listenerAddr.str() << " for mc_server return connection.");
    } catch (CxxError& e) {
        std::ostringstream errmsg;
        errmsg << "SecureMailboxMonitor failed to register listener socket.  " << e.what();
        LOG_ERROR_MSG(errmsg.str());
        reply << FAIL << errmsg.str() << DONE;
        return;
    }

    //
    // send a RegisterEventListener request with our port number to the mcServer
    //
    try
    {
	RegisterEventListenerRequest mcRegisterEventListenerRequest(_blockController->getBlockName(),listenerAddr.str(),
								    false, atoi(MCServerMessageSpec::VERSION.c_str()));
	RegisterEventListenerReply mcRegisterEventListenerReply;
        if(!_blockController->hardWareAccessBlocked())
            _blockController->_mcServer->registerEventListener(mcRegisterEventListenerRequest, mcRegisterEventListenerReply);
        else {
            mcRegisterEventListenerReply._rc = -1;
            mcRegisterEventListenerReply._rt = "SubnetMc temporarily unavailable.";
        }

	if (mcRegisterEventListenerReply._rc)
	{
	    reply << FAIL << "SecureMailboxMonitor failed to register event listener: "
		  << mcRegisterEventListenerReply._rt << DONE;
	    return;
	}
	_mailboxRegistered = true;
	reply << OK << DONE;
    }
    catch (exception& e)
    {
	reply << FAIL << e.what() << DONE;
	return;
    }
}

void SecureMailboxMonitor::stop()
{
    _mailboxListener.stop(SIGUSR1);		// stop the mailbox listener

    // Stop the mailbox listener from reading any more RAS
    // before we send a close request.  This will allow us
    // to continue to send any ras acks.
    if(_mailboxListener._mailbox) {
        _mailboxListener._mailbox->Shutdown(CxxSockets::Socket::RECEIVE);
    } else return;
    //
    // send a CloseEventListener request to the mcServer
    //
    if (_mailboxRegistered)
    {
        LOGGING_DECLARE_MBOX_BLOCK_ID_MDC;
	try {
            string blockname = _blockController->getBlockName();
	    CloseEventListenerRequest mcCloseEventListenerRequest;
	    mcCloseEventListenerRequest._set = _blockController->getBlockName();
	    CloseEventListenerReply mcCloseEventListenerReply;
            _blockController->_mcServer->closeEventListener(mcCloseEventListenerRequest, mcCloseEventListenerReply);
            if (mcCloseEventListenerReply._rc)
                LOG_ERROR_MSG("SecureMailboxMonitor failed to close event listener: "
                              << mcCloseEventListenerReply._rt);

	}
	catch (exception &e)
	{
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

    LOG_INFO_MSG("SecureMailboxListener starting");

    // ignore termination signals -- let the foreground thread handle them
    sigemptyset(&newmask);
    for (int i = 0; i < num_signals; i++)
	sigaddset(&newmask, signals[i]);
    pthread_sigmask(SIG_BLOCK, &newmask, NULL);

    //
    // wait for a connection from the mcServer
    //
    CxxSockets::PollingListenerSetPtr listenerset(new CxxSockets::PollingListenerSet);
    listenerset->AddSock(_listener);
    CxxSockets::SecureTCPSocketPtr newsock(new CxxSockets::SecureTCPSocket);
    bool accepted = false;
    int count = 0;
    int accept_retries = 30;
    while(!accepted && count < accept_retries && !isThreadStopping()) {
        try {
            bgq::utility::ServerPortConfiguration port_config(0);
            port_config.setProperties(MMCSProperties::getProperties(), "");
            port_config.notifyComplete();
            accepted = listenerset->AcceptNew(newsock, port_config);
            _mailbox = newsock;
            if(accepted) {
                LOG_DEBUG_MSG("Accepted new connection from mcserver");
            } else {
                LOG_TRACE_MSG("Failed to accept new connection from mcserver in poll timeout");
            }
        } catch(CxxSockets::CxxError& e) {
            LOG_ERROR_MSG("SecureMailboxListener failed to accept connection: " << e.what());
        }
        ++count;
        sleep(1); // Snooze a bit.  This may take a while.
    }

    listenerset->RemoveSock(_listener);
    _listener.reset();

    if(!accepted)
        return NULL;

    //    unsigned readCount = 0;

    //
    // receive and process mailbox messages from the mcServer
    //
    // loop until SecureMailboxMonitor::stop() is called
    string messageType;		// mailbox message type
    while (1)
    {
        if (isThreadStopping())	// check for thread cancellation
            break;
        try {
            // check for a mailbox message
            CxxSockets::Message classNameMessage;
            try {
                _mailbox->Receive(classNameMessage);
                messageType = classNameMessage.str();
            } catch (CxxSockets::SockSoftError& e) {
                std::ostringstream errmsg;
                errmsg << "SecureMailboxListener: connection interrupted: " << e.what();
                LOG_INFO_MSG(errmsg.str());
                if(isThreadStopping())
                    break;
                else continue;  // We can keep going here.
            } catch (CxxSockets::SockHardError& e) {
                std::ostringstream errmsg;
                if(errmsg.str().find("Success") == std::string::npos) {
                    errmsg << "SecureMailboxListener: connection failed: " << e.what();
                    LOG_ERROR_MSG(errmsg.str());
                } else {
                    LOG_INFO_MSG("Connection closed");
                }
                break;
            }  catch (CxxSockets::CxxError& e) {
                std::ostringstream errmsg;
                errmsg << "SecureMailboxListener: connection failed: " << e.what();
                if(errmsg.str().find("Success") == std::string::npos) {
                    errmsg << "SecureMailboxListener: connection failed: " << e.what();
                    LOG_ERROR_MSG(errmsg.str());
                } else {
                    LOG_INFO_MSG("Connection closed");
                }
                break;
            }

            if (isThreadStopping())	// check for thread cancellation
                break;

            // ++readCount;
            // if ( readCount % 150 == 1 ) {
            //     LOG_DEBUG_MSG( "received message " << readCount );
            // }

            // receive and process a mailbox message
            if (messageType == RASMessage::getClassName())
                {
                    RASMessage rasMessage;
                    CxxSockets::Message msg;
                    int recid = 0;
                    try {
                        _mailbox->Receive(msg);

                        std::istringstream is(msg.str());
                        rasMessage.read(is);
                        recid = processRASMessage(rasMessage);
                    } catch (CxxSockets::CxxError& e) {
                        std::ostringstream errmsg;
                        errmsg << "SecureMailboxListener: connection failed: " << e.what();
                        LOG_ERROR_MSG(errmsg.str());
                        break;
                    }

                    // Responded to the RAS message.  Now we need to do post-processing and barrier acking.
                    if(_blockController->doBarrierAck()) {
                        MCServerRef* ref;
                        MMCSCommandReply reply;
                        BlockControllerBase::mcserver_connect(ref, _blockController->_userName, reply);
                        if (reply.getStatus() != 0)
                            {
                                deque<std::string> dq;
                                _blockController->disconnect(dq);
                                return false;
                            }
                        else
                            LOG_DEBUG_MSG("connected to mcServer");
                        BlockPtr p = _blockController->shared_from_this();
                        BlockControllerTarget target(p, "{*}", reply);
                        _blockController->makeAndOpenTargetSet(ref, true, reply, &target);

                        MCServerMessageSpec::BarrierAckRequest ackreq(_blockController->_blockName);
                        MCServerMessageSpec::BarrierAckReply ackrep;
                        ref->barrierAck(ackreq, ackrep);
                        delete ref;
                        _blockController->doBarrierAck(false);
                    }
                    if (recid != 0)
                        _blockController->getHelper()->postProcessRASMessage(recid);
                }
            else if (messageType == MCServerMessageSpec::ConsoleMessage::getClassName())
                {
                    MCServerMessageSpec::ConsoleMessage consoleMessage;
                    CxxSockets::Message msg;
                    try {
                        _mailbox->Receive(msg);

                        std::istringstream is(msg.str());
                        consoleMessage.read(is);
                    } catch(CxxSockets::CxxError& e) {
                        std::ostringstream errmsg;
                        errmsg << "SecureMailboxListener: failure receiving console message" << e.what();
                        break;
                    }
                    processConsoleMessage(consoleMessage);
                }
            else		       // if it wasn't a RAS or Console message, quit
                {
                    LOG_ERROR_MSG("SecureMailboxListener: unsupported message type " << messageType);
                    break;
                }
        } catch (PollerException& e) {
            LOG_DEBUG_MSG("Caught mailbox poller exception.  Must stop polling.");
            break;
        }
    }

    // terminate
    LOG_INFO_MSG("SecureMailboxListener stopping");
    return NULL;
}

int SecureMailboxListener::processRASMessage(RASMessage& rasMessage)
{
    LOGGING_DECLARE_MBOX_BLOCK_ID_MDC;
    // Build a RasEventImpl object from the rasMessage
    RasEventImpl rasEvent(rasMessage._msgId,
			  rasMessage._handled,
			  rasMessage._time_secs,
			  rasMessage._time_usecs,
			  rasMessage._keys,
			  rasMessage._values);

    // Invoke the RAS event handler chain
    RasEventHandlerChain::handle(rasEvent);

    // Log the RAS event in the database
    return _blockController->getHelper()->processRASMessage(rasEvent);
}

void SecureMailboxListener::processConsoleMessage(MCServerMessageSpec::ConsoleMessage& consoleMessage)
{
    LOGGING_DECLARE_MBOX_BLOCK_ID_MDC;
    _blockController->getHelper()->processConsoleMessage(consoleMessage);
}
