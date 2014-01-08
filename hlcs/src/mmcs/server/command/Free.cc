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

#include "Free.h"

#include "../BlockControllerBase.h"
#include "../DBBlockController.h"
#include "../DBConsoleController.h"

#include <utility/include/Log.h>

#include <boost/scope_exit.hpp>

using namespace std;

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace command {

Free*
Free::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::USER);
    Attributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new Free("free", "free <blockId> [abnormal]", commandAttributes);
}

void
Free::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    std::vector<std::string> vn;
    vn.push_back(args[0]);
    execute(args, reply, pController, pTarget, &vn);
    return;
}

void
Free::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    if (validnames->size() != 1) {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }

    const string block = validnames->at(0);
    if (!DBConsoleController::setAllocating(block)) {
        reply << mmcs_client::FAIL << "Cannot free block being allocated in another thread." << mmcs_client::DONE;
        return;
    }

    BOOST_SCOPE_EXIT( (&block) ) {
        DBConsoleController::doneAllocating(block);
    } BOOST_SCOPE_EXIT_END;

    pController->selectBlock(args, reply, true);  // the 3rd arg (true) says only select allocated block
    if (reply.getStatus()) {
        return;
    }

    if (args.size() == 2 ) {
        if (args[1] != "abnormal") {
            reply << mmcs_client::FAIL << "unknown argument " << args[1] << mmcs_client::DONE;
            return;
        }
    }

    const DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper()); // get the selected BlockController
    pBlock->freeBlock(args, reply);
    if (reply.getStatus()) {
        pController->deselectBlock();
        return;
    }

    pController->deselectBlock();

    // wait for the block to complete Termination
    pBlock->waitFree(reply);
}

void
Free::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Release a specified block. Drops the mc_server connections and marks a block as free in the BGQBLOCK table."
          << ";options:"
          << ";  abnormal - Skip normal kernel shutdown. Also skips I/O link shutdown for compute blocks as part of the"
          << ";             abnormal action. This will leave the I/O node software in an unknown state and may cause"
          << ";             spurious RAS events. The status of all linked I/O nodes is changed to Software (F)ailure"
          << ";             when using this option for compute blocks."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
