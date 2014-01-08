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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#include <algorithm>
#include <control/include/mcServer/MCServerRef.h>
#include <control/include/mcServer/MCServer_errno.h>
#include <control/include/mcServer/MCServerAPIHelpers.h>
#include "MMCSMasterMonitor.h"
#include "MMCSProperties.h"
#include "DBConsoleController.h"
#include "DBBlockController.h"
#include "HardwareBlockList.h"
#include "DBStatics.h"
#include "RunJobConnection.h"

LOG_DECLARE_FILE("mmcs");

const int WAIT_TIMEOUT = 600;
const std::string tsetname = "ReInitializeSet";

// Static object instantiations
WaiterList MasterMonitor::_waitlist;
MCServerRefPtr MasterMonitor::_server;
std::vector<std::string> MasterMonitor::_bringup_options;
std::map<std::string,BinaryId> MasterMonitor::_alias_binary_map;
boost::condition_variable MasterMonitor::_startup_notifier;
boost::mutex MasterMonitor::_startup_lock;
bool MasterMonitor::_started = false;

void WaiterList::setStat(const std::string& alias, Status stat) {
    PthreadMutexHolder _holder(&_list_lock);
    LOG_DEBUG_MSG(alias << " status set to " << stat);
    _list[alias] = stat;
}

void* MasterWaiter::threadStart() {
    // Open a client API connect to bgmaster_server
    // Needs to get master location from properties and command line
    bgq::utility::PortConfiguration::Pairs portpairs;
    LOG_INFO_MSG("Starting subnet_mc termination waiter for " << _alias);
    bgq::utility::ClientPortConfiguration port_config(32042);
    port_config.setProperties(MMCSProperties::getProperties(), "master.client");
    port_config.notifyComplete();
    portpairs = port_config.getPairs();
    if(portpairs[0].first.length() == 0) {
        LOG_FATAL_MSG("No port pairs or invalid port pairs specified");
        exit(1);
    }

    BGMasterClient client;
    bgq::utility::Properties::Ptr tprop = MMCSProperties::getProperties();
    client.initProperties(tprop);
    while(isThreadStopping() == false) {
        try {
            client.connectMaster(portpairs);
            LOG_DEBUG_MSG("Connected to bgmaster_server");
        } catch(CxxSockets::CxxError& e) {
            LOG_ERROR_MSG("Socket error detected: " << e.errcode << " " << e.what());

            // If we're spinning, do so slowly.
            sleep(1);
            continue;
        } catch (BGMasterExceptions::CommunicationError& e) {
            LOG_ERROR_MSG("Communication error detected: " << e.errcode << " " << e.what());
            sleep(1);
            continue;
        }

        break; // We're connected and it is started, so we're done with this loop.
    }

    // At this point, the alias is known to be started.
    MasterMonitor::_waitlist.setStat(_alias, WaiterList::RUNNING);

    bool abend = false;
    try {
        // So wait for it to end.
        if(client.wait_for_terminate(_bin_to_wait->get_binid()) < 0) {
            // Abnormally ended!  bgmaster_server might have gone
            // down.  We'll need to start waiting again.
            LOG_WARN_MSG("Wait for " << _alias << " ended prematurely."
                         << "  bgmaster_server may have gone down.  Will make hardware"
                         << " unavailable for new boots until bgmaster_server returns.");
            MasterMonitor::_waitlist.setStat(_alias, WaiterList::ERROR);
            abend = true;
        }
    } catch(BGMasterExceptions::BGMasterError& e) {
        if(e.errcode == BGMasterExceptions::INFO) {
            // Log it, but consider it a failure and restart.
            LOG_DEBUG_MSG("Soft communications error " << e.what() << " detected.");
        }
        LOG_ERROR_MSG("BGmaster connection failed: " << e.what()
                      << " No longer monitoring for " << _alias << " termination.");
        // Set the alias to error...
        MasterMonitor::_waitlist.setStat(_alias, WaiterList::ERROR);
        abend = true;
    }

    if(abend == false) {
        LOG_INFO_MSG("Terminated subnet mc detected");
        MasterMonitor::_waitlist.setStat(_alias, WaiterList::STOPPED);
        MasterMonitor::_waitlist.addBinId(_bin_to_wait->get_binid().str());
    }
    
    // We don't send a message to mc_server when it goes down, but we
    // do when it comes back up.

    // Find out which hardware this subnet is managing and put it
    // in an exclusion list.
    std::string hardware = MMCSProperties::getProperty((char*)(_alias.c_str()));
    LOG_INFO_MSG("Making " << hardware << " unavailable.");
    HardwareBlockList::add_to_list(hardware);

    LOG_INFO_MSG("Ending subnet_mc termination waiter for " << _alias);
    return 0;
}

