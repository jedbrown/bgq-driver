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

#include <string>
#include <vector>
#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/scope_exit.hpp>
#include <utility/include/cxxsockets/SocketTypes.h>
#include <control/include/mcServer/MCServerAPIHelpers.h>
#include <db/include/api/BGQDBlib.h>
#include <db/include/api/job/types.h>
#include <utility/include/Log.h>
#include <hlcs/include/runjob/commands/KillJob.h>
#include "DefaultControlEventListener.h"
#include "DBConsoleController.h"
#include "DBBlockController.h"
#include "MMCSProperties.h"
#include "MMCSCommandProcessor.h"
#include "MMCSServerParms.h"
#include "MMCSDBMonitor.h"
#include "CNBlockController.h"
#include "ReconnectBlocks.h"
#include "RunJobConnection.h"
#include "MMCSMasterMonitor.h"
#include "DBStatics.h"

using namespace MCServerMessageSpec;

LOG_DECLARE_FILE( "mmcs" );

boost::mutex error_write_mutex;
std::vector<std::string> error_list;

boost::mutex spec_write_mutex;
std::vector<std::string> special_free;

extern boost::mutex            reconnect_lock;
extern bool reconnect_done;
extern boost::condition_variable reconnect_notifier;
extern MCServerMessageType message_type;
extern bool subnets_home;

void subnetRestartThread(const std::string& curr_subnet, 
                         const std::vector<std::string>& all_blocks,
                         boost::shared_ptr<boost::barrier> restart_barrier) {
    LOG_INFO_MSG(__FUNCTION__ << " 0x" << std::hex << pthread_self());
    MCServerMessageSpec::FailoverRequest failreq;
    MCServerMessageSpec::FailoverReply failrep;
    BinaryId b = MasterMonitor::_alias_binary_map.at(curr_subnet);
    if(AliasWaiter::buildFailover(failreq, failrep, curr_subnet, b) == false) {
        LOG_FATAL_MSG("Cannot build a proper failover message.  " 
                      << "Database state may be inconsistent.  "
                      << "mmcs_server ending.");
        exit(EXIT_FAILURE);
    }
    MCServerRef* failref = 0;
    MMCSCommandReply failreply;
    BlockControllerBase::mcserver_connect(failref, curr_subnet, failreply);
    if(failref) {
        LOG_INFO_MSG("Sending failover request to mc_server for " << curr_subnet);
        failref->failover(failreq, failrep);
    } else if (!failref || failreply.getStatus() != 0) {
        // Could not connect to mc_server.  That's a termination condition.
        LOG_FATAL_MSG("Could not connect to mc_server to send a failover message.  " 
                      << "mmcs_server ending.");
        exit(EXIT_FAILURE);
    } 

    bool good = true;
    std::vector<std::string> hw_to_unmark;

    if(failrep._rc != 0) {
        MCServerRefPtr refp(failref);
        good = AliasWaiter::sendInitialize(refp, hw_to_unmark, failrep);
        // Anything that now has hardware in error needs to be freed.
        BOOST_FOREACH(std::string blockname, all_blocks) {
            std::vector<std::string> curr_errors;
            BGQDB::queryMissing(blockname, curr_errors);
            // Get the locations necessary to kill the jobs.
            BOOST_FOREACH(std::string& error, curr_errors) {
                std::string trimmed_error = error.substr(error.find_last_of("R"));
                if(trimmed_error.length() == 0) {
                    // It might be an IO rack.
                    trimmed_error = error.substr(error.find_last_of("Q"));
                }
                LOG_DEBUG_MSG("Adding " << trimmed_error << " to error list.");
                boost::mutex::scoped_lock sl(error_write_mutex);
                error_list.push_back(trimmed_error);
            }
            // Now just get the block names.
            if(curr_errors.size() != 0) {
                boost::mutex::scoped_lock sl(spec_write_mutex);
                special_free.push_back(blockname);
                LOG_DEBUG_MSG("Found block " << blockname << " to free.");
            }
        }
    }

    std::ostringstream dumpstr;
    failreq.write(dumpstr);
    LOG_INFO_MSG("Sent failover messages to mc_server for " << curr_subnet);

    // Mark any nodes that were set in error during reconnect to 'A'.
    if(hw_to_unmark.size() > 0) {
        typedef MCServerMessageSpec::FailoverReply::NodeCard ReplyNodeCard;
        typedef MCServerMessageSpec::FailoverReply::IoCard ReplyIoCard;
        typedef MCServerMessageSpec::FailoverReply::ServiceCard ReplyServiceCard;

        std::vector<std::string> ncs;
        BOOST_FOREACH(ReplyNodeCard& nc, failrep._nodeCards) {
            if(std::find(hw_to_unmark.begin(), hw_to_unmark.end(), nc._location) != hw_to_unmark.end())
                ncs.push_back(nc._location);
        }
        DBStatics::setLocationStatus(ncs, failreply, DBStatics::AVAILABLE, DBStatics::COMPUTECARD);

        std::vector<std::string> iocs;
        BOOST_FOREACH(ReplyIoCard& ioc, failrep._ioCards) {
            if(std::find(hw_to_unmark.begin(), hw_to_unmark.end(), ioc._location) != hw_to_unmark.end())
                iocs.push_back(ioc._location);
        }
        DBStatics::setLocationStatus(iocs, failreply, DBStatics::AVAILABLE, DBStatics::IOCARD);

        std::vector<std::string> scs;
        BOOST_FOREACH(ReplyServiceCard& sc, failrep._serviceCards) {
            if(std::find(hw_to_unmark.begin(), hw_to_unmark.end(), sc._location) != hw_to_unmark.end())
                scs.push_back(sc._location);
        }
        DBStatics::setLocationStatus(scs, failreply, DBStatics::AVAILABLE, DBStatics::SERVICECARD);
    }
    restart_barrier->wait();
}


