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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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


#include "BlockControllerTarget.h"
#include "CNBlockController.h"
#include "DBConsoleController.h"
#include "DBBlockController.h"
#include "DBStatics.h"
#include "DefaultControlEventListener.h"
#include "ReconnectBlocks.h"
#include "RunJobConnection.h"

#include "../MMCSCommandProcessor.h"

#include "master/AliasWaiter.h"
#include "master/Monitor.h"

#include "common/Properties.h"

#include <control/include/mcServer/MCServerAPIHelpers.h>

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/job/types.h>

#include <utility/include/Log.h>

#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/scope_exit.hpp>

#include <string>
#include <vector>

LOG_DECLARE_FILE( "mmcs.server" );

using namespace MCServerMessageSpec;
using mmcs::common::Properties;


extern bool reconnect_done;
extern boost::condition_variable reconnect_notifier;


namespace mmcs {
namespace server {

extern MCServerMessageType message_type;


boost::mutex error_write_mutex;
std::vector<std::string> error_list;

boost::mutex spec_write_mutex;
std::vector<std::string> special_free;

void
subnetRestartThread(
        const std::string& curr_subnet,
        const std::vector<std::string>& all_blocks,
        boost::barrier* restart_barrier
        )
{
    LOG_INFO_MSG(__FUNCTION__ << " 0x" << std::hex << pthread_self() << " subnet " << curr_subnet );
    MCServerMessageSpec::FailoverRequest failreq;
    MCServerMessageSpec::FailoverReply failrep;
    const master::Monitor::AliasMap::const_iterator b(
            master::Monitor::_alias_binary_map.find(curr_subnet)
            );
    if ( b == master::Monitor::_alias_binary_map.end() ) {
        LOG_FATAL_MSG(
                "Cannot find " << curr_subnet
                );
        _exit(EXIT_FAILURE);
    }

    if (!master::AliasWaiter::buildFailover(failreq, curr_subnet, b->second)) {
        LOG_FATAL_MSG(
                "Cannot build a proper failover message. " << 
                "Database state may be inconsistent. mmcs_server ending."
                );
        _exit(EXIT_FAILURE);
    }
    MCServerRef* failref = 0;
    mmcs_client::CommandReply failreply;
    BlockControllerBase::mcserver_connect(failref, curr_subnet, failreply);
    if (failref) {
        LOG_INFO_MSG("Sending failover request to mc_server for " << curr_subnet);
        try {
            failref->failover(failreq, failrep);
        } catch ( const std::exception& e ) {
            LOG_FATAL_MSG( e.what() );
            _exit(EXIT_FAILURE);
        }
    } else if (!failref || failreply.getStatus()) {
        // Could not connect to mc_server.  That's a termination condition.
        LOG_FATAL_MSG("Could not connect to mc_server to send a failover message.  "
                      << "mmcs_server ending.");
        _exit(EXIT_FAILURE);
    }

    const MCServerRefPtr refp(failref);
    std::vector<std::string> hw_to_unmark;

    if (failrep._rc) {
        (void)master::AliasWaiter::sendInitialize(refp, hw_to_unmark, failrep);
        // Anything that now has hardware in error needs to be freed.
        BOOST_FOREACH(const std::string& blockname, all_blocks) {
            std::vector<std::string> curr_errors;
            BGQDB::queryMissing(blockname, curr_errors);
            // Get the locations necessary to kill the jobs.
            BOOST_FOREACH(const std::string& error, curr_errors) {
                std::string trimmed_error = error.substr(error.find_last_of("R"));
                if (trimmed_error.empty()) {
                    // It might be an IO rack.
                    trimmed_error = error.substr(error.find_last_of("Q"));
                }
                LOG_DEBUG_MSG("Adding " << trimmed_error << " to error list.");
                boost::mutex::scoped_lock sl(error_write_mutex);
                error_list.push_back(trimmed_error);
            }
            // Now just get the block names.
            if (!curr_errors.empty()) {
                boost::mutex::scoped_lock sl(spec_write_mutex);
                special_free.push_back(blockname);
                LOG_DEBUG_MSG("Found block " << blockname << " to free.");
            }
        }
    }

    LOG_INFO_MSG("Sent failover messages to mc_server for " << curr_subnet);

    // Mark any nodes that were set in error during reconnect to 'A'.
    if (!hw_to_unmark.empty()) {
        typedef MCServerMessageSpec::FailoverReply::NodeCard ReplyNodeCard;
        typedef MCServerMessageSpec::FailoverReply::IoCard ReplyIoCard;
        typedef MCServerMessageSpec::FailoverReply::ServiceCard ReplyServiceCard;

        std::vector<std::string> ncs;
        BOOST_FOREACH(const ReplyNodeCard& nc, failrep._nodeCards) {
            if (std::find(hw_to_unmark.begin(), hw_to_unmark.end(), nc._location) != hw_to_unmark.end())
                ncs.push_back(nc._location);
        }
        DBStatics::setLocationStatus(ncs, failreply, DBStatics::AVAILABLE, DBStatics::COMPUTECARD);

        std::vector<std::string> iocs;
        BOOST_FOREACH(const ReplyIoCard& ioc, failrep._ioCards) {
            if (std::find(hw_to_unmark.begin(), hw_to_unmark.end(), ioc._location) != hw_to_unmark.end())
                iocs.push_back(ioc._location);
        }
        DBStatics::setLocationStatus(iocs, failreply, DBStatics::AVAILABLE, DBStatics::IOCARD);

        std::vector<std::string> scs;
        BOOST_FOREACH(const ReplyServiceCard& sc, failrep._serviceCards) {
            if (std::find(hw_to_unmark.begin(), hw_to_unmark.end(), sc._location) != hw_to_unmark.end())
                scs.push_back(sc._location);
        }
        DBStatics::setLocationStatus(scs, failreply, DBStatics::AVAILABLE, DBStatics::SERVICECARD);
    }
    restart_barrier->wait();
}

void
reconnectBlocks(
        MMCSCommandProcessor* commandProcessor
        )
{
    BGQDB::BLOCK_STATUS blockState;
    std::vector<std::string> allBlocks;
    std::vector<std::string> reconnectBlocks;
    std::vector<std::string> freeBlocks;

    MCServerRef* temp = NULL;
    mmcs_client::CommandReply reply;
    BlockControllerBase::mcserver_connect(temp, "reconnect", reply);
    if (!temp) {
        LOG_WARN_MSG( "could not connect to mc_server: " << reply.str() );
        return;
    }
    const MCRefPtr mcServer( temp );

    LOG_INFO_MSG("starting");

    BOOST_SCOPE_EXIT( (&reconnect_done) ) {
        reconnect_done = true;
        reconnect_notifier.notify_all();
    } BOOST_SCOPE_EXIT_END;

    // get a list of all non-free blocks 
    BGQDB::STATUS result;
    result = BGQDB::getBlockIds("where status <> 'F'", allBlocks);
    if (result != BGQDB::OK)
    {
        LOG_ERROR_MSG( "getBlockIds() failed with error " << result );
        return;
    }
    LOG_DEBUG_MSG( "found " << allBlocks.size() << " blocks" );

    // Get a list of all blocks specified on the reconnect-blocks startup parameter,
    // but only if we have reconnect true and bringup false.  We have already
    // checked to make sure that both aren't true.
    if (Properties::getProperty(RECONNECT_BLOCKS) == "true" &&
        Properties::getProperty(BRINGUP) == "false")
    {
        reconnectBlocks = allBlocks;
    } else {
        LOG_INFO_MSG( "block reconnection disabled" );
    }

    // create a list of all blocks to be reconnected
    for (std::vector<std::string>::iterator it = reconnectBlocks.begin(); it != reconnectBlocks.end(); )
    {
        const std::string blockName(*it);

        // get the current block state
        if ((result = BGQDB::getBlockStatus(blockName, blockState)) != BGQDB::OK)
        {
            LOG_ERROR_MSG("getBlockStatus(" << blockName << ") failed with error " << result);
            ++it;
            continue;
        }

        // if the block is not fully initialized, remove it from the reconnectBlocks list
        if (blockState != BGQDB::INITIALIZED) {
            LOG_DEBUG_MSG( "not reconnecting to " << blockName << " with status " << blockState );
            it = reconnectBlocks.erase(it);
        } else {
            ++it;
        }
    }

    // create a list of all blocks to be freed
    for (std::vector<std::string>::const_iterator it = allBlocks.begin(); it != allBlocks.end(); ++it)
    {
        const std::string blockName(*it);

        // get the current block state
        if ((result = BGQDB::getBlockStatus(blockName, blockState)) != BGQDB::OK)
        {
            LOG_ERROR_MSG("getBlockStatus(" << blockName << ") failed with error " << result);
            continue;
        }

        // if the block is not in the reconnectBlocks list, then add it to the freeBlock list
        if (find(reconnectBlocks.begin(), reconnectBlocks.end(), blockName) == reconnectBlocks.end())
            freeBlocks.push_back(blockName);
    }

    // try to reconnect to blocks in the reconnectBlocks list
    // if reconnect fails, add the block to the freeBlock list
    typedef std::map< DBBlockPtr,boost::shared_ptr<DBConsoleController> > BlockContainer;
    BlockContainer blocks_to_connect;
    for (std::vector<std::string>::const_iterator it = reconnectBlocks.begin(); it != reconnectBlocks.end(); ++it)
    {
        const std::string blockName(*it);

        // get user
        std::string username;
        int qualifier;
        BGQDB::STATUS result = BGQDB::getBlockUser(blockName, username, qualifier);
        if (result != BGQDB::OK)
        {
            LOG_ERROR_MSG("getBlockOwner(" << blockName << ") failed with error " << result);
            freeBlocks.push_back(blockName);
            continue;
        }

        // create console controller
        bgq::utility::UserId uid;
        try {
            const bgq::utility::UserId myuid( username );
            uid = myuid;
        } catch(const std::exception& e) {
            LOG_ERROR_MSG("Can't get user id. " << e.what());
            freeBlocks.push_back(blockName);
            continue;
        }

        const boost::shared_ptr<DBConsoleController> consoleController( new DBConsoleController(commandProcessor, uid) );

        //  select the block
        std::deque<std::string> args; args.push_back(blockName);
        mmcs_client::CommandReply reply;
        consoleController->selectBlock(args, reply, true);
        if (reply.getStatus()) // can't create block
        {
            LOG_ERROR_MSG("selectBlock(" << blockName << ") failed: " << reply.str());
            freeBlocks.push_back(blockName);
            continue;
        }
        const DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(consoleController->getBlockHelper());

        // remember our boot cookie for RAS events
        pBlock->getBase()->setBootCookie(qualifier);

        // create the BlockController and targets
        pBlock->create_block(args, reply);
        if (reply.getStatus()) // can't create block
        {
            LOG_ERROR_MSG("create_block(" << blockName << ") failed: " << reply.str());
            freeBlock( blockName, username, mcServer );
            consoleController->deselectBlock();
            continue;
        }

        // Create an MCServer connection and an MCServer target for all of the block resources,
        // and connect to the targets in the block
        if (!pBlock->getBase()->isIOBlock()) {
            // Only actually connect compute blocks.  IO blocks are disconnected when INITIALIZED.
            blocks_to_connect[pBlock] = consoleController;
        } else {
            pBlock->getBase()->setReconnected();
        }
    }

    if (message_type == FAILOVER_MSG) {
        LOG_INFO_MSG("Reconnecting: sending failover messages to the subnets");
        // Before reconnecting, send a list of the hardware associated with booted blocks
        std::vector<std::string> subnets;
        Properties::getSubnetNames(subnets);
        boost::barrier restart_barrier(subnets.size() + 1);
        BOOST_FOREACH(const std::string& curr_subnet, subnets) {
            boost::thread srt(&subnetRestartThread, curr_subnet, reconnectBlocks, &restart_barrier);
        }
        restart_barrier.wait();
    }

    // Now connect to the compute blocks.
    std::deque<std::string> connectArgs;
    connectArgs.push_back(Properties::getProperty(DFT_TGTSET_TYPE));
    for(BlockContainer::const_iterator it = blocks_to_connect.begin(); it != blocks_to_connect.end(); ++it) {
        const DBBlockPtr curr_block = it->first;
        if (std::find(freeBlocks.begin(), freeBlocks.end(), curr_block->getBlockName()) != freeBlocks.end()) {
            continue; // Skip it if it's in the free list
        }
        const boost::shared_ptr<DBConsoleController> console( it->second );
        mmcs_client::CommandReply reply;
        BlockControllerTarget target(curr_block->getBase(), "{*}", reply);
        curr_block->getBase()->connect(connectArgs, reply, &target);

        if (reply.getStatus() && !curr_block->getBase()->isIOBlock())
        {
            LOG_ERROR_MSG("connect(" << curr_block->getBlockName() << ") failed: " << reply.str());
            curr_block->setBlockStatus(BGQDB::FREE);
            curr_block->getBase()->setDisconnecting(true, reply.str()); // reset icon connections and stop mailbox
            console->deselectBlock();
            freeBlocks.push_back(curr_block->getBlockName());
        }
        else
            LOG_INFO_MSG("block " << curr_block->getBlockName() << " successfully reconnected");
        curr_block->getBase()->setReconnected();
    }

    // Finally, free blocks that will not be reconnected
    for (std::vector<std::string>::const_iterator it = freeBlocks.begin(); it != freeBlocks.end(); ++it)
    {
        const std::string blockName(*it);

        // get the block owner
        std::string userId;
        result = BGQDB::getBlockOwner(blockName, userId);
        if (result != BGQDB::OK)
        {
            LOG_ERROR_MSG("getBlockOwner(" << blockName << ") failed with error " << result);
            // continue without a userid
            userId.clear();
        }

        // delete the target set and free the block
        freeBlock(blockName, userId, mcServer);

        // clear any pending actions for blocks we are not reconnecting
        result = BGQDB::clearBlockAction(blockName);
        if ( result != BGQDB::OK ) {
            LOG_ERROR_MSG("clearBlockAction(" << blockName << ") failed with return code= " << result);
        }
    }

    std::set<BGQDB::job::Id> jobs;
    // Kill the jobs associated with hardware in the error list.
    BOOST_FOREACH(const std::string& location, error_list) {
        LOG_DEBUG_MSG("Freeing blocks and jobs for " << location);

        std::vector<BGQDB::job::Id> temp;
        const BGQDB::STATUS result = BGQDB::killMidplaneJobs(location, &temp, true);
        if (result != BGQDB::OK) {
            LOG_WARN_MSG( "error on BGQDB::KillMidplaneJobs trying to free blocks");
        }

        BOOST_FOREACH( const BGQDB::job::Id& job, temp ) {
            jobs.insert( job );
        }
    }

    // Kill the jobs
    BOOST_FOREACH(const BGQDB::job::Id& job, jobs) {
        RunJobConnection::instance().Kill(job, 9);
    }

    // Now free the blocks.
    BOOST_FOREACH(const std::string& blockid, special_free) {
        // If this is an IO block, we've got to free the computes, too.
        std::vector<std::string> compute_blocks;
        const BGQDB::STATUS result = BGQDB::checkIOBlockConnection(blockid, &compute_blocks);
        if (result != BGQDB::OK) {
            LOG_ERROR_MSG(
                    DBBlockController::strDBError(result)
                    << ": Database cannot determine connected compute blocks."
                    );
        }
        // Stick it in with the compute blocks and free 'em all in one loop.
        compute_blocks.push_back(blockid);
        BOOST_FOREACH(const std::string& block, compute_blocks) {
            const BGQDB::STATUS result = BGQDB::setBlockAction(block, BGQDB::DEALLOCATE_BLOCK);
            if (result == BGQDB::OK) {
                LOG_INFO_MSG("block " << blockid << " set to DEALLOCATE");
            } else if (result == BGQDB::DUPLICATE) {
                // still ok
            } else {
                LOG_ERROR_MSG("Could not deallocate block " << blockid);
            }
        }
    }

    if (message_type == DIE_MSG) {
        // If we aren't reconnecting and we have some blocks that are booted or "in flight"
        // then mc_server needs to be told this so we can restart the world.
        FailoverRequest failreq;
        FailoverReply failrep;
        failreq._bootedHardware._failed_subnet_id = "MC_SERVER_DIE";
        LOG_INFO_MSG("Sending failover request to mc_server to make it die.");
        mcServer->failover(failreq, failrep);
    }

    LOG_INFO_MSG("done");
}

void
freeBlock(
        const std::string& blockName,
        const std::string& userName,
        const MCRefPtr& mcServer
        )
{
    // delete the target set
    deleteTargetSet(blockName, userName, mcServer);

    // mark the specified block as free
    std::deque<std::string> errmsg; 
    errmsg.push_back("errmsg=block freed by reconnect_blocks");
    // The block actions happen during startup serially here
    // so we don't need to worry about other threads racing
    // to the static unlocked BGQDB method.
    const BGQDB::STATUS result = BGQDB::setBlockStatus(blockName, BGQDB::FREE, errmsg);
    if (result == BGQDB::OK) {
        LOG_INFO_MSG("block " << blockName << " successfully freed");
    } else {
        BGQDB::BLOCK_STATUS blockState;
        BGQDB::getBlockStatus(blockName, blockState);
        LOG_ERROR_MSG("setBlockStatus(" << blockName << ", FREE) failed with return code= " << result
                << ", current block state=" << blockState);
    }
}

void
deleteTargetSet(
        const std::string& blockName,
        const std::string& userName,
        const MCRefPtr& mcServer
        )
{
    mmcs_client::CommandReply rep;

    // open the target set in Control mode
    MCServerMessageSpec::OpenTargetRequest mcOpenTargetSetRequest(
            blockName,
            userName,
            WUAR,
            true
            );
    MCServerMessageSpec::OpenTargetReply mcOpenTargetSetReply;
    try
    {
        mcServer->openTarget(mcOpenTargetSetRequest, mcOpenTargetSetReply);
    }
    catch (const std::exception &e)
    {
        mcOpenTargetSetReply._rc = -1;
        mcOpenTargetSetReply._rt = e.what();
    }
    if (mcOpenTargetSetReply._rc)
    {
        LOG_DEBUG_MSG("openTargetSet: " << mcOpenTargetSetReply._rt);
        LOG_ERROR_MSG("unable to open target set " << blockName);
        return;
    }

    LOG_DEBUG_MSG("mcServer target set " << blockName << " opened for " << userName);

    MCServerMessageSpec::CloseTargetRequest mcCloseTargetRequest(
            MCServerAPIHelpers::createCloseRequest( mcOpenTargetSetRequest, mcOpenTargetSetReply)
            );
    MCServerMessageSpec::CloseTargetReply mcCloseTargetReply;
    try
    {
        mcServer->closeTarget(mcCloseTargetRequest, mcCloseTargetReply);
    }
    catch (const std::exception &e)
    {
        mcCloseTargetReply._rc = -1;
        mcCloseTargetReply._rt = e.what();
    }

    if (mcCloseTargetReply._rc) {
        LOG_ERROR_MSG("closeTargetSet: " << mcCloseTargetReply._rt);
        LOG_ERROR_MSG("unable to close target set " << blockName);
        return;
    }
        
    LOG_DEBUG_MSG("mcServer target set " << blockName << " closed");

    // delete the target set
    MCServerMessageSpec::DeleteTargetSetRequest mcDeleteTargetSetRequest(blockName, userName);
    MCServerMessageSpec::DeleteTargetSetReply   mcDeleteTargetSetReply;

    try {
        mcServer->deleteTargetSet(mcDeleteTargetSetRequest, mcDeleteTargetSetReply);
    }
    catch (const std::exception &e)
    {
        mcDeleteTargetSetReply._rc = -1;
        mcDeleteTargetSetReply._rt = e.what();
    }

    if (mcDeleteTargetSetReply._rc)
    {
        LOG_ERROR_MSG("deleteTargetSet: " << mcDeleteTargetSetReply._rt);
        LOG_ERROR_MSG("unable to delete target set " << blockName);
        return;
    }

    LOG_DEBUG_MSG("mcServer target set " << blockName << " deleted");
}

} } // namespace mmcs::server
