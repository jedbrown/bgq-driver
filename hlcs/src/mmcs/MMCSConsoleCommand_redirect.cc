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
/* (C) Copyright IBM Corp.  2005, 2011                              */
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
 * \file MMCSConsoleCommand_redirect.cc
 */

#include <signal.h>
#include <errno.h>
#include "ConsoleController.h"
#include "MMCSConsolePort.h"
#include "MMCSConsoleCommand_redirect.h"
#include "MMCSCommand_mmcs_server_connect.h"
#include "MMCSCommandReply.h"
#include "MMCSCommandProcessor.h"
#include "MMCSThread.h"
#include "MMCSProperties.h"

#include <bgq_util/include/pthreadmutex.h>

#include <utility/include/Log.h>
#include <utility/include/UserId.h>

#include <boost/scoped_ptr.hpp>

using namespace std;


LOG_DECLARE_FILE( "mmcs" );


class RedirectMonitorThread;	// forward reference

// Each mmcs console is allowed to redirect a single block
RedirectMonitorThread* redirectMonitorThread = NULL;

// Condition variable for synchronization between main thread and redirect thread
PthreadMutex    redirectInitMutex(PTHREAD_MUTEX_ERRORCHECK_NP);
pthread_cond_t redirectInitCond = PTHREAD_COND_INITIALIZER;
bool redirectInitComplete;

// Class for the redirect monitor thread
class RedirectMonitorThread : public MMCSThread
{
public:
    RedirectMonitorThread() : MMCSThread() { stdout = true; force_end = false; };
    void* threadStart();
    bool stdout;
    bool force_end;
};

// parameters passed to the redirect monitor thread
struct RedirectParms
{
    RedirectParms(
            const bgq::utility::UserId& user,
            string block,
            MMCSCommandMap* map,
            MMCSCommandReply* replyarg
            ) :
	user(user),
        blockName(block),
        mmcsCommands(map),
        reply(replyarg)
    {

    }
    bgq::utility::UserId user;
    string blockName;
    MMCSCommandMap* mmcsCommands;
    MMCSCommandReply* reply;
};

// Cleanup handler for the Redirect monitor thread
// arg is MMCSCommandReply, but is only valid while redirectInitComplete is false
void redirectCleanupHandler(void* arg)
{
    if (!redirectInitComplete)
    {
	MMCSCommandReply* reply = (MMCSCommandReply*) arg;
	if (reply && reply->getStatus() == MMCSCommandReply::STATUS_NOT_SET)
	    *reply << FAIL << "internal error: redirect thread initialization failed" << DONE;
	redirectInitComplete = true;
	pthread_cond_signal(&redirectInitCond);
	redirectInitMutex.Unlock();

    }
    redirectMonitorThread = NULL;
}

