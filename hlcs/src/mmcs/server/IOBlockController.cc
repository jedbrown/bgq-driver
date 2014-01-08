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
    _residual_target( NULL )
{
    _isIOblock = true;
    _tolerating_faults = false;
}

IOBlockController::~IOBlockController()
{
    if(_residual_target) {
        delete _residual_target;
        _residual_target = 0;
    }
}

void
IOBlockController::create_block(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        std::istream* blockStreamXML
        )
{
    LOG_DEBUG_MSG(__FUNCTION__);
    BGQDB::DIAGS_MODE diags = BGQDB::NO_DIAGS; // 'diags' parameter specified
    FILE *svchost_config = NULL; // file for svchost configuration file
    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);

    BOOST_SCOPE_EXIT( (&svchost_config) ) {
        if(svchost_config != NULL)
            fclose(svchost_config);
    } BOOST_SCOPE_EXIT_END;

    if (isCreated())
    {
        reply << mmcs_client::FAIL << "block is already created" << mmcs_client::DONE;
        return;
    }

    _targetsetMode = MCServerMessageSpec::WUAR;      // default target set open mode

    // parse options
    if ( args.empty() && !blockStreamXML)
    {
        reply << mmcs_client::FAIL << "args?" << mmcs_client::DONE;
        return;
    }

    string blockFile;
    if (blockStreamXML == NULL) {
        blockFile = args[0];
        args.pop_front(); // take off the block
    }

    bool no_check = false;

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
        } else if (tokens[0] == "no_check") {
            no_check = true;
        } else if (tokens[0] == "hw_check") {
            no_check = false;
        } else if (tokens[0] == "tolerate_faults") {
            _tolerating_faults = true;
        } else if (tokens[0] == "svchost_options") {
            if (tokens.size() == 2) {
                if ((svchost_config = fopen(tokens[1].c_str(), "r")) == NULL) {
                    reply << mmcs_client::FAIL << "can't open svchost_options file \"" << tokens[1] << "\" : " << strerror(errno) << mmcs_client::DONE;
                    return;
                }
                continue;
            }
        }
    }

    // extract machine configuration information from the BGQMachineXML
    initMachineConfig(reply);
    if (reply.getStatus() != 0) {
        return;
    }

    // create the BGQBlockXML classes from the input XML file
    if (_blockXML != NULL)
    {
        delete _blockXML;
        _blockXML = NULL;
    }
    try
    {
        if (blockStreamXML == NULL) {
            //assume a filename was given
            _blockXML = BGQBlockXML::create(blockFile.c_str(), _machineXML);
        } else {
            //use the stream object
            _blockXML = BGQBlockXML::create(*blockStreamXML, _machineXML);
        }
    }
    catch (const XMLException& e)
    {
        reply << mmcs_client::FAIL << "create_block: " << e.what() << mmcs_client::DONE;
        return;
    }
    catch (const BGQNodeConfigException& e)
    {
        reply << mmcs_client::FAIL << "create_block: " << e.what() << mmcs_client::DONE;
        return;
    }
    if (_blockXML == NULL)
    {
        reply << mmcs_client::FAIL << "create_block: can't load block XML, hardware resources may be marked Missing or Error " << mmcs_client::DONE;
        return;
    }

    // save the blockname for use as target set name
    if (_blockName.empty())
        _blockName = _blockXML->_name;

    // Check that the required block resources exist in the database
    if (no_check == false) { // we're not no_check
        std::vector<std::string> missing;
        if (BGQDB::queryMissing(_blockName, missing, diags) == 0) {
            if (missing.size() > 0) {
                // build an error message identifying the missing resources
                reply << mmcs_client::FAIL << "create_block: resources are unavailable - ";
                for (unsigned i = 0; i < missing.size(); ++i)
                    if (i == 0)
                        reply << missing[i];
                    else
                        reply << ", " << missing[i];
                reply << mmcs_client::DONE;
                return;
            }
        } else {
            reply << mmcs_client::FAIL << "create_block: error from BGQDB::queryMissing, can't determine status of hardware resources " << mmcs_client::DONE;
            return;
        }
    } else {
        // no_check means we don't care if there are nodes in error.
        // So, we get the list of nodes in error to use later.
        if(BGQDB::queryError(_blockName, _error_nodes) == BGQDB::OK) {
            if(_error_nodes.size() > 0) {
                std::ostringstream msg;
                msg << "The following nodes are in error: ";
                bool first = true;
                BOOST_FOREACH(const std::string& node, _error_nodes) {
                    if(first) {
                        first = false;
                    } else msg << ", ";
                    msg << node;
                }
                msg << ". Block will boot anyway because no_check specified.";
                LOG_INFO_MSG(msg.str());
            }
        } else {
            LOG_WARN_MSG("Could not determine nodes in error.");
        }
    }

    // create a new BGQBlockNodeConfig object
    try
    {
        _block = new BGQBlockNodeConfig(_machineXML, _blockXML, svchost_config);
    }
    catch (const BGQNodeConfigException& e)
    {
        reply << mmcs_client::FAIL << "create_block: " << e.what() << mmcs_client::DONE;
        return;
    }

    _machine_config_data = _block->getConfigData();

    // populate the BCNodeInfo and BCNodecardInfo lists
    {
        BCNodeInfo *nodeInfo;
        BCNodecardInfo *nodecardInfo;
        map<string, BCNodeInfo*> nodesByLocation; // for ordering pset nodes by location within pset

        // iterate over IO boards
        for (
                BGQBlockNodeConfig::ioboard_iterator ioiter = _block->ioboardBegin();
                ioiter != _block->ioboardEnd();
                ++ioiter
            )
        {
            BGQIOBoardNodeConfig& ioboard = *ioiter;

            std::string rack = ioboard.posInMachine().substr(0,3);
            IOBoardBitset bs(rack);
            _rackbits[rack] = bs;

            if(HardwareBlockList::find_in_list(rack) == true) {
                std::ostringstream msg;
                msg << "IO rack " << rack << "'s controlling subnet is temporarily unavailable.";
                reply << mmcs_client::FAIL << msg.str() << mmcs_client::DONE;
                LOG_INFO_MSG(msg.str());
                return;
            }

            nodesByLocation.clear();

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
                BCLinkchipInfo* chipinfo = new BCLinkchipInfo();
                chipinfo->_jtag = chipit->jtag();
                chipinfo->_personality = chipit->personality();
                chipinfo->_block = _block;
                chipinfo->_ioboard = true;
                chipinfo->init_location();
                nodecardInfo->_linkChips.push_back(chipinfo);
                LOG_TRACE_MSG( "added link chip " << chipinfo->location() );
            }

            // iterate over IO nodes
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
                if(node.trainOnly())
                    nodeInfo->_linkio = true; // This keeps train only nodes out of the target set
                nodeInfo->_iopersonality = ioboard.nodeConfig(node)->_personality;
                _targetLocationMap[nodeInfo->location()] = nodeInfo; // maintain a map for location lookup
                nodesByLocation[nodeInfo->location()] = nodeInfo;    // order nodes by location within pset
                ++_numNodesTotal;                                    // computes total number of nodes in the block

                // Find the node card and add the node
                nodecardInfo->_nodes.push_back(nodeInfo);  // keep track of the io nodes in each node card
            }

            // order nodes by jtag id within pset
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
    LOG_DEBUG_MSG( _numNodesTotal << " total nodes in target list" );

    if(_error_nodes.size() >= getNodes().size()) {
        reply << mmcs_client::FAIL << "All nodes are in error state.  Cannot boot the block." << mmcs_client::DONE;
        return;
    }

    // add all BCNodeInfo, BCNodecardInfo,
    // BCLinkchipInfo objects to IOBlockController::_targets
    for (unsigned i = 0; i < getNodes().size(); ++i)
        _targets.push_back(getNodes()[i]);

    // if not sharing the IO board, then include it in the target set, so it will be locked
    if (_isshared == false && getNodes().size() >= 8 ) {
        for (unsigned i = 0; i < getIcons().size(); ++i)
            _targets.push_back(getIcons()[i]);
    }
    for (unsigned i = 0; i < getLinkchips().size(); ++i)
        _targets.push_back(getLinkchips()[i]);

    for (unsigned i = 0; i < _targets.size(); ++i) {
        _targets[i]->_locateId = i; // Give each device under our control a unique target specifier
        LOG_TRACE_MSG( " location: " << _targets[i]->location() << " in target list" );
    }

    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
