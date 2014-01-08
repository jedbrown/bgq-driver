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


#include "CreateBlock.h"

#include "server/BlockControllerBase.h"
#include "server/BlockHelper.h"

#include "common/ConsoleController.h"


using namespace std;


namespace mmcs {
namespace lite {
namespace command {


CreateBlock*
CreateBlock::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.mmcsServerCommand(false);
    commandAttributes.bgConsoleCommand(false);
    commandAttributes.helpCategory(common::ADMIN);             // 'help admin'  will include this command's summary
    Attributes::AuthPair hardwarecreate(hlcs::security::Object::Hardware,
                                                   hlcs::security::Action::Create);
    commandAttributes.addAuthPair(hardwarecreate);
    return new CreateBlock("create_block", "create_block <bgqblock.xml> [ options ]", commandAttributes);
}


void
CreateBlock::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        server::BlockControllerTarget* pTarget
        )
{
    server::BlockPtr pBlock = pController->getBlockHelper()->getBase();    // get selected block
    pBlock->create_block(args, reply);
    if(reply.str() == "args?")
      reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
}

void
CreateBlock::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description()
      << ";define a block of nodes to be controlled by mmcs"
      << ";   bgqblock.xml - xml block description file name"
      << ";   options:"
      << ";     svchost_options=<svc_host_configuration_file>"
      << ";     diags    - enable block to be created when components are in Service state"
      << mmcs_client::DONE;
}


} } } // namespace mmcs::lite::command