void* AliasWaiter::threadStart() {
    LOG_INFO_MSG("Starting alias start waiter for " << _alias);
    MasterMonitor::_waitlist.setStat(_alias, WaiterList::WAIT_FOR_START);
    // Open a client connection to bgmaster_server and wait for an alias to start
    bgq::utility::ClientPortConfiguration port_config(32042);
    port_config.setProperties(MMCSProperties::getProperties(), "master.client");
    port_config.notifyComplete();
    bgq::utility::PortConfiguration::Pairs portpairs = port_config.getPairs();
    if(portpairs[0].first.length() == 0) {
        LOG_FATAL_MSG("No port pairs or invalid port pairs specified");
        exit(1);
    }

    BGMasterClient client;
    bgq::utility::Properties::Ptr tprop = MMCSProperties::getProperties();
    client.initProperties(tprop);
    while(isThreadStopping() == false) {
        try {
            client.connectMaster(portpairs);
        } catch(CxxSockets::CxxError& e) {
            LOG_ERROR_MSG("Socket error detected: " << e.errcode << " " << e.what());
            // If we're spinning, do so slowly.
            sleep(1);
            continue;
        } catch (BGMasterExceptions::CommunicationError& e) {
            LOG_ERROR_MSG("Communication error detected: " << e.errcode << " " << e.what());
        }
        break;
    }

    // Wait for the alias to start
    bool done = false;
    while(!done) {
        try {
            BinaryId newid = client.alias_wait(_alias, WAIT_TIMEOUT);
            // Check to make sure we didn't get an old one that is still going down.
            LOG_INFO_MSG("New id from bgmaster is " << newid.str());
            if(MasterMonitor::_waitlist.findBinId(newid.str()) != true) {
                done = true;
                _binid = newid;
                MasterMonitor::_alias_binary_map[_alias] = _binid;
                BOOST_FOREACH(MMCSSubnet& curr_subnet, MMCSProperties::_subnets) {
                    if(_alias == curr_subnet.get_name())
                        curr_subnet.set_home(_binid.get_host().ip());
                }
            } else {
                LOG_INFO_MSG("ID " << newid.str() << " is already in list.");
            }
        } catch(BGMasterExceptions::BGMasterError& e) {
            LOG_ERROR_MSG("Start waiter failed " << e.what() << " No longer monitoring for "
                          << _alias << " failover.");
            MasterMonitor::_waitlist.setStat(_alias, WaiterList::ERROR);
            return 0;
        }
        sleep(1);
    }

    std::vector<std::string> blocks_to_free;
    std::vector<std::string> error_list;
    if(_update_mc)
        updateMcServer(_alias, blocks_to_free, error_list, _binid);
    MasterMonitor::_waitlist.setStat(_alias, WaiterList::RUNNING);
    // Now return the hardware to the available pool
    std::string hardware = MMCSProperties::getProperty((char*)(_alias.c_str()));
    LOG_INFO_MSG("Making " << hardware << " available");
    HardwareBlockList::remove_from_list(hardware);

    // First, we've got to kill the jobs associated with any errored hardware.
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

    // Now free any blocks that could not be failed over.
    BOOST_FOREACH(std::string blockid, blocks_to_free) {
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
    LOG_INFO_MSG("Ending alias start waiter for " << _alias);
    return 0;
}

bool getIOCards(std::vector<std::string>& iocards) {
    LOG_INFO_MSG(__FUNCTION__);
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG("unable to connect to database");
        return false;
    }
        // Get list of IO drawers
    SQLHANDLE ioDrawers;
    SQLLEN index;
    SQLRETURN sqlrc, result;
    char ioDrawerLoc[7];
    string sqlstr;
    sqlstr.clear();
    sqlstr.append("select LOCATION from BGQIODrawer where status <> 'M' ");
    result = tx.execQuery( sqlstr.c_str(), &ioDrawers );
    SQLBindCol(ioDrawers, 1, SQL_C_CHAR, ioDrawerLoc, 7, &index);
    sqlrc = SQLFetch(ioDrawers);
    while ( sqlrc == SQL_SUCCESS )   {
        std::string ioloc(ioDrawerLoc);
        iocards.push_back(ioloc);
        sqlrc = SQLFetch(ioDrawers);
    }
    SQLCloseCursor(ioDrawers);
    return true;
}

bool getServiceCards(std::vector<std::string>& servicecards) {
    LOG_INFO_MSG(__FUNCTION__);
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG("unable to connect to database");
        return false;
    }
        // Get list of service cards.
    SQLHANDLE scards;
    SQLLEN index;
    SQLRETURN sqlrc, result;
    char scLoc[9];
    string sqlstr;
    sqlstr.clear();
    sqlstr.append("select LOCATION from BGQSERVICECARD where status <> 'M' ");
    result = tx.execQuery( sqlstr.c_str(), &scards );
    SQLBindCol(scards, 1, SQL_C_CHAR, scLoc, 9, &index);
    sqlrc = SQLFetch(scards);
    while (sqlrc == SQL_SUCCESS)   {
        std::string scloc(scLoc);
        LOG_TRACE_MSG("Adding Service Card " << scloc << " to list");
        servicecards.push_back(scloc);
        sqlrc = SQLFetch(scards);
    }
    SQLCloseCursor(scards);
    return true;
}

