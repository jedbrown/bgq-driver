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
 * \file MMCSCommand_redirect.cc
 */

#include "MMCSCommand_redirect.h"
#include "MMCSCommandReply.h"
#include "MMCSConsolePort.h"
#include "DBConsoleController.h"
#include "DBBlockController.h"

using namespace std;

// common function called by redirect and redirect_block
static void
redirect_block(deque<string> args,
	       MMCSCommandReply& reply,
	       DBConsoleController* pController)
{
    if (args.size() == 0)
    {
	reply << FAIL << "args?" <<  DONE;
	return;
    }
    if (args[0] != "on" && args[0] != "off") {
	reply << FAIL << "args?" <<  DONE;
	return;
    }
    DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper());	// get selected block
    if (!pBlock->getBase()->isConnected())
    {
        std::deque<std::string> a;
        a.push_back("mode=monitor");
        std::string targspec = "{*}";
        BlockControllerTarget* target = new BlockControllerTarget(pBlock->getBase(), targspec, reply);
        pBlock->getBase()->connect(a, reply, target);
        if(reply.getStatus() != 0) {
            return;
        }
    }
    if (args[0] == "on")
    {
	if (pController->getRedirecting() || // is this DBConsoleController already redirecting?
	    pBlock->getBase()->getRedirectSock() != 0)    // is the DBBlockController already redirecting?
	{
	    reply << FAIL << "block is already redirected in another process" << DONE;
	    return;
	}
	pBlock->getBase()->startRedirection(std::tr1::static_pointer_cast<CxxSockets::SecureTCPSocket>(pController->getConsolePort()->getSock()), reply);
	if (reply.getStatus() != 0)
	    return;
	pController->setRedirecting(true);
    }
    else
    {
	if (!pController->getRedirecting())		// is this DBConsoleController redirecting?
	{
	    reply << FAIL << "client not redirecting" << DONE;
	    return;
	}
	pBlock->getBase()->stopRedirection(reply);
        if(pBlock->getBase()->isConnected() && pBlock->getBase()->isIOBlock()) {
            std::deque<std::string> a;
            a.push_back("no_shutdown");
            pBlock->getBase()->disconnect(a);
        }
	pController->setRedirecting(false);
    }
    reply << OK << DONE;
}

MMCSCommand_redirect*
MMCSCommand_redirect::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(USER);              // 'help user'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new MMCSCommand_redirect("redirect", "redirect <blockid> on|off", commandAttributes);
}


void
MMCSCommand_redirect::execute(deque<string> args,
			      MMCSCommandReply& reply,
			      DBConsoleController* pController,
			      BlockControllerTarget* pTarget,
                              std::vector<std::string>* validnames)
{
    return execute(args, reply, pController, pTarget);
}

void
MMCSCommand_redirect::execute(deque<string> args,
			      MMCSCommandReply& reply,
			      DBConsoleController* pController,
			      BlockControllerTarget* pTarget)
{
    if (args.size() != 2)
    {
	reply << FAIL << "args? " << usage <<  DONE;
	return;
    }
    pController->selectBlock(args, reply, true);
    if (reply.getStatus() != 0)
	return;
    args.pop_front();		// remove block name from args
    redirect_block(args, reply, pController);
    if(reply.str() == "args?")
      reply << FAIL << "args? " << usage <<  DONE;
}

void
MMCSCommand_redirect::help(deque<string> args,
			   MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
	  << ";Redirect I/O node output for the specified block to the mmcs console."
	  << ";Directs subsequent mailbox output back to the socket connection that this command is received on."
	  << ";Allocating or freeing the block will stop the mailbox redirection."
	  << DONE;
}

MMCSCommand_redirect_block*
MMCSCommand_redirect_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // command is for internal use
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(USER);              // 'help user'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new MMCSCommand_redirect_block("redirect_block", "redirect_block on|off", commandAttributes);
}


void
MMCSCommand_redirect_block::execute(deque<string> args,
			      MMCSCommandReply& reply,
			      DBConsoleController* pController,
			      BlockControllerTarget* pTarget)
{
    redirect_block(args, reply, pController);
    if(reply.str() == "args?")
      reply << FAIL << "args? " << usage <<  DONE;
}

void
MMCSCommand_redirect_block::help(deque<string> args,
				 MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
	  << ";Redirect I/O node output for the selected block to the mmcs console."
	  << ";Directs subsequent mailbox output back to the socket connection that this command is received on."
	  << ";Allocating or freeing the block will stop the mailbox redirection."
	  << DONE;
}
