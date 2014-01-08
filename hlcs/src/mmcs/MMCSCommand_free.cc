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
 * \file MMCSCommand_free.cc
 */

#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/scope_exit.hpp>
#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/BGQDBlib.h>
#include <utility/include/Log.h>
#include "MMCSCommand_free.h"
#include "MMCSCommandReply.h"
#include "MMCSCommandProcessor.h"
#include "DBConsoleController.h"
#include "DBBlockController.h"
#include "MMCSProperties.h"


using namespace std;

LOG_DECLARE_FILE( "mmcs" );

/*!
** Release a specified block.  Drops the  mc_server connections and marks a block as free in the BGQBLOCK table.
*/
MMCSCommand_free*
MMCSCommand_free::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(USER);
    MMCSCommandAttributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new MMCSCommand_free("free", "free <blockId>", commandAttributes);
}

void
MMCSCommand_free::execute(deque<string> args,
			  MMCSCommandReply& reply,
			  DBConsoleController* pController,
			  BlockControllerTarget* pTarget)
{
    std::vector<std::string> vn;
    vn.push_back(args[0]);
    execute(args, reply, pController, pTarget, &vn);
    return;
}

void
MMCSCommand_free::execute(deque<string> args,
                          MMCSCommandReply& reply,
                          DBConsoleController* pController,
                          BlockControllerTarget* pTarget,
                          std::vector<std::string>* validnames)
{
    BGQDB::BLOCK_ACTION blockAction = BGQDB::NO_BLOCK_ACTION;

    if (validnames->size() != 1 )
    {
	reply << FAIL << "args? " << usage <<  DONE;
	return;
    }

    string block = validnames->at(0);
    if(!DBConsoleController::setAllocating(block)) {
      reply << FAIL << "Cannot free block being allocated in another thread." << DONE;
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
                            LOG_ERROR_MSG("MMCSCommand_free() setBlockStatus(" << args[0] << ", FREE) failed, result=" << result << ", current block action=" << blockAction);
                        } else {
                            LOG_INFO_MSG("MMCSCommand_free() setBlockStatus(" << args[0] << ", FREE) successful");
                        }
                        reply << OK << DONE;
                    }
            }

        return;
    }



    // free the block
    DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper()); // get the selected BlockController
    string blockName = pBlock->getBase()->getBlockName(); // get the block name
    pBlock->freeBlock(args, reply);
    if (reply.getStatus() != 0) {
      pController->deselectBlock();
      return;
    }

    // deselect the block
    pController->deselectBlock();

    // wait for the block to complete Termination
    BGQDB::BLOCK_STATUS blockState = BGQDB::INVALID_STATE;
    while (1)
    {
	BGQDB::getBlockStatus(blockName, blockState);
	if (blockState != BGQDB::TERMINATING)
	    break;
	sleep(5);
    }

    reply << OK << DONE;
}

void
MMCSCommand_free::help(deque<string> args,
		       MMCSCommandReply& reply)
{
	reply << OK << description()
	      << ";Release a specified block.  Drops the  mc_server connections and marks a block as free in the BGQBLOCK table."
	      << DONE;
}

/*!
** Release a selected block.  An alternate form of free that operates on the currently selected block.
*/
MMCSCommand_free_block*
MMCSCommand_free_block::build()
{
    MMCSCommandAttributes commandAttributes;

    commandAttributes.requiresBlock(true);         // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(USER);
    MMCSCommandAttributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new MMCSCommand_free_block("free_block", "free_block", commandAttributes);
}

void
MMCSCommand_free_block::execute(deque<string> args,
				MMCSCommandReply& reply,
				DBConsoleController* pController,
				BlockControllerTarget* pTarget)
{
    BGQDB::BLOCK_STATUS blockState = BGQDB::INVALID_STATE;

    DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper());	// get selected block
    string blockName = pBlock->getBase()->getBlockName(); // get the block name

    if(!DBConsoleController::setAllocating(blockName)) {
      reply << FAIL << "Block is being allocated or freed in another thread" << DONE;
      return;
    }

    BOOST_SCOPE_EXIT( (&blockName) ) {
        DBConsoleController::doneAllocating(blockName);
    } BOOST_SCOPE_EXIT_END;

    pBlock->freeBlock(args, reply);
    if (reply.getStatus() != 0) {
      pController->deselectBlock();
      return;
    }

    // deselect the block
    pController->deselectBlock();

    // wait for the block to complete Termination
    while (1)
    {
	BGQDB::getBlockStatus(blockName, blockState);
	if (blockState != BGQDB::TERMINATING)
	    break;
	sleep(5);
    }
    reply << OK << DONE;
}

void
MMCSCommand_free_block::help(deque<string> args,
			     MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
	  << ";Release a selected block.  An alternate form of free that operates on the currently selected block."
	  << DONE;
}