bool getIdleComputeCards(std::string& hw,
                         std::vector<std::string>& idle_compute_nodecards,
                         std::vector<std::string>& booted_nodecards) {
    LOG_INFO_MSG(__FUNCTION__);
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection())
    {
        LOG_ERROR_MSG("unable to connect to database");
        return false;
    }

    std::ostringstream sqlstrm;
    sqlstrm << "select MIDPLANEPOS,POSITION,STATUS from TBGQNODECARD";
    LOG_DEBUG_MSG("Sending query \"" << sqlstrm.str() << "\" to database.");
    SQLHANDLE hstmt;
    char midplanepos[17];
    char position[4];
    char status[2];

    SQLLEN index[3];
    SQLRETURN sqlresult = tx.execQuery( sqlstrm.str().c_str(), &hstmt);
    SQLBindCol(hstmt, 1, SQL_C_CHAR, &midplanepos, 17, &index[0]);
    SQLBindCol(hstmt, 2, SQL_C_CHAR, &position, 4, &index[1]);
    SQLBindCol(hstmt, 3, SQL_C_CHAR, &status, 2, &index[2]);
    sqlresult = SQLFetch(hstmt);
    while(sqlresult == SQL_SUCCESS) {
        // If the node card is available and in the subnet's hardware map, and
        // we haven't done so already, add it to the list.
        std::string mposstr(midplanepos);
        std::string posstr(position);
        std::string statstr(status);
        if(statstr == "A" && hw.find(mposstr.substr(0,3)) != std::string::npos) {
            std::string location = mposstr + "-" + posstr;
            // Also make sure we're not in the booted list.
            if(std::find(booted_nodecards.begin(), booted_nodecards.end(), location)
               != booted_nodecards.end()) {
            } else if(std::find(idle_compute_nodecards.begin(), idle_compute_nodecards.end(),
                                location) == idle_compute_nodecards.end()) {
                idle_compute_nodecards.push_back(location);
            }
        }
        // Get the next one.
        sqlresult = SQLFetch(hstmt);
    }
    SQLCloseCursor(hstmt);
    return true;
}

