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
 * \file MMCSCommand_select_block.cc
 */

#include "MMCSCommand_select_block.h"
#include "DBConsoleController.h"
#include "DBBlockController.h"
#include "MMCSCommandReply.h"
#include "MMCSProperties.h"

using namespace std;

/*!
** select_block - make a specified block the current block
*/

/*!
** build() - MMCSCommand factory
** This is invoked at MMCS startup when MMCS builds its list of commands
** @return an MMCSCommand object for this specific command
*/
MMCSCommand_select_block*
MMCSCommand_select_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(DEFAULT);           // 'help default'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new MMCSCommand_select_block("select_block", "select_block <blockId>", commandAttributes);
}

/*!
** execute() - Perform specific MMCS command
** @param args the command arguments
** @param reply       the command output stream. Refer to class MMCSCommandReply
** @param pController the ConsoleController object that the command is to work on
*/
void
MMCSCommand_select_block::execute(deque<string> args,
				  MMCSCommandReply& reply,
				  DBConsoleController* pController,
				  BlockControllerTarget* pTarget,
                                  std::vector<std::string>* validnames)
{
    if (validnames->size() != 1 || validnames->at(0) != args[0])    {
      reply << FAIL << "args? " << usage <<  DONE;
      return;
    }
    pController->selectBlock(args, reply, true);
}

void
MMCSCommand_select_block::execute(deque<string> args,
				  MMCSCommandReply& reply,
				  DBConsoleController* pController,
				  BlockControllerTarget* pTarget)
{
    std::vector<std::string> vn;
    vn.push_back(args[0]);
    execute(args, reply, pController, pTarget, &vn);
}

/*!
** help() - Print extended command help to the reply stream
** @param args  the help command arguments
** @param reply the command output stream. Refer to class MMCSCommandReply
*/
void
MMCSCommand_select_block::help(deque<string> args,
			       MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
          << ";Select an already allocated block to work with."
          << ";Does not initialize the block nor reset icon connections. "
          << ";This command is intended to be used to reconnect to an"
          << ";allocated block from another bg_console session."
          << DONE;
}

/*!
** list_selected_block - print the name of the selected block
*/
MMCSCommand_list_selected_block*
MMCSCommand_list_selected_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);            // does require a BlockController object
    commandAttributes.requiresConnection(false);      // does not require  mc_server connections
    commandAttributes.requiresTarget(false);          // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);          // internal use only commmand
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(DEFAULT);              // 'help user'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new MMCSCommand_list_selected_block("list_selected_block", "list_selected_block", commandAttributes);
}

void MMCSCommand_list_selected_block::execute(deque<string> args,
					 MMCSCommandReply& reply,
					 DBConsoleController* pController,
                                         BlockControllerTarget* pTarget,
                                         std::vector<std::string>* validnames) {
    if(pController->getBlockHelper() == 0)
	reply << FAIL << "block not selected" << DONE;
    else
        return execute(args, reply, pController, pTarget);
}

void
MMCSCommand_list_selected_block::execute(deque<string> args,
					 MMCSCommandReply& reply,
					 DBConsoleController* pController,
					 BlockControllerTarget* pTarget)
{
    reply << OK << pController->getBlockBaseController()->getBlockName() << DONE;
}

void
MMCSCommand_list_selected_block::help(deque<string> args,
				      MMCSCommandReply& reply)
{
    reply << OK << description()
	  << ";prints the name of the currently selected block"
	  << DONE;
}

/*!
** deselect_block - stop working with the selected block
*/

/*!
** build() - MMCSCommand factory
** This is invoked at MMCS startup when MMCS builds its list of commands
** @return an MMCSCommand object for this specific command
*/
MMCSCommand_deselect_block*
MMCSCommand_deselect_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(USER);              // 'help user'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new MMCSCommand_deselect_block("deselect_block", "deselect_block", commandAttributes);
}

/*!
** execute() - Perform specific MMCS command
** @param args the command arguments
** @param reply       the command output stream. Refer to class MMCSCommandReply
** @param pController the ConsoleController object that the command is to work on
*/
void
MMCSCommand_deselect_block::execute(deque<string> args,
				    MMCSCommandReply& reply,
				    DBConsoleController* pController,
				    BlockControllerTarget* pTarget)
{
    pController->deselectBlock();
    reply << OK << DONE;
}

/*!
** help() - Print extended command help to the reply stream
** @param args  the help command arguments
** @param reply the command output stream. Refer to class MMCSCommandReply
*/
void
MMCSCommand_deselect_block::help(deque<string> args,
				 MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description() << ";Stop working with the selected block" << DONE;
}
