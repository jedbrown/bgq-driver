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

/*!
 * \file IOBlockController.cc
 */

#include "IOBlockController.h"

#include "BCLinkchipInfo.h"
#include "BCNodecardInfo.h"
#include "BCNodeInfo.h"
#include "BlockControllerTarget.h"
#include "BlockHelper.h"
#include "HardwareBlockList.h"

#include "common/Properties.h"

#include <bgq_util/include/Location.h>

#include <db/include/api/BGQDBlib.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>

#include <boost/scope_exit.hpp>

LOG_DECLARE_FILE( "mmcs.server" );

using namespace std;

using mmcs::common::Properties;

namespace mmcs {
namespace server {

IOBlockController::IOBlockController(
        BGQMachineXML* machine,
        const std::string& userName,
        const std::string& blockName,
        bool delete_machine
        ) :
    BlockControllerBase(machine, userName, blockName, delete_machine),
    _rackbits(),
    _bootOptions( )
{
    _tolerating_faults = false;
}

IOBlockController::~IOBlockController()
{

}

void
IOBlockController::create_block(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        std::istream* blockStreamXML
        )
{
    BGQDB::DIAGS_MODE diags = BGQDB::NO_DIAGS; // 'diags' parameter specified
    FILE *svchost_config = NULL; // file for svchost configuration file
    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);

    BOOST_SCOPE_EXIT( (&svchost_config) ) {
        if (svchost_config != NULL) {
            fclose(svchost_config);
        }
    } BOOST_SCOPE_EXIT_END;

    _targetsetMode = MCServerMessageSpec::WUAR;  // Default target set open mode

    // Parse options
    if ( args.empty() && !blockStreamXML) {
        reply << mmcs_client::FAIL << "args?" << mmcs_client::DONE;
        return;
    }

    string blockFile;
    if (blockStreamXML == NULL) {
        blockFile = args[0];
        args.pop_front(); // take off the block
    }

    bool no_check = false;

    // Parse args
    for (unsigned i = 0; i < args.size(); ++i) {
        // Split based on the equal sign
        vector<string> tokens;
        tokens = Tokenize(args[i], "=");

        if (tokens[0] == "diags") {
            diags = BGQDB::NORMAL_DIAGS;
            _diags = true;
        } else if (tokens[0] == "serviceaction") {
            diags = BGQDB::SVCACTION_DIAGS;
            _diags = true;
        } else if (tokens[0] == "insert_ras") {
            _insertDiagsRAS = true;
        } else if (tokens[0] == "shared") {
            _isshared = true;
        } else if (tokens[0] == "no_check") {
            no_check = true;
        } else if (tokens[0] == "hw_check") {
            no_check = false;
        } else if (tokens[0] == "tolerate_faults") {
            _tolerating_faults = true;
        } else if (tokens[0] == "svchost_options") {
            if (tokens.size() == 2) {
                if ((svchost_config = fopen(tokens[1].c_str(), "r")) == NULL) {
                    reply << mmcs_client::FAIL << "Cannot open svchost_options file \"" << tokens[1] << "\" : " << strerror(errno) << mmcs_client::DONE;
                    return;
                }
                continue;
            }
        }
    }

    // Extract machine configuration information from the BGQMachineXML
    initMachineConfig(reply);
    if (reply.getStatus() != 0) {
        return;
    }

    // Create the BGQBlockXML classes from the input XML file
    if (_blockXML != NULL) {
        delete _blockXML;
        _blockXML = NULL;
    }

    try {
        if (blockStreamXML == NULL) {
            // Assume a filename was given
            _blockXML = BGQBlockXML::create(blockFile.c_str(), _machineXML);
        } else {
            // Use stream object
            _blockXML = BGQBlockXML::create(*blockStreamXML, _machineXML);
        }
    } catch (const XMLException& e) {
        reply << mmcs_client::FAIL << "create_block: " << e.what() << mmcs_client::DONE;
        return;
    } catch (const BGQNodeConfigException& e) {
        reply << mmcs_client::FAIL << "create_block: " << e.what() << mmcs_client::DONE;
        return;
    }

    if (_blockXML == NULL) {
        reply << mmcs_client::FAIL << "create_block: cannot load block XML, hardware resources may be marked Missing or Error " << mmcs_client::DONE;
        return;
    }

    // Save the block name for use as target set name
    if (_blockName.empty())
        _blockName = _blockXML->_name;

    // Check that the required block resources exist in the database
    if (no_check == false) { // we're not no_check
        std::vector<std::string> missing;
        if (BGQDB::queryMissing(_blockName, missing, diags) == 0) {
            if (missing.size() > 0) {
                // Build an error message identifying the missing resources
                reply << mmcs_client::FAIL << "create_block: resources are unavailable - ";
                for (unsigned i = 0; i < missing.size(); ++i) {
                    if (i == 0) {
                        reply << missing[i];
                    } else {
                        reply << ", " << missing[i];
                    }
                }
                reply << mmcs_client::DONE;
                return;
            }
        } else {
            reply << mmcs_client::FAIL << "create_block: error from BGQDB::queryMissing, cannot determine status of hardware resources " << mmcs_client::DONE;
            return;
        }
    } else {
        // no_check means we don't care if there are nodes in error.
        // So, we get the list of nodes in error to use later.
        if (BGQDB::queryError(_blockName, _error_nodes) == BGQDB::OK) {
            if (_error_nodes.size() > 0) {
                std::ostringstream msg;
                msg << "The following I/O nodes are in error: ";
                bool first = true;
                BOOST_FOREACH(const std::string& node, _error_nodes) {
                    if (first) {
                        first = false;
                    } else  {
                        msg << ", ";
                    }
                    msg << node;
                }
                msg << ". Block will boot anyway because no_check specified.";
                LOG_WARN_MSG(msg.str());
            }
        } else {
            LOG_WARN_MSG("Could not determine I/O nodes in error.");
        }
    }

    // Create a new BGQBlockNodeConfig object
    try {
        _block = new BGQBlockNodeConfig(_machineXML, _blockXML, svchost_config);
    } catch (const BGQNodeConfigException& e) {
        reply << mmcs_client::FAIL << "create_block: " << e.what() << mmcs_client::DONE;
        return;
    }

    _machine_config_data = _block->getConfigData();

