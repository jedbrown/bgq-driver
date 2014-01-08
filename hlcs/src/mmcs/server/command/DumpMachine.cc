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

#include <db/include/api/BGQDBlib.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

DumpMachine*
DumpMachine::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.bgConsoleCommand(false);
    commandAttributes.helpCategory(common::SPECIAL);
    Attributes::AuthPair hardwareread(hlcs::security::Object::Hardware, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(hardwareread);

    return new DumpMachine("dump_machine_server", "dump_machine <file.xml>", commandAttributes);
}

void
DumpMachine::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    std::ostringstream machineStreamXML;
    if (BGQDB::getMachineXML(machineStreamXML) == 0) {
        reply << mmcs_client::OK << machineStreamXML.str() << mmcs_client::DONE;
        return;
    }
    reply << mmcs_client::FAIL << "Generation of machine xml failed." << mmcs_client::DONE;
}

void
DumpMachine::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description() << ";Export a machine from the database into <file.xml>" << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
