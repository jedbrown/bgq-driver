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

#include "BlockControllerBase.h"
#include "MMCSCommandReply.h"
#include "MMCSProperties.h"
#include "MailboxMonitor.h"
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

MailboxMonitor::MailboxMonitor(BlockControllerBase* blockController)
    : _mailboxRegistered(false),
      _blockController(blockController)
{
    // initialize mailbox listener
    _mailboxListener._blockController = _blockController;
    _mailboxListener._mailboxFd = -1;
    _mailboxListener._listenerFd = -1;
}

MailboxMonitor::~MailboxMonitor()
{
    if(isStarted() == true) {
        stop();
    }
}

void MailboxMonitor::start(MMCSCommandReply& reply)
{
    LOGGING_DECLARE_MBOX_BLOCK_ID_MDC;
    if (isStarted())	// already started?
	return;

    //    static const size_t thread_stacksize = 2 * 1024 * 1024; // thread stack size for Mailbox thread

    //
    // listen on an ephemeral port for a connection from the mcServer mailbox monitor
    //

    char buff[8];
    bzero(buff, 8);

    int listenerFd = TCPcreateListener(0, BlockControllerBase::_local_addr.sin_addr.s_addr);
    if (listenerFd == -1)
    {
	reply << FAIL << "MailboxMonitor failed to register listener socket" << DONE;
	return;
    }

    //
    // get the port number that we are listening on
    //
    struct sockaddr_in myAddr;
    socklen_t myAddr_len = sizeof(myAddr);
    int rc = getsockname(listenerFd, (struct sockaddr*) &myAddr, &myAddr_len);
    if (rc != 0)
    {
        reply << FAIL << "MailboxMonitor failed to get socket address: " << strerror(errno) << DONE;
        close(listenerFd);
        return;
    }

    // start the MailboxListener thread
    _mailboxListener._listenerFd = listenerFd;
    //    _mailboxListener.setStacksize(thread_stacksize);
    _mailboxListener.setJoinable(true);
    _mailboxListener.start();
    //
    // send a RegisterEventListener request with our port number to the mcServer
    //
    std::ostringstream listenerAddr;
    int port = ntohs(myAddr.sin_port);
    char host_buf[NI_MAXHOST];
    int rc2 = getnameinfo((sockaddr*)(&myAddr), sizeof(sockaddr_storage), host_buf, sizeof(host_buf),
                         0, 0, NI_NUMERICHOST);
    if(rc2 != 0) {
        reply << FAIL << "Could not get listening socket name info: " << strerror(errno) << DONE;
        close(listenerFd);
        return;
    }

    listenerAddr << host_buf << ":" << port;
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
	    reply << FAIL << "MailboxMonitor failed to register event listener: "
		  << mcRegisterEventListenerReply._rt << DONE;
	    close(listenerFd);
	    return;
	}
	_mailboxRegistered = true;
	reply << OK << DONE;
    }
    catch (exception& e)
    {
	reply << FAIL << e.what() << DONE;
	close(listenerFd);
	return;
    }
    LOG_INFO_MSG("Event listener registered");
}