    // Populate the BCNodeInfo and BCNodecardInfo lists
    {
        BCNodeInfo *nodeInfo;
        map<string, BCNodeInfo*> nodesByLocation; // for ordering pset nodes by location within pset

        // iterate over IO boards
        for (
                BGQBlockNodeConfig::ioboard_iterator ioiter = _block->ioboardBegin();
                ioiter != _block->ioboardEnd();
                ++ioiter
            )
        {
            BGQIOBoardNodeConfig& ioboard = *ioiter;

            const std::string rack = ioboard.posInMachine().substr(0,3);
            const IOBoardBitset bs(rack);
            _rackbits[rack] = bs;

            if (HardwareBlockList::find_in_list(rack) == true) {
                std::ostringstream msg;
                msg << "I/O rack " << rack << "'s controlling subnet is temporarily unavailable.";
                reply << mmcs_client::FAIL << msg.str() << mmcs_client::DONE;
                LOG_WARN_MSG(msg.str());
                return;
            }

            nodesByLocation.clear();

            BCNodecardInfo *nodecardInfo;
            nodecardInfo = new BCNodecardInfo();
            nodecardInfo->_block = _block;
            nodecardInfo->_ioboard = true;
            nodecardInfo->_boardcoordA = ioboard.allIOBoardA();
            nodecardInfo->_boardcoordB = ioboard.allIOBoardB();
            nodecardInfo->_boardcoordC = ioboard.allIOBoardC();
            nodecardInfo->_boardcoordD = ioboard.allIOBoardD();
            LOG_TRACE_MSG(
                    ioboard.posInMachine() << " (" <<
                    ioboard.allIOBoardA() << "," <<
                    ioboard.allIOBoardB() << "," <<
                    ioboard.allIOBoardC() << "," <<
                    ioboard.allIOBoardD() <<
                    ")"
                    );

            nodecardInfo->init_location();
            getIcons().push_back(nodecardInfo);

            // Add all the link chips
            LOG_TRACE_MSG( std::distance(ioboard.linkChipBegin(), ioboard.linkChipEnd()) << " link chips" );
            for (
                    BGQIOBoardNodeConfig::linkchip_iterator chipit = ioboard.linkChipBegin();
                    chipit != ioboard.linkChipEnd();
                    ++chipit
                )
            {
                BCLinkchipInfo* const chipinfo = new BCLinkchipInfo();
                chipinfo->_jtag = chipit->jtag();
                chipinfo->_personality = chipit->personality();
                chipinfo->_block = _block;
                chipinfo->_ioboard = true;
                chipinfo->init_location();
                nodecardInfo->_linkChips.push_back(chipinfo);
                LOG_TRACE_MSG( "Added link chip " << chipinfo->location() );
            }

            // Iterate over I/O nodes
            for (
                    vector<BGQIONodePos>::const_iterator citer = ioboard.computes()->begin();
                    citer != ioboard.computes()->end();
                    ++citer
                )
            {
                const BGQIONodePos& node = *citer;

                nodeInfo = new BCNodeInfo();
                nodeInfo->_block = _block;
                nodeInfo->_boardcoordA = ioboard.allIOBoardA();
                nodeInfo->_boardcoordB = ioboard.allIOBoardB();
                nodeInfo->_boardcoordC = ioboard.allIOBoardC();
                nodeInfo->_boardcoordD = ioboard.allIOBoardD();

                BGQNodePos nullpos(0,0);
                nodeInfo->_pos = nullpos;
                nodeInfo->_iopos = node;
                nodeInfo->init_location();
                if (node.trainOnly()) {
                    nodeInfo->_linkio = true; // This keeps train only nodes out of the target set
                }
                nodeInfo->_iopersonality = ioboard.nodeConfig(node)->_personality;
                _targetLocationMap[nodeInfo->location()] = nodeInfo; // maintain a map for location lookup
                nodesByLocation[nodeInfo->location()] = nodeInfo;    // order nodes by location
                ++_numNodesTotal;                                    // computes total number of nodes in the block

                // Find the node card and add the node
                nodecardInfo->_nodes.push_back(nodeInfo);  // keep track of the I/O nodes in each node board
            }

            // Order nodes by jtag id
            for (
                    map<string, BCNodeInfo*>::const_iterator it = nodesByLocation.begin();
                    it != nodesByLocation.end();
                    ++it
                )
            {
                nodeInfo = it->second;
                getNodes().push_back(nodeInfo);
            }
        }
    }
    LOG_TRACE_MSG( _numNodesTotal << " total I/O nodes in target list." );

    if (_error_nodes.size() >= getNodes().size()) {
        reply << mmcs_client::FAIL << "All I/O nodes are in error state. Cannot boot the block." << mmcs_client::DONE;
        return;
    }

    // Add all BCNodeInfo, BCNodecardInfo,
    // BCLinkchipInfo objects to IOBlockController::_targets
    for (unsigned i = 0; i < getNodes().size(); ++i)
        _targets.push_back(getNodes()[i]);

    // If not sharing the I/O board, then include it in the target set, so it will be locked
    if (_isshared == false && getNodes().size() >= 8 ) {
        for (unsigned i = 0; i < getIcons().size(); ++i) {
            _targets.push_back(getIcons()[i]);
        }
    }
    for (unsigned i = 0; i < getLinkchips().size(); ++i) {
        _targets.push_back(getLinkchips()[i]);
    }

    for (unsigned i = 0; i < _targets.size(); ++i) {
        _targets[i]->_locateId = i; // Give each device under our control a unique target specifier
        // LOG_TRACE_MSG( " location: " << _targets[i]->location() << " in target list" );
    }

    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
IOBlockController::connect(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        const BlockControllerTarget* pTarget
        )
{
    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);

    BlockControllerBase::connect(args, reply, pTarget);
    if (reply.getStatus() != mmcs_client::CommandReply::STATUS_OK) {
        return;  // connecting to mcserver failed
    }

    // Should a pgood be sent before the BootBlockRequest?
    const bool usePgood = std::find( args.begin(), args.end(), "pgood" ) != args.end();

    // Create a log file for I/O nodes if --iologdir was specified at startup
    for (unsigned i = 0; i < pTarget->getNodes().size(); ++i) {
        BCNodeInfo *nodeInfo = pTarget->getNodes()[i];
        if (nodeInfo->_open) {
            if (!BlockControllerBase::openLog(nodeInfo)) {
                reply << mmcs_client::FAIL << "Cannot open I/O log file for " << nodeInfo->location() << mmcs_client::DONE;
                return;
            }
        }
    }