bool AliasWaiter::actuallySendMessages(MCServerRefPtr ref,
                                       MCServerMessageSpec::FailoverReply& failrep,
                                       std::vector<std::string>& markAvail) {
    LOG_INFO_MSG(__FUNCTION__);
    markAvail.clear();
    bool retval = true;

    MCServerMessageSpec::InitializeServiceCardsRequest servreq;
    servreq._set = tsetname;
    MCServerMessageSpec::InitializeServiceCardsReply servrep;
    servrep._rc = 0;

    MCServerMessageSpec::InitializeNodeCardsRequest ncreq;
    ncreq._set = tsetname;
    MCServerMessageSpec::InitializeNodeCardsReply ncrep;
    ncrep._rc = 0;

    MCServerMessageSpec::InitializeIoCardsRequest ioreq;
    ioreq._set = tsetname;
    MCServerMessageSpec::InitializeIoCardsReply iorep;
    iorep._rc = 0;

    typedef MCServerMessageSpec::FailoverReply::NodeCard ReplyNodeCard;
    typedef MCServerMessageSpec::FailoverReply::IoCard ReplyIoCard;
    typedef MCServerMessageSpec::FailoverReply::ServiceCard ReplyServiceCard;

    typedef MCServerMessageSpec::FailoverRequest::BootedHardware::NodeCard BootedRequestNodeCard;
    typedef MCServerMessageSpec::FailoverRequest::BootedHardware::IoCard BootedRequestIoCard;
    typedef MCServerMessageSpec::FailoverRequest::BootedHardware::ServiceCard BootedRequestServiceCard;

    typedef MCServerMessageSpec::FailoverRequest::IdleHardware::NodeCard IdleRequestNodeCard;
    typedef MCServerMessageSpec::FailoverRequest::IdleHardware::IoCard IdleRequestIoCard;
    typedef MCServerMessageSpec::FailoverRequest::IdleHardware::ServiceCard IdleRequestServiceCard;

    std::string user = "bgqadmin";
    MCServerMessageSpec::MakeTargetSetRequest mcMakeTargetSetRequest(tsetname, user, true);
    MCServerMessageSpec::MakeTargetSetReply mcMakeTargetSetReply;

    BOOST_FOREACH(ReplyNodeCard& nc, failrep._nodeCards) {
        // Loop through the failed reply node cards.  They all failed.
        // We need to initialize them.
        ncreq._location.push_back(nc._location);
        mcMakeTargetSetRequest._location.push_back(nc._location);
    }

    BOOST_FOREACH(ReplyIoCard& ioc, failrep._ioCards) {
        ioreq._location.push_back(ioc._location);
        mcMakeTargetSetRequest._location.push_back(ioc._location);
    }

    BOOST_FOREACH(ReplyServiceCard& sc, failrep._serviceCards) {
        servreq._location.push_back(sc._location);
        mcMakeTargetSetRequest._location.push_back(sc._location);
    }

    try
    {
	ref->makeTargetSet(mcMakeTargetSetRequest, mcMakeTargetSetReply);
    }
    catch (exception& e)
    {
	mcMakeTargetSetReply._rc = -1;
	mcMakeTargetSetReply._rt = e.what();
    }
    if (mcMakeTargetSetReply._rc)
    {
	if (mcMakeTargetSetReply._rc != BGERR_MCSERVER_TARGETSET_EXISTS)
	{
	    LOG_ERROR_MSG("makeTargetSet: " << mcMakeTargetSetReply._rt);
            return false;
	}
    }

    MCServerMessageSpec::OpenTargetRequest mcOpenTargetSetRequest(tsetname, user, MCServerMessageSpec::WAAR, true);     		// autoclose
    MCServerMessageSpec::OpenTargetReply   mcOpenTargetSetReply;
    try
    {
	ref->openTarget(mcOpenTargetSetRequest, mcOpenTargetSetReply);
    }
    catch (exception &e)
    {
	mcOpenTargetSetReply._rc = -1;
	mcOpenTargetSetReply._rt = e.what();
    }
    if (mcOpenTargetSetReply._rc)
    {
	LOG_ERROR_MSG("openTargetSet: " << mcOpenTargetSetReply._rt);
    }
    else
	LOG_INFO_MSG("mcServer target set " << tsetname << " opened");

    if(servreq._location.size() != 0)
        ref->initializeServiceCards(servreq, servrep);
    if(ncreq._location.size() != 0)
        ref->initializeNodeCards(ncreq, ncrep);
    if(ioreq._location.size() != 0)
        ref->initializeIoCards(ioreq, iorep);
    if(servrep._rc != 0 ||
       iorep._rc != 0 ||
       ncrep._rc != 0) {
        retval = false;
    } else retval = true;

    LOG_DEBUG_MSG("Sent request to reinitialize hardware.  Reply val is " << retval);

    // Now put all of the GOOD ones in the markAvail vector.
    BOOST_FOREACH(MCServerMessageSpec::ServiceCardReply& curr_rep, servrep._values) {
        if(curr_rep._error == 0) {
            markAvail.push_back(curr_rep._location);
        } else {
            LOG_DEBUG_MSG(curr_rep._location << " failed initialization. " 
                          << curr_rep._error);
        }
    }

    // Now put all of the GOOD ones in the markAvail vector.
    BOOST_FOREACH(MCServerMessageSpec::NodeCardReply& curr_rep, ncrep._values) {
        if(curr_rep._error == 0) {
            markAvail.push_back(curr_rep._location);
        } else {
            LOG_DEBUG_MSG(curr_rep._location << " failed initialization. " 
                          << curr_rep._error);
        }
    }

    // Now put all of the GOOD ones in the markAvail vector.
    BOOST_FOREACH(MCServerMessageSpec::IoCardReply& curr_rep, iorep._values) {
        if(curr_rep._error == 0) {
            markAvail.push_back(curr_rep._location);
        } else {
            LOG_DEBUG_MSG(curr_rep._location << " failed initialization. " 
                          << curr_rep._error);
        }
    }

    try
    {
        MCServerMessageSpec::CloseTargetRequest mcCloseTargetSetRequest = MCServerAPIHelpers::createCloseRequest( mcOpenTargetSetRequest, mcOpenTargetSetReply);
        MCServerMessageSpec::CloseTargetReply   mcCloseTargetSetReply;
        ref->closeTarget(mcCloseTargetSetRequest, mcCloseTargetSetReply);
        if (mcCloseTargetSetReply._rc) {
            LOG_ERROR_MSG("closeTargetSet: " << mcCloseTargetSetReply._rt);
        } else {
            LOG_DEBUG_MSG("mcServer target set " << tsetname << " closed");
        }

        // delete the target set
        MCServerMessageSpec::DeleteTargetSetRequest mcDeleteTargetSetRequest(tsetname, user);
        MCServerMessageSpec::DeleteTargetSetReply   mcDeleteTargetSetReply;
        ref->deleteTargetSet(mcDeleteTargetSetRequest, mcDeleteTargetSetReply);
        if (mcDeleteTargetSetReply._rc)
        {
            LOG_ERROR_MSG("deleteTargetSet: " << mcDeleteTargetSetReply._rt);
            LOG_ERROR_MSG("unable to delete target set " << tsetname);
        }
        else
            LOG_DEBUG_MSG("mcServer target set " << tsetname << " deleted");
    }
    catch (exception &e)
    {
        LOG_ERROR_MSG(e.what());
        LOG_ERROR_MSG("unable to delete target set " << tsetname);
    }
    return retval;
}

