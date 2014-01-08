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
#include "DefaultListener.h"
#include "DBStatics.h"
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

#include <csignal>
#include <set>
#include <string>
#include <vector>

LOG_DECLARE_FILE( "mmcs.server" );

using namespace MCServerMessageSpec;
using mmcs::common::Properties;

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
        const std::vector<std::string>& all_blocks
        )
{
    LOG_INFO_MSG(__FUNCTION__ << " " << curr_subnet );
    MCServerMessageSpec::FailoverRequest failreq;
    MCServerMessageSpec::FailoverReply failrep;
    const master::Monitor::AliasMap::const_iterator b(
            master::Monitor::_alias_binary_map.find(curr_subnet)
            );
    if ( b == master::Monitor::_alias_binary_map.end() ) {
        LOG_FATAL_MSG("Cannot find " << curr_subnet);
        _exit(EXIT_FAILURE);
    }

    if (!master::AliasWaiter::buildFailover(failreq, curr_subnet, b->second)) {
        LOG_FATAL_MSG(
                "Cannot build a proper failover message. " <<
                "Database state may be inconsistent, mmcs_server is ending."
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
        LOG_FATAL_MSG("Could not connect to mc_server to send a failover message, mmcs_server is ending.");
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
                // The results from queryMissing() are a little strange, it will look like
                // NODEBOARD: R00-M0-N00
                // Ee want to use the location string to get a list of jobs to kill so we
                // need to strip off the prefix
                const std::string::size_type colon = error.find_first_of( ": " );
                const std::string trimmed( colon == std::string::npos ? error : error.substr( colon + 2 ) );
                LOG_DEBUG_MSG("Adding " << trimmed << " to error list.");
                boost::mutex::scoped_lock sl(error_write_mutex);
                error_list.push_back(trimmed);
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
        DBStatics::setLocationStatus(ncs, DBStatics::AVAILABLE, bgq::util::Location::NodeBoard);

        std::vector<std::string> iocs;
        BOOST_FOREACH(const ReplyIoCard& ioc, failrep._ioCards) {
            if (std::find(hw_to_unmark.begin(), hw_to_unmark.end(), ioc._location) != hw_to_unmark.end())
                iocs.push_back(ioc._location);
        }
        DBStatics::setLocationStatus(iocs, DBStatics::AVAILABLE, bgq::util::Location::IoBoardOnComputeRack );

        std::vector<std::string> scs;
        BOOST_FOREACH(const ReplyServiceCard& sc, failrep._serviceCards) {
            if (std::find(hw_to_unmark.begin(), hw_to_unmark.end(), sc._location) != hw_to_unmark.end())
                scs.push_back(sc._location);
        }
        DBStatics::setLocationStatus(scs, DBStatics::AVAILABLE, bgq::util::Location::ServiceCard);
    }
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
        LOG_WARN_MSG( "Could not connect to mc_server: " << reply.str() );
        return;
    }
    const MCRefPtr mcServer( temp );

    LOG_INFO_MSG("Performing reconnect blocks work.");

    // Get a list of all non-free blocks
    BGQDB::STATUS result;
    result = BGQDB::getBlockIds("where status <> 'F'", allBlocks);
    if (result != BGQDB::OK) {
        LOG_ERROR_MSG( "Getting non-free blocks failed with error " << result );
        return;
    }
    LOG_DEBUG_MSG( "Found " << allBlocks.size() << " blocks to process." );

    // Get a list of all blocks specified on the reconnect-blocks startup parameter,
    // but only if we have reconnect true and bringup false. We have already
    // checked to make sure that both aren't true.
    if (Properties::getProperty(RECONNECT_BLOCKS) == "true" &&
        Properties::getProperty(BRINGUP) == "false")
    {
        reconnectBlocks = allBlocks;
    } else {
        LOG_INFO_MSG( "Block reconnection is disabled." );
    }

    // Create a list of all blocks to be reconnected
    for (std::vector<std::string>::iterator it = reconnectBlocks.begin(); it != reconnectBlocks.end(); )
    {
        const std::string blockName(*it);

        // Get the current block status
        if ((result = BGQDB::getBlockStatus(blockName, blockState)) != BGQDB::OK) {
            LOG_ERROR_MSG("Getting block status for "  << blockName << " failed with error " << result);
            ++it;
            continue;
        }

        // If the block is not fully initialized, remove it from the reconnectBlocks list
        if (blockState != BGQDB::INITIALIZED) {
            LOG_DEBUG_MSG( "Not reconnecting to " << blockName << " with status " << BGQDB::blockStatusToString(blockState) );
            it = reconnectBlocks.erase(it);
        } else {
            ++it;
        }
    }

    // Create a list of all blocks to be freed
    for (std::vector<std::string>::const_iterator it = allBlocks.begin(); it != allBlocks.end(); ++it)
    {
        const std::string blockName(*it);

        // Get the current block state
        if ((result = BGQDB::getBlockStatus(blockName, blockState)) != BGQDB::OK) {
            LOG_ERROR_MSG("Getting block status for " << blockName << " failed with error " << result);
            continue;
        }

        // If the block is not in the reconnectBlocks list, then add it to the freeBlock list
        if (find(reconnectBlocks.begin(), reconnectBlocks.end(), blockName) == reconnectBlocks.end())
            freeBlocks.push_back(blockName);
    }

    // Try to reconnect to blocks in the reconnectBlocks list.
    // If reconnect fails, add the block to the freeBlock list
    typedef std::map< DBBlockPtr,boost::shared_ptr<DBConsoleController> > BlockContainer;
    BlockContainer blocks_to_connect;
    for (std::vector<std::string>::const_iterator it = reconnectBlocks.begin(); it != reconnectBlocks.end(); ++it) {
        const std::string blockName(*it);

        // Get user
        std::string username;
        int qualifier;
        BGQDB::STATUS result = BGQDB::getBlockUser(blockName, username, qualifier);
        if (result != BGQDB::OK) {
            LOG_ERROR_MSG("Getting user for block " << blockName << " failed with error " << result);
            freeBlocks.push_back(blockName);
            continue;
        }

        // Create console controller
        bgq::utility::UserId uid;
        try {
            const bgq::utility::UserId myuid( username );
            uid = myuid;
        } catch (const std::exception& e) {
            LOG_ERROR_MSG("Cannot get user id. " << e.what());
            freeBlocks.push_back(blockName);
            continue;
        }

        const boost::shared_ptr<DBConsoleController> consoleController( new DBConsoleController(commandProcessor, uid) );

        // Select the block
        std::deque<std::string> args; args.push_back(blockName);
        mmcs_client::CommandReply reply;
        consoleController->selectBlock(args, reply, true);
        if (reply.getStatus()) { // Can't create block
            LOG_ERROR_MSG("selectBlock(" << blockName << ") failed: " << reply.str());
            freeBlocks.push_back(blockName);
            continue;
        }
        const DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(consoleController->getBlockHelper());

        // Remember our boot cookie for RAS events
        pBlock->getBase()->setBootCookie(qualifier);

        // Create the BlockController and targets
        pBlock->create_block(args, reply);
        if (reply.getStatus()) { // Can't create block
            LOG_ERROR_MSG("create_block(" << blockName << ") failed: " << reply.str());
            freeBlock( blockName, username, mcServer );
            consoleController->deselectBlock();
            continue;
        }

        // Create an MCServer connection and an MCServer target for all of the block resources,
        // and connect to the targets in the block
        if (!pBlock->getBase()->isIoBlock()) {
            // Only actually connect compute blocks. I/O blocks are disconnected when INITIALIZED.
            blocks_to_connect[pBlock] = consoleController;
        } else {
            pBlock->getBase()->setReconnected();
        }

        if (!pBlock->getBase()->isIoBlock()) {
            // Add I/O node to compute mapping
            DefaultListener::get()->add( pBlock->getBase()->getBlockNodeConfig() );
        }
    }

    if (message_type == FAILOVER_MSG) {
        LOG_INFO_MSG("Reconnecting: sending failover messages to the subnets");
        // Before reconnecting, send a list of the hardware associated with booted blocks
        std::vector<std::string> subnets;
        Properties::getSubnetNames(subnets);
        boost::thread_group g;
        BOOST_FOREACH(const std::string& curr_subnet, subnets) {
            g.create_thread(
                    boost::bind(
                        &subnetRestartThread, curr_subnet, reconnectBlocks
                        )
                    );
        }
        g.join_all();
        LOG_DEBUG_MSG( subnets.size() << " subnet restart thread" << (subnets.size() == 1 ? "" : "s") << " joined" );
    }

    // Now connect to the compute blocks.
    std::deque<std::string> connectArgs;
    connectArgs.push_back(Properties::getProperty(DFT_TGTSET_TYPE));
    for (BlockContainer::const_iterator it = blocks_to_connect.begin(); it != blocks_to_connect.end(); ++it) {
        const DBBlockPtr curr_block = it->first;
        if (std::find(freeBlocks.begin(), freeBlocks.end(), curr_block->getBlockName()) != freeBlocks.end()) {
            continue; // Skip it if it's in the free list
        }
        const boost::shared_ptr<DBConsoleController> console( it->second );
        mmcs_client::CommandReply reply;
        BlockControllerTarget target(curr_block->getBase(), "{*}", reply);
        curr_block->getBase()->connect(connectArgs, reply, &target);

        if (reply.getStatus() && !curr_block->getBase()->isIoBlock()) {
            LOG_ERROR_MSG("connect(" << curr_block->getBlockName() << ") failed: " << reply.str());
            curr_block->setBlockStatus(BGQDB::FREE);
            curr_block->getBase()->setDisconnecting(true, reply.str()); // reset icon connections and stop mailbox
            console->deselectBlock();
            freeBlocks.push_back(curr_block->getBlockName());
        } else {
            LOG_INFO_MSG("Block " << curr_block->getBlockName() << " successfully reconnected.");
        }
        curr_block->getBase()->setReconnected();
    }

    // Finally, free blocks that will not be reconnected
    for (std::vector<std::string>::const_iterator it = freeBlocks.begin(); it != freeBlocks.end(); ++it) {
        const std::string blockName(*it);

        // Get the block owner
        std::string username;
        int qualifier;
        result = BGQDB::getBlockUser(blockName, username, qualifier);
        if (result != BGQDB::OK) {
            LOG_ERROR_MSG("Getting user for block " << blockName << " failed with error " << result);
            // Continue without a userid
            username.clear();
        }

        // Delete the target set and free the block
        freeBlock(blockName, username, mcServer);

        // Clear any pending actions for blocks we are not reconnecting
        result = BGQDB::clearBlockAction(blockName);
        if ( result != BGQDB::OK ) {
            LOG_ERROR_MSG("clearBlockAction(" << blockName << ") failed with return code= " << result);
        }
    }

    // Kill the jobs associated with hardware in the error list.
    std::set<BGQDB::job::Id> jobs;
    BOOST_FOREACH(const std::string& location, error_list) {
        LOG_DEBUG_MSG("Freeing blocks and jobs for " << location);
        std::vector<BGQDB::job::Id> temp;
        const BGQDB::STATUS result = BGQDB::killMidplaneJobs(location, &temp, true);
        if (result != BGQDB::OK) {
            LOG_WARN_MSG( "BGQDB::killMidplaneJobs(" << location << ") failed: " << result );
        } else {
            BOOST_FOREACH( const BGQDB::job::Id job, temp ) {
                jobs.insert( job );
            }
        }
    }

    BOOST_FOREACH( const BGQDB::job::Id job, jobs ) {
        RunJobConnection::instance().kill(job, SIGKILL, "Failed block reconnection");
    }

    // Now free the blocks.
    BOOST_FOREACH(const std::string& blockid, special_free) {
        // If this is an I/O block, we've got to free the computes too.
        std::vector<std::string> compute_blocks;
        const BGQDB::STATUS result = BGQDB::checkIOBlockConnection(blockid, &compute_blocks);
        if (result != BGQDB::OK) {
            LOG_ERROR_MSG(DBBlockController::strDBError(result) << ": Database cannot determine connected compute blocks.");
        }
        // Stick it in with the compute blocks and free 'em all in one loop.
        compute_blocks.push_back(blockid);
        BOOST_FOREACH(const std::string& block, compute_blocks) {
            const BGQDB::STATUS result = BGQDB::setBlockAction(block, BGQDB::DEALLOCATE_BLOCK);
            if (result == BGQDB::OK) {
                LOG_INFO_MSG("Block action for block " << blockid << " was set to DEALLOCATE.");
            } else if (result == BGQDB::DUPLICATE) {
                // Still ok
            } else {
                LOG_ERROR_MSG("Could not set block action to DEALLOCATE for block " << blockid);
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
}

void
freeBlock(
        const std::string& blockName,
        const std::string& userName,
        const MCRefPtr& mcServer
        )
{
    // Delete the target set
    deleteTargetSet(blockName, userName, mcServer);

    // Mark the specified block as free
    std::deque<std::string> errmsg;
    errmsg.push_back("errmsg=block freed by reconnect_blocks");
    // The block actions happen during startup serially here so we don't need to worry
    // about other threads racing to the static unlocked BGQDB method.
    const BGQDB::STATUS result = BGQDB::setBlockStatus(blockName, BGQDB::FREE, errmsg);
    if (result == BGQDB::OK) {
        LOG_INFO_MSG("Block " << blockName << " was successfully freed.");
    } else {
        BGQDB::BLOCK_STATUS blockState;
        BGQDB::getBlockStatus(blockName, blockState);
        LOG_ERROR_MSG("Setting block status to FREE for block " << blockName << " failed with return code= " << result
                << ", current block status is " << BGQDB::blockStatusToString(blockState));
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

    // Open the target set in Control mode
    MCServerMessageSpec::OpenTargetRequest mcOpenTargetSetRequest(
            blockName,
            userName,
            WUAR,
            true
            );
    MCServerMessageSpec::OpenTargetReply mcOpenTargetSetReply;
    try {
        mcServer->openTarget(mcOpenTargetSetRequest, mcOpenTargetSetReply);
    } catch (const std::exception& e) {
        mcOpenTargetSetReply._rc = -1;
        mcOpenTargetSetReply._rt = e.what();
    }

    if (mcOpenTargetSetReply._rc) {
        LOG_DEBUG_MSG("openTargetSet: " << mcOpenTargetSetReply._rt);
        LOG_ERROR_MSG("Unable to open target set " << blockName);
        return;
    }

    LOG_DEBUG_MSG("mc_server target set " << blockName << " opened for " << userName);

    MCServerMessageSpec::CloseTargetRequest mcCloseTargetRequest(
            MCServerAPIHelpers::createCloseRequest( mcOpenTargetSetRequest, mcOpenTargetSetReply)
            );
    MCServerMessageSpec::CloseTargetReply mcCloseTargetReply;
    try {
        mcServer->closeTarget(mcCloseTargetRequest, mcCloseTargetReply);
    } catch (const std::exception& e) {
        mcCloseTargetReply._rc = -1;
        mcCloseTargetReply._rt = e.what();
    }

    if (mcCloseTargetReply._rc) {
        LOG_DEBUG_MSG("closeTargetSet: " << mcCloseTargetReply._rt);
        LOG_ERROR_MSG("Unable to close target set " << blockName);
        return;
    }

    LOG_DEBUG_MSG("mc_server target set " << blockName << " closed.");

    // Delete the target set
    MCServerMessageSpec::DeleteTargetSetRequest mcDeleteTargetSetRequest(blockName, userName);
    MCServerMessageSpec::DeleteTargetSetReply   mcDeleteTargetSetReply;

    try {
        mcServer->deleteTargetSet(mcDeleteTargetSetRequest, mcDeleteTargetSetReply);
    } catch (const std::exception& e) {
        mcDeleteTargetSetReply._rc = -1;
        mcDeleteTargetSetReply._rt = e.what();
    }

    if (mcDeleteTargetSetReply._rc) {
        LOG_DEBUG_MSG("deleteTargetSet: " << mcDeleteTargetSetReply._rt);
        LOG_ERROR_MSG("Unable to delete target set " << blockName);
        return;
    }

    LOG_DEBUG_MSG("mc_server target set " << blockName << " deleted.");
}

} } // namespace mmcs::server