void
reconnectBlocks(
        MMCSServerParms *mmcsParms,
        MMCSCommandReply& reply,
        MMCSCommandProcessor* commandProcessor
        )
{
    BGQDB::BLOCK_STATUS blockState;
    std::vector<string> allBlocks;
    std::vector<string> reconnectBlocks;
    std::vector<string> freeBlocks;
    string whereClause = "";
    MCRefPtr mcServer;

    LOG_INFO_MSG("reconnecting/freeing blocks and target sets");

    BOOST_SCOPE_EXIT( (&reconnect_done) ) {
#ifdef WITH_DB
        reconnect_done = true;
        reconnect_notifier.notify_all();
#endif
    } BOOST_SCOPE_EXIT_END;

    // get a list of all blocks defined in the database
    BGQDB::STATUS result;
    result = BGQDB::getBlockIds(whereClause, allBlocks);
    if (result != BGQDB::OK)
    {
        reply << FAIL << "getBlockIds() failed with error " << result << DONE;
        return;
    }

    // Get a list of all blocks specified on the reconnect-blocks startup parameter,
    // but only if we have reconnect true and bringup false.  We have already
    // checked to make sure that both aren't true.
    if (MMCSProperties::getProperty(RECONNECT_BLOCKS) == "true" &&
        MMCSProperties::getProperty(BRINGUP) == "false")
    {
        reconnectBlocks = allBlocks;
    }

    bool blocks_are_booted = false;
    // create a list of all blocks to be reconnected
    for (std::vector<string>::iterator it = reconnectBlocks.begin(); it != reconnectBlocks.end(); ++it)
    {
        string blockName(*it);

        // get the current block state
        if ((result = BGQDB::getBlockStatus(blockName, blockState)) != BGQDB::OK)
        {
            LOG_ERROR_MSG("getBlockStatus(" << blockName << ") failed with error " << result);
            continue;
        }

        // if the block is not fully initialized, remove it from the reconnectBlocks list
        if (blockState != BGQDB::INITIALIZED)
            *it = "";		// replace reconnectBlocks vector element with null string
        else blocks_are_booted = true;
    }

    // remove null string elements from reconnectBlocks vector
    // see "The C++ Standard Library: A Tutorial and Reference" (1999), by Nicolai M. Josuttis. Section 5.6.1
    // for an explanation of the following statment
    // Alternatively, see Meyer's Effective STL item 33 or google "erase remove idiom".
    reconnectBlocks.erase(remove(reconnectBlocks.begin(),reconnectBlocks.end(),""), reconnectBlocks.end());

    // create a list of all blocks to be freed
    for (std::vector<string>::iterator it = allBlocks.begin(); it != allBlocks.end(); ++it)
    {
        string blockName(*it);

        // get the current block state
        if ((result = BGQDB::getBlockStatus(blockName, blockState)) != BGQDB::OK)
        {
            LOG_ERROR_MSG("getBlockStatus(" << blockName << ") failed with error " << result);
            continue;
        }

        // if the block is already free, continue with the next block
        if (blockState == BGQDB::FREE)
            continue;

        BGQDB::BlockInfo bInfo;
        if ((result = BGQDB::getBlockInfo(blockName, bInfo)) != BGQDB::OK) {
            LOG_ERROR_MSG("getBlockInfo(" << blockName << ") failed with error " << result);
            continue;
        }

        // if the block is not in the reconnectBlocks list, then add it to the freeBlock list
        if (find(reconnectBlocks.begin(), reconnectBlocks.end(), blockName) == reconnectBlocks.end())
            freeBlocks.push_back(blockName);
    }

    // try to reconnect to blocks in the reconnectBlocks list
    // if reconnect fails, add the block to the freeBlock list
    std::map<DBBlockPtr,DBConsoleController*> blocks_to_connect;
    for (std::vector<string>::iterator it = reconnectBlocks.begin(); it != reconnectBlocks.end(); ++it)
    {
        string blockName(*it);

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
            bgq::utility::UserId myuid( username );
            uid = myuid;
        } catch(std::exception& e) {
            LOG_ERROR_MSG("Can't get user id. " << e.what());
            freeBlocks.push_back(blockName);
            continue;
        }
        
        DBConsoleController consoleController(commandProcessor, uid);

        //  select the block
        std::deque<string> args; args.push_back(blockName);
        consoleController.selectBlock(args, reply, true);
        if (reply.getStatus() < 0)		// can't create block
        {
            LOG_ERROR_MSG("selectBlock(" << blockName << ") failed: " << reply.str());
            freeBlocks.push_back(blockName);
            continue;
        }
        DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController> (consoleController.getBlockHelper());	// get selected block

        // remember our boot cookie for RAS events
        pBlock->getBase()->setBootCookie(qualifier);

        // create the BlockController and targets
        pBlock->create_block(args, reply);
        if (reply.getStatus() < 0)		// can't create block
        {
            LOG_ERROR_MSG("create_block(" << blockName << ") failed: " << reply.str());
            freeBlocks.push_back(blockName);
            continue;
        }

        // Create an MCServer connection and an MCServer target for all of the block resources,
        // and connect to the targets in the block
        if(pBlock->getBase()->isIOBlock() == false)
            // Only actually connect compute blocks.  IO blocks are disconnected when INITIALIZED.
            blocks_to_connect[pBlock] = &consoleController;
        else pBlock->getBase()->setReconnected();
    }

    if(message_type == FAILOVER_MSG) {
        LOG_INFO_MSG("Reconnecting: sending failover messages to the subnets");
        // Before reconnecting, send a list of the hardware associated with booted blocks
        std::vector<std::string> subnets;
        MMCSProperties::getSubnetNames(subnets);
        boost::shared_ptr<boost::barrier> restart_barrier(new boost::barrier(subnets.size() + 1));
        BOOST_FOREACH(std::string curr_subnet, subnets) {
            boost::thread srt(&subnetRestartThread, curr_subnet, reconnectBlocks, restart_barrier);
        }
        restart_barrier->wait();
    }

    // Now connect to the compute blocks.
    deque<string> connectArgs;
    connectArgs.push_back(MMCSProperties::getProperty(DFT_TGTSET_TYPE));
    for(std::map<DBBlockPtr,DBConsoleController*>::iterator it = blocks_to_connect.begin();
        it != blocks_to_connect.end(); ++it) {
        DBBlockPtr curr_block = it->first;
        if(std::find(freeBlocks.begin(), freeBlocks.end(), curr_block->getBlockName()) != freeBlocks.end()) {
            continue; // Skip it if it's in the free list
        }
        DBConsoleController* console = it->second;
        BlockControllerTarget target(curr_block->getBase(), "{*}", reply);
        curr_block->getBase()->connect(connectArgs, reply, &target);

        if (reply.getStatus() != 0 && curr_block->getBase()->isIOBlock() == false)
        {
            LOG_ERROR_MSG("connect(" << curr_block->getBlockName() << ") failed: " << reply.str());
            curr_block->setBlockStatus(curr_block->getBlockName(), BGQDB::FREE);
            curr_block->getBase()->setDisconnecting(true, reply.str()); // reset icon connections and stop mailbox
            console->deselectBlock();
            freeBlocks.push_back(curr_block->getBlockName());
        }
        else
            LOG_INFO_MSG("block " << curr_block->getBlockName() << " successfully reconnected");
        curr_block->getBase()->setReconnected();
    }

    //    bool blocks_active = areBlocksActive();

    // Finally, free blocks that will not be reconnected
    for (std::vector<string>::iterator it = freeBlocks.begin(); it != freeBlocks.end(); ++it)
    {
        string blockName(*it);

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

    // Kill the jobs associated with hardware in the error list.
    BOOST_FOREACH(std::string& location, error_list) {
        LOG_DEBUG_MSG("Freeing blocks and jobs for " << location);
        std::vector<BGQDB::job::Id> jobs;
        // First get the jobs to kill.
        int result = BGQDB::killMidplaneJobs(location, &jobs, true);
        if (result != BGQDB::OK) {
            LOG_WARN_MSG( "error on BGQDB::KillMidplaneJobs trying to free blocks");
        }
        // Kill the jobs
        BOOST_FOREACH(BGQDB::job::Id& job, jobs) {
            if(RunJobConnection::Kill(job, 9) != 0) {
                LOG_ERROR_MSG("error contacting runjob server to kill jobs");
            }
        }
    }


    // Now free the blocks.
    BOOST_FOREACH(std::string blockid, special_free) {
        // If this is an IO block, we've got to free the computes, too.
        std::vector<std::string> compute_blocks;
        int result = BGQDB::checkIOBlockConnection(blockid, &compute_blocks);
        if(result != BGQDB::OK) {
            LOG_ERROR_MSG(DBBlockController::strDBError(result)
                          << ": Database cannot determine connected compute blocks.");
        }
        // Stick it in with the compute blocks and free 'em all in one loop.
        compute_blocks.push_back(blockid);
        BOOST_FOREACH(std::string& block, compute_blocks) {
            result = BGQDB::setBlockAction(block, BGQDB::DEALLOCATE_BLOCK);
            if (result == BGQDB::OK) {
                LOG_INFO_MSG("block " << blockid << " set to DEALLOCATE");
            } else {
                LOG_ERROR_MSG("Could not deallocate block " << blockid 
                              << " .");
            }
        }
    }

    if(message_type == DIE_MSG) {
        // If we aren't reconnecting and we have some blocks that are booted or "in flight"
        // then mc_server needs to be told this so we can restart the world.
        FailoverRequest failreq;
        FailoverReply failrep;
        failreq._bootedHardware._failed_subnet_id = "MC_SERVER_DIE";
        MCServerRef* failref = 0;
        MMCSCommandReply failreply;
        BlockControllerBase::mcserver_connect(failref, "mc_server_die", failreply);
        if(failref) {
            LOG_INFO_MSG("Sending failover request to mc_server to make it die.");
            failref->failover(failreq, failrep);
        } else if (!failref || failreply.getStatus() != 0) {
            // Could not connect to mc_server.  That's a termination condition.
            LOG_FATAL_MSG("Could not connect to mc_server to send a failover message.  " 
                          << "mmcs_server ending.");
            exit(EXIT_FAILURE);
        } 
    }
    LOG_INFO_MSG("reconnecting/freeing blocks and target sets complete");
    reply << OK << DONE;
}

