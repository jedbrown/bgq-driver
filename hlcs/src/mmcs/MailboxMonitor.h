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
 * \file MailboxMonitor.h
 */

#ifndef MAILBOX_MONITOR_H_
#define MAILBOX_MONITOR_H_

#include <boost/utility.hpp>
#include "MMCSThread.h"
#include <stdexcept>

class BlockController;
class MMCSCommandReply;

namespace MCServerMessageSpec
{
    class RASMessage;
    class ConsoleMessage;
}

class PollerException: public std::runtime_error {
public:
    int errcode;
    PollerException(int err=0, const char* what="") : std::runtime_error(what), errcode(err) {}
};

/*!
 * \brief Receive and process MailboxMessages from a block of nodes.
 */
class MailboxListener : public MMCSThread
{
public:
    friend class MailboxMonitor;
    MailboxListener() : MMCSThread() {};
    virtual ~MailboxListener() { /* Nothing to do */ }
    void* threadStart();
private:
    int  _listenerFd;		        // socket to wait for a connection from mcServer
    CxxSockets::ListeningSocketPtr _listener;
    int  _mailboxFd;			// socket that MailboxMessages are received on
    CxxSockets::SecureTCPSocketPtr _mailbox;
    BlockControllerBase* _blockController;	// associated block provided to constructor
    int processRASMessage(MCServerMessageSpec::RASMessage& rasMessage);
    void processConsoleMessage(MCServerMessageSpec::ConsoleMessage& consoleMessage);
    static int mailboxPoller(int fd);	// poll for messages on the MailboxListener connection
    static int mailboxPoller(CxxSockets::ListeningSocketPtr& listener);	// poll for messages on the MailboxListener connection
};

/*!
 *
 * \brief Monitor the mailboxes for nodes within a block.
 */
class MailboxMonitor : private boost::noncopyable
{
public:
    /** \brief Start listening for Mailbox messages
     */
    void start(MMCSCommandReply& reply);

    /** \brief Stop listening for Mailbox messages
     */
    void stop();

    /** \brief Return true if we are listening for Mailbox messages */
    bool isStarted() { return _mailboxListener.getThreadId() != 0; }

    /** \brief Construct a mailbox monitor.
     */
    MailboxMonitor(BlockControllerBase* blockController);

    /** \brief The destructor will automatically stop listening. */
    ~MailboxMonitor();

protected:
    MailboxMonitor() {}  // Only the inheriting SecureMailboxMonitor should call this.
    MailboxListener     _mailboxListener;       // thread to receive and process mailbox messages

protected:
    bool		_mailboxRegistered;     // did we succesfully register a mailbox listener?
    BlockControllerBase*    _blockController;       // associated block provided to constructor
};

#endif