IOBlockController::connect(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        const BlockControllerTarget* pTarget,
        bool add_targets
        )
{
    LOG_DEBUG_MSG(__FUNCTION__);
    BlockControllerBase::connect(args, reply, pTarget, add_targets);
    if(reply.getStatus() != mmcs_client::CommandReply::STATUS_OK)
        return;  // connecting to mcserver failed

    bool   usePgood = false;        // should a pgood be sent before the BootIoBlockRequest?

    for (unsigned i = 0; i < args.size(); ++i) {
        if (args[i] == "pgood")
            usePgood = true;
    }

    // create a log file for I/O nodes if --iologdir was specified at startup
    for (unsigned i = 0; i < pTarget->getNodes().size(); ++i)
    {
        BCNodeInfo *nodeInfo = pTarget->getNodes()[i];
        if (nodeInfo->_open)
        {
            if (!BlockControllerBase::openLog(nodeInfo))
            {
                reply << mmcs_client::FAIL << "can't open I/O log file for " << nodeInfo->location() << mmcs_client::DONE;
                return;
            }
        }
    }

    // send a pgood reset request to the processor nodes in the target set
    if (usePgood)
    {
        pgood_reset(reply, pTarget);
        if (reply.getStatus() < 0)
        {
            LOG_DEBUG_MSG("pgood_reset: " << reply.str());
            mmcs_client::CommandReply bogus;
            disconnect(args, bogus);
            return;
        }
    }

    // start the mailbox monitor
    startMailbox(reply);
    if (reply.getStatus() != 0)
    {
        LOG_INFO_MSG("startMailbox: " << reply.str());
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
    LOG_DEBUG_MSG(__FUNCTION__);
    BlockControllerBase::disconnect(args, reply);
    if(reply.getStatus() != mmcs_client::CommandReply::STATUS_OK)
        return;

    // Clear barrier bits
    for(std::map<std::string, IOBoardBitset>::iterator it = _rackbits.begin();
        it != _rackbits.end(); ++it) {
        it->second.Reset();
    }

    // close log files
    for (unsigned i = 0; i < _nodes.size(); ++i )
    {
        BCNodeInfo *nodeInfo = _nodes[i];
        if ( !nodeInfo ) continue;
        if ( !nodeInfo->_mailboxOutput ) continue;

        if ( fclose(nodeInfo->_mailboxOutput) ) {
            const int error = errno;
            LOG_WARN_MSG( "could not close descriptor " << fileno( nodeInfo->_mailboxOutput) << ": " << strerror(error) );
        } else {
            LOG_DEBUG_MSG( "closed I/O log file for " << nodeInfo->location() );
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
    LOG_DEBUG_MSG(__FUNCTION__);
    PthreadMutexHolder mutex;
    FILE* svchost_config = NULL; // file for svchost configuration file

    BOOST_SCOPE_EXIT( (&svchost_config) ) {
        if(svchost_config != NULL)
            fclose(svchost_config);
    } BOOST_SCOPE_EXIT_END

    if (!isCreated())
    {
        reply << mmcs_client::FAIL << "block is not created" << mmcs_client::DONE;
        return;
    }
    if (!isConnected())
    {
        reply << mmcs_client::FAIL << "block is not connected" << mmcs_client::DONE;
        return;
    }

    // parse arguments

    string uload;
    string domains;
    string steps;
    string bootOptions; // name=value boot options
    bool tolerateFaults = _tolerating_faults;   // LLCS should NOT attempt to continue booting after detecting a fault on a compute block

    // log boot_block arguments
    ostringstream oss;
    oss << __FUNCTION__ << " ";
    for (deque<string>::const_iterator arg = args.begin(); arg != args.end(); ++arg)
        oss << *arg << " ";
    LOG_INFO_MSG(oss.str());

    bool bootsteps = false;
    // parse arguments
    for (deque<string>::const_iterator arg = args.begin(); arg != args.end(); ++arg)
    {
        bool valid = true; // valid argument
        vector<string> tokens;
        tokens = Tokenize(*arg, "=");           // split based on the equal sign.

        if (tokens.size() == 0)                 // handle an invalid format
            continue;
        if (tokens[0].size() == 0)              // handle a null argument
            continue;
        if (tokens[0] == "uloader")
        {
            if (tokens.size() == 2)
                uload = tokens[1];
            else
                valid = false;
        }
        else if (tokens[0] == "steps")
        {
            bootsteps = true;
            steps = *arg;
        }
        else if (tokens[0] == "domain") {
            domains += *arg;
        }
        else if (tokens[0] == "tolerate_faults") {
            tolerateFaults = true;
        }
        else if (tokens[0] == "svchost_options") {
            if (tokens.size() == 2) {
                if ((svchost_config = fopen(tokens[1].c_str(), "r")) == NULL) {
                    reply << mmcs_client::FAIL << "can't open svchost_options file \"" << tokens[1] << "\" : " << strerror(errno) << mmcs_client::DONE;
                    return;
                }
                continue;
            }
        }
        else
        {
            bootOptions.append(*arg).append(" ");
        }

        if (!valid)
        {
            reply << mmcs_client::FAIL << "unrecognized argument: " << *arg << mmcs_client::DONE;
            return;
        }
    }

    if (isStarted() && !bootsteps)
    {
        reply << mmcs_client::FAIL << "block is already booted" << mmcs_client::DONE;
        return;
    }

    // This may be mmcs_lite, in which the default images are passed via properties
    if (uload.empty())
        uload.append(Properties::getProperty("uloader"));

    if (domains.empty()) {
        domains = Properties::getProperty("io_domains");
        if(domains.find("cores=") != std::string::npos) {
            domains.replace(domains.find('-'), 1, 1, '$');
        }
        if(domains.find("memory=") != std::string::npos) {
            domains.replace(domains.find('-'), 1, 1, '$');
        }
    }

    std::vector<std::string> svc_options;
    if(svchost_config && _block) {
        try {
            svc_options = _block->processSvcHostFile(svchost_config);
        }
        catch (const BGQNodeConfigException& e)
        {
            reply << mmcs_client::FAIL << "boot_block: " << e.what() << mmcs_client::DONE;
            return;
        }
    }

    // Save the node customization in case we need it on reboot_nodes
    _nodeCustomization = ncust;
    this->calculateRatios();

    // create the BootIoBlockRequest
    MCServerMessageSpec::BootBlockRequest mcBootIoBlockRequest(
            _blockName,
            _userName,
            tolerateFaults,
            uload,
            _bootCookie,
            bootOptions,
            0 /* boot start time */
            );

    // remember boot options for reboot_nodes
    _bootOptions = bootOptions;

    // initialize the Nodecard vector in the BootIoBlockRequest
    for (vector<BCIconInfo*>::const_iterator nciter = getIcons().begin(); nciter != getIcons().end(); ++nciter) {
        if (typeid(**nciter) != typeid(BCNodecardInfo)) // only process node cards
            continue;

        // only process connected node cards or node cards with open processor cards
        if ((*nciter)->_open || (_isshared && (*nciter)->nodesOpen()))
        {
            BCNodecardInfo* ncinfo = dynamic_cast<BCNodecardInfo*>(*nciter);

            MCServerMessageSpec::BootBlockRequest::IoCard nodeCard(ncinfo->location());

            // add BootIoBlockRequest::IoNode entries for all the nodes in the node card
            for (unsigned i = 0; i < ncinfo->_nodes.size(); ++i)
            {
                BCNodeInfo* ninfo = dynamic_cast<BCNodeInfo*>(ncinfo->_nodes[i]);
                if (ninfo->_open) // only boot connected nodes
                {
                    const string nodeLoc = ninfo->location();
                    const string nodeCust = _machine_config_data + _nodeCustomization[nodeLoc];

                    Personality_t pers = ninfo->personality();
                    if(svchost_config && _block) {
                        pers = BGQBlockNodeConfig::updatePersonality(ninfo->personality(), svc_options);
                    }

                    MCServerMessageSpec::BootBlockRequest::Node node(
                            ninfo->_iopos.jtagPort(), // jtag position of the node
                            true, // isIoNode
                            ninfo->_iopos.trainOnly(),
                            string((char*) &(pers),sizeof(pers)),
                            nodeCust,
                            nodeCust.length()
                            );
                    // Make sure it is in not in the list of nodes in error.
                    if(std::find(_error_nodes.begin(),
                                _error_nodes.end(),
                                ninfo->location()) ==
                            _error_nodes.end()) {
                        nodeCard._nodes.push_back(node);
                    }
                }
            }

            // Now get all of the link chips on the node card.
            for(std::vector<BCLinkchipInfo*>::const_iterator chipit = ncinfo->_linkChips.begin();
                    chipit != ncinfo->_linkChips.end(); ++chipit) {
                // Create the personality.
                MCServerMessageSpec::BootBlockRequest::LinkChipPersonality pers;

                // First, add the BQLSwitches.
                for(unsigned i = 0; i < 4; ++i){
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
                for(unsigned i = 0; i < 4; ++i){
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

            // save the BootIoBlockRequest::IoCard object in the BootIoBlockRequest
            mcBootIoBlockRequest._ioCards.push_back(nodeCard);
        }
    }

    // remember domain for reboot_nodes
    _domains = domains;

    // Add domain info to the boot request.
    if(parseDomains(mcBootIoBlockRequest, domains) == false) {
        reply << mmcs_client::FAIL << "Bad domain syntax" << mmcs_client::DONE;
        return;
    }

    if(!parseSteps(reply, mcBootIoBlockRequest, steps))
        return; // Bad step, our reply is already set

    mutex.Lock(&_mutex);

    // Check if we are disconnecting
    if (isDisconnecting())
    {
        reply << mmcs_client::FAIL << disconnectReason() << mmcs_client::DONE;
        return;
    }

    // mark node targets as started
    for (unsigned i = 0; i < getNodes().size(); ++i)
    {
        BCNodeInfo *nodeInfo = getNodes()[i];
        if (nodeInfo->_open)
        {
            nodeInfo->_state = NST_PROGRAM_RUNNING;
            nodeInfo->_initialized = false;
            nodeInfo->_haltComplete    = false;
            _numNodesStarted   += nodeInfo->isIOnode();
        }
    }
    _isstarted = true; // boot has been started

    mutex.Unlock();

    LOG_DEBUG_MSG("Sending boot request");
    if (timer) timer->stop();

    // send Boot command to mcMonitor
    MCServerMessageSpec::BootBlockReply   mcBootIoBlockReply;
    try {
        static const boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
        _boot_block_start = boost::posix_time::microsec_clock::local_time();
        BOOST_SCOPE_EXIT( (&_boot_block_start) ) {
            _boot_block_start = boost::posix_time::ptime();
        } BOOST_SCOPE_EXIT_END;

        if(!hardWareAccessBlocked()) {
            mcBootIoBlockRequest._startTime = (_boot_block_start - epoch).total_seconds();
            _mcServer->bootBlock(mcBootIoBlockRequest, mcBootIoBlockReply);
        } else {
            std::ostringstream msg;
            msg << "SubnetMc managing hardware for " << _blockName
                << " temporarily unavailable.";
            LOG_ERROR_MSG(msg.str());
            reply << mmcs_client::FAIL << msg.str() << mmcs_client::DONE;
            return;
        }
    }
    catch (const exception &e)
    {
        mcBootIoBlockReply._rc = -1;
        mcBootIoBlockReply._rt = e.what();
    }

    if (mcBootIoBlockReply._rc) {
        reply << mmcs_client::FAIL << "bootBlock: " << mcBootIoBlockReply._rt << mmcs_client::DONE;
        return;
    }

    _block_shut_down = false;
    // Check if we are disconnecting
    if (isDisconnecting())
    {
        reply << mmcs_client::FAIL << disconnectReason() << mmcs_client::DONE;
        return;
    }
    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
IOBlockController::build_shutdown_req(
        MCServerMessageSpec::ShutdownBlockRequest& mcShutdownBlockRequest,
        MCServerMessageSpec::ShutdownBlockReply& mcShutdownBlockReply,
        mmcs_client::CommandReply& reply
        )
{
    LOG_INFO_MSG("Shutting down I/O nodes for " << _blockName);
    time(&_shutdown_sent_time);

    // initialize the Nodecard vector in the ShutdownBlockRequest
    for (vector<BCIconInfo*>::const_iterator nciter = getIcons().begin(); nciter != getIcons().end(); ++nciter)
    {
        if (typeid(**nciter) != typeid(BCNodecardInfo)) // only process node cards
            continue;

        // only process connected node cards or node cards with open processor cards
        if ((*nciter)->_open || (_isshared && (*nciter)->nodesOpen())) {
            BCNodecardInfo* ncinfo = dynamic_cast<BCNodecardInfo*>(*nciter);

            // create the ShutdownBlockRequest::IoCard object for this node card
            MCServerMessageSpec::ShutdownBlockRequest::IoCard nodeCard(ncinfo->location());

            // add ShutdownBlockRequest::Node entries for all the nodes in the node card
            for (unsigned i = 0; i < ncinfo->_nodes.size(); ++i)
            {
                BCNodeInfo* ninfo = dynamic_cast<BCNodeInfo*>(ncinfo->_nodes[i]);
                if (ninfo->_open) // only boot connected nodes
                {
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

            // Now get all of the link chips on the node card.
            for(std::vector<BCLinkchipInfo*>::const_iterator chipit = ncinfo->_linkChips.begin();
                    chipit != ncinfo->_linkChips.end();
                    ++chipit) {

                // Next, create the link object
                MCServerMessageSpec::ShutdownBlockRequest::Link link((*chipit)->location());
                // And push it in the chip vector.
                nodeCard._link_location.push_back(link);

                // Create the personality.
                MCServerMessageSpec::ShutdownBlockRequest::LinkChipPersonality pers;
                // First, add the BQLSwitches.
                for(unsigned i = 0; i < 4; ++i){
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
                for(unsigned i = 0; i < 4; ++i){
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
            // save the ShutdownBlockRequest::IoCard object in the ShutdownBlockRequest
            mcShutdownBlockRequest._ioCards.push_back(nodeCard);
        }
    }
    std::ostringstream os;
    mcShutdownBlockRequest.write(os);
    LOG_TRACE_MSG(os.str());
}

void
IOBlockController::reboot_nodes(
        BlockControllerTarget* pTarget,
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    LOG_DEBUG_MSG(__FUNCTION__);
    PthreadMutexHolder mutex;
    _rebooting = true;

    // parse arguments

    string uload;
    string ioLoad;
    bool tolerateFaults = true;

    deque<string>::const_iterator arg;

    // parse arguments
    for (arg = args.begin(); arg != args.end(); arg++)
    {
        bool valid = true; // valid argument
        vector<string> tokens;
        tokens = Tokenize(*arg, "=");           // split based on the equal sign.
        if (tokens[0].size() == 0)              // handle a null argument
            continue;
        if (tokens[0] == "uloader")
        {
            if (tokens.size() == 2)
                uload = tokens[1];
            else
                valid = false;
        }

        if (!valid)
        {
            reply << mmcs_client::FAIL << "unrecognized argument: " << *arg << mmcs_client::DONE;
            return;
        }
    }

    // Gonna do it, so make this the new wait_boot start time.
    time_t reboot_start;
    time(&reboot_start);
    getHelper()->setAllocateStartTime(reboot_start);

    // Update the list of nodes in error.
    if(BGQDB::queryError(_blockName, _error_nodes) == BGQDB::OK) {
        if(_error_nodes.size() > 0) {
            std::ostringstream msg;
            msg << "The following nodes are in error: ";
            bool first = true;
            BOOST_FOREACH(const std::string& node, _error_nodes) {
                if(first) {
                    first = false;
                } else msg << ", ";
                msg << node;
            }
            msg << ". Block will boot anyway because no_check specified.";
            LOG_INFO_MSG(msg.str());
        }
    } else {
        LOG_WARN_MSG("Could not determine nodes in error.");
    }

    // create the BootIoBlockRequest
    MCServerMessageSpec::BootBlockRequest mcBootIoBlockRequest(
            _blockName, _userName,
            tolerateFaults,
            uload,
            _bootCookie,
            _bootOptions,
            reboot_start
            );
    // initialize the Nodecard vector in the BootIoBlockRequest
    for (vector<BCIconInfo*>::const_iterator nciter = getIcons().begin(); nciter != getIcons().end(); ++nciter) {
        if (typeid(**nciter) != typeid(BCNodecardInfo)) // only process node cards
            continue;

        // only process connected node cards or node cards with open processor cards
        if ((*nciter)->_open || (_isshared && (*nciter)->nodesOpen())) {
            const BCNodecardInfo* ncinfo = dynamic_cast<BCNodecardInfo*>(*nciter);

            MCServerMessageSpec::BootBlockRequest::IoCard nodeCard(ncinfo->location());
            MCServerMessageSpec::BootBlockRequest::IoCardReboot rebootIoCard(ncinfo->location());

            // add BootIoBlockRequest::IoNode entries for all the nodes in the node card
            for (unsigned i = 0; i < ncinfo->_nodes.size(); ++i) {
                bool rebootme = false;
                BCNodeInfo* const ninfo = dynamic_cast<BCNodeInfo*>(ncinfo->_nodes[i]);
                if(ninfo->isIOnode()) {
                    // - clear "ionodeinitialized" flag for each node
                    ninfo->_initialized = false;
                    // If it is an I/O node, see if it is in our target list.
                    // If it isn't, we aren't going to reboot it.
                    for(unsigned j = 0; j < pTarget->getNodes().size(); ++j) {
                        if(ninfo->location() == pTarget->getNodes()[j]->location()) {
                            // If we're rebooting it, stick it in the list
                            rebootme = true;
                        }
                    }
                }
                if (ninfo->_open)  { // only boot connected nodes
                    const string nodeLoc = ninfo->location();
                    const string nodeCust = _machine_config_data + _nodeCustomization[nodeLoc];

                    MCServerMessageSpec::BootBlockRequest::Node node(ninfo->_iopos.jtagPort(), // jtag position of the node
                            true, // isIoNode
                            false, // do not use this node for IO Link training only
                            string((char*) &(ninfo->personality()),sizeof(ninfo->personality())), // node personality
                            nodeCust, nodeCust.length());
                    // Make sure it is in not in the list of nodes in error.
                    if(std::find(_error_nodes.begin(),
                                _error_nodes.end(),
                                ninfo->location()) ==
                            _error_nodes.end()) {
                        if(rebootme)
                            rebootIoCard._nodes.push_back(node);
                        else
                            nodeCard._nodes.push_back(node);
                    }
                }
            }

            // save the BootIoBlockRequest::IoCard object in the BootIoBlockRequest
            mcBootIoBlockRequest._ioCards.push_back(nodeCard);
            mcBootIoBlockRequest._rebootIoCards.push_back(rebootIoCard);
        }
    }

    // Add domain info to the boot request.
    if(parseDomains(mcBootIoBlockRequest, _domains) == false) {
        reply << mmcs_client::FAIL << "Bad domain syntax" << mmcs_client::DONE;
        return;
    }

    mutex.Lock(&_mutex);

    // Check if we are disconnecting
    if (isDisconnecting())
    {
        reply << mmcs_client::FAIL << disconnectReason() << mmcs_client::DONE;
        return;
    }

    //  RAS events to be handled normally
    _numNodesStarted          = 0;
    for (unsigned i = 0; i < getNodes().size(); ++i)
    {
        BCNodeInfo* const nodeInfo = getNodes()[i];
        if (nodeInfo->_open)
        {
            nodeInfo->_state = NST_PROGRAM_RUNNING;
            nodeInfo->_initialized = false;
            nodeInfo->_haltComplete    = false;
            _numNodesStarted   += nodeInfo->isIOnode();
        }
    }
    _isstarted = true; // boot has been started

    mutex.Unlock();

    // send Boot command to mcServer
    MCServerMessageSpec::BootBlockReply   mcBootIoBlockReply;
    try {
        if(!hardWareAccessBlocked())
            _mcServer->bootBlock(mcBootIoBlockRequest, mcBootIoBlockReply);
        else {
            std::ostringstream msg;
            msg << "SubnetMc managing hardware for " << _blockName
                << " temporarily unavailable.";
            LOG_ERROR_MSG(msg.str());
            reply << mmcs_client::FAIL << msg.str() << mmcs_client::DONE;
            return;
        }
    }
    catch (const exception &e)
    {
        mcBootIoBlockReply._rc = -1;
        mcBootIoBlockReply._rt = e.what();
    }

    if (mcBootIoBlockReply._rc) {
        reply << mmcs_client::FAIL << "rebootBlock: " << mcBootIoBlockReply._rt << mmcs_client::DONE;
        return;
    }

    // Check if we are disconnecting
    if (isDisconnecting())
    {
        reply << mmcs_client::FAIL << disconnectReason() << mmcs_client::DONE;
        return;
    }
    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
IOBlockController::shutdown_block(
        mmcs_client::CommandReply& reply,
        BlockControllerTarget* target
        )
{
    LOG_DEBUG_MSG(__FUNCTION__);
    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);

    if(_block_shut_down) {
        LOG_INFO_MSG("IO Block already shut down");
        reply << mmcs_client::OK << mmcs_client::DONE;
        return;
    }

    if (!isCreated())
    {
        reply << mmcs_client::FAIL << "block is not created" << mmcs_client::DONE;
        return;
    }

    bool target_created = false;
    if (!isConnected()) {
        // Must reconnect
        if(target == 0) {
            std::string targspec = "{*}";
            BlockPtr self_ptr = shared_from_this();
            target = new BlockControllerTarget(self_ptr, targspec, reply);
            target_created = true;
        }
        std::deque<string> args;
        args.push_back(Properties::getProperty(DFT_TGTSET_TYPE));
        connect(args, reply, target);
    }

    if (!isStarted())
    {
        reply << mmcs_client::FAIL << "block is not booted" << mmcs_client::DONE;
        return;
    }

    MCServerMessageSpec::HaltNodesRequest mcHaltNodesRequest;
    MCServerMessageSpec::HaltNodesReply mcHaltNodesReply;
    mcHaltNodesReply._rc = 0;

    for (vector<BCIconInfo*>::const_iterator nciter = getIcons().begin(); nciter != getIcons().end(); ++nciter)
    {
        if (typeid(**nciter) != typeid(BCNodecardInfo)) // only process node cards
            continue;

        // only process connected node cards or node cards with open processor cards
        if ((*nciter)->_open || (_isshared && (*nciter)->nodesOpen()))
        {
            BCNodecardInfo* ncinfo = dynamic_cast<BCNodecardInfo*>(*nciter);
            // add entries for all the nodes in the node card
            for (unsigned i = 0; i < ncinfo->_nodes.size(); ++i)
            {
                BCNodeInfo* ninfo = dynamic_cast<BCNodeInfo*>(ncinfo->_nodes[i]);
                if (ninfo->_open) // only process connected nodes
                {
                    if(ninfo->isIOnode() == true) // Only add it to sysrq if it's a compute
                        mcHaltNodesRequest._location.push_back(ninfo->location());
                    LOG_TRACE_MSG( "added node " << ninfo->location() << " to halt request." );
                }
            }
        }
    }

    // Need to build shutdown block request
    //
    // create the ShutdownBlockRequest
    //
    MCServerMessageSpec::ShutdownBlockRequest mcShutdownBlockRequest(_blockName, _bootCookie, _block->blockId(), _diags, "");
    MCServerMessageSpec::ShutdownBlockReply mcShutdownBlockReply;
    build_shutdown_req(mcShutdownBlockRequest, mcShutdownBlockReply, reply);

    time(&_shutdown_sent_time);
    reply << mmcs_client::OK;

    // Send the ShutdownBlockRequest
    bool sent = false;
    try {
        if(!hardWareAccessBlocked()) {
            if(_redirectSock != 0) {
                // If we're redirecting, we've got to free the tset and reopen it WUAR.
                // First free the tset.  "no_shutdown" to avoid eating our tail.
                std::deque<std::string> args;
                args.push_back("no_shutdown");
                LOG_INFO_MSG("Freeing IO targets.");
                disconnect(args, reply);
                std::deque<std::string> a;
                a.push_back("mode=control");
                std::string targspec = "{*}";
                BlockPtr self_ptr = shared_from_this();
                BlockControllerTarget target(self_ptr, targspec, reply);
                connect(a, reply, &target);
            }
            if(_mcServer) {
                _mcServer->shutdownBlock(mcShutdownBlockRequest, mcShutdownBlockReply);
                sent = true;
            } else {
                mcShutdownBlockReply._rc = -1;
                mcShutdownBlockReply._rt = "mc_server not connected";
            }
        } else {
            std::ostringstream msg;
            msg << "SubnetMc managing hardware for " << _blockName
                << " temporarily unavailable.";
            LOG_ERROR_MSG(msg.str());
            reply << mmcs_client::FAIL << msg.str() << mmcs_client::DONE;
            return;
        }
    } catch (const exception &e) {
        mcShutdownBlockReply._rc = -1;
        mcShutdownBlockReply._rt = e.what();
    }
    if (mcShutdownBlockReply._rc != 0)
    {
        reply << mmcs_client::ABORT << "shutdown_block: " << mcShutdownBlockReply._rt;
        LOG_ERROR_MSG(mcShutdownBlockReply._rt);
        // Don't return here.  Let it go to FREE and rely on LLCS to set
        // hardware in error.
        // return;
    }

    if(sent) {
        _block_shut_down = true;
        LOG_INFO_MSG("Shutdown sent");
    }
    if(!_diags && _mcServer)
        quiesceMailbox(target);

    _isstarted = false;
    if(target && target_created) {
        _residual_target = target;
    }
    reply << mmcs_client::DONE;
}

void
IOBlockController::show_barrier(
        mmcs_client::CommandReply& reply
        )
{
    LOG_DEBUG_MSG(__FUNCTION__);
    reply << mmcs_client::OK;

    // Spin through all of our nodes.  For each one, find its
    // associated rack and see if its location is set.
    for(std::vector<BCNodeInfo*>::const_iterator it = _nodes.begin();
        it != _nodes.end(); ++it) {
        std::string loc = (*it)->location();
        std::string rack = loc.substr(0,3);
        if(_rackbits.find(rack) != _rackbits.end() &&
           _rackbits[rack].Flagged(loc) == false)
            reply << loc << std::endl;
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
    LOG_DEBUG_MSG("Process RAS message " << rasEvent.getDetails()[RasEvent::MSG_ID]);

    // Barrier RAS message
    if (rasEvent.msgId() == 0x00040096) {
        std::string locstr = rasEvent.getDetails()[RasEvent::LOCATION];
        LOG_TRACE_MSG("Barrier RAS received for " << locstr);
        std::string rack = locstr.substr(0,3);
        // Set the bit in the rack that satisfies this
        // particular location
        _rackbits[rack].Set(locstr);

        unsigned satisfied_nodes = 0;
        // Loop through the IO racks.  Count each rack that is
        // completely satisfied.
        for(std::map<std::string, IOBoardBitset>::iterator it = _rackbits.begin();
            it != _rackbits.end(); ++it) {
            satisfied_nodes += it->second.BitsSet();
        }

        if(satisfied_nodes == _nodes.size()) {
            LOG_INFO_MSG("IO block barrier satisfied");
            // Clear all bits so we can handle another.
            for(std::map<std::string, IOBoardBitset>::iterator it = _rackbits.begin();
                it != _rackbits.end(); ++it) {
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

    if (_diags && !_insertDiagsRAS) // filter RAS that occurs from diags unless they request it
        filtered = true;

    return filtered; // don't filter
}

void
IOBlockController::calculateRatios()
{
    const BGQMachineXML* machine = this->getMachineXML();
    BOOST_ASSERT( machine );

    // midplane location is key, I/O node location is value
    typedef std::multimap<std::string,std::string> Midplanes;
    Midplanes linkedMidplanes;

    // iterate through all I/O nodes in the machine
    BOOST_FOREACH( const BGQMachineIOBoard* board, machine->_ioBoards ) {
        BOOST_FOREACH( const BGQMachineNode* i, board->_nodes ) {
            const std::string location( board->_board + "-" + i->_location );
            // ensure this I/O node has an I/O link
            const std::vector<BGQMachineIOLink*>::const_iterator link(
                    std::find_if(
                        machine->_ioLinks.begin(),
                        machine->_ioLinks.end(),
                        boost::bind(
                            &BGQMachineIOLink::_ioNode,
                            _1
                            ) == location
                        )
                    );
            if ( link == machine->_ioLinks.end() ) continue;

            // remember the midplane location attached to this I/O node
            const std::string midplane( (*link)->_computeNode.substr(0, 6) );
            LOG_TRACE_MSG( midplane << " <--> " << (*link)->_ioNode );
            linkedMidplanes.insert(
                    Midplanes::value_type( 
                        midplane,
                        (*link)->_ioNode
                        )
                    );
        }
    }

    // I/O node location is key, value is number of attached compute nodes
    typedef std::map<std::string,unsigned> IoRatio;
    IoRatio nodes;

    // for every I/O link in the machine, find the connected midplane
    // from I/O nodes in this block
    BOOST_FOREACH( const BGQMachineIOLink* i, machine->_ioLinks ) {
        if ( _targetLocationMap.find(i->_ioNode) == _targetLocationMap.end() ) continue;
        const std::string midplane( i->_computeNode.substr(0,6) );
        const unsigned ioNodes = linkedMidplanes.count( midplane );
        BOOST_ASSERT( ioNodes );
        const unsigned ratio = 512 / ioNodes;

        nodes.insert(
                IoRatio::value_type( i->_ioNode, ratio )
                );
    }

    // find the configured large region size
    unsigned largeRegionSize = 0x100000;
    const std::string key( "large_region_size" );
    try {
        const int value(
                boost::lexical_cast<int>(
                    Properties::getProperties()->getValue( "cios", key )
                    )
                );
        if ( value <= 0 ) {
            LOG_WARN_MSG( "invalid " << key << " in [cios] section, must be positive" );
            LOG_WARN_MSG( "using default value of " << largeRegionSize );
        } else {
            largeRegionSize = value;
        }
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( "garbage " << key << " in [cios] section, must be a number" );
        LOG_WARN_MSG( "using default value of " << largeRegionSize );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( "could not find key " << key << " in [cios] section" );
        LOG_WARN_MSG( "using default value of " << largeRegionSize );
    }
    LOG_TRACE_MSG( "large region size: " << largeRegionSize );

    // add node customization for every node in our block
    BOOST_FOREACH( const BCNodeInfo* i, _nodes ) {
        const std::string location( i->_location );
        const IoRatio::const_iterator node = nodes.find( location );
        unsigned ratio = 0;
        if ( node == nodes.end() ) {
            // no ratio means this node has no I/O link
        } else {
            ratio = node->second;
        }

        LOG_DEBUG_MSG( location << " has " << ratio << " attached compute nodes" );
        const NodeCustomization::iterator customization = _nodeCustomization.find( location );

        // note that the bell character is the delimiter
        const std::string attachCount(
                "ATTACHED_COMPUTE_NODES=" +
                boost::lexical_cast<std::string>(ratio) +
                "\a" + 
                "LARGE_REGION_SIZE=" +
                boost::lexical_cast<std::string>(largeRegionSize) +
                "\a"
                );

        // prepend customization if found, otherwise insert it
        if ( customization == _nodeCustomization.end() ) {
            _nodeCustomization.insert( 
                    NodeCustomization::value_type( location, attachCount )
                    );
        } else {
            customization->second.insert(0, attachCount);
        }
    }
}

} } // namespace mmcs::server