bool AliasWaiter::buildFailover(MCServerMessageSpec::FailoverRequest& failreq,
                                MCServerMessageSpec::FailoverReply& failrep,
                                std::string subnet_mc,
                                BinaryId& binid) {
    LOG_DEBUG_MSG(__FUNCTION__);
    // We'll send the id of the subnet_mc that failed, its backup, and some hardware information
    // We need to find all of the blocks running jobs and find their nodeboards.  Until the job
    // management infrastructure is ironed out, we'll just send all nodeboards.  So, find the
    // list of blocks...
    LOG_INFO_MSG("Alias startup detected.  Alerting mc_server.");
    std::string hw = "";
    if(subnet_mc != "reconnect") {// If it's not a reconnect, we're just doing one subnet.
        hw = MMCSProperties::getProperty(subnet_mc.c_str());
    } else {
        // It's a reconnect.  Get all hardware.
        MMCSProperties::getSubnets(hw);
    }

    if(binid.str().length() != 0) {
        // Add the eth to the failover request.
        BOOST_FOREACH(MMCSSubnet& msub, MMCSProperties::_subnets) {
            if(msub.get_name() == subnet_mc) {
                std::string subnet_ip = binid.get_host().ip();
                try {
                    failreq._ethAdapterForHw = msub.getBGEthForIP(subnet_ip);
                } catch(std::runtime_error& e) {
                    LOG_ERROR_MSG("Could not get BG HW Ethernet for address " << subnet_ip << ". " << e.what());
                    return false;
                }
            }
        }
    }

    LOG_DEBUG_MSG("Hardware in failed subnet " << subnet_mc << " is " << hw);
    PthreadMutexHolder holder;
    int muxrc = holder.Lock(&DBConsoleController::getBlockMapMutex());
    assert(muxrc == 0);
    LOG_INFO_MSG("There are " << DBConsoleController::getBlockMap().size() << " blocks in map.");

    std::vector<std::string> booted_compute_nodecards;
    std::vector<std::string> booted_compute_blocks;
    std::vector<std::string> io_nodecards;
    std::vector<std::string> loaded_cards;

    if (DBConsoleController::getBlockMap().size() != 0) {
        // Zip through the list of booted blocks and get the node cards for each block.
        DBBlockPtr curr_block;

        for(BlockMap::iterator it = DBConsoleController::getBlockMap().begin();
            it != DBConsoleController::getBlockMap().end(); ++it) {
            curr_block = boost::dynamic_pointer_cast<DBBlockController>(it->second);
            LOG_DEBUG_MSG("Found block " << curr_block->getBlockName() << " in map.");
            BGQDB::STATUS result;
            BGQDB::BLOCK_STATUS bState;
            // See if the block is actually booted, not just allocated somewhere.
            // We only want to reconnect to fully INITIALIZED blocks!
            if ((result = BGQDB::getBlockStatus(curr_block->getBlockName(), bState)) == BGQDB::OK) {
                if (bState != BGQDB::INITIALIZED) {
                    LOG_DEBUG_MSG("Block " << curr_block->getBlockName() << " is idle.");
                    continue;
                } else {
                    LOG_DEBUG_MSG("Block " << curr_block->getBlockName() << " added to booted list.");
                }
            }

            std::vector<std::string> io_nodes;
            std::vector<std::string> linkchips;
            for(vector<BCIconInfo*>::iterator iter = curr_block->getBase()->getIcons().begin();
                iter != curr_block->getBase()->getIcons().end(); ++iter) 	{
                std::string nodecard_loc = (*iter)->location();
                // If the node card is not in the hw list for this subnet, skip it.
                if(hw.find(nodecard_loc.substr(0,3)) == std::string::npos) {
                    continue;
                } else {
                    // If this nodecard location isn't in our list yet, add it.
                    if(std::find(booted_compute_nodecards.begin(), booted_compute_nodecards.end(), nodecard_loc) ==
                       booted_compute_nodecards.end() && ((BCNodecardInfo*)((*iter)))->isIOcard() == false) {
                        LOG_DEBUG_MSG("Putting node card " << nodecard_loc << " in booted list.");
                        booted_compute_nodecards.push_back(nodecard_loc);
                    }
                    else if(std::find(io_nodecards.begin(), io_nodecards.end(), nodecard_loc) ==
                            io_nodecards.end() && ((BCNodecardInfo*)((*iter)))->isIOcard() == true) {
                        LOG_DEBUG_MSG("Putting io card " << nodecard_loc << " in io list.");
                        io_nodecards.push_back(nodecard_loc);
                    }
                }
            }

            // Send booted_compute_nodecards and io_nodecards to BGQDB to get the jtag IDs.
            typedef std::map<std::string,uint32_t> JtagIdMap;
            JtagIdMap compute_idmap, io_idmap;
            BGQDB::STATUS rc = BGQDB::getJtagID(booted_compute_nodecards, compute_idmap);
            rc = BGQDB::getJtagID(io_nodecards, io_idmap);
            if(rc != BGQDB::OK) {
                LOG_ERROR_MSG("Unable to find jtag IDs for booted hardware");
                return false;
            }

            // Loop through the maps from BGQDB and add both the location and jtag id.
            // Now we've populated the nodecard vectors so add them to the request
            typedef JtagIdMap::iterator LJMapIterator;
            for(LJMapIterator it = compute_idmap.begin(); it != compute_idmap.end(); ++it) {
                MCServerMessageSpec::FailoverRequest::BootedHardware::NodeCard nc;
                nc._location = it->first;
                nc._computesJtagId = it->second;
                nc._blinksToBeMonitoredMask = 0xFFFFFFFF;
                nc._computesToBeMonitoredMask = 0xFFFFFFFF;
                if(std::find(loaded_cards.begin(), loaded_cards.end(), nc._location) == 
                   loaded_cards.end()) {
                    loaded_cards.push_back(nc._location);
                    failreq._bootedHardware._nodeCards.push_back(nc);
                }
            }
            for(LJMapIterator it = io_idmap.begin(); it != io_idmap.end(); ++it) {
                MCServerMessageSpec::FailoverRequest::BootedHardware::IoCard ioc;
                ioc._location = it->first;
                ioc._computesJtagId = it->second;
                ioc._blinksToBeMonitoredMask = 0xFFFFFFFF;
                ioc._computesToBeMonitoredMask = 0xFFFFFFFF;
                if(std::find(loaded_cards.begin(), loaded_cards.end(), ioc._location) == 
                   loaded_cards.end()) {
                    loaded_cards.push_back(ioc._location);
                    failreq._bootedHardware._ioCards.push_back(ioc);
                }
            }
        }
    }

    failreq._bootedHardware._failed_subnet_id = subnet_mc;
    std::string idle_subnet = "idle";
    std::vector<std::string> idle_compute_nodecards;
    if(getIdleComputeCards(hw, idle_compute_nodecards,
                           booted_compute_nodecards)) {
        BOOST_FOREACH(std::string& curr_nc, idle_compute_nodecards) {
            MCServerMessageSpec::FailoverRequest::IdleHardware::NodeCard nc(curr_nc);
            failreq._idleHardware._nodeCards.push_back(nc);
        }
    } else {
        return false;
    }
    
    std::vector<std::string> all_iocards;
    if(getIOCards(all_iocards) == false) {
        return false;
    }

    // We have the total IO.  Loop through it and if we find the card isn't
    // in the booted list, add it to the idle list.  For IO blocks,
    // we connect to ALL booted IO blocks.  We don't limit it to the 
    // ones with booted, connected, compute blocks.
    BOOST_FOREACH(std::string& curr_io_card, all_iocards) {
        if(std::find(io_nodecards.begin(), io_nodecards.end(),
                     curr_io_card) == io_nodecards.end()) {
            // Card isn't in the booted list, see if it's in the
            // list of hardware for this subnet.
            if(hw.find(curr_io_card.substr(0,3)) != std::string::npos) {
                failreq._idleHardware._ioCards.push_back(curr_io_card);
            }
        }
    }
    bool first = true;
    std::ostringstream optstrm;
    BOOST_FOREACH(std::string& curr_opt, MasterMonitor::_bringup_options) {
        if(first) {
            optstrm << curr_opt;
            first = false;
        } else {
            optstrm << "," << curr_opt;
        }
    }

    std::vector<std::string> service_cards;
    if(getServiceCards(service_cards) == false) {
        return false;
    } else {
        LOG_DEBUG_MSG("Adding " << service_cards.size() << " service cards to failover request.");
        BOOST_FOREACH(std::string& curr_sc, service_cards) {
            if(hw.find(curr_sc.substr(0,3)) == std::string::npos) {
                LOG_TRACE_MSG("Skipping service card " << curr_sc 
                              << " because it isn't in the failed subnet.");
                continue;  // This service card is not managed by the failed subnet.
            }
            MCServerMessageSpec::FailoverRequest::IdleHardware::ServiceCard sci(curr_sc);
            MCServerMessageSpec::FailoverRequest::BootedHardware::ServiceCard scb(curr_sc);
            failreq._idleHardware._serviceCards.push_back(sci);
            failreq._bootedHardware._serviceCards.push_back(scb);
        }
    }
    failreq._bringupOptions = optstrm.str();
    std::ostringstream debugstr;
    failreq.write(debugstr);
    LOG_TRACE_MSG(debugstr.str());
    return true;
}