// Thread start function for the Redirect monitor thread
void*
RedirectMonitorThread::threadStart()
{
    std::string reply_str;
    auto_ptr<RedirectParms> parms((RedirectParms*) getArg());
    MMCSCommandReply& reply = *(parms->reply);
    reply.reset();
    bool errorend = false;  // If we get an error from the other side, don't send an 'off'.

    // coordinate the initialization with the main thread
    redirectInitMutex.Lock();
    redirectInitComplete = false;
    pthread_cleanup_push(redirectCleanupHandler, parms->reply);

    // Create a command processor
    MMCSCommandProcessor mmcsCommandProcessor(parms->mmcsCommands); // MMCS command processor
    mmcsCommandProcessor.logFailures(false); // don't clutter the console with log messages

    // Create a ConsoleController
    BlockHelperPtr p;
    CxxSockets::UserType utype = CxxSockets::Normal;
    auto_ptr<ConsoleController> pController(new ConsoleController(&mmcsCommandProcessor, parms->user, utype));

    deque<string> mmcs_redirect = MMCSCommandProcessor::parseCommand("mmcs_server_cmd redirect " + parms->blockName + " on");

    unsigned timeout = 2;	// two seconds

    // connect to the mmcs server
    deque<string> mmcs_connect;
    boost::scoped_ptr<MMCSCommand_mmcs_server_connect> conncmd( MMCSCommand_mmcs_server_connect::build() );
    conncmd->execute(mmcs_connect, reply, pController.get());

    if (reply.getStatus() != 0)
	goto done;

    // send the redirect command
    mmcsCommandProcessor.execute(mmcs_redirect, reply, pController.get());
    if (reply.getStatus() != 0)
	goto done;

    // notify main thread that we have completed setup
    reply << OK << DONE;
    redirectMonitorThread = this;
    redirectInitComplete = true;
    pthread_cond_signal(&redirectInitCond);
    redirectInitMutex.Unlock();

    // loop receiving and printing mailbox output
    while (isThreadStopping() == false)
    {
	try
	{
            pController->getConsolePort()->pollReceiveMessage(reply_str, timeout);
            if(reply_str.length() != 0) {
                if (reply_str[reply_str.length()-1] == '\n')
                    reply_str.resize(reply_str.length()-1);
                if(stdout) std::cout << reply_str << std::endl;
                else std::cerr << reply_str << std::endl;
            }
	}
        catch (MMCSConsolePort::Error& e) {
            errorend = true;  // Assume that the other side is done.
	    switch (e.errcode)
	    {
            case EAGAIN:
	    case EINTR:
                continue;
	    case ECONNRESET:
	    case ESPIPE:
	    case EPIPE:
	    case EBADF:
		LOG_INFO_MSG("Redirection terminating");
		goto done;
	    default:
		LOG_ERROR_MSG("Redirection terminating: " << e.what());
		goto done;
	    }
            LOG_ERROR_MSG("Redirection terminating: " << e.what());
            goto done;
        }
    }

    // return when done
 done:
    // Turn off redirect
    if(force_end) {
        MMCSCommandReply redirectOffReply;
        mmcs_redirect = MMCSCommandProcessor::parseCommand("mmcs_server_cmd redirect " + parms->blockName + " off");
        mmcsCommandProcessor.execute(mmcs_redirect, redirectOffReply, pController.get());
    }
    pthread_cleanup_pop(1);
    return NULL;
}

//
// common function used by redirect and redirect_block
//
static void
redirect(deque<string> args,
	 MMCSCommandReply& reply,
	 ConsoleController* pController,
	 BlockControllerTarget* pTarget)
{
    if (args.size() < 2)
    {
	reply << FAIL << "args?" <<  DONE;
	return;
    }
    else if (args.size() > 3)
    {
	reply << FAIL << "args?" <<  DONE;
	return;
    }
    if (args[1] != "on" && args[1] != "off") {
	reply << FAIL << "args?" <<  DONE;
	return;
    }

    bool stdout = true;
    if(args.size() == 3) {
        if(args[2] == "stderr")
            stdout = false;
        else if(args[2] == "stdout") {
            stdout = true;
        }
        else {
            reply << FAIL << "args?" <<  DONE;
            return;
        }
    }


    if (args[1] == "on")
    {
	if (redirectMonitorThread)  // is this console already redirecting?
	{
          RedirectMonitorThread* temp = redirectMonitorThread;
          redirectMonitorThread->stop(SIGUSR1);
          delete temp;
          redirectMonitorThread = NULL;
	}

        // Start the redirection thread
	if (redirectInitMutex.Lock() < 0)
	{
	    reply << FAIL << "internal error: pthread_mutex_lock: " << strerror(errno) << DONE;
	    return;
	}

	RedirectMonitorThread* my_redirectMonitorThread = new RedirectMonitorThread;
        RedirectParms* parms = new RedirectParms(pController->getUser(), args[0], pController->getCommandProcessor()->getCommandMap(), &reply);
        my_redirectMonitorThread->setArg(parms);
        my_redirectMonitorThread->setJoinable(true);
	my_redirectMonitorThread->start();
        my_redirectMonitorThread->stdout = stdout;
	// Wait for the redirection thread to initialize
	// The reply is set by the redirection thread
	if (pthread_cond_wait(&redirectInitCond, &redirectInitMutex.GetRawMutex()) < 0)
	{
	    reply << FAIL << "internal error: pthread_cond_wait: " << strerror(errno) << DONE;
	    redirectInitMutex.Unlock();
	    return;
	}
	redirectInitMutex.Unlock();
    }
    else if (args[1] == "off")
    {
	if (redirectMonitorThread != NULL)
          {
            // If the user asks to turn off redirection on a block we're not redirecting,
            // we'll fail and generate an error.
            RedirectParms* p = (RedirectParms*)(redirectMonitorThread->getArg());
            if(p->blockName != args[0]) {
              reply << FAIL << "Not redirecting " << args[0] << ". Redirecting "
                    << p->blockName << DONE;
            }
            else {
              RedirectMonitorThread* temp = redirectMonitorThread;
              redirectMonitorThread->force_end = true;
              redirectMonitorThread->stop(SIGUSR1);
              delete temp;
              redirectMonitorThread = NULL;
              reply << OK << DONE;
            }
          }
	else
	{
	    reply << FAIL << "not redirected in this console" << DONE;
	}
    }
    else
    {
	reply << FAIL << "args?" <<  DONE;
    }
}


