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
 * \file MMCSCommand_ionodes.cc
 */

#include "MMCSCommand_ionodes.h"
#include "MMCSCommandReply.h"
#include "BlockControllerTarget.h"
#include "BlockControllerNodeInfo.h"
#include "MMCSProperties.h"
#include <control/include/mcServer/MCServerRef.h>

/*!
** sysrq
** [<target>] sysrq [option]
** Sends a sysrq command to the I/O node
** Options:
** loglevel0-8 reBoot tErm Halt kIll showMem showPc unRaw Sync showTasks Unmount Xmon
*/
MMCSCommand_sysrq*
MMCSCommand_sysrq::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(true);        // does require  mc_server connections
    commandAttributes.requiresTarget(true);            // does require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(ADMIN);
    MMCSCommandAttributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new MMCSCommand_sysrq("sysrq", "[<target>] sysrq [options]", commandAttributes);
}

void
MMCSCommand_sysrq::execute(deque<string> args,
			   MMCSCommandReply& reply,
			   ConsoleController* pController,
			   BlockControllerTarget* pTarget)
{
    string text;

    if (pTarget->getNodes().size() == 0)
    {
        reply << FAIL << "no targets selected" << DONE;
        return;
    }

    // magic sysrq prefix (^O)
    // For now sysrq is a magic write_con with this as a prefix.
    // We could change to a special mbox message code if necessary.
    text += "\017";
    for (unsigned i = 0; i < args.size(); ++i)
    {
        text += args[i];
        if (i != args.size() - 1)
            text += " ";
    }

    BlockPtr pBlock = pTarget->getBlockController();	// get selected block

    for (unsigned i = 0; i < pTarget->getNodes().size(); ++i)
    {
        BCNodeInfo *nodeInfo = pTarget->getNodes()[i];
	if (pBlock->isIOBlock())
	{
	    if (nodeInfo->_state == NST_PROGRAM_RUNNING)
	    {
		//
		// Send a MailboxRequest to the I/O node
		//
		MCServerMessageSpec::MailboxRequest mcMailboxRequest;
		mcMailboxRequest._text = text;
		mcMailboxRequest._location.push_back(nodeInfo->location());
		MCServerMessageSpec::MailboxReply   mcMailboxReply;
		try
		{
		    pBlock->getMCServer()->mailbox(mcMailboxRequest, mcMailboxReply);
		}
		catch (exception &e)
		{
		    mcMailboxReply._rc = -1;
		    mcMailboxReply._rt = e.what();
		}
		catch (XML::Exception &e)
		{
		    ostringstream buf; buf << e;
		    mcMailboxReply._rc = -1;
		    mcMailboxReply._rt = buf.str();
		}
		if (mcMailboxReply._rc)
		{
		    reply << FAIL << mcMailboxReply._rt << DONE;
		    return;
		}
	    }
	    else
	    {
		reply << FAIL << "no program running" << DONE;
		return;
	    }
	}
	else
	{
	    reply << FAIL << "not an I/O node" << DONE;
	    return;
	}
    }
    reply << OK << DONE;
}

void
MMCSCommand_sysrq::help(deque<string> args,
			MMCSCommandReply& reply)
{
    reply << OK << description()
	  << ";Sends a sysrq command to the I/O node"
	  << ";Options:"
	  << ";    loglevel0-8 reBoot tErm Halt kIll showMem showPc unRaw Sync showTasks Unmount Xmon"
	  << DONE;

}

/*!
** write_con
** [<target>] write_con <console-command>
** Send <console-command> to target node for execution.
** Output will be returned to mailbox (either console or I/O node log).
*/
MMCSCommand_write_con*
MMCSCommand_write_con::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);        // does require  mc_server connections
    commandAttributes.requiresTarget(true);            // does require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(ADMIN);
    MMCSCommandAttributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new MMCSCommand_write_con("write_con", "[<target>] write_con <console-command>", commandAttributes);
}

void
MMCSCommand_write_con::execute(deque<string> args,
			       MMCSCommandReply& reply,
			       ConsoleController* pController,
			       BlockControllerTarget* pTarget)
{
    string text;

    if (pTarget->getNodes().size() == 0)
    {
        reply << FAIL << "no targets selected" << DONE;
        return;
    }

    for (unsigned i = 0; i < args.size(); ++i)
    {
        text += args[i];
        if (i != args.size() - 1)
            text += " ";
    }

    BlockPtr pBlock = pTarget->getBlockController();	// get selected block

    bool wasconnected = true;
    if(pBlock->isConnected() == false) {
        wasconnected = false;
        // Need to connect
        std::deque<string> pargs;
        pargs.push_back(MMCSProperties::getProperty(DFT_TGTSET_TYPE));
        pBlock->connect(pargs, reply, pTarget);
    }

    for (unsigned i = 0; i < pTarget->getNodes().size(); ++i)
    {
        BCNodeInfo *nodeInfo = pTarget->getNodes()[i];

        if (!nodeInfo->_iopos.trainOnly()) {

            //
            // Send a MailboxRequest to the node
            //
            MCServerMessageSpec::MailboxRequest mcMailboxRequest;
            mcMailboxRequest._text = text;
            mcMailboxRequest._location.push_back(nodeInfo->location());
            MCServerMessageSpec::MailboxReply   mcMailboxReply;
            pBlock->getMCServer()->mailbox(mcMailboxRequest, mcMailboxReply);
            if (mcMailboxReply._rc)
                {
                    reply << FAIL << mcMailboxReply._rt << DONE;
                    return;
                }
        }
    }
    if(!wasconnected) {
        // Now disconnect since we had to connect
        std::deque<std::string> args;
        args.push_back("no_shutdown");
        pBlock->disconnect(args);
    }

    reply << OK << DONE;
}

void
MMCSCommand_write_con::help(deque<string> args,
			    MMCSCommandReply& reply)
{
    reply << OK << description()
	  << ";Send <console-command> to target node for execution."
	  << ";Output will be returned asynchronously to mailbox (either console or I/O node log)."
	  << DONE;

}

MMCSCommand_wc*
MMCSCommand_wc::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(true);        // does require  mc_server connections
    commandAttributes.requiresTarget(true);            // does require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(ADMIN);
    MMCSCommandAttributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new MMCSCommand_wc("wc", "[<target>] write_con <console-command>", commandAttributes);
}