void MailboxMonitor::stop()
{
    _mailboxListener.stop(SIGUSR1); // stop the mailbox listener

    // Stop the mailbox listener from reading any more RAS
    // before we send a close request.  This will allow us
    // to continue to send any ras acks.
    ::shutdown(_mailboxListener._mailboxFd, SHUT_RD);

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
                LOG_ERROR_MSG("MailboxMonitor failed to close event listener: "
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
MailboxListener::threadStart()
{
    LOGGING_DECLARE_MBOX_BLOCK_ID_MDC;
    string messageType;		// mailbox message type

    LOG_INFO_MSG("MailboxListener starting");
    _mailboxFd = -1;		// no mailbox connection yet

    //
    // wait for a connection from the mcServer
    //

    // mailboxPoller returns 1 when data is available on the socket,
    // or -1 when the thread is being cancelled
    int poll_rc;
    try {
        while ((poll_rc = mailboxPoller(_listenerFd)) == 0);
    } catch (PollerException& e) {
        LOG_DEBUG_MSG("Caught mailbox poller exception waiting for call back from mc_server."
                      << " Ending listener.");
        return NULL;
    }

    if (poll_rc > 0)		// data is availble to be read
    {
	_mailboxFd = TCPaccept(_listenerFd);
	if (_mailboxFd == -1)
	    LOG_ERROR_MSG("MailboxListener failed to accept connection: " << strerror(errno));
    }

    // listener socket is not needed after accept -- close it
    close(_listenerFd);
    _listenerFd = -1;

    // quit if we did not receive a mailbox connection
    if (_mailboxFd == -1)
	return NULL;

    //
    // receive and process mailbox messages from the mcServer
    //

    // create the streams for request and reply
    XML::IOstream mailboxMessageStream(_mailboxFd);//, false);

    // register a callback routine for the XML::IOstream receive function
    // to enable us to interrupt an input operation when the MailboxMonitor
    // is being terminated
    PollInterruptCallback mailboxCallback = mailboxPoller;
    mailboxMessageStream.setPollInterruptCallback(mailboxCallback);
    
    // loop until MailboxMonitor::stop() is called
    while (1)
    {
        try {
            // check for a mailbox message
            messageType = mailboxMessageStream.getClassName();
            if (isThreadStopping())	// check for thread cancellation
                break;

            if (mailboxMessageStream.fail())
                {
                    LOG_ERROR_MSG("MailboxListener: connection failed: " << strerror(errno));
                    break;
                }
            // receive and process a mailbox message
            if (messageType == RASMessage::getClassName())
                {
                    RASMessage rasMessage;
                    if(!mailboxMessageStream.readObject(rasMessage))
                        {
                            LOG_ERROR_MSG("MailboxListener: failure receiving RAS message");
                            break;
                        }
                    int recid = processRASMessage(rasMessage);
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
                    if(!mailboxMessageStream.readObject(consoleMessage))
                        {
                            LOG_ERROR_MSG("MailboxListener: failure receiving console message");
                            break;
                        }
                    if(!isThreadStopping())
                        processConsoleMessage(consoleMessage);
                }
            else		       // if it wasn't a RAS or Console message, quit
                {
                    LOG_ERROR_MSG("MailboxListener: unsupported message type " << messageType);
                    break;
                }
        } catch (PollerException& e) {
            LOG_DEBUG_MSG("Caught mailbox poller exception.  Must stop polling.");
            break;
        }
    }

    // Close the mailbox connection
    _mailboxFd = -1;

    // terminate
    LOG_INFO_MSG("MailboxListener stopping");
    return NULL;
}

int MailboxListener::processRASMessage(RASMessage& rasMessage)
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

void MailboxListener::processConsoleMessage(MCServerMessageSpec::ConsoleMessage& consoleMessage)
{
    LOGGING_DECLARE_MBOX_BLOCK_ID_MDC;
    _blockController->getHelper()->processConsoleMessage(consoleMessage);
}

int MailboxListener::mailboxPoller(int fd)
{
    static const int poll_timeout = 3000;	// 3-second poll timeout
    int fdsReady = 0;		                // return code

    // get the MailboxListener object from thread-specific storage
    MMCSThread* this_p = MailboxListener::getMyThread();
    if (this_p == NULL)
    {
	LOG_FATAL_MSG("internal failure in MailboxPoller");
	return -1;
    }

    // Initialize the read set.
    struct pollfd rfds = { fd, POLLIN, 0 };
    int retries =8;
    int usecs = 500;
    // Check descriptors for data available.
    while (this_p->isThreadStopping() == false)
    {
	fdsReady = poll(&rfds, 1, poll_timeout);
	if (fdsReady == -1)
	{
          if ((errno == EINTR || errno == EAGAIN) && retries > 0) {
            if(this_p->isThreadStopping()) {
              LOG_INFO_MSG("poll errored with " << strerror(errno) << " on fd " << fd << " thread stopping.");
              break;
            }
            --retries;
            usleep(usecs);
            usecs *= 10;
            continue;
          }
          else
            LOG_ERROR_MSG("MailboxPoller: " << strerror(errno));
        }
	break;
    }

    if (this_p->isThreadStopping()) {// are we being terminated?
        LOG_DEBUG_MSG("Terminating mailbox polling because thread is ending");
	fdsReady = -1;
        throw PollerException(0, "mailbox thread ending");
    }

    return fdsReady;
}