//
// redirect output for the specified block
//
MMCSConsoleCommand_redirect*
MMCSConsoleCommand_redirect::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.helpCategory(USER);              // 'help user'  will include this command's summary
    // This stays a console command and thus has no security
    // attributes of its own.  The server version handles security.
    MMCSCommandAttributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    //    commandAttributes.internalAuth(true);
    return new MMCSConsoleCommand_redirect("redirect", "redirect <blockId> [on|off] [stdout|stderr]", commandAttributes);
}

void
MMCSConsoleCommand_redirect::execute(deque<string> args,
				     MMCSCommandReply& reply,
				     ConsoleController* pController,
				     BlockControllerTarget* pTarget)
{
    redirect(args, reply, pController, pTarget);
    if ( reply.getStatus() ) {
        if ( reply.str() == "args?" ) {
            reply << FAIL << "args? " << usage <<  DONE;
        }
    }
}

void
MMCSConsoleCommand_redirect::help(deque<string> args,
				  MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
	reply << OK << description()
	      << ";Redirect I/O node output for the specified block back to the mmcs console."
	      << ";Directs subsequent mailbox output back to the socket connection that this command is received on."
	      << ";Allocating or freeing the block will stop the mailbox redirection."
              << ";By default, redirection goes to standard out.  The stdout|stderr option"
              << ";allows the user to optionally send the output to standard error"
	      << DONE;
}

//
// redirect output for the selected block
//
MMCSConsoleCommand_redirect_block*
MMCSConsoleCommand_redirect_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // internal use only command
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.helpCategory(USER);              // 'help user'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new MMCSConsoleCommand_redirect_block("redirect_block", "redirect_block on|off stdout|stderr", commandAttributes);
}

void
MMCSConsoleCommand_redirect_block::execute(deque<string> args,
					   MMCSCommandReply& reply,
					   ConsoleController* pController,
					   BlockControllerTarget* pTarget)
{
    // get the currently selected block
    deque<string> list_selected_block = MMCSCommandProcessor::parseCommand("mmcs_server_cmd list_selected_block");
    pController->getCommandProcessor()->execute(list_selected_block, reply, pController);
    if (reply.getStatus() != 0) {
      if(reply.str() == "args?")
        reply << FAIL << "args? " << usage <<  DONE;
      return;
    }

    // add the name of the selected block to the front of the argument list
    args.push_front(reply.str());

    // redirect the block output
    redirect(args, reply, pController, pTarget);
      if(reply.str() == "args?")
        reply << FAIL << "args? " << usage <<  DONE;
}

void
MMCSConsoleCommand_redirect_block::help(deque<string> args,
				  MMCSCommandReply& reply)
{
    reply << OK << description()
	  << ";Redirect I/O node output for the selected block back to the mmcs console."
	  << ";Directs subsequent mailbox output back to the socket connection that this command is received on."
	  << ";Allocating or freeing the block will stop the mailbox redirection."
          << ";By default, redirection goes to standard out.  The stdout|stderr option"
          << ";allows the user to optionally send the output to standard error"
	  << DONE;
}
