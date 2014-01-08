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


#include "DumpMachine.h"

#include "server/BlockControllerBase.h"
#include "server/BlockHelper.h"

#include "common/ConsoleController.h"


namespace mmcs {
namespace lite {
namespace command {


DumpMachine*
DumpMachine::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsServerCommand(false);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(common::ADMIN);
    Attributes::AuthPair hardwareread(hlcs::security::Object::Hardware, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(hardwareread);
    return new DumpMachine("dump_machine", "dump_machine", commandAttributes);
}

void
DumpMachine::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        server::BlockControllerTarget* pTarget
        )
{
    server::BlockPtr pBlock = pController->getBlockHelper()->getBase();    // get selected block

    if (pBlock->getMachineXML() != NULL)
        reply << mmcs_client::OK << *(pBlock->getMachineXML()) << mmcs_client::DONE;
    else
        reply << mmcs_client::OK << mmcs_client::DONE;
}

void
DumpMachine::help(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description()
        << ";Print the BGQMachineXML configuration object"
        << mmcs_client::DONE;

}


} } } // namespace mmcs::lite::command
