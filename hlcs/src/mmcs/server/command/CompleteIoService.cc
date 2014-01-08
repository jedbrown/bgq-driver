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


#include "CompleteIoService.h"

#include "RebootNodes.h"

#include "../BlockControllerTarget.h"
#include "../BlockHelper.h"
#include "../DBConsoleController.h"

#include <db/include/api/BGQDBlib.h>

#include <boost/scoped_ptr.hpp>


using namespace std;


namespace mmcs {
namespace server {
namespace command {


CompleteIoService*
CompleteIoService::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false) ;      // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.internalCommand(true);
    commandAttributes.helpCategory(common::SPECIAL);
    commandAttributes.bgadminAuth(true);
    return new CompleteIoService("complete_io_service", "complete_io_service <location>", commandAttributes);
}


void
CompleteIoService::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget)
{
    BGQDB::STATUS result;

    std::string containingBlock = "<none>";
    result = BGQDB::completeIOService(args[0].c_str(), containingBlock);
    if (result != BGQDB::OK)
    {
        reply << mmcs_client::FAIL << "error on BGQDB::completeIOService" << mmcs_client::DONE;
        return;
    }

    if (containingBlock != "<none>") {
        // reboot the IO drawer's nodes within the block

        // first get the block controller for the containingBlock
        deque<string> selectArgs;
        selectArgs.push_back(containingBlock);
        pController->selectBlock(selectArgs, reply, true);
        if ( !pController->getBlockHelper() ) return;
        if ( !pController->getBlockHelper()->getBase() ) return;

        string cmd;
        cmd = "{" + args[0] + string("-J..$}");
        BlockControllerTarget target(pController->getBlockHelper()->getBase(), cmd, reply);
        deque<std::string> args;
        boost::scoped_ptr<RebootNodes> reboot_nodes_cmd_ptr(RebootNodes::build());
        reboot_nodes_cmd_ptr->execute(args, reply, pController, &target);
    }
    else
        reply << mmcs_client::OK << mmcs_client::DONE;
}

void
CompleteIoService::help(deque<string> args,
              mmcs_client::CommandReply& reply)
{
  reply << mmcs_client::OK << description()
    << ";Internal command, not to be used from console, only to be used internally during service actions. "
    << mmcs_client::DONE;
}


} } } // namespace mmcs::server::command
