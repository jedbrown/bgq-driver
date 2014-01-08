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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#include "MMCSCommand_barrier.h"

#include "MMCSCommandReply.h"
#include "BlockHelper.h"
#include "ConsoleController.h"
#include "MMCSCommand_lite.h"

#include "utility/include/Log.h"

LOG_DECLARE_FILE( "mmcs.command" );

MMCSCommand_show_barrier*
MMCSCommand_show_barrier::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);            // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new MMCSCommand_show_barrier("show_barrier", "show_barrier", commandAttributes);
}


void
MMCSCommand_show_barrier::execute(deque<string> args,
				  MMCSCommandReply& reply,
				  ConsoleController* pController,
				  BlockControllerTarget* pTarget)
{
    BlockPtr pBlock = pController->getBlockBaseController();	// get selected block
    pBlock->show_barrier(reply);
    if(reply.str() == "args?")
      reply << FAIL << "args? " << usage << DONE;
}

void
MMCSCommand_show_barrier::help(deque<string> args,
			       MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
	  << ";show nodes with unsatisfied control system barrier bits in the selected block"
	  << DONE;
}