/*!
 *
 */
MMCSCommand_free_all*
MMCSCommand_free_all::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.requiresObjNames(false);
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(USER);
    MMCSCommandAttributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new MMCSCommand_free_all("free_all", "free_all [ options ]", commandAttributes);
}

void
MMCSCommand_free_all::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget)
{
    getBlockObjects(args, pController);
    execute(args, reply, pController, pTarget, &_blocks);
    _blocks.clear();
}

void
MMCSCommand_free_all::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
        )
{
    // here's the heavy lifting to do the actual freeing
    size_t failureCount = 0;
    for(unsigned int i = 0; i < validnames->size(); ++i) {
        std::vector<std::string> name;
        std::string block = validnames->at(i);
        name.push_back(block);
        MMCSCommand_free* free_cmd = MMCSCommand_free::build();
        free_cmd->execute(
                boost::assign::list_of(std::string(block)),
                reply,
                pController,
                pTarget,
                &name
                );
        if ( reply.getStatus() != 0 ) {
            LOG_WARN_MSG( "could not free block " << block << ": " << reply.str());
            failureCount++;
            reply.str("");
        }
        name.clear();
        delete free_cmd;
    }

    // send reply if we didn't have any failures
    if ( !failureCount ) {
        reply << OK << "freed " << validnames->size() << " " << (validnames->size() == 1 ? "block" : "blocks") << DONE;
    } else {
        reply << FAIL << "freed " << validnames->size() - failureCount << " out of " << validnames->size() << " blocks" << DONE;
    }
}

std::vector<std::string>
MMCSCommand_free_all::getBlockObjects(std::deque<std::string>& cmdString, DBConsoleController* pController) {
    // default scope is to free all compute blocks, the arguments to this
    // command can alter that behavior
    std::vector<std::string> blocks;
    std::string scope( "AND " + BGQDB::DBTBlock::NUMCNODES_COL + " <> 0" );
    if ( cmdString.size() == 1 ) {
        if ( cmdString[0] == "io" ) {
            scope = std::string( "AND " + BGQDB::DBTBlock::NUMIONODES_COL + " <> 0" );
        } else if ( cmdString[0] == "compute" ) {
            // default scope is fine
        } else if ( cmdString[0] == "both" ) {
            scope.clear();
        } else {
            LOG_ERROR_MSG( "unknown argument: " << cmdString[0]);
            return blocks;
        }
    }

    try {
        // get list of all non-free blocks owned by this user
        const cxxdb::ConnectionPtr connection(
                BGQDB::DBConnectionPool::instance().getConnection()
                );
        const cxxdb::QueryStatementPtr statement(
                connection->prepareQuery(
                    "SELECT " + BGQDB::DBTBlock::BLOCKID_COL + ", " +
                    BGQDB::DBTBlock::NUMIONODES_COL + " FROM TBGQBlock "
                    "WHERE " + BGQDB::DBTBlock::STATUS_COL + " <> 'F' "
                    "AND " + BGQDB::DBTBlock::USERNAME_COL + "=? "
                    + scope,
                    boost::assign::list_of( BGQDB::DBTBlock::USERNAME_COL )
                    )
                );
        statement->parameters()[ BGQDB::DBTBlock::USERNAME_COL ].set( pController->getUser().getUser() );
        const cxxdb::ResultSetPtr result( statement->execute() );
        std::vector<std::string> ioblocks;
        while ( result->fetch() ) {
            LOG_DEBUG_MSG("block " << result->columns()[ BGQDB::DBTBlock::BLOCKID_COL ].getString() << " set to be freed.");
            if(result->columns()[BGQDB::DBTBlock::NUMIONODES_COL].getInt64() == 0)
                blocks.push_back( result->columns()[ BGQDB::DBTBlock::BLOCKID_COL ].getString() );
            else
                ioblocks.push_back(result->columns()[ BGQDB::DBTBlock::BLOCKID_COL ].getString() );
        }

        // Now put any/all IO blocks at the end of the list so the CN blocks
        // get freed first.
        for(std::vector<std::string>::iterator it = ioblocks.begin();
            it != ioblocks.end(); ++it) {
            blocks.push_back(*it);
        }
    } catch ( const cxxdb::DatabaseException& e ) {
        LOG_ERROR_MSG(e.what());
    }
    _blocks = blocks;
    return blocks;
}

void
MMCSCommand_free_all::help(
        deque<string> args,
        MMCSCommandReply& reply
        )
{
    reply << OK << description()
        << ";Release all blocks in use by the console user. By default only compute"
        << ";blocks are freed, to alter this behavior use one of the options below."
        << ";options:"
        << ";  io - free I/O blocks"
        << ";  compute - free compute blocks"
        << ";  both - free both compute and I/O blocks"
        << DONE;
}
