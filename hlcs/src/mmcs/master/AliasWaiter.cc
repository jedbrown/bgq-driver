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

#include "master/AliasWaiter.h"
#include "master/Monitor.h"
#include "master/WaiterList.h"

#include "DBBlockController.h"
#include "DBConsoleController.h"
#include "DBStatics.h"
#include "HardwareBlockList.h"
#include "MMCSSubnet.h"
#include "RunJobConnection.h"

#include "../../master/lib/exceptions.h"

#include <control/include/mcServer/MCServer_errno.h>
#include <control/include/mcServer/MCServerAPIHelpers.h>
#include <db/include/api/job/types.h>
#include <db/include/api/BGQDBlib.h>
#include <utility/include/Log.h>

#include <vector>

LOG_DECLARE_FILE( "mmcs" );

namespace mmcs {
namespace master {

const int WAIT_TIMEOUT = 600;
const std::string tsetname = "ReInitializeSet";

bool
getIdleComputeCards(
        const std::string& hw,
        std::vector<std::string>& idle_compute_nodecards,
        std::vector<std::string>& booted_nodecards
        )
{
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

bool
getIOCards(
        std::vector<std::string>& iocards
        )
{
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

bool
getServiceCards(
        std::vector<std::string>& servicecards
        )
{
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

AliasWaiter::AliasWaiter(
        const std::string& alias,
        bool update_mc
        ) :
    MMCSThread(), 
    _alias(alias), 
    _update_mc(update_mc)
{

}

void*
AliasWaiter::threadStart()
{
    LOG_INFO_MSG("Starting alias start waiter for " << _alias);
    Monitor::_waitlist.setStat(_alias, WaiterList::WAIT_FOR_START);
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
        } catch (exceptions::CommunicationError& e) {
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
            if(Monitor::_waitlist.findBinId(newid.str()) != true) {
                done = true;
                _binid = newid;
                Monitor::_alias_binary_map[_alias] = _binid;
                BOOST_FOREACH(MMCSSubnet& curr_subnet, MMCSProperties::_subnets) {
                    if(_alias == curr_subnet.get_name())
                        curr_subnet.set_home(_binid.get_host().ip());
                }
            } else {
                LOG_INFO_MSG("ID " << newid.str() << " is already in list.");
                sleep(1);
            }
        } catch(exceptions::BGMasterError& e) {
            LOG_ERROR_MSG("Start waiter failed " << e.what() << " No longer monitoring for "
                          << _alias << " failover.");
            Monitor::_waitlist.setStat(_alias, WaiterList::ERROR);
            return 0;
        }
    }

    std::vector<std::string> blocks_to_free;
    std::vector<std::string> error_list;
    if(_update_mc)
        updateMcServer(_alias, blocks_to_free, error_list, _binid);
    Monitor::_waitlist.setStat(_alias, WaiterList::RUNNING);
    // Now return the hardware to the available pool
    std::string hardware = MMCSProperties::getProperty((char*)(_alias.c_str()));
    LOG_INFO_MSG("Making " << hardware << " available");
    HardwareBlockList::remove_from_list(hardware);

    // First, we've got to kill the jobs associated with any errored hardware.
    BOOST_FOREACH(const std::string& location, error_list) {
        LOG_DEBUG_MSG("Freeing blocks and jobs for " << location);
        std::vector<BGQDB::job::Id> jobs;
        // First get the jobs to kill.
        int result = BGQDB::killMidplaneJobs(location, &jobs, true);
        if (result != BGQDB::OK) {
            LOG_WARN_MSG( "error on BGQDB::KillMidplaneJobs trying to free blocks");
        }
        // Kill the jobs
        BOOST_FOREACH(const BGQDB::job::Id& job, jobs) {
            if(RunJobConnection::Kill(job, 9) != 0) {
                LOG_ERROR_MSG("error contacting runjob server to kill jobs");
            }
        }
    }

    // Now free any blocks that could not be failed over.
    BOOST_FOREACH(const std::string& blockid, blocks_to_free) {
        // If this is an IO block, we've got to free the computes, too.
        std::vector<std::string> compute_blocks;
        int result = BGQDB::checkIOBlockConnection(blockid, &compute_blocks);
        if(result != BGQDB::OK) {
            LOG_ERROR_MSG(DBBlockController::strDBError(result)
                          << ": Database cannot determine connected compute blocks.");
        }
        // Stick it in with the compute blocks and free 'em all in one loop.
        compute_blocks.push_back(blockid);
        BOOST_FOREACH(const std::string& block, compute_blocks) {
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

bool
AliasWaiter::actuallySendMessages(
        MCServerRefPtr ref,
        const MCServerMessageSpec::FailoverReply& failrep,
        std::vector<std::string>& markAvail
        )
{
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

    BOOST_FOREACH(const ReplyNodeCard& nc, failrep._nodeCards) {
        // Loop through the failed reply node cards.  They all failed.
        // We need to initialize them.
        ncreq._location.push_back(nc._location);
        mcMakeTargetSetRequest._location.push_back(nc._location);
    }

    BOOST_FOREACH(const ReplyIoCard& ioc, failrep._ioCards) {
        ioreq._location.push_back(ioc._location);
        mcMakeTargetSetRequest._location.push_back(ioc._location);
    }

    BOOST_FOREACH(const ReplyServiceCard& sc, failrep._serviceCards) {
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

    MCServerMessageSpec::OpenTargetRequest mcOpenTargetSetRequest(tsetname, user, MCServerMessageSpec::OVER, true);
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
        return false;
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
    BOOST_FOREACH(const MCServerMessageSpec::ServiceCardReply& curr_rep, servrep._values) {
        if(curr_rep._error == 0) {
            markAvail.push_back(curr_rep._location);
        } else {
            LOG_DEBUG_MSG(curr_rep._location << " failed initialization. " 
                          << curr_rep._error);
        }
    }

    // Now put all of the GOOD ones in the markAvail vector.
    BOOST_FOREACH(const MCServerMessageSpec::NodeCardReply& curr_rep, ncrep._values) {
        if(curr_rep._error == 0) {
            markAvail.push_back(curr_rep._location);
        } else {
            LOG_DEBUG_MSG(curr_rep._location << " failed initialization. " 
                          << curr_rep._error);
        }
    }

    // Now put all of the GOOD ones in the markAvail vector.
    BOOST_FOREACH(const MCServerMessageSpec::IoCardReply& curr_rep, iorep._values) {
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

bool
AliasWaiter::buildFailover(
        MCServerMessageSpec::FailoverRequest& failreq,
        const std::string& subnet_mc,
        const BinaryId& binid
        )
{
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
        BOOST_FOREACH(const MMCSSubnet& msub, MMCSProperties::_subnets) {
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
            BGQDB::BLOCK_STATUS bState;
            // See if the block is actually booted, not just allocated somewhere.
            // We only want to reconnect to fully INITIALIZED blocks!
            if (BGQDB::getBlockStatus(curr_block->getBlockName(), bState) == BGQDB::OK) {
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
    std::vector<std::string> idle_compute_nodecards;
    if(getIdleComputeCards(hw, idle_compute_nodecards,
                           booted_compute_nodecards)) {
        BOOST_FOREACH(const std::string& curr_nc, idle_compute_nodecards) {
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
    BOOST_FOREACH(const std::string& curr_io_card, all_iocards) {
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
    BOOST_FOREACH(const std::string& curr_opt, Monitor::_bringup_options) {
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
        BOOST_FOREACH(const std::string& curr_sc, service_cards) {
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

bool
AliasWaiter::sendInitialize(
        MCServerRefPtr ref, 
        std::vector<std::string>& hw_to_unmark, 
        MCServerMessageSpec::FailoverReply& failrep
        )
{
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

void
AliasWaiter::updateMcServer(
        const std::string& subnet_mc,
        std::vector<std::string>& blocks_to_free, 
        std::vector<std::string>& error_list, 
        const BinaryId& binid
        )
{
    LOG_INFO_MSG(__FUNCTION__);
    MCServerMessageSpec::FailoverRequest failreq;
    MCServerMessageSpec::FailoverReply failrep;
    if(buildFailover(failreq, subnet_mc, binid) == false) {
        // Failed to build a failover message.  This is very,
        // very bad.  Need to restart.
        LOG_FATAL_MSG("Cannot build a proper failover message.  " 
                      << "Database state may be inconsistent.  "
                      << "mmcs_server ending.");
        exit(EXIT_FAILURE);
    }
    // And send the request...
    LOG_INFO_MSG("Sending Failover message to mc_server");
    try {
        Monitor::_server->failover(failreq, failrep);
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
        return;
    }

    std::vector<std::string> hw_to_unmark;
    if(failrep._rc != 0) {
        sendInitialize(Monitor::_server, hw_to_unmark, failrep);
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
                if(!curr_errors.empty()) {
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
    if(!hw_to_unmark.empty()) {
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

    return;
}

} // master
} // mmcs
