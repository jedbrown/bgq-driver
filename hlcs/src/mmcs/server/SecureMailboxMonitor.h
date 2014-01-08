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
 * \file SecureMailboxMonitor.h
 */

#ifndef MMCS_SERVER_SECURE_MAILBOX_MONITOR_H_
#define MMCS_SERVER_SECURE_MAILBOX_MONITOR_H_


#include "common/Thread.h"
#include "types.h"

#include "libmmcs_client/CommandReply.h"

#include <utility/include/cxxsockets/types.h>

#include <xml/include/c_api/MCServerMessageSpec.h>

#include <boost/utility.hpp>

#include <stdexcept>


namespace mmcs {
namespace server {


/*!
 * \brief Receive and process SecureMailboxMessages from a block of nodes.
 */
class SecureMailboxListener : public common::Thread
{
public:
    friend class SecureMailboxMonitor;
    SecureMailboxListener() : Thread() {};
    void* threadStart();
private:
    CxxSockets::ListeningSocketPtr _listener;
    CxxSockets::SecureTCPSocketPtr _mailbox;
    BlockControllerBase* _blockController;	// associated block provided to constructor
    int processRASMessage(MCServerMessageSpec::RASMessage& rasMessage);
    void processConsoleMessage(MCServerMessageSpec::ConsoleMessage& consoleMessage);
};

/*!
 * \brief Monitor the mailboxes for nodes within a block.
 */
class SecureMailboxMonitor : private boost::noncopyable
{
public:
    /** \brief Start listening for SecureMailbox messages
     */
    void start(mmcs_client::CommandReply& reply);

    /** \brief Stop listening for SecureMailbox messages
     */
    void stop();

    /** \brief Return true if we are listening for SecureMailbox messages */
    bool isStarted() { return _mailboxListener.getThreadId() != 0; }

    /** \brief Construct a mailbox monitor.
     */
    SecureMailboxMonitor(BlockControllerBase* blockController);

    /** \brief The destructor will automatically stop listening. */
    ~SecureMailboxMonitor();

protected:
    SecureMailboxListener     _mailboxListener;       // thread to receive and process mailbox messages

protected:
    bool		_mailboxRegistered;     // did we succesfully register a mailbox listener?
    BlockControllerBase*    _blockController;       // associated block provided to constructor
};

} } // namespace mmcs::server

#endif