bool AliasWaiter::sendInitialize(MCServerRefPtr ref, 
                                  std::vector<std::string>& hw_to_unmark, 
                                  MCServerMessageSpec::FailoverReply& failrep) {
    LOG_INFO_MSG(__FUNCTION__);
    typedef MCServerMessageSpec::FailoverReply::NodeCard ReplyNodeCard;
    typedef MCServerMessageSpec::FailoverReply::IoCard ReplyIoCard;
    typedef MCServerMessageSpec::FailoverReply::ServiceCard ReplyServiceCard;

    MMCSCommandReply reply;
    std::vector<std::string> ncs;
    BOOST_FOREACH(ReplyNodeCard& nc, failrep._nodeCards) {
        hw_to_unmark.push_back(nc._location);
        ncs.push_back(nc._location);
    }
    DBStatics::setLocationStatus(ncs, reply, DBStatics::ERROR, DBStatics::COMPUTECARD);

    std::vector<std::string> iocs;
    BOOST_FOREACH(ReplyIoCard& ioc, failrep._ioCards) {
        hw_to_unmark.push_back(ioc._location);
        iocs.push_back(ioc._location);
    }
    DBStatics::setLocationStatus(iocs, reply, DBStatics::ERROR, DBStatics::IOCARD);

    std::vector<std::string> scs;
    BOOST_FOREACH(ReplyServiceCard& sc, failrep._serviceCards) {
        hw_to_unmark.push_back(sc._location);
        scs.push_back(sc._location);
    }                    
    DBStatics::setLocationStatus(scs, reply, DBStatics::ERROR, DBStatics::SERVICECARD);
    return AliasWaiter::actuallySendMessages(ref, failrep, hw_to_unmark);
}

