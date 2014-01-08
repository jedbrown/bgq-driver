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
    return new Free("free", "free <blockId>", commandAttributes);
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
    BGQDB::BLOCK_ACTION blockAction = BGQDB::NO_BLOCK_ACTION;

    if (validnames->size() != 1 )
    {
        reply << mmcs_client::FAIL << "args? " << usage << mmcs_client::DONE;
        return;
    }

    string block = validnames->at(0);
    if(!DBConsoleController::setAllocating(block)) {
        reply << mmcs_client::FAIL << "Cannot free block being allocated in another thread." << mmcs_client::DONE;
        return;
    }

    BOOST_SCOPE_EXIT( (&block) ) {
        DBConsoleController::doneAllocating(block);
    } BOOST_SCOPE_EXIT_END;

    //  select the block
    pController->selectBlock(args, reply, true);  // the 3rd arg (true) says only select allocated block
    if (reply.getStatus() != 0) {

        // If the BlockController was not found and the block action is Deallocate, just set the block state to free
        if (reply.str() == "BlockController not found")
        {
            BGQDB::STATUS result;
            unsigned int creationId;
            BGQDB::getBlockAction(block, blockAction, creationId);
            if (blockAction == BGQDB::DEALLOCATE_BLOCK)
            {
                // We can safely call the unserialized DB lib version of
                // setBlockStatus here because we don't have a block object.
                result = BGQDB::setBlockStatus(args[0], BGQDB::FREE);
                if (result != BGQDB::OK) {
                    LOG_ERROR_MSG(__FUNCTION__ << "() setBlockStatus(" << args[0] << ", FREE) failed, result=" << result << ", current block action=" << blockAction);
                } else {
                    LOG_INFO_MSG(__FUNCTION__ << "() setBlockStatus(" << args[0] << ", FREE) successful");
                }
                reply << mmcs_client::OK << mmcs_client::DONE;
            }
        }

        return;
    }

    // free the block
    const DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper()); // get the selected BlockController
    const string blockName = pBlock->getBase()->getBlockName(); // get the block name
    const log4cxx::MDC _blockid_mdc_( "blockId", std::string("{") + blockName + "} " );
    pBlock->freeBlock(args, reply);
    if (reply.getStatus() != 0) {
        pController->deselectBlock();
        return;
    }

    // deselect the block
    pController->deselectBlock();

    // wait for the block to complete Termination
    pBlock->waitFree(reply);

    LOG_DEBUG_MSG( "done" );
}

void
Free::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description()
        << ";Release a specified block.  Drops the  mc_server connections and marks a block as free in the BGQBLOCK table."
        << mmcs_client::DONE;
}


} } } // namespace mmcs::server::command
