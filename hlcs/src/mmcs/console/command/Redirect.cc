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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#include "Redirect.h"

#include "../RedirectMonitorThread.h"

#include <signal.h>

using namespace std;

namespace mmcs {
namespace console {
namespace command {

void
Redirect::redirect(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        server::BlockControllerTarget* pTarget
        )
{
    if (args.size() < 2) {
        reply << mmcs_client::FAIL << "args?" << mmcs_client::DONE;
        return;
    } else if (args.size() > 3) {
        reply << mmcs_client::FAIL << "args?" << mmcs_client::DONE;
        return;
    }

    if (args[1] != "on" && args[1] != "off") {
        reply << mmcs_client::FAIL << "args?" << mmcs_client::DONE;
        return;
    }

    bool stdout = true;
    if (args.size() == 3) {
        if (args[2] == "stderr")
            stdout = false;
        else if(args[2] == "stdout") {
            stdout = true;
        } else {
            reply << mmcs_client::FAIL << "args?" << mmcs_client::DONE;
            return;
        }
    }

    if (args[1] == "on") {
        if (RedirectMonitorThread::redirectMonitorThread) { // is this console already redirecting?
            RedirectMonitorThread* temp = RedirectMonitorThread::redirectMonitorThread;
            RedirectMonitorThread::redirectMonitorThread->stop(SIGUSR1);
            delete temp;
            RedirectMonitorThread::redirectMonitorThread = NULL;
        }

        // Start the redirection thread
        if (RedirectMonitorThread::redirectInitMutex.Lock() < 0) {
            reply << mmcs_client::FAIL << "Internal error: pthread_mutex_lock: " << strerror(errno) << mmcs_client::DONE;
            return;
        }

        RedirectMonitorThread* my_redirectMonitorThread = new RedirectMonitorThread;
        boost::shared_ptr<RedirectMonitorThread::Parms> parms(
                new RedirectMonitorThread::Parms(pController, args[0], &reply)
                );
        my_redirectMonitorThread->setArg(parms);
        my_redirectMonitorThread->setJoinable(true);
        my_redirectMonitorThread->start();
        my_redirectMonitorThread->stdout = stdout;
        // Wait for the redirection thread to initialize
        // The reply is set by the redirection thread
        if (pthread_cond_wait(&RedirectMonitorThread::redirectInitCond, &RedirectMonitorThread::redirectInitMutex.GetRawMutex()) < 0)
        {
            reply << mmcs_client::FAIL << "Internal error: pthread_cond_wait: " << strerror(errno) << mmcs_client::DONE;
            RedirectMonitorThread::redirectInitMutex.Unlock();
            return;
        }
        RedirectMonitorThread::redirectInitMutex.Unlock();
    } else if (args[1] == "off") {
        if (RedirectMonitorThread::redirectMonitorThread != NULL) {
            // If the user asks to turn off redirection on a block we're not redirecting,
            // we'll fail and generate an error.
            boost::shared_ptr<RedirectMonitorThread::Parms> p;
            RedirectMonitorThread::redirectMonitorThread->getArg( p );
            if (p->blockName != args[0]) {
                reply << mmcs_client::FAIL << "Not redirecting " << args[0] << ". Redirecting "
                    << p->blockName << mmcs_client::DONE;
            } else {
                const RedirectMonitorThread* temp = RedirectMonitorThread::redirectMonitorThread;
                RedirectMonitorThread::redirectMonitorThread->force_end = true;
                RedirectMonitorThread::redirectMonitorThread->stop(SIGUSR1);
                delete temp;
                RedirectMonitorThread::redirectMonitorThread = NULL;
                reply << mmcs_client::OK << mmcs_client::DONE;
            }
        } else {
            reply << mmcs_client::FAIL << "Not redirected in this console" << mmcs_client::DONE;
        }
    } else {
        reply << mmcs_client::FAIL << "args?" << mmcs_client::DONE;
    }
}

Redirect*
Redirect::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.bgConsoleCommand(true);
    commandAttributes.helpCategory(common::USER);              // 'help user'  will include this command's summary
    // This stays a console command and thus has no security
    // attributes of its own.  The server version handles security.
    Attributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new Redirect("redirect", "redirect <blockId> [on|off] [stdout|stderr]", commandAttributes);
}

void
Redirect::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        server::BlockControllerTarget* pTarget
        )
{
    redirect(args, reply, pController, pTarget);
    if ( reply.getStatus() ) {
        if ( reply.str() == "args?" ) {
            reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        }
    }
}

void
Redirect::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description()
        << ";Redirect I/O node output for the specified block back to the bg_console."
        << ";Directs subsequent mailbox output back to the socket connection that this command is received on."
        << ";Allocating or freeing the block will stop the mailbox redirection."
        << ";By default, redirection goes to standard out. The stdout|stderr option"
        << ";allows the user to optionally send the output to standard error"
        << mmcs_client::DONE;
}

} } } // namespace mmcs::console::command