void* AliasWaiter::updateMcServer(std::string& subnet_mc, std::vector<std::string>& blocks_to_free, std::vector<std::string>& error_list, BinaryId& binid) {
    LOG_INFO_MSG(__FUNCTION__);
    MCServerMessageSpec::FailoverRequest failreq;
    MCServerMessageSpec::FailoverReply failrep;
    if(buildFailover(failreq, failrep, subnet_mc, binid) == false) {
        // Failed to build a failover message.  This is very,
        // very bad.  Need to restart.
        LOG_FATAL_MSG("Cannot build a proper failover message.  " 
                      << "Database state may be inconsistent.  "
                      << "mmcs_server ending.");
        exit(EXIT_FAILURE);
    }
    // And send the request...
    LOG_INFO_MSG("Sending Failover message to mc_server");
    MasterMonitor::_server->failover(failreq, failrep);
    std::vector<std::string> hw_to_unmark;
    bool good = true;
    if(failrep._rc != 0) {
        good = sendInitialize(MasterMonitor::_server, hw_to_unmark, failrep);
        // Now get list of blocks with hardware in error.
        PthreadMutexHolder holder;
        int muxrc = holder.Lock(&DBConsoleController::getBlockMapMutex());
        assert(muxrc == 0);

        if (DBConsoleController::getBlockMap().size() != 0) {
            // Zip through the list of booted blocks and get the node cards for each block.
            DBBlockPtr curr_block;

            for(BlockMap::iterator it = DBConsoleController::getBlockMap().begin();
                it != DBConsoleController::getBlockMap().end(); ++it) {
                curr_block = boost::dynamic_pointer_cast<DBBlockController>(it->second);
                std::vector<std::string> curr_errors;
                BGQDB::queryMissing(curr_block->getBlockName(), curr_errors);
                if(curr_errors.size() != 0) {
                    blocks_to_free.push_back(curr_block->getBlockName());
                    LOG_DEBUG_MSG("Found block " << curr_block->getBlockName() << " to free.");
                }
                BOOST_FOREACH(std::string& error, curr_errors) {
                    std::string trimmed_error = error.substr(error.find_last_of("R"));
                    if(trimmed_error.length() == 0) {
                        // It might be an IO rack.
                        trimmed_error = error.substr(error.find_last_of("Q"));
                    }
                    LOG_DEBUG_MSG("Adding " << trimmed_error << " to error list.");
                    error_list.push_back(trimmed_error);
                }
            }
        }
    }

    // Mark any nodes that were set in error during reconnect to 'A'.
    if(hw_to_unmark.size() > 0) {
        typedef MCServerMessageSpec::FailoverReply::NodeCard ReplyNodeCard;
        typedef MCServerMessageSpec::FailoverReply::IoCard ReplyIoCard;
        typedef MCServerMessageSpec::FailoverReply::ServiceCard ReplyServiceCard;
        
        MMCSCommandReply reply;
        std::vector<std::string> ncs;
        BOOST_FOREACH(ReplyNodeCard& nc, failrep._nodeCards) {
            if(std::find(hw_to_unmark.begin(), hw_to_unmark.end(), nc._location) != 
               hw_to_unmark.end()) { // If the node is now good, put it in the list.
                ncs.push_back(nc._location);
            }
        }
        DBStatics::setLocationStatus(ncs, reply, DBStatics::AVAILABLE, DBStatics::COMPUTECARD);

        std::vector<std::string> iocs;
        BOOST_FOREACH(ReplyIoCard& ioc, failrep._ioCards) {
            if(std::find(hw_to_unmark.begin(), hw_to_unmark.end(), ioc._location) != 
               hw_to_unmark.end()) { // If the node is now good, put it in the list.
                iocs.push_back(ioc._location);
            }
        }
        DBStatics::setLocationStatus(iocs, reply, DBStatics::AVAILABLE, DBStatics::IOCARD);

        std::vector<std::string> scs;
        BOOST_FOREACH(ReplyServiceCard& sc, failrep._serviceCards) {
            if(std::find(hw_to_unmark.begin(), hw_to_unmark.end(), sc._location) != 
               hw_to_unmark.end()) { // If the node is now good, put it in the list.
                scs.push_back(sc._location);
            }
        }
        DBStatics::setLocationStatus(scs, reply, DBStatics::AVAILABLE, DBStatics::SERVICECARD);
    }

    return 0;
}

bool MasterMonitor::connect_mc_server() {
    MMCSCommandReply reply;
    std::string monstr = "master_monitor";
    MCServerRef* ref;
    BlockControllerBase::mcserver_connect(ref, monstr, reply);
    if(reply.getStatus() != 0) {
        return false;
    }
    _server = ref;
    return true;
}

// Start a waiter for an alias to STOP.
void MasterMonitor::start_terminate_waiter(AliasWaiter* aw, std::string& alias) {
    int seconds = 0;
    if(_waitlist.getStat(alias) == WaiterList::RUNNING) {
        // Bin is running, so we can now wait for it to end.
        MasterWaiter* mw = new MasterWaiter;
        std::string null;
        BinaryControllerPtr bptr(new BinaryController(aw->getBinId(), null, alias, null));
        mw->setBin(bptr);
        mw->setAlias(alias);
        mw->setJoinable(true);
        mw->start();
    } else {
        // It's not running yet.
        LOG_DEBUG_MSG("Alias " << alias << " not yet running. " << _waitlist.getStat(alias));
        ++seconds;
        sleep(1);
    }
}