void
freeBlock(
        const std::string& blockName,
        const std::string& userName,
        MCRefPtr& mcServer
        )
{
    // delete the target set
    deleteTargetSet(blockName, userName, mcServer);

    // mark the specified block as free
    int result;
    deque<string> errmsg; errmsg.push_back("errmsg=block freed by reconnect_blocks");
    // The block actions happen during startup serially here
    // so we don't need to worry about other threads racing 
    // to the static unlocked BGQDB method.
    result = BGQDB::setBlockStatus(blockName, BGQDB::FREE, errmsg);
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
        MCRefPtr& mcServer
        )
{
    int targetsetHandle; // mcServer handle returned on OpenTargetSetReply

    MMCSCommandReply rep;
    MCServerRef* ref = mcServer.get();
    // get an mcServer connection
    BlockControllerBase::mcserver_connect(ref, userName, rep);
    if (rep.getStatus() != 0) {
        return;
    }

    mcServer.reset(ref);

    //
    // open the target set in Control mode
    //
    MCServerMessageSpec::OpenTargetRequest mcOpenTargetSetRequest(
            blockName,
            userName,  		// user name
            WUAR,
            true);     		// autoclose
    MCServerMessageSpec::OpenTargetReply   mcOpenTargetSetReply;
    try
    {
        mcServer->openTarget(mcOpenTargetSetRequest, mcOpenTargetSetReply);
    }
    catch (exception &e)
    {
        mcOpenTargetSetReply._rc = -1;
        mcOpenTargetSetReply._rt = e.what();
    }
    if (mcOpenTargetSetReply._rc)
    {
        LOG_DEBUG_MSG("openTargetSet: " << mcOpenTargetSetReply._rt);
        LOG_ERROR_MSG("unable to delete target set " << blockName);
        targetsetHandle = -1;
        return;
    }
    else
    {
        LOG_DEBUG_MSG("mcServer target set " << blockName << " opened for " << userName);
        targetsetHandle=mcOpenTargetSetReply._handle;
    }
}

