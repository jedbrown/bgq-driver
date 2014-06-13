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
 * \file CNBlockController.cc
 */

#include "CNBlockController.h"

#include "BCLinkchipInfo.h"
#include "BCNodecardInfo.h"
#include "BCNodeInfo.h"
#include "BlockControllerTarget.h"
#include "HardwareBlockList.h"

#include "common/Properties.h"

#include <db/include/api/BGQDBlib.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>

#include <boost/scope_exit.hpp>

LOG_DECLARE_FILE( "mmcs.server" );

using namespace std;

namespace mmcs {
namespace server {

CNBlockController::CNBlockController(
        BGQMachineXML* machine,
        const std::string& userName,
        const std::string& blockName,
        bool delete_machine
        ) :
    BlockControllerBase(machine, userName, blockName, delete_machine)
{

}

bool
CNBlockController::process_midplane(
        BGQMidplaneNodeConfig& midplane,
        mmcs_client::CommandReply& reply,
        bool passthru
        )
{
    BCNodeInfo *nodeInfo;
    BCNodecardInfo *nodecardInfo;
    map<string, BCNodeInfo*> nodesByLocation; // for ordering nodes by location

    const std::string rack = midplane.posInMachine().substr(0,3);
    RackBitset bs(rack);
    _rackbits[rack] = bs;

    if (HardwareBlockList::find_in_list(rack) == true) {
        std::ostringstream msg;
        msg << "Rack " << rack << "'s controlling subnet is temporarily unavailable.";
        reply << mmcs_client::FAIL << msg.str() << mmcs_client::DONE;
        LOG_WARN_MSG(msg.str());
        return false;
    }

    // Note it if we have fewer than one IO link
    if (midplane.ioLinks() < 1 && !passthru) {
        _io_linkless_midplanes.insert( midplane.posInMachine() );
    }

    nodesByLocation.clear();

    // Iterate over node boards in midplane
    for (
            BGQMidplaneNodeConfig::nodeboard_iterator nodeit = midplane.nodeBoardBegin();
            nodeit != midplane.nodeBoardEnd();
            ++nodeit
        )
    {
        nodecardInfo = new BCNodecardInfo();

        // Add all the link chips
        for (
                BGQNodeBoard::linkchip_iterator chipit = (*nodeit)->linkChipBegin();
                chipit != (*nodeit)->linkChipEnd();
                ++chipit
            )
        {
            BCLinkchipInfo* const chipinfo = new BCLinkchipInfo();
            chipinfo->_jtag = chipit->jtag();
            chipinfo->_personality = chipit->personality();
            chipinfo->_passthru_only = passthru;
            nodecardInfo->_linkChips.push_back(chipinfo);
        }

        nodecardInfo->_block = _block;
        nodecardInfo->_boardcoordA = midplane.allMidplaneA();
        nodecardInfo->_boardcoordB = midplane.allMidplaneB();
        nodecardInfo->_boardcoordC = midplane.allMidplaneC();
        nodecardInfo->_boardcoordD = midplane.allMidplaneD();
        nodecardInfo->_card = (*nodeit)->jtag();
        nodecardInfo->init_location();
        nodecardInfo->_passthru_only = passthru;
        getIcons().push_back(nodecardInfo);
        _targetLocationMap[nodecardInfo->location()] = nodecardInfo; // maintain a map for location lookup
    }

    // Iterate compute nodes.
    for (
            vector<BGQNodePos>::const_iterator citer = midplane.computes()->begin();
            citer != midplane.computes()->end();
            ++citer
        )
    {
        const BGQNodePos& node = *citer;
        nodeInfo = new BCNodeInfo();
        nodeInfo->_block = _block;
        nodeInfo->_boardcoordA = midplane.allMidplaneA();
        nodeInfo->_boardcoordB = midplane.allMidplaneB();
        nodeInfo->_boardcoordC = midplane.allMidplaneC();
        nodeInfo->_boardcoordD = midplane.allMidplaneD();

        nodeInfo->_iopos = 0;
        nodeInfo->_pos = node;
        nodeInfo->init_location();
        nodeInfo->_linkio = false;
        nodeInfo->_passthru_only = passthru;
        _targetLocationMap[nodeInfo->location()] = nodeInfo;    // maintain a map for location lookup
        nodesByLocation[nodeInfo->location()] = nodeInfo;       // order nodes by location within pset
        ++_numNodesTotal;                                       // computes total number of nodes in the block
        ++_numComputesTotal;

        // Find the node card and add the node
        nodecardInfo = findNodeBoardInfo(nodeInfo->nodeCardLocation());
        nodecardInfo->_nodes.push_back(nodeInfo);  // keep track of the compute nodes in each node card
    }

    // order nodes by jtag id within pset
    for (
         map<string, BCNodeInfo*>::const_iterator it = nodesByLocation.begin();
         it != nodesByLocation.end();
         ++it
        )
    {
        nodeInfo = it->second;
        nodeInfo->_passthru_only = passthru;
        getNodes().push_back(nodeInfo);
    }

    return true;
}

void
CNBlockController::create_block(
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

    _targetsetMode = MCServerMessageSpec::WUAR; // default target set open mode

    // parse options
    if ( args.empty() && !blockStreamXML ) {
        reply << mmcs_client::FAIL << "args?" << mmcs_client::DONE;
        return;
    }

    string blockFile;
    if (blockStreamXML == NULL) {
        blockFile = args[0];
        args.pop_front(); // take off the block
    }

    bool no_check = false;  // Note: this was true, during early bringup
    bool no_torus = false;

    // parse args
    for (unsigned i = 0; i < args.size(); ++i) {
        // split based on the equal sign
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
        } else if (tokens[0] == "no_torus") {  // if forceTorus=true was in the bg.properties file, this will override it to false (for FCT)
            no_torus = true;
        } else if (tokens[0] == "no_check") {
            no_check = true;
        } else if (tokens[0] == "hw_check") {
            no_check = false;
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
            // Use the stream object
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
        reply << mmcs_client::FAIL << "create_block: cannot load block XML, hardware resources may be marked Missing or in Error " << mmcs_client::DONE;
        return;
    }

    // Remove the switches if no_torus is specified
    if (no_torus &&  _blockXML->_midplanes.size() == 1 ) {
        _blockXML->_midplanes[0]->_switches.clear();
    }

    // Check that the required block resources exist in the database
    vector<string> missing;
    if (no_check == false) { // we're not no_check
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
            reply << mmcs_client::FAIL << "create_block: cannot determine status of hardware resources " << mmcs_client::DONE;
            return;
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

    // Populate the BCNodeInfo and BCNodecardInfo lists with information from the compute nodes and I/O nodes.
    // Iterate over populated midplanes.

    BCNodeInfo *nodeInfo;

    // Iterate over midplanes
    for (
            BGQBlockNodeConfig::midplane_iterator mpiter = _block->midplaneBegin();
            mpiter != _block->midplaneEnd();
            ++mpiter
    )
    {
        if (process_midplane(*mpiter, reply, false) == false) {
            // Note that reply is set on previous call
            return;
        }
    }

    // Do passthru midplanes
    BOOST_FOREACH(BGQMidplaneNodeConfig* midplane, _block->passThruMidplanes()) {
        if (process_midplane(*midplane, reply, true) == false) {
            // Note that reply is set on previous call
            return;
        }
    }

    // Get the link training I/O nodes. These will NOT be all of the I/O nodes,
    // just the ones we need for link training.
    vector<BGQIOBoardNodeConfig *> ioBoards = _block->linkTrainingIOBoards();
    LOG_TRACE_MSG( ioBoards.size() << " I/O boards used for link training." );
    for (unsigned i = 0; i < ioBoards.size(); ++i) {
        BGQIOBoardNodeConfig* ioboard = ioBoards[i];
        BCNodecardInfo *nodecardInfo = new BCNodecardInfo();
        for (
                BGQNodeBoard::linkchip_iterator chipit = ioboard->linkChipBegin();
                chipit != ioboard->linkChipEnd();
                ++chipit
            )
        {
            BCLinkchipInfo* chipinfo = new BCLinkchipInfo();
            chipinfo->_jtag = chipit->jtag();
            chipinfo->_personality = chipit->personality();
            chipinfo->_block = _block;
            chipinfo->_ioboard = true;
            // Can't use init_location() for I/O board link chip in compute block
            ostringstream ostr;
            ostr << ioboard->posInMachine() << "-" << IO_CARD_JTAGPORT_TO_LCTN[chipit->jtag()];
            chipinfo->_location = ostr.str();
            nodecardInfo->_linkChips.push_back(chipinfo);
            LOG_TRACE_MSG( "Added link chip " << chipinfo->location() );
        }

        // For each I/O board, build it and stick it in the icon list.
        // Note: We need to add each I/O board to the board bitset
        nodecardInfo->_block = _block;
        nodecardInfo->_ioboard = true;
        nodecardInfo->_boardcoordA = ioboard->allIOBoardA();
        nodecardInfo->_boardcoordB = ioboard->allIOBoardB();
        nodecardInfo->_boardcoordC = ioboard->allIOBoardC();
        nodecardInfo->_boardcoordD = ioboard->allIOBoardD();
        nodecardInfo->_linkio = true;
        nodecardInfo->init_location(ioboard);
        getIcons().push_back(nodecardInfo);
        LOG_TRACE_MSG( "Added I/O board " << nodecardInfo->location());

        vector<BGQIONodePos>* ioNodes = ioBoards[i]->computes();
        for (unsigned j = 0; j < ioNodes->size(); ++j) {
            // ALL of the I/O nodes that are in this list are connected I/O.
            // Non-connected I/O nodes will NOT be returned by bgqconfig.
            BGQIONodePos pos = (*ioNodes)[j];
            BGQNodePos nullpos(0,0);

            nodeInfo = new BCNodeInfo();
            nodeInfo->_block = _block;
            nodeInfo->_pos = nullpos;
            nodeInfo->_iopos = pos;
            nodeInfo->_linkio = true;
            nodeInfo->_boardcoordA = ioboard->allIOBoardA();
            nodeInfo->_boardcoordB = ioboard->allIOBoardB();
            nodeInfo->_boardcoordC = ioboard->allIOBoardC();
            nodeInfo->_boardcoordD = ioboard->allIOBoardD();
            nodeInfo->_iopersonality = ioboard->nodeConfig(pos)->_personality;

            // Can't use init_location() for I/O in a compute block.
            ostringstream ostr;
            ostr << ioboard->posInMachine() << "-" << nodeInfo->_iopos;
            nodeInfo->_location = ostr.str();

            ++_numNodesTotal; // computes total number of nodes in the block
            getNodes().push_back(nodeInfo);
            nodecardInfo->_nodes.push_back(nodeInfo);
            LOG_TRACE_MSG( "Added I/O node " << nodeInfo->location() );
        }
    }

    // Add all BCNodeInfo, BCNodecardInfo, BCLinkcardInfo, BCLinkchipInfo objects to CNBlockController::_targets
    for (unsigned i = 0; i < getNodes().size(); ++i) {
        _targets.push_back(getNodes()[i]);
    }

    // If not sharing the node board, then include it in the target set, so it will be locked
    if (_isshared == false) {
        for (unsigned i = 0; i < getIcons().size(); ++i) {
            _targets.push_back(getIcons()[i]);
        }
    }

    for (unsigned i = 0; i < getLinkchips().size(); ++i) {
        _targets.push_back(getLinkchips()[i]);
    }

    for (unsigned i = 0; i < _targets.size(); ++i) {
        _targets[i]->_locateId = i; // Give each device under our control a unique target specifier
    }

    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
CNBlockController::connect(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        const BlockControllerTarget* pTarget
        )
{
    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);

    BlockControllerBase::connect(args, reply, pTarget);
    if (reply.getStatus() != mmcs_client::CommandReply::STATUS_OK) {
        return;  // connecting to mcServer failed
    }

    // should a pgood be sent before the BootBlockRequest?
    const bool usePgood = std::find( args.begin(), args.end(), "pgood" ) != args.end();

    // Send a pgood reset request to the processor nodes in the target set
    if (usePgood) {
        pgood_reset(reply, pTarget);
        if (reply.getStatus() < 0) {
            LOG_ERROR_MSG("pgood reset: " << reply.str());
            mmcs_client::CommandReply bogus;
            disconnect(args, bogus);
            return;
        }
    }

    // Start the mailbox monitor
    startMailbox(reply);
    if (reply.getStatus() != 0) {
        LOG_ERROR_MSG("starting mailbox monitor: " << reply.str());
        mmcs_client::CommandReply bogus;
        disconnect(args, bogus);
        return;
    }

    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
CNBlockController::disconnect(
        const deque<string>& args,
        mmcs_client::CommandReply& reply
        )
{
    BlockControllerBase::disconnect(args, reply);
    if (reply.getStatus() != mmcs_client::CommandReply::STATUS_OK) {
        return;
    }

    // Clear the barrier bitsets
    for (std::map<std::string, RackBitset>::iterator it = _rackbits.begin();
        it != _rackbits.end(); ++it) {
        it->second.Reset();
    }
}

void
CNBlockController::boot_block(
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
    } BOOST_SCOPE_EXIT_END;

    if (!isConnected()) {
        reply << mmcs_client::FAIL << "Block is not connected" << mmcs_client::DONE;
        return;
    }

    // Parse arguments
    string uload;
    string domains;
    string bootOptions;  // name=value boot options
    string steps;
    bool tolerateFaults = false;   // LLCS should NOT attempt to continue booting after detecting a fault on a compute block

    // Log boot_block arguments
    ostringstream oss;
    oss << "Boot block arguments: ";
    for (deque<string>::const_iterator arg = args.begin(); arg != args.end(); ++arg) {
        oss << *arg << " ";
    }
    LOG_INFO_MSG(oss.str());

    // Parse arguments
    bool bootsteps = false;

    for (deque<string>::const_iterator arg = args.begin(); arg != args.end(); ++arg) {
        bool valid = true; // valid argument
        const vector<string> tokens( Tokenize(*arg, "=") );

        if (tokens.size() == 0)                 // handle an invalid format
            continue;
        if (tokens[0].size() == 0)              // handle a null argument
            continue;
        if (tokens[0] == "uloader") {
            if (tokens.size() == 2)
                uload = tokens[1];
            else
                valid = false;
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

        if  (!valid) {
            reply << mmcs_client::FAIL << "Unrecognized argument: " << *arg << mmcs_client::DONE;
            return;
        }
    }

    if (isStarted() && !bootsteps)
    {
        reply << mmcs_client::FAIL << "Block is already booted" << mmcs_client::DONE;
        return;
    }

    // This may be mmcs_lite, in which the default images are passed via properties
    if (uload.empty()) {
        uload.append(common::Properties::getProperty("uloader"));
    }

    if (domains.empty()) {
        domains = common::Properties::getProperty("cn_domains");
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

    // Create the BootBlockRequest
    MCServerMessageSpec::BootBlockRequest mcBootBlockRequest(
            _blockName,
            _userName,
            tolerateFaults,
            uload,
            _bootCookie,
            bootOptions,
            0 /* boot start time */
            );

    // Initialize the node board vector in the BootBlockRequest
    for (vector<BCIconInfo*>::const_iterator nciter = getIcons().begin(); nciter != getIcons().end(); ++nciter) {
        if (typeid(**nciter) != typeid(BCNodecardInfo)) { // only process node boards
            continue;
        }

        // Only process connected node boards or node boards with open processor cards
        if ((*nciter)->_open || (_isshared && (*nciter)->nodesOpen())) {
            BCNodecardInfo* ncinfo = dynamic_cast<BCNodecardInfo*>(*nciter);

            MCServerMessageSpec::BootBlockRequest::NodeCard nodeCard(ncinfo->location());
            // It might be an I/O node board. We send a trainonly message for connected I/O
            MCServerMessageSpec::BootBlockRequest::IoCard ioCard(ncinfo->location());

            // Add BootBlockRequest::Node entries for all the nodes on the node board
            for (unsigned i = 0; i < ncinfo->_nodes.size(); ++i) {
                BCNodeInfo* const ninfo = dynamic_cast<BCNodeInfo*>(ncinfo->_nodes[i]);
                if (ninfo->_open) { // only boot connected nodes
                    bool trainOnly = false; // do all boot steps, not just I/O link training
                    int jport = 0;
                    // Use the _linkio flag instead of isIONode within a compute block
                    if (ninfo->_linkio) { // I/O nodes are always trainOnly in a compute block.
                        if (ninfo->_iopos.trainOnly()) {
                            trainOnly = true;
                        }
                        jport = ninfo->_iopos.jtagPort();
                    } else {
                        jport = ninfo->_pos.jtagPort();
                    }

                    const string nodeLoc = ninfo->location();
                    const string nodeCust = _machine_config_data + _nodeCustomization[nodeLoc];
                    Personality_t pers = ninfo->personality();
                    if (svchost_config && _block) {
                        pers = BGQBlockNodeConfig::updatePersonality(ninfo->personality(), svc_options);
                    }
                    const MCServerMessageSpec::BootBlockRequest::Node node(
                            jport, // jtag position of the node
                            ninfo->_linkio, // is this an I/O node?
                            trainOnly,
                            string(reinterpret_cast<char*>(&pers), sizeof(pers)),
                            nodeCust,
                            nodeCust.length()
                            );
                    if (ncinfo->_ioboard) {
                        ioCard._nodes.push_back(node);
                    } else {
                        nodeCard._nodes.push_back(node);
                    }
                }
            }

            // Now get all of the link chips on the node card.
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
                if (ncinfo->_ioboard) {
                    ioCard._linkChips.push_back(chip);
                } else {
                    nodeCard._linkChips.push_back(chip);
                }
            }

            // Save the BootBlockRequest::NodeCard object in the BootBlockRequest
            if (ncinfo->_ioboard && !_diags) {
                mcBootBlockRequest._ioCards.push_back(ioCard);
            } else if(ncinfo->_ioboard == false) {
                mcBootBlockRequest._nodeCards.push_back(nodeCard);
            }
        }
    }

    if (parseDomains(mcBootBlockRequest, domains) == false) {
        reply << mmcs_client::FAIL << "Bad domain syntax" << mmcs_client::DONE;
        return;
    }

    if (!parseSteps(reply, mcBootBlockRequest, steps)) {
        return; // Bad step, our reply is already set
    }

    mutex.Lock(&_mutex);

    // Check if we are disconnecting
    if (isDisconnecting()) {
        reply << mmcs_client::FAIL << disconnectReason() << mmcs_client::DONE;
        return;
    }

    // Mark node targets as started
    for (unsigned i = 0; i < getNodes().size(); ++i) {
        BCNodeInfo *nodeInfo = getNodes()[i];
        if (nodeInfo->_open) {
            nodeInfo->_state = NST_PROGRAM_RUNNING;
            nodeInfo->_initialized = false;
            nodeInfo->_haltComplete = false;
            _numNodesStarted   += !(nodeInfo->isIOnode());
        }
    }
    _isstarted = true; // boot has been started
    mutex.Unlock();

    LOG_DEBUG_MSG("Sending boot request.");
    if (timer) {
        timer->stop();
    }

    // send Boot command to mcMonitor
    MCServerMessageSpec::BootBlockReply   mcBootBlockReply;
    try {
        static const boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
        _boot_block_start = boost::posix_time::microsec_clock::local_time();
        BOOST_SCOPE_EXIT( (&_boot_block_start) ) {
            _boot_block_start = boost::posix_time::ptime();
        } BOOST_SCOPE_EXIT_END;

        if (!hardWareAccessBlocked()) {
            mcBootBlockRequest._startTime = (_boot_block_start - epoch).total_seconds();
            _mcServer->bootBlock(mcBootBlockRequest, mcBootBlockReply);
        } else {
            std::ostringstream msg;
            msg << "SubnetMc managing hardware for " << _blockName << " is temporarily unavailable.";
            LOG_ERROR_MSG(msg.str());
            reply << mmcs_client::FAIL << msg.str() << mmcs_client::DONE;
            return;
        }
    } catch (const exception &e) {
        mcBootBlockReply._rc = -1;
        mcBootBlockReply._rt = e.what();
    }

    if (mcBootBlockReply._rc) {
        std::ostringstream msg;
        msg << "Boot block error: " << mcBootBlockReply._rt;
        LOG_ERROR_MSG(msg.str());
        reply << mmcs_client::FAIL << msg.str() << mmcs_client::DONE;
        return;
    }

    LOG_DEBUG_MSG("Received boot reply.");
    _block_shut_down = false;

    // Check if we are disconnecting
    if (isDisconnecting()) {
        reply << mmcs_client::FAIL << disconnectReason() << mmcs_client::DONE;
        return;
    }
    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
CNBlockController::build_shutdown_req(
        MCServerMessageSpec::ShutdownBlockRequest& mcShutdownBlockRequest
        )
{
    // Initialize the node board vector in the ShutdownBlockRequest
    for (vector<BCIconInfo*>::const_iterator nciter = getIcons().begin(); nciter != getIcons().end(); ++nciter) {
        if (typeid(**nciter) != typeid(BCNodecardInfo)) { // only process node boards
            continue;
        }

        // Only process connected node boards or node boards with open processor cards
        if ((*nciter)->_open || (_isshared && (*nciter)->nodesOpen())) {
            BCNodecardInfo* const ncinfo = dynamic_cast<BCNodecardInfo*>(*nciter);

            MCServerMessageSpec::ShutdownBlockRequest::NodeCard nodeCard(ncinfo->location());
            // It might be an I/O node board. We send a trainonly message for connected I/O
            MCServerMessageSpec::ShutdownBlockRequest::IoCard ioCard(ncinfo->location());

            // Add ShutdownBlockRequest::Node entries for all the nodes on the node board
            for (unsigned i = 0; i < ncinfo->_nodes.size(); ++i) {
                BCNodeInfo* ninfo = dynamic_cast<BCNodeInfo*>(ncinfo->_nodes[i]);
                if (ninfo->_open) { // only boot connected nodes
                    bool trainOnly = false; // do all boot steps, not just I/O link training
                    int jport = 0;
                    // Use the _linkio flag instead of isIONode within a compute block
                    if (ninfo->_linkio) { // I/O nodes are always trainOnly in a compute block.
                        if (ninfo->_iopos.trainOnly()) {
                            trainOnly = true;
                        }
                        jport = ninfo->_iopos.jtagPort();
                    } else {
                        jport = ninfo->_pos.jtagPort();
                    }

                    const string nodeLoc = ninfo->location();
                    const string nodeCust = _machine_config_data + _nodeCustomization[nodeLoc];
                    MCServerMessageSpec::ShutdownBlockRequest::Node node(
                            jport, // jtag position of the node
                            ninfo->_linkio, // is this an I/O node?
                            trainOnly,
                            string((char*) &(ninfo->personality()), sizeof(ninfo->personality())), // node personality
                            nodeCust, nodeCust.length(),
                            nodeLoc
                            );

                    if (ncinfo->_ioboard) {
                        ioCard._node_location.push_back(node);
                    } else {
                        nodeCard._node_location.push_back(node);
                    }
                }
            }

            // Now get all of the link chips on the node card.
            for (
                    std::vector<BCLinkchipInfo*>::const_iterator chipit = ncinfo->_linkChips.begin();
                    chipit != ncinfo->_linkChips.end();
                    ++chipit
               )
            {
                // Create the personality.
                MCServerMessageSpec::ShutdownBlockRequest::LinkChipPersonality pers;
                // First, add the BQLSwitches.
                for (unsigned i = 0; i < 4; ++i) {
                    const BQLSwitch ncBqlSwitch = (*chipit)->_personality.getSwitch(i);
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
                const MCServerMessageSpec::ShutdownBlockRequest::Link link((*chipit)->location());
                const MCServerMessageSpec::ShutdownBlockRequest::LinkChip chip((*chipit)->_jtag, pers);

                // And push it in the chip vector.
                if (ncinfo->_ioboard) {
                    ioCard._linkChips.push_back(chip);
                    ioCard._link_location.push_back(link);
                } else {
                    nodeCard._linkChips.push_back(chip);
                    nodeCard._link_location.push_back(link);
                }
            }

            // Save the ShutdownBlockRequest::NodeCard object in the ShutdownBlockRequest
            if (ncinfo->_ioboard) {
                mcShutdownBlockRequest._ioCards.push_back(ioCard);
            } else {
                mcShutdownBlockRequest._nodeCards.push_back(nodeCard);
            }
        }
    }
    // LOG_TRACE_MSG("Shutdown block request: " << mcShutdownBlockRequest );
}

void
CNBlockController::shutdown_block(
        mmcs_client::CommandReply& reply,
        const std::deque<std::string>& args
        )
{
    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);

    if (_block_shut_down) {
        LOG_DEBUG_MSG("Compute block " << _blockName << " already shut down.");
        reply << mmcs_client::OK << mmcs_client::DONE;
        return;
    }

    if (!isConnected()) {
        reply << mmcs_client::FAIL << "Block is not connected" << mmcs_client::DONE;
        return;
    }

    if (!isStarted()) {
        reply << mmcs_client::FAIL << "Block is not booted" << mmcs_client::DONE;
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
    if (!_block_shut_down) {
        try {
            if (!hardWareAccessBlocked()) {
                _mcServer->shutdownBlock(mcShutdownBlockRequest, mcShutdownBlockReply);
                sent = true;
            } else {
                std::ostringstream msg;
                msg << "SubnetMc managing hardware for " << _blockName << " is temporarily unavailable.";
                LOG_ERROR_MSG(msg.str());
                reply << mmcs_client::FAIL << msg.str() << mmcs_client::DONE;
                return;
            }
        } catch (const exception &e) {
            mcShutdownBlockReply._rc = -1;
            mcShutdownBlockReply._rt = e.what();
        }

        if (mcShutdownBlockReply._rc != 0) {
            LOG_WARN_MSG("Error shutting down block " << _blockName << ": " << mcShutdownBlockReply._rt);
            reply << mmcs_client::ABORT << "shutdown_block: " << mcShutdownBlockReply._rt;
            // Don't return here.  Let it go to FREE and rely on LLCS to set hardware in error.
        }

        _isstarted = false;
        if (sent) {
            LOG_INFO_MSG("Shutdown sent");
        }
    }

    if (sent) {
        _block_shut_down = true;
    }
    reply << mmcs_client::DONE;
}

void
CNBlockController::show_barrier(
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK;

    for ( std::vector<BCNodeInfo*>::const_iterator it = _nodes.begin(); it != _nodes.end(); ++it ) {
        const std::string loc = (*it)->location();
        const std::string rack = loc.substr(0,3);
        if ( _rackbits.find(rack) != _rackbits.end() && !_rackbits[rack].Flagged(loc) ) {
            reply << loc << std::endl;
        }
    }

    reply << mmcs_client::DONE;
    return;
}

bool
CNBlockController::processRASMessage(
        RasEvent& rasEvent
        )
{
    LOG_TRACE_MSG("Process RAS message " << rasEvent.getDetails()[RasEvent::MSG_ID]);
    bool filtered = false;

    // Barrier RAS message
    if (rasEvent.msgId() == 0x00040096) {
        const std::string locstr = rasEvent.getDetails()[RasEvent::LOCATION];
        const std::string rack = locstr.substr(0,3);
        _rackbits[rack].Set(locstr);

        unsigned satisfied_nodes = 0;
        for (std::map<std::string, RackBitset>::const_iterator it = _rackbits.begin(); it != _rackbits.end(); ++it) {
            satisfied_nodes += it->second.BitsSet();
        }

        if (satisfied_nodes == _numComputesTotal) {
            LOG_DEBUG_MSG("Barrier satisfied.");
            // Clear all barrier bits so we can do another.
            for (std::map<std::string, RackBitset>::iterator it = _rackbits.begin(); it != _rackbits.end(); ++it) {
                it->second.Reset();
            }

            _do_barrier_ack = true;
        }

        filtered = true;
    }

    // Write the RAS message to one or more log files
    if (!filtered) {
        printRASMessage(rasEvent);
    }

    if (_diags && !_insertDiagsRAS) {
        // filter RAS that occurs from diags unless they request it
        filtered = true;
    }

    return filtered;
}


} } // namespace mmcs::server