void* MasterMonitor::threadStart() {
    LOG_INFO_MSG("Starting BGmaster monitor thread");
    // Need to wait on all SubnetMcs.  Any one fails, we let mc_server know.
    // So we'll get all of the running binaries, and all of the configured subnet_mc
    // aliases.  If any aliases aren't running, we'll wait for them to start. Concurrently,
    // we'll create waiters for each binary.

    // Initialize the wait list.
    _waitlist.init();

    // First get all stats
    bgq::utility::Properties::Ptr props = boost::const_pointer_cast<bgq::utility::Properties>(MMCSProperties::getProperties());
    bgq::utility::ClientPortConfiguration port_config(32042);
    port_config.setProperties(MMCSProperties::getProperties(), "master.client");
    port_config.notifyComplete();
    BGMasterClient client(props);
    bgq::utility::PortConfiguration::Pairs portpairs = port_config.getPairs();
    if(portpairs[0].first.length() == 0) {
        LOG_FATAL_MSG("No port pairs or invalid port pairs specified");
        exit(1);
    }


    while(isThreadStopping() == false) {
        try {
            LOG_INFO_MSG("Connecting to bgmaster_server");
            client.connectMaster(portpairs);
            LOG_INFO_MSG("Connected to bgmaster_server successfully");
            break;
        } catch(CxxSockets::CxxError& e) {
            LOG_ERROR_MSG("Socket error detected: " << e.errcode << " " << e.what());
        } catch (BGMasterExceptions::CommunicationError& e) {
            LOG_ERROR_MSG("Communication error detected: " << e.errcode << " " << e.what());
        }
        sleep(1);
    }

    // Get all of the defined subnets
    std::vector<std::string> subnets;
    int i = 0;
    while(true) {
        std::string subnet = "Name." + boost::lexical_cast<std::string>(i);
        std::string subnet_name = MMCSProperties::getProperty((char*)(subnet.c_str()));
        if(subnet_name.length() == 0)
            break;
        LOG_DEBUG_MSG("Subnet " << subnet_name << " added to list");
        subnets.push_back(subnet_name);
        ++i;
    }
    LOG_INFO_MSG( "found " << subnets.size() << " subnets" );

    // iterate through list of subnets from bg.properties, starting a thread per subnet
    std::vector<MasterWaiter*> waiting_threads;
    for(std::vector<std::string>::const_iterator i = subnets.begin(); i != subnets.end(); ++i) {
        // get status from bgmaster_server
        typedef std::map<BinaryId, BinaryControllerPtr, Id::Comp> BinaryMap;
        BinaryMap mm;
        try {
            client.status(mm);
        } catch (BGMasterExceptions::BGMasterError& e) {
            LOG_ERROR_MSG("Unable to retrieve status from BGmaster");
            return 0;
        }

        // find this subnet in the status returned from bgmaster_server
        const BinaryMap::const_iterator subnet = std::find_if(
                mm.begin(),
                mm.end(),
                boost::bind(
                    std::equal_to<std::string>(),
                    *i,
                    boost::bind(
                        &BinaryController::get_alias_name,
                        boost::bind(
                            &BinaryMap::value_type::second,
                            _1
                            )
                        )
                    )
                );
        if ( subnet != mm.end() ) {
            // Found a subnet.  First wait for it to start
            std::string alias = *i;
            AliasWaiter* aw = new AliasWaiter(alias);
            aw->start();
            while(!isThreadStopping()) {
                if(_waitlist.getStat(alias) == WaiterList::RUNNING) {
                    break;
                } else if(_waitlist.getStat(alias) == WaiterList::ERROR) {
                    delete aw;
                    aw = new AliasWaiter(alias);
                    aw->start();
                }
                sleep(1);
            }
            start_terminate_waiter(aw, alias);
        } else {
            LOG_INFO_MSG( "did not find subnet " << *i << " in master status" );
        }
    }

    // This is start point to wait for in mmcs_server
    _started = true;
    MasterMonitor::_startup_notifier.notify_all();

    // attempt to connect to mc_server
    while(!isThreadStopping() && !connect_mc_server()) {
        sleep(1);
    }

    // Now we just periodically check the waiter list to see
    // if anybody is stopped.
    while(!isThreadStopping()) {
        usleep(5000);
        std::string stopped_alias = _waitlist.getStopped();
        if(stopped_alias == "") {
            stopped_alias = _waitlist.getErrored();
        }
        if(stopped_alias.length() != 0) {
            // At this point, we should be in either ERROR or STOPPED state,
            // not WAIT_FOR_START or RUNNING.
            // Wait for it to start back up.
            AliasWaiter* aw = new AliasWaiter(stopped_alias, true);
            aw->start();
            aw->wait(); // This joins the thread so that we can be assured
            // that we have a started subnet before we try to
            // wait for it to end.
            // Once it starts, we'll do a termination waiter again.
            start_terminate_waiter(aw, stopped_alias);
        }
    }

    LOG_INFO_MSG("Ending BGmaster monitor thread");
    return 0;
}