    // Send a pgood reset request to the processor nodes in the target set
    if (usePgood) {
        pgood_reset(reply, pTarget);
        if (reply.getStatus() < 0) {
            LOG_TRACE_MSG("pgood_reset: " << reply.str());
            mmcs_client::CommandReply bogus;
            disconnect(args, bogus);
            return;
        }
    }

    // Start the mailbox monitor
    startMailbox(reply);
    if (reply.getStatus() != 0) {
        LOG_INFO_MSG("Starting mailbox monitor: " << reply.str());
        mmcs_client::CommandReply bogus;
        disconnect(args, bogus);
        return;
    }

    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
IOBlockController::disconnect(
        const deque<string>& args,
        mmcs_client::CommandReply& reply
        )
{
    BlockControllerBase::disconnect(args, reply);
    if (reply.getStatus() != mmcs_client::CommandReply::STATUS_OK) {
        return;
    }

    // Clear barrier bits
    for (std::map<std::string, IOBoardBitset>::iterator it = _rackbits.begin(); it != _rackbits.end(); ++it) {
        it->second.Reset();
    }

    // Close log files
    for (unsigned i = 0; i < _nodes.size(); ++i ) {
        BCNodeInfo *nodeInfo = _nodes[i];
        if ( !nodeInfo ) {
            continue;
        }
        if ( !nodeInfo->_mailboxOutput ) {
            continue;
        }
        if ( fclose(nodeInfo->_mailboxOutput) ) {
            const int error = errno;
            LOG_WARN_MSG( "Could not close I/O log file descriptor " << fileno( nodeInfo->_mailboxOutput) << ": " << strerror(error) );
        } else {
            // LOG_TRACE_MSG( "Closed I/O log file for " << nodeInfo->location() );
        }
        nodeInfo->_mailboxOutput = NULL;
    }
}

void
IOBlockController::boot_block(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        const NodeCustomization& ncust,
        PerformanceCounters::Timer::Ptr timer
        )
{
    PthreadMutexHolder mutex;
    FILE* svchost_config = NULL; // file for svchost configuration file

    BOOST_SCOPE_EXIT( (&svchost_config) ) {
        if (svchost_config != NULL) {
            fclose(svchost_config);
        }
    } BOOST_SCOPE_EXIT_END

    if (!isConnected()) {
        reply << mmcs_client::FAIL << "Block is not connected." << mmcs_client::DONE;
        return;
    }

    // Parse arguments
    string uload;
    string domains;
    string steps;
    string bootOptions; // name=value boot options
    bool tolerateFaults = _tolerating_faults;   // LLCS should NOT attempt to continue booting after detecting a fault on a compute block

    // Log boot_block arguments
    ostringstream oss;
    oss << __FUNCTION__ << " ";
    std::copy( args.begin(), args.end(), std::ostream_iterator<std::string>(oss, " ") );
    LOG_INFO_MSG(oss.str());

    bool bootsteps = false;
    // Parse arguments
    for (deque<string>::const_iterator arg = args.begin(); arg != args.end(); ++arg) {
        bool valid = true; // valid argument
        const vector<string> tokens( Tokenize(*arg, "=") );

        if (tokens.empty()) continue;
        if (tokens[0].empty()) continue;

        if (tokens[0] == "uloader") {
            if (tokens.size() == 2) {
                uload = tokens[1];
            } else {
                valid = false;
            }
        } else if (tokens[0] == "steps") {
            bootsteps = true;
            steps = *arg;
        } else if (tokens[0] == "domain") {
            domains += *arg;
        } else if (tokens[0] == "tolerate_faults") {
            tolerateFaults = true;
        } else if (tokens[0] == "svchost_options") {
            if (tokens.size() == 2) {
                if ((svchost_config = fopen(tokens[1].c_str(), "r")) == NULL) {
                    reply << mmcs_client::FAIL << "Cannot open svchost_options file \"" << tokens[1] << "\" : " << strerror(errno) << mmcs_client::DONE;
                    return;
                }
                continue;
            }
        } else {
            bootOptions.append(*arg).append(" ");
        }

        if (!valid) {
            reply << mmcs_client::FAIL << "Unrecognized argument: " << *arg << mmcs_client::DONE;
            return;
        }
    }

    if (isStarted() && !bootsteps) {
        reply << mmcs_client::FAIL << "Block is already booted" << mmcs_client::DONE;
        return;
    }

    // This may be mmcs_lite, in which the default images are passed via properties
    if (uload.empty()) {
        uload.append(Properties::getProperty("uloader"));
    }

    if (domains.empty()) {
        domains = Properties::getProperty("io_domains");
        if (domains.find("cores=") != std::string::npos) {
            domains.replace(domains.find('-'), 1, 1, '$');
        }
        if (domains.find("memory=") != std::string::npos) {
            domains.replace(domains.find('-'), 1, 1, '$');
        }
    }

    std::vector<std::string> svc_options;
    if (svchost_config && _block) {
        try {
            svc_options = _block->processSvcHostFile(svchost_config);
        } catch (const BGQNodeConfigException& e) {
            reply << mmcs_client::FAIL << "boot_block: " << e.what() << mmcs_client::DONE;
            return;
        }
    }

    // Save the node customization in case we need it on reboot_nodes
    _nodeCustomization = ncust;
    this->calculateRatios();

    // Create the BootIoBlockRequest
    MCServerMessageSpec::BootBlockRequest mcBootIoBlockRequest(
            _blockName,
            _userName,
            tolerateFaults,
            uload,
            _bootCookie,
            bootOptions,
            0 /* boot start time */
            );

    // Remember boot options for reboot_nodes
    _bootOptions = bootOptions;

    // Initialize the node board vector in the BootIoBlockRequest
    for (vector<BCIconInfo*>::const_iterator nciter = getIcons().begin(); nciter != getIcons().end(); ++nciter) {
        if (typeid(**nciter) != typeid(BCNodecardInfo)) { // only process node boards
            continue;
        }

        // Only process connected node boards or node boards with open processor cards
        if ((*nciter)->_open || (_isshared && (*nciter)->nodesOpen())) {
            BCNodecardInfo* ncinfo = dynamic_cast<BCNodecardInfo*>(*nciter);

            MCServerMessageSpec::BootBlockRequest::IoCard nodeCard(ncinfo->location());

            // Add BootIoBlockRequest::IoNode entries for all the I/O nodes on the node board
            for (unsigned i = 0; i < ncinfo->_nodes.size(); ++i) {
                BCNodeInfo* const ninfo = dynamic_cast<BCNodeInfo*>(ncinfo->_nodes[i]);
                if (ninfo->_open) { // only boot connected I/O nodes
                    const string nodeLoc = ninfo->location();
                    const string nodeCust = _machine_config_data + _nodeCustomization[nodeLoc];

                    Personality_t pers = ninfo->personality();
                    if (svchost_config && _block) {
                        pers = BGQBlockNodeConfig::updatePersonality(ninfo->personality(), svc_options);
                    }

                    const MCServerMessageSpec::BootBlockRequest::Node node(
                            ninfo->_iopos.jtagPort(), // jtag position of the node
                            true, // isIoNode
                            ninfo->_iopos.trainOnly(),
                            string(reinterpret_cast<char*>(&pers), sizeof(pers)),
                            nodeCust,
                            nodeCust.length()
                            );
                    // Make sure it is in not in the list of I/O nodes in error.
                    if (std::find(_error_nodes.begin(), _error_nodes.end(), ninfo->location()) == _error_nodes.end()) {
                        nodeCard._nodes.push_back(node);
                    }
                }
            }

            // Now get all of the link chips on the node board.
            for (
                    std::vector<BCLinkchipInfo*>::const_iterator chipit = ncinfo->_linkChips.begin();
                    chipit != ncinfo->_linkChips.end();
                    ++chipit
                )
            {
                // Create the personality.
                MCServerMessageSpec::BootBlockRequest::LinkChipPersonality pers;

                // First, add the BQLSwitches.
                for (unsigned i = 0; i < 4; ++i) {
                    BQLSwitch ncBqlSwitch = (*chipit)->_personality.getSwitch(i);
                    MCServerMessageSpec::BootBlockRequest::BQLSwitch bqlSwitch;
                    bqlSwitch._state = ncBqlSwitch._state;
                    bqlSwitch._hss4GmaskA = ncBqlSwitch._4GmaskA;
                    bqlSwitch._hss4GmaskB = ncBqlSwitch._4GmaskB;
                    bqlSwitch._hss10GmaskC = ncBqlSwitch._10GmaskC;
                    bqlSwitch._hss10GmaskD = ncBqlSwitch._10GmaskD;
                    bqlSwitch._optModRX = ncBqlSwitch._optModRX;
                    bqlSwitch._optModTX = ncBqlSwitch._optModTX;
                    pers._bqlSwitches.push_back(bqlSwitch);
                }
                // Now, add the bad wires
                for (unsigned i = 0; i < 4; ++i) {
                    std::bitset<12> badwires = (*chipit)->_personality.getBadWires(i);
                    pers._badWires.push_back(badwires.to_ulong());
                }
                // Now add the sparing mode flag
                pers._mode56 = (*chipit)->_personality.getMode56();
                // Next, create the chip object.
                MCServerMessageSpec::BootBlockRequest::LinkChip chip((*chipit)->_jtag, pers);
                // And push it in the chip vector.
                nodeCard._linkChips.push_back(chip);
            }

            // Save the BootIoBlockRequest::IoCard object in the BootIoBlockRequest
            mcBootIoBlockRequest._ioCards.push_back(nodeCard);
        }
    }

    // Remember domain for reboot_nodes
    _domains = domains;

    // Add domain info to the boot request.
    if (parseDomains(mcBootIoBlockRequest, domains) == false) {
        reply << mmcs_client::FAIL << "Bad domain syntax." << mmcs_client::DONE;
        return;
    }

    if (!parseSteps(reply, mcBootIoBlockRequest, steps)) {
        return; // Bad step, our reply is already set
    }

    mutex.Lock(&_mutex);

    // Check if we are disconnecting
    if (isDisconnecting()) {
        reply << mmcs_client::FAIL << disconnectReason() << mmcs_client::DONE;
        return;
    }

    // Mark I/O node targets as started
    for (unsigned i = 0; i < getNodes().size(); ++i) {
        BCNodeInfo *nodeInfo = getNodes()[i];
        if (nodeInfo->_open) {
            nodeInfo->_state = NST_PROGRAM_RUNNING;
            nodeInfo->_initialized = false;
            nodeInfo->_haltComplete    = false;
            _numNodesStarted   += nodeInfo->isIOnode();
        }
    }
    _isstarted = true; // Boot has been started

    mutex.Unlock();

    LOG_DEBUG_MSG("Sending boot request.");
    if (timer) {
        timer->stop();
    }

    // Send Boot command to mcMonitor
    MCServerMessageSpec::BootBlockReply   mcBootIoBlockReply;
    try {
        static const boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
        _boot_block_start = boost::posix_time::microsec_clock::local_time();
        BOOST_SCOPE_EXIT( (&_boot_block_start) ) {
            _boot_block_start = boost::posix_time::ptime();
        } BOOST_SCOPE_EXIT_END;

        if (!hardWareAccessBlocked()) {
            mcBootIoBlockRequest._startTime = (_boot_block_start - epoch).total_seconds();
            _mcServer->bootBlock(mcBootIoBlockRequest, mcBootIoBlockReply);
        } else {
            std::ostringstream msg;
            msg << "SubnetMc managing hardware for " << _blockName << " temporarily unavailable.";
            LOG_ERROR_MSG(msg.str());
            reply << mmcs_client::FAIL << msg.str() << mmcs_client::DONE;
            return;
        }
    } catch (const exception &e) {
        mcBootIoBlockReply._rc = -1;
        mcBootIoBlockReply._rt = e.what();
    }

    if (mcBootIoBlockReply._rc) {
        reply << mmcs_client::FAIL << "bootBlock: " << mcBootIoBlockReply._rt << mmcs_client::DONE;
        return;
    }

    _block_shut_down = false;

    // Check if we are disconnecting
    if (isDisconnecting()) {
        reply << mmcs_client::FAIL << disconnectReason() << mmcs_client::DONE;
        return;
    }
    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
IOBlockController::build_shutdown_req(
        MCServerMessageSpec::ShutdownBlockRequest& mcShutdownBlockRequest
        )
{
    // Initialize the node board vector in the ShutdownBlockRequest
    for (vector<BCIconInfo*>::const_iterator nciter = getIcons().begin(); nciter != getIcons().end(); ++nciter) {
        if (typeid(**nciter) != typeid(BCNodecardInfo)) { // Only process node boards
            continue;
        }

        // Only process connected node boards or node boards with open processor cards
        if ((*nciter)->_open || (_isshared && (*nciter)->nodesOpen())) {
            BCNodecardInfo* ncinfo = dynamic_cast<BCNodecardInfo*>(*nciter);

            // Create the ShutdownBlockRequest::IoCard object for this node board
            MCServerMessageSpec::ShutdownBlockRequest::IoCard nodeCard(ncinfo->location());

            // Add ShutdownBlockRequest::Node entries for all the I/O nodes on the node board
            for (unsigned i = 0; i < ncinfo->_nodes.size(); ++i) {
                BCNodeInfo* ninfo = dynamic_cast<BCNodeInfo*>(ncinfo->_nodes[i]);
                if (ninfo->_open) { // Only boot connected I/O nodes
                    const string nodeLoc = ninfo->location();
                    const string nodeCust = _machine_config_data + _nodeCustomization[nodeLoc];

                    MCServerMessageSpec::ShutdownBlockRequest::Node node(ninfo->_iopos.jtagPort(), // jtag position of the node
                            true, // isIoNode
                            ninfo->_iopos.trainOnly(),
                            string((char*) &(ninfo->personality()),sizeof(ninfo->personality())),
                            nodeCust, nodeCust.length(),
                            nodeLoc);
                    nodeCard._node_location.push_back(node);
                }
            }

            // Now get all of the link chips on the node board.
            for (
                    std::vector<BCLinkchipInfo*>::const_iterator chipit = ncinfo->_linkChips.begin();
                    chipit != ncinfo->_linkChips.end();
                    ++chipit
                )
            {
                // Next, create the link object.
                MCServerMessageSpec::ShutdownBlockRequest::Link link((*chipit)->location());
                // And push it in the chip vector.
                nodeCard._link_location.push_back(link);

                // Create the personality.
                MCServerMessageSpec::ShutdownBlockRequest::LinkChipPersonality pers;
                // First, add the BQLSwitches.
                for (unsigned i = 0; i < 4; ++i) {
                    BQLSwitch ncBqlSwitch = (*chipit)->_personality.getSwitch(i);
                    MCServerMessageSpec::ShutdownBlockRequest::BQLSwitch bqlSwitch;
                    bqlSwitch._state = ncBqlSwitch._state;
                    bqlSwitch._hss4GmaskA = ncBqlSwitch._4GmaskA;
                    bqlSwitch._hss4GmaskB = ncBqlSwitch._4GmaskB;
                    bqlSwitch._hss10GmaskC = ncBqlSwitch._10GmaskC;
                    bqlSwitch._hss10GmaskD = ncBqlSwitch._10GmaskD;
                    bqlSwitch._optModRX = ncBqlSwitch._optModRX;
                    bqlSwitch._optModTX = ncBqlSwitch._optModTX;
                    pers._bqlSwitches.push_back(bqlSwitch);
                }
                // Now, add the bad wires
                for (unsigned i = 0; i < 4; ++i) {
                    std::bitset<12> badwires = (*chipit)->_personality.getBadWires(i);
                    pers._badWires.push_back(badwires.to_ulong());
                }
                // Now add the sparing mode flag
                pers._mode56 = (*chipit)->_personality.getMode56();
                // Next, create the chip object.
                MCServerMessageSpec::ShutdownBlockRequest::LinkChip chip((*chipit)->_jtag, pers);
                // And push it in the chip vector.
                nodeCard._linkChips.push_back(chip);
            }
            // Save the ShutdownBlockRequest::IoCard object in the ShutdownBlockRequest
            mcShutdownBlockRequest._ioCards.push_back(nodeCard);
        }
    }
    // LOG_TRACE_MSG( mcShutdownBlockRequest );
}

void
IOBlockController::reboot_nodes(
        BlockControllerTarget* pTarget,
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    PthreadMutexHolder mutex;
    _rebooting = true;

    // Parse arguments
    string uload;
    bool tolerateFaults = true;

    deque<string>::const_iterator arg;

    // Parse arguments
    for (arg = args.begin(); arg != args.end(); ++arg) {
        bool valid = true;                      // valid argument
        vector<string> tokens;
        tokens = Tokenize(*arg, "=");           // split based on the equal sign.
        if (tokens[0].size() == 0) {            // handle a null argument
            continue;
        }
        if (tokens[0] == "uloader") {
            if (tokens.size() == 2) {
                uload = tokens[1];
            } else {
                valid = false;
            }
        }

        if (!valid) {
            reply << mmcs_client::FAIL << "Unrecognized argument: " << *arg << mmcs_client::DONE;
            return;
        }
    }

    // Gonna do it, so make this the new wait_boot start time.
    time_t reboot_start;
    time(&reboot_start);
    _helper->setAllocateStartTime(reboot_start);

    // Update the list of nodes in error.
    if (BGQDB::queryError(_blockName, _error_nodes) == BGQDB::OK) {
        if (_error_nodes.size() > 0) {
            std::ostringstream msg;
            msg << "The following I/O nodes are in error: ";
            bool first = true;
            BOOST_FOREACH(const std::string& node, _error_nodes) {
                if (first) {
                    first = false;
                } else {
                    msg << ", ";
                }
                msg << node;
            }
            msg << ". I/O block will boot anyway because no_check specified.";
            LOG_INFO_MSG(msg.str());
        }
    } else {
        LOG_WARN_MSG("Could not determine I/O nodes in error.");
    }

    // Create the BootIoBlockRequest
    MCServerMessageSpec::BootBlockRequest mcBootIoBlockRequest(
            _blockName,
            _userName,
            tolerateFaults,
            uload,
            _bootCookie,
            _bootOptions,
            0 /* boot start time */
            );
    // Initialize the node board vector in the BootIoBlockRequest
    for (vector<BCIconInfo*>::const_iterator nciter = getIcons().begin(); nciter != getIcons().end(); ++nciter) {
        if (typeid(**nciter) != typeid(BCNodecardInfo)) { // Only process node boards
            continue;
        }

        // Only process connected node boards or node boards with open processor cards
        if ((*nciter)->_open || (_isshared && (*nciter)->nodesOpen())) {
            const BCNodecardInfo* ncinfo = dynamic_cast<BCNodecardInfo*>(*nciter);

            MCServerMessageSpec::BootBlockRequest::IoCard nodeCard(ncinfo->location());
            MCServerMessageSpec::BootBlockRequest::IoCardReboot rebootIoCard(ncinfo->location());

            // Add BootIoBlockRequest::IoNode entries for all the I/o nodes on the node board
            for (unsigned i = 0; i < ncinfo->_nodes.size(); ++i) {
                BCNodeInfo* const ninfo = dynamic_cast<BCNodeInfo*>(ncinfo->_nodes[i]);
                if (ninfo->isIOnode()) {
                    // Clear "ionodeinitialized" flag for each node
                    ninfo->_initialized = false;
                }
                if (ninfo->_open)  { // Only boot connected I/O nodes
                    const string nodeLoc = ninfo->location();
                    const string nodeCust = _machine_config_data + _nodeCustomization[nodeLoc];

                    MCServerMessageSpec::BootBlockRequest::Node node(ninfo->_iopos.jtagPort(), // jtag position of the node
                            true, // isIoNode
                            false, // do not use this node for I/O Link training only
                            string((char*) &(ninfo->personality()),sizeof(ninfo->personality())), // node personality
                            nodeCust, nodeCust.length());
                    // Make sure it is in not in the list of I/O nodes in error.
                    if (std::find(_error_nodes.begin(), _error_nodes.end(), ninfo->location()) == _error_nodes.end()) {
                        rebootIoCard._nodes.push_back(node);
                    }
                }
            }

            mcBootIoBlockRequest._rebootIoCards.push_back(rebootIoCard);
        }
    }

    // Add domain info to the boot request.
    if (parseDomains(mcBootIoBlockRequest, _domains) == false) {
        reply << mmcs_client::FAIL << "Bad domain syntax." << mmcs_client::DONE;
        return;
    }

    mutex.Lock(&_mutex);

    // Check if we are disconnecting
    if (isDisconnecting()) {
        reply << mmcs_client::FAIL << disconnectReason() << mmcs_client::DONE;
        return;
    }

    //  RAS events to be handled normally
    _numNodesStarted          = 0;
    for (unsigned i = 0; i < getNodes().size(); ++i) {
        BCNodeInfo* const nodeInfo = getNodes()[i];
        if (nodeInfo->_open) {
            nodeInfo->_state = NST_PROGRAM_RUNNING;
            nodeInfo->_initialized = false;
            nodeInfo->_haltComplete    = false;
            _numNodesStarted   += nodeInfo->isIOnode();
        }
    }
    _isstarted = true; // Boot has been started

    mutex.Unlock();

    // Send Boot command to mcServer
    MCServerMessageSpec::BootBlockReply   mcBootIoBlockReply;
    try {
        static const boost::posix_time::ptime epoch1(boost::gregorian::date(1970,1,1));
        boost::posix_time::ptime reboot_block_start = boost::posix_time::microsec_clock::local_time();
        if (!hardWareAccessBlocked()) {
            mcBootIoBlockRequest._startTime = (reboot_block_start - epoch1).total_seconds();
            _mcServer->bootBlock(mcBootIoBlockRequest, mcBootIoBlockReply);
        } else {
            std::ostringstream msg;
            msg << "SubnetMc managing hardware for " << _blockName << " temporarily unavailable.";
            LOG_ERROR_MSG(msg.str());
            reply << mmcs_client::FAIL << msg.str() << mmcs_client::DONE;
            return;
        }
    } catch (const exception &e) {
        mcBootIoBlockReply._rc = -1;
        mcBootIoBlockReply._rt = e.what();
    }

    if (mcBootIoBlockReply._rc) {
        reply << mmcs_client::FAIL << "rebootBlock: " << mcBootIoBlockReply._rt << mmcs_client::DONE;
        return;
    }

    // Check if we are disconnecting
    if (isDisconnecting()) {
        reply << mmcs_client::FAIL << disconnectReason() << mmcs_client::DONE;
        return;
    }
    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
IOBlockController::shutdown_block(
        mmcs_client::CommandReply& reply,
        const std::deque<std::string>& args
        )
{
    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);

    if (_block_shut_down) {
        LOG_INFO_MSG("I/O block already shut down.");
        reply << mmcs_client::OK << mmcs_client::DONE;
        return;
    }

    if (!isConnected()) {
        std::deque<string> args;
        args.push_back(Properties::getProperty(DFT_TGTSET_TYPE));

        const BlockControllerTarget temp(shared_from_this(), "{*}", reply);
        connect(args, reply, &temp);
    }

    if (!isStarted()) {
        reply << mmcs_client::FAIL << "Block is not booted." << mmcs_client::DONE;
        return;
    }

    // Create the ShutdownBlockRequest
    MCServerMessageSpec::ShutdownBlockRequest mcShutdownBlockRequest(_blockName, _bootCookie, _block->blockId(), _diags, "");
    MCServerMessageSpec::ShutdownBlockReply mcShutdownBlockReply;
    build_shutdown_req(mcShutdownBlockRequest);

    if ( !_diags ) {
        // skip kernel verification if abnormal was requested
        mcShutdownBlockRequest._skipKernel = (std::find(args.begin(), args.end(), "abnormal") != args.end());
    }
    LOG_DEBUG_MSG( "kernel shutdown: " << (mcShutdownBlockRequest._skipKernel ? "disabled" : "enabled") );

    reply << mmcs_client::OK;
    bool sent = false;
    try {
        if (!hardWareAccessBlocked()) {
            if (_redirectSock != 0) {
                // If we're redirecting, we've got to free the target set and reopen it WUAR.
                // First free the target set.  "no_shutdown" to avoid eating our tail.
                std::deque<std::string> args;
                args.push_back("no_shutdown");
                LOG_DEBUG_MSG("Freeing I/O targets.");
                disconnect(args, reply);
                std::deque<std::string> a;
                a.push_back("mode=control");
                const BlockControllerTarget target(shared_from_this(), "{*}", reply);
                connect(a, reply, &target);
            }
            if (_mcServer) {
                _mcServer->shutdownBlock(mcShutdownBlockRequest, mcShutdownBlockReply);
                sent = true;
            } else {
                mcShutdownBlockReply._rc = -1;
                mcShutdownBlockReply._rt = "mc_server not connected";
            }
        } else {
            std::ostringstream msg;
            msg << "SubnetMc managing hardware for " << _blockName << " temporarily unavailable.";
            LOG_ERROR_MSG(msg.str());
            reply << mmcs_client::FAIL << msg.str() << mmcs_client::DONE;
            return;
        }
    } catch (const exception &e) {
        mcShutdownBlockReply._rc = -1;
        mcShutdownBlockReply._rt = e.what();
    }

    if (mcShutdownBlockReply._rc != 0) {
        reply << mmcs_client::ABORT << "shutdown_block: " << mcShutdownBlockReply._rt;
        LOG_ERROR_MSG(mcShutdownBlockReply._rt);
        // Don't return here.  Let it go to FREE and rely on LLCS to set hardware in error.
    }

    if (sent) {
        _block_shut_down = true;
        LOG_INFO_MSG("Shutdown sent.");
    }

    _isstarted = false;
    reply << mmcs_client::DONE;
}

void
IOBlockController::show_barrier(
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK;

    // Spin through all of our I/o nodes.  For each one, find its
    // associated rack and see if its location is set.
    for (std::vector<BCNodeInfo*>::const_iterator it = _nodes.begin(); it != _nodes.end(); ++it) {
        std::string loc = (*it)->location();
        std::string rack = loc.substr(0,3);
        if (_rackbits.find(rack) != _rackbits.end() && _rackbits[rack].Flagged(loc) == false) {
            reply << loc << std::endl;
        }
    }

    reply << mmcs_client::DONE;
    return;
}

bool
IOBlockController::processRASMessage(
        RasEvent& rasEvent
        )
{
    bool filtered = false;
    LOG_TRACE_MSG("Process RAS message " << rasEvent.getDetails()[RasEvent::MSG_ID]);

    // Barrier RAS message
    if (rasEvent.msgId() == 0x00040096) {
        const std::string locstr = rasEvent.getDetails()[RasEvent::LOCATION];
        LOG_TRACE_MSG("Barrier RAS received for " << locstr);
        std::string rack = locstr.substr(0,3);
        // Set the bit in the rack that satisfies this particular location
        _rackbits[rack].Set(locstr);

        unsigned satisfied_nodes = 0;
        // Loop through the IO racks.  Count each rack that is completely satisfied.
        for (std::map<std::string, IOBoardBitset>::iterator it = _rackbits.begin(); it != _rackbits.end(); ++it) {
            satisfied_nodes += it->second.BitsSet();
        }

        if (satisfied_nodes == _nodes.size()) {
            LOG_TRACE_MSG("I/O block barrier satisfied.");
            // Clear all bits so we can handle another.
            for (
                    std::map<std::string, IOBoardBitset>::iterator it = _rackbits.begin();
                    it != _rackbits.end();
                    ++it
                )
            {
                it->second.Reset();
            }

            _do_barrier_ack = true;
        }

        filtered = true;
    }

    // write the RAS message to one or more log files
    if (!filtered) {
        printRASMessage(rasEvent);
    }

    if (_diags && !_insertDiagsRAS) {
        // filter RAS that occurs from diags unless they request it
        filtered = true;
    }

    return filtered;
}

void
IOBlockController::calculateRatios()
{
    const BGQMachineXML* machine = _machineXML;
    BOOST_ASSERT( machine );

    // Check if bgas keyword in [mmcs] section exists
    bool bgas = false;
    try {
        std::string bgasValue = Properties::getProperties()->getValue( "mmcs", "bgas" );
        if ( bgasValue.compare("true") == 0 || bgasValue.compare("TRUE") == 0 ) {
            bgas = true;
        }
    } catch ( const std::exception& e ) {
        LOG_TRACE_MSG( "Could not find key bgas in [mmcs] section. Using default value of false" );
    }
    if (bgas) {
        LOG_TRACE_MSG( "Calculating ATTACHED_COMPUTE_NODES on a BGAS system" );
    } else {
        LOG_TRACE_MSG( "Calculating ATTACHED_COMPUTE_NODES on a standard system" );
    }

    // Midplane location is key, I/O node location is value
    typedef std::multimap<std::string,std::string> Midplanes;
    Midplanes linkedMidplanes;

    bool firstIOLinkInserted = false;
    // Iterate through all I/O nodes in the machine
    BOOST_FOREACH( const BGQMachineIOBoard* board, machine->_ioBoards ) {
        BOOST_FOREACH( const BGQMachineNode* i, board->_nodes ) {
            const std::string location( board->_board + "-" + i->_location );

            // Find the first I/O link for this I/O node
            const std::vector<BGQMachineIOLink*>::const_iterator firstLink = std::find_if(
                    machine->_ioLinks.begin(),
                    machine->_ioLinks.end(),
                    boost::bind(
                        &BGQMachineIOLink::_ioNode,
                        _1
                        ) == location
                    );
            if ( firstLink == machine->_ioLinks.end() ) continue;
            // Remember the first midplane location attached to this I/O node
            const std::string firstMidplane( (*firstLink)->_computeNode.substr(0,6) );
            // Check if BGAS environment
            if (bgas) {
                // In BGAS environment the I/O link can be marked "Missing" so use this as indicator to ignore this I/O node when calculating ratio
                if ((*firstLink)->_ioLinkMissing == false) {
                    LOG_TRACE_MSG( "I/O node " <<  location << " <--> compute node " << (*firstLink)->_computeNode << " (1st link present)" );
                    linkedMidplanes.insert( Midplanes::value_type(firstMidplane, location) );
                    firstIOLinkInserted = true;
                } else {
                    LOG_TRACE_MSG( "I/O node " <<  location << " <--> compute node " << (*firstLink)->_computeNode << " (1st link missing)" );
                    firstIOLinkInserted = false;
                }
            } else {
                LOG_TRACE_MSG( "I/O node " <<  location << " <--> compute node " << (*firstLink)->_computeNode << " (1st link)" );
                linkedMidplanes.insert( Midplanes::value_type(firstMidplane, location) );
                firstIOLinkInserted = true;
            }

            // Find the second I/O link for this I/O node
            std::vector<BGQMachineIOLink*>::const_iterator start(firstLink);
            std::advance(start, 1);
            const std::vector<BGQMachineIOLink*>::const_iterator secondLink = std::find_if(
                    start,
                    machine->_ioLinks.end(),
                    boost::bind(
                        &BGQMachineIOLink::_ioNode,
                        _1
                        ) == location
                    );
            if ( secondLink == machine->_ioLinks.end() ) continue;
            // Remember the second midplane location attached to this I/O node, if it is different from the first
            const std::string secondMidplane( (*secondLink)->_computeNode.substr(0,6) );
            // Check if BGAS environment
            if (bgas) {
                if ((*secondLink)->_ioLinkMissing == false) {
                    LOG_TRACE_MSG( "I/O node " <<  location << " <--> compute node " << (*secondLink)->_computeNode << " (2nd link present)" );
                    // This is just a safety check and should never happen in real world but we account for it
                    if ( firstIOLinkInserted == false) {
                        // The first link was not added to the map since it was "missing" but second link is present so add it to map
                        linkedMidplanes.insert( Midplanes::value_type(secondMidplane, location) );
                    } else {
                        if ( secondMidplane != firstMidplane ) {
                            linkedMidplanes.insert( Midplanes::value_type(secondMidplane, location) );
                        }
                    }
                } else {
                    LOG_TRACE_MSG( "I/O node " <<  location << " <--> compute node " << (*secondLink)->_computeNode << " (2nd link missing)" );
                }
            } else {
                LOG_TRACE_MSG( "I/O node " <<  location << " <--> compute node " << (*secondLink)->_computeNode << " (2nd link)" );
                if ( secondMidplane != firstMidplane ) {
                    linkedMidplanes.insert( Midplanes::value_type(secondMidplane, location) );
                }
            }
        }
    }

    // I/O node location is key, value is number of attached compute nodes
    typedef std::map<std::string, unsigned> IoRatio;
    IoRatio nodes;

    // For every I/O node in this block, find their connected midplanes
    BOOST_FOREACH( const BCNodeInfo* i, _nodes ) {
        // Find the first I/O link for this I/O node
        const std::vector<BGQMachineIOLink*>::const_iterator firstLink = std::find_if(
                machine->_ioLinks.begin(),
                machine->_ioLinks.end(),
                boost::bind(
                    &BGQMachineIOLink::_ioNode,
                    _1
                    ) == i->location()
                );
        if ( firstLink == machine->_ioLinks.end() ) continue;
        LOG_TRACE_MSG( i->location() << " 1st link --> " << (*firstLink)->_computeNode );

        unsigned ratio = 0;
        // Get the connected midplane for the first I/O link
        const std::string firstMidplane( (*firstLink)->_computeNode.substr(0,6) );
        if ( unsigned ioNodes = linkedMidplanes.count(firstMidplane) ) {
            ratio = (bgq::util::Location::NodeBoardsOnMidplane * bgq::util::Location::ComputeCardsOnNodeBoard) / ioNodes;
            LOG_TRACE_MSG("Ratio after 1st link " << ratio << ". I/O nodes = " << ioNodes );
            // Handle non-uniform I/O on midplane (typically development systems) Normal is 8,16,32,64,128,256 or 512 compute to I/O
            if (ratio == 8 || ratio == 16 || ratio == 32 || ratio == 64 || ratio == 128 || ratio == 256 || ratio == 512) {
                LOG_TRACE_MSG("Uniform I/O found for midplane " << firstMidplane);
            } else {
                // Special case by increasing to next calculated ratio boundry
                if (ratio > 256) {
                    ratio = 512;
                } else if (ratio > 128) {
                    ratio = 256;
                } else if (ratio > 64) {
                    ratio = 128;
                } else if (ratio > 32) {
                    ratio = 64;
                } else if (ratio > 16) {
                    ratio = 32;
                } else if (ratio > 8) {
                    ratio = 16;
                } else if (ratio > 0) {
                    ratio = 8;
                }
                LOG_WARN_MSG("Non-uniform I/O found for midplane " << firstMidplane << " changing ratio to " << ratio);
            }
        } else {
            LOG_WARN_MSG( "Could not find connected I/O node for " << (*firstLink)->_computeNode );
        }

        // Find the second I/O link for this I/O node
        std::vector<BGQMachineIOLink*>::const_iterator start(firstLink);
        std::advance(start, 1);
        const std::vector<BGQMachineIOLink*>::const_iterator secondLink = std::find_if(
                start,
                machine->_ioLinks.end(),
                boost::bind(
                    &BGQMachineIOLink::_ioNode,
                    _1
                    ) == i->location()
                );
        if ( secondLink == machine->_ioLinks.end() ) {
            // this I/O node only has a single link cabled instead of the usual two
            nodes.insert( IoRatio::value_type(i->location(), ratio) );
            LOG_TRACE_MSG( firstMidplane << " has " << ratio << " computes per I/O node." );
        } else {
            LOG_TRACE_MSG( i->location() << " 2nd link --> " << (*secondLink)->_computeNode );

            // Some customers have cabling such that an I/O node connects to a different
            // midplane. We need to handle that here and calculate the appropriate ratio
            // of compute nodes per I/O node
            const std::string secondMidplane( (*secondLink)->_computeNode.substr(0,6) );
            if ( secondMidplane != firstMidplane ) {
                ratio *= 2;
            }
            nodes.insert( IoRatio::value_type(i->location(), ratio) );
            LOG_TRACE_MSG( firstMidplane << " has " << ratio << " computes per I/O node." );
        }
    }

    // Find the configured large region size
    unsigned largeRegionSize = 0x100000;
    const std::string key( "large_region_size" );
    try {
        const int value(
                boost::lexical_cast<int>(
                    Properties::getProperties()->getValue( "cios", key )
                    )
                );
        if ( value <= 0 ) {
            LOG_WARN_MSG( "Invalid " << key << " in [cios] section, must be positive." );
            LOG_WARN_MSG( "Using default value of " << largeRegionSize );
        } else {
            largeRegionSize = value;
        }
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( "Bad " << key << " in [cios] section, must be a number." );
        LOG_WARN_MSG( "Using default value of " << largeRegionSize );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( "Could not find key " << key << " in [cios] section." );
        LOG_WARN_MSG( "Using default value of " << largeRegionSize );
    }
    LOG_TRACE_MSG( "Large region size: " << largeRegionSize );

    // Add node customization for every node in our block
    BOOST_FOREACH( const BCNodeInfo* i, _nodes ) {
        const std::string location( i->_location );
        const IoRatio::const_iterator node = nodes.find( location );
        unsigned ratio = 0;
        if ( node == nodes.end() ) {
            // No ratio means this node has no I/O link
        } else {
            ratio = node->second;
        }

        LOG_TRACE_MSG( location << " has " << ratio << " attached compute nodes." );
        const NodeCustomization::iterator customization = _nodeCustomization.find( location );

        // Note that the bell character is the delimiter
        const std::string attachCount(
                "ATTACHED_COMPUTE_NODES=" +
                boost::lexical_cast<std::string>(ratio) +
                "\a" +
                "LARGE_REGION_SIZE=" +
                boost::lexical_cast<std::string>(largeRegionSize) +
                "\a"
                );

        // Prepend customization if found, otherwise insert it
        if ( customization == _nodeCustomization.end() ) {
            _nodeCustomization.insert(NodeCustomization::value_type(location, attachCount));
        } else {
            customization->second.insert(0, attachCount);
        }
    }
}

} } // namespace mmcs::server
