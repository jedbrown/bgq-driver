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

#include "MMCSServerCommand_disconnect.h"
#include "DBBlockController.h"

/*!
//  disconnect -- disconnect from the block's resources
*/
MMCSServerCommand_disconnect*
MMCSServerCommand_disconnect::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(false);
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(ADMIN);              // 'help user'  will include this command's summary
    MMCSCommandAttributes::AuthPair hardwareread(hlcs::security::Object::Hardware, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(hardwareread);
    MMCSCommandAttributes::AuthPair hardwareexecute(hlcs::security::Object::Hardware, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(hardwareexecute);
    return new MMCSServerCommand_disconnect("disconnect", "disconnect <no_sysrq>", commandAttributes);
}


void
MMCSServerCommand_disconnect::execute(deque<string> args,
				MMCSCommandReply& reply,
				ConsoleController* pController,
				BlockControllerTarget* pTarget)
{
    DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper()); // get the selected BlockController
    pBlock->disconnect(args);
    reply << OK << DONE;
}

void
MMCSServerCommand_disconnect::help(deque<string> args,
			     MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
	  << ";disconnect to a set of the block's resources"
	  << DONE;
}
