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
 * \file MMCSCommand_block.cc
 */

#include "MMCSCommand_block.h"
#include "MMCSCommandReply.h"
#include "BlockHelper.h"
#include "ConsoleController.h"
#include "MMCSCommand_lite.h"

#include "utility/include/Log.h"

LOG_DECLARE_FILE( "mmcs.command" );

using namespace std;

/*!
//  create_block
//  syntax:
//     create_block <bgqblock.xml> [ options ] - define a block of nodes to be controlled by mmcs
//        bgqblock.xml - xml block description file name
//        options:
//          svchost_options=<svc_host_configuration_file>
*/
MMCSCommand_create_block*
MMCSCommand_create_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.mmcsServerCommand(false);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    MMCSCommandAttributes::AuthPair hardwarecreate(hlcs::security::Object::Hardware,
                                                   hlcs::security::Action::Create);
    commandAttributes.addAuthPair(hardwarecreate);
    return new MMCSCommand_create_block("create_block", "create_block <bgqblock.xml> [ options ]", commandAttributes);
}


void
MMCSCommand_create_block::execute(deque<string> args,
				  MMCSCommandReply& reply,
				  ConsoleController* pController,
				  BlockControllerTarget* pTarget)
{
    BlockPtr pBlock = pController->getBlockBaseController();	// get selected block
    pBlock->create_block(args, reply);
    if(reply.str() == "args?")
      reply << FAIL << "args? " << usage << DONE;
}

void
MMCSCommand_create_block::help(deque<string> args,
			       MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
	  << ";define a block of nodes to be controlled by mmcs"
	  << ";   bgqblock.xml - xml block description file name"
	  << ";   options:"
	  << ";     svchost_options=<svc_host_configuration_file>"
	  << ";     diags    - enable block to be created when components are in Service state"
	  << DONE;
}
MMCSCommand_connect*
MMCSCommand_connect::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(true);            // does require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(ADMIN);              // 'help admin'  will include this command's summary
    MMCSCommandAttributes::AuthPair hardwareread(hlcs::security::Object::Hardware, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(hardwareread);
    MMCSCommandAttributes::AuthPair hardwareexecute(hlcs::security::Object::Hardware, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(hardwareexecute);
    return new MMCSCommand_connect("connect", "[<target>] connect [ options ]", commandAttributes);
}


void
MMCSCommand_connect::execute(deque<string> args,
			     MMCSCommandReply& reply,
			     ConsoleController* pController,
			     BlockControllerTarget* pTarget)
{
    BlockPtr pBlock = pController->getBlockBaseController();	// get selected block
    pBlock->connect(args, reply, pTarget);
}

void
MMCSCommand_connect::help(deque<string> args,
			  MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
          << ";connect to a set of the block's resources"
          << ";options:"
          << ";  targetset=[ temp | perm ]                                     - create the mcServer target set as temporary (default) or permanent"
          << ";  outfile=<filename>                                            - direct the console messages to a file"
          << ";  rasfile=<filename>                                            - direct the ras messages to a file"
          << ";  mode=[control | debug | service | reserve | monitor ]         - specify mode to open target set."
          << ";  pgood                                                         - reset pgood on block hardware"
          << DONE;
}

/*!
//  disconnect -- disconnect from the block's resources
*/
MMCSCommand_disconnect*
MMCSCommand_disconnect::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.mmcsServerCommand(false);
    commandAttributes.helpCategory(ADMIN);              // 'help admin'  will include this command's summary
    MMCSCommandAttributes::AuthPair hardwareread(hlcs::security::Object::Hardware, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(hardwareread);
    MMCSCommandAttributes::AuthPair hardwareexecute(hlcs::security::Object::Hardware, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(hardwareexecute);
    return new MMCSCommand_disconnect("disconnect", "disconnect <no_sysrq>", commandAttributes);
}


void
MMCSCommand_disconnect::execute(deque<string> args,
				MMCSCommandReply& reply,
				ConsoleController* pController,
				BlockControllerTarget* pTarget)
{
    BlockPtr pBlock = pController->getBlockBaseController();	// get selected block
    pBlock->disconnect(args);
    reply << OK << DONE;
}

void
MMCSCommand_disconnect::help(deque<string> args,
			     MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
	  << ";disconnect to a set of the block's resources"
	  << DONE;
}

