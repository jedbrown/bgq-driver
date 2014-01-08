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

#include <algorithm>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <list>
#include <map>
#include <netinet/in.h>
#include <pwd.h>
#include <set>
#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/scope_exit.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <control/include/mcServer/MCServerRef.h>
#include <control/include/mcServer/MCServer_errno.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>

#ifdef WITH_DB
#include <db/include/api/BGQDBlib.h>
#else
#include "fakeDb.h"
#endif

#include "HardwareBlockList.h"
#include "MMCSCommandReply.h"
#include "MMCSProperties.h"
#include "ras.h"

LOG_DECLARE_FILE( "mmcs" );


using namespace std;
using namespace MCServerMessageSpec;
CNBlockController::CNBlockController(
        BGQMachineXML* machine,
        const std::string& userName,
        const std::string& blockName,
        bool delete_machine
        ) :
    BlockControllerBase(machine, userName, blockName, delete_machine)
{
    _io_notified = false;
    // nothing to do
}

bool
CNBlockController::process_midplane(BGQMidplaneNodeConfig& midplane, MMCSCommandReply& reply, bool passthru) {

    BCNodeInfo 	*nodeInfo;
    BCNodecardInfo 	*nodecardInfo;
    map<string, BCNodeInfo*> nodesByLocation; // for ordering pset nodes by location within pset
    
    std::string rack = midplane.posInMachine().substr(0,3);
    RackBitset bs(rack);
    _rackbits[rack] = bs;
    
    if(HardwareBlockList::find_in_list(rack) == true) {
        std::ostringstream msg;
        msg << "Rack " << rack << "'s controlling subnet is temporarily unavailable.";
        reply << FAIL << msg.str() << DONE;
        LOG_INFO_MSG(msg.str());
        return false;
    }

    // Note it if we have fewer than one IO link
    if(midplane.ioLinks() < 1 && !passthru) ++_io_linkless_midplanes;
    
    nodesByLocation.clear(); // clear map to order nodes by jtag id within pset
    
    // Iterate over nodeboards in midplane
    for (BGQMidplaneNodeConfig::nodeboard_iterator nodeit = midplane.nodeBoardBegin();
         nodeit != midplane.nodeBoardEnd(); ++nodeit)
        {
            nodecardInfo = new BCNodecardInfo();
            
            // Add all the link chips
            for (
                 BGQNodeBoard::linkchip_iterator chipit = (*nodeit)->linkChipBegin();
                 chipit != (*nodeit)->linkChipEnd();
                 ++chipit
                 )
                {
                    BCLinkchipInfo* chipinfo = new BCLinkchipInfo();
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
         vector<BGQNodePos>::iterator citer = midplane.computes()->begin();
         citer != midplane.computes()->end();
         ++citer
         )
        {
            BGQNodePos& node = *citer;
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
            nodecardInfo = findNodecardInfo(nodeInfo->nodeCardLocation());
            nodecardInfo->_nodes.push_back(nodeInfo);  // keep track of the compute nodes in each node card
        }

    // order nodes by jtag id within pset
    for (
         map<string, BCNodeInfo*>::iterator it = nodesByLocation.begin();
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
CNBlockController::create_block(deque<string> args, MMCSCommandReply& reply, std::istream* blockStreamXML)
{
    LOGGING_DECLARE_BLOCK_ID_MDC; LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    BGQDB::DIAGS_MODE diags = BGQDB::NO_DIAGS; 	// 'diags' parameter specified
    FILE *svchost_config = NULL; // file for svchost configuration file
    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);	// serialize this code

    BOOST_SCOPE_EXIT( (&svchost_config) ) {
        if(svchost_config != NULL)
            fclose(svchost_config);
    } BOOST_SCOPE_EXIT_END;

    if (isCreated())
    {
        reply << FAIL << "block is already created" << DONE;
        return;
    }

    _isblock	   = true;				// initialized by create_block
    _targetsetMode = WUAR; 	// default target set open mode

    // parse options
    //
    if ( args.empty() && !blockStreamXML )
    {
        reply << FAIL << "args?" << DONE;
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
                    reply << FAIL << "can't open svchost_options file \"" << tokens[1] << "\" : " << strerror(errno) << DONE;
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
    catch (XMLException& e)
    {
        reply << FAIL << "create_block: " << e.what() << DONE;
        return;
    }
    catch (BGQNodeConfigException& e)
    {
        reply << FAIL << "create_block: " << e.what() << DONE;
        return;
    }
    if (_blockXML == NULL)
    {
        reply << FAIL << "create_block: can't load block XML, hardware resources may be marked Missing or Error " << DONE;
        return;
    }

    // save the blockname for use as target set name
    if (_blockName.empty())
        _blockName = _blockXML->_name;


    // remove the switches if no_torus is specified
    if (no_torus &&  _blockXML->_midplanes.size() == 1 ) {
        _blockXML->_midplanes[0]->_switches.clear();
    }

    // Check that the required block resources exist in the database
    //
    vector<string> missing;
    if (no_check == false) { // we're not no_check
        if (BGQDB::queryMissing(_blockName, missing, diags) == 0)
            {
                if (missing.size() > 0) {
                    // build an error message identifying the missing resources
                    reply << FAIL << "create_block: resources are unavailable - ";
                    for (unsigned i = 0; i < missing.size(); ++i)
                        if (i == 0)
                            reply << missing[i];
                        else
                            reply << ", " << missing[i];
                    reply << DONE;
                    return;
                }
            }
        else
            {
                reply << FAIL << "create_block: error from BGQDB::queryMissing, can't determine status of hardware resources " << DONE;
                return;
            }
    }

    LOG_DEBUG_MSG("Create BGQBlockNodeConfig");
    // create a new BGQBlockNodeConfig object
    try
    {
        _block = new BGQBlockNodeConfig(_machineXML, _blockXML, svchost_config);
    }
    catch (BGQNodeConfigException& e)
    {
        reply << FAIL << "create_block: " << e.what() << DONE;
        return;
    }

    _machine_config_data = _block->getConfigData();

    LOG_DEBUG_MSG("populate data");
    // populate the BCNodeInfo and BCNodecardInfo lists
    // with information from the compute nodes and I/O nodes
    // Iterate over populated midplanes
    { 
        BCNodeInfo 	*nodeInfo;
        BCNodecardInfo 	*nodecardInfo;
        // iterate over midplanes
        for (
             BGQBlockNodeConfig::midplane_iterator mpiter = _block->midplaneBegin();
             mpiter != _block->midplaneEnd();
             ++mpiter
             )
            {

                if(process_midplane(*mpiter, reply, false) == false) return;
            }

        // Now do passthru
        BOOST_FOREACH(BGQMidplaneNodeConfig* midplane, _block->passThruMidplanes()) {
            if(process_midplane(*midplane, reply, true) == false) return;
        }

        // Now get the link training IO nodes.  These will NOT be all of the IO nodes,
        // just the ones we need for link training.
        vector<BGQIOBoardNodeConfig *> ioBoards = _block->linkTrainingIOBoards();
        LOG_TRACE_MSG( ioBoards.size() << " I/O boards used for link training" );
        for (unsigned i = 0; i < ioBoards.size(); ++i) {
            BGQIOBoardNodeConfig* ioboard = ioBoards[i];
            nodecardInfo = new BCNodecardInfo();
            for ( BGQNodeBoard::linkchip_iterator chipit = ioboard->linkChipBegin();
                  chipit != ioboard->linkChipEnd(); ++chipit) {
                BCLinkchipInfo* chipinfo = new BCLinkchipInfo();
                chipinfo->_jtag = chipit->jtag();
                chipinfo->_personality = chipit->personality();
                chipinfo->_block = _block;
                chipinfo->_ioboard = true;
                // Can't use init_location() for io board link chip in compute block
                ostringstream ostr;
                ostr << ioboard->posInMachine() << "-" << IO_CARD_JTAGPORT_TO_LCTN[chipit->jtag()];
                chipinfo->_location = ostr.str();
                nodecardInfo->_linkChips.push_back(chipinfo);
                LOG_TRACE_MSG( "added link chip " << chipinfo->location() );
            }

            // For each IO board/card whatever, build it and stick it in the icon list.
            // NOTE: WE NEED TO ADD EACH IOBOARD TO THE BOARD BITSET
            nodecardInfo->_block = _block;
            nodecardInfo->_ioboard = true;
            nodecardInfo->_boardcoordA = ioboard->allIOBoardA();
            nodecardInfo->_boardcoordB = ioboard->allIOBoardB();
            nodecardInfo->_boardcoordC = ioboard->allIOBoardC();
            nodecardInfo->_boardcoordD = ioboard->allIOBoardD();
            nodecardInfo->_linkio = true;
            nodecardInfo->init_location(ioboard);
            std::string board = nodecardInfo->location().substr(0,3);

#if 0       // NOTE: Never mind.  JimVo says we don't need it.
            IOBoardBitset bs(board);
            _ioboardbits[board] = bs;
#endif
            getIcons().push_back(nodecardInfo);
            LOG_TRACE_MSG(
                          "adding I/O board " <<
                          nodecardInfo->location()
                          );

            vector<BGQIONodePos>* ioNodes = ioBoards[i]->computes();
            for (unsigned j = 0; j < ioNodes->size(); ++j) {
                // ALL of the IO nodes that are in this list are connected IO.
                // Non-connected IO nodes will NOT be returned by bgqconfig.
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

                // Can't use init_location() for IO in a compute block.
                ostringstream ostr;
                ostr << ioboard->posInMachine() << "-" << nodeInfo->_iopos;
                nodeInfo->_location = ostr.str();

                ++_numNodesTotal; // computes total number of nodes in the block
                getNodes().push_back(nodeInfo);
                nodecardInfo->_nodes.push_back(nodeInfo);
                LOG_TRACE_MSG( "adding I/O node " << nodeInfo->location() );
            }
        }
    }

    // add all BCNodeInfo, BCNodecardInfo, BCLinkcardInfo
    // BCLinkchipInfo objects to CNBlockController::_targets
    for (unsigned i = 0; i < getNodes().size(); ++i)
        _targets.push_back(getNodes()[i]);

    // if not sharing the node board, then include it in the target set, so it will be locked
    if (_isshared == false) {
        for (unsigned i = 0; i < getIcons().size(); ++i) {
            _targets.push_back(getIcons()[i]);
        }
    }

    for (unsigned i = 0; i < getLinkchips().size(); ++i)
        _targets.push_back(getLinkchips()[i]);

    for (unsigned i = 0; i < _targets.size(); ++i)
        _targets[i]->_locateId = i; // Give each device under our control a unique target specifier

    reply << OK << DONE;
}

void
CNBlockController::connect(deque<string> args, MMCSCommandReply& reply, BlockControllerTarget* pTarget, bool add_targets)
{
    LOGGING_DECLARE_BLOCK_ID_MDC; LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    BlockControllerBase::connect(args, reply, pTarget, add_targets);
    if(reply.getStatus() != MMCSCommandReply::STATUS_OK)
        return;  // connecting to mcserver failed

    bool   usePgood	= false;        // should a pgood be sent before the BootBlockRequest?

    for (unsigned i = 0; i < args.size(); ++i)     {
        if (args[i] == "pgood")
            usePgood = true;
    }

    // send a pgood reset request to the processor nodes in the target set
    if (usePgood)
    {
	pgood_reset(reply, pTarget);
	if (reply.getStatus() < 0)
	{
	    LOG_DEBUG_MSG("pgood_reset: " << reply.str());
            disconnect(args);
            return;
	}
    }

    // start the mailbox monitor
    startMailbox(reply);
    if (reply.getStatus() != 0)
    {
	LOG_DEBUG_MSG("startMailbox: " << reply.str());
	disconnect(args);
	return;
    }

    reply << OK << DONE;
}

void
CNBlockController::disconnect(deque<string>& args)
{
    LOGGING_DECLARE_BLOCK_ID_MDC; LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    BlockControllerBase::disconnect(args);
    // Clear the barrier bitsets
    for(std::map<std::string, RackBitset>::iterator it = _rackbits.begin();
        it != _rackbits.end(); ++it) {
        it->second.Reset();
    }
    for(std::map<std::string, IOBoardBitset>::iterator it = _ioboardbits.begin();
        it != _ioboardbits.end(); ++it) {
        it->second.Reset();
    }
}

void
CNBlockController::boot_block(deque<string> args, MMCSCommandReply& reply, const NodeCustomization& ncust, PerformanceCounters::Timer::Ptr timer )
{
    LOGGING_DECLARE_BLOCK_ID_MDC; LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    PthreadMutexHolder mutex;
    FILE* svchost_config = NULL; // file for svchost configuration file

    BOOST_SCOPE_EXIT( (&svchost_config) ) {
        if(svchost_config != NULL)
            fclose(svchost_config);
    } BOOST_SCOPE_EXIT_END;

    if (!isCreated())
    {
        reply << FAIL << "block is not created" << DONE;
	return;
    }
    if (!isConnected())
    {
        reply << FAIL << "block is not connected" << DONE;
	return;
    }
    if (!isBlock())
    {
	reply << FAIL << "target sets are not bootable" << DONE;
	return;
    }

    // parse arguments

    string uload;				// microloader
    string domains = "";                              // domains
    string bootOptions;				// name=value boot options
    string steps = "";
    bool tolerateFaults = false;   // LLCS should NOT attempt to continue booting after detecting a fault on a compute block

    deque<string>::iterator arg;

    // log boot_block arguments

    ostringstream oss;

    oss << "CNBlockController::boot_block ";
    for (arg = args.begin(); arg != args.end(); arg++)
	oss << *arg << " ";
    LOG_INFO_MSG(oss.str());

    // parse arguments
    int i = 0;
    bool bootsteps = false;
    for (arg = args.begin(); arg != args.end(); arg++)
    {
        bool valid = true;	// valid argument
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
                    reply << FAIL << "can't open svchost_options file \"" << tokens[1] << "\" : " << strerror(errno) << DONE;
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
            reply << FAIL << "unrecognized argument: " << *arg << DONE;
            return;
        }
        ++i;
    }

    if (isStarted() && !bootsteps)
    {
        reply << FAIL << "block is already booted" << DONE;
        return;
    }

    // This may be mmcs_lite, in which the default images are passed via properties
    if (uload.empty())
        uload.append(MMCSProperties::getProperty("uloader"));

    if (domains.empty()) {
        domains = MMCSProperties::getProperty("cn_domains");
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
        catch (BGQNodeConfigException& e)
        {
            reply << FAIL << "boot_block: " << e.what() << DONE;
            return;
        }
    }

    // Save the node customization in case we need it on reboot_nodes
    _nodeCustomization = ncust;

    //
    // create the BootBlockRequest
    //
    MCServerMessageSpec::BootBlockRequest mcBootBlockRequest(_blockName, _userName,
            tolerateFaults,
            uload, _bootCookie, bootOptions, 0 /* boot start time */);
    //
    // initialize the Nodecard vector in the BootBlockRequest
    //
    for (vector<BCIconInfo*>::iterator nciter = getIcons().begin(); nciter != getIcons().end(); ++nciter)
    {
        if (typeid(**nciter) != typeid(BCNodecardInfo))	// only process node cards
            continue;

        // only process connected node cards or node cards with open processor cards
        if ((*nciter)->_open || (_isshared && (*nciter)->nodesOpen()))
        {
            BCNodecardInfo* ncinfo = dynamic_cast<BCNodecardInfo*>(*nciter);

            // create the BootBlockRequest::NodeCard object for this node card
            MCServerMessageSpec::BootBlockRequest::NodeCard nodeCard(ncinfo->location());
            // It might be an IO node card.  We send a trainonly message for connected IO
            MCServerMessageSpec::BootBlockRequest::IoCard ioCard(ncinfo->location());

            // add BootBlockRequest::Node entries for all the nodes in the node card
            for (unsigned i = 0; i < ncinfo->_nodes.size(); ++i)
            {
                BCNodeInfo* ninfo = dynamic_cast<BCNodeInfo*>(ncinfo->_nodes[i]);
                if (ninfo->_open) // only boot connected nodes
                {
                    bool trainOnly = false; // do all boot steps, not just I/O link training
                    int jport = 0;
                    // Use the _linkio flag instead of isIONode within a compute block
                    if(ninfo->_linkio) { // IO nodes are always trainOnly in a compute block.
                        if(ninfo->_iopos.trainOnly()) trainOnly = true;
                        jport = ninfo->_iopos.jtagPort();
                    } else {
                        jport = ninfo->_pos.jtagPort();
                    }

                    string nodeLoc = ninfo->location();
                    string nodeCust = _machine_config_data + _nodeCustomization[nodeLoc];
                    Personality_t pers = ninfo->personality();
                    if(svchost_config && _block) {
                        pers = BGQBlockNodeConfig::updatePersonality(ninfo->personality(), svc_options);
                    }
                    MCServerMessageSpec::BootBlockRequest::Node node(jport, // jtag position of the node
                                                                     ninfo->_linkio,		// is this an I/O node?
                                                                     trainOnly,
                                                                     string((char*) &(pers), sizeof(pers)),	// node personality
                                                                     nodeCust, nodeCust.length());
                    if(ncinfo->_ioboard)
                        ioCard._nodes.push_back(node);
                    else
                        nodeCard._nodes.push_back(node);
                }
            }

            // Now get all of the link chips on the node card.
            for(
                    std::vector<BCLinkchipInfo*>::iterator chipit = ncinfo->_linkChips.begin();
                    chipit != ncinfo->_linkChips.end();
                    ++chipit
                    )
            {
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
                if(ncinfo->_ioboard) {
                    ioCard._linkChips.push_back(chip);
                } else
                    nodeCard._linkChips.push_back(chip);
            }

            // save the BootBlockRequest::NodeCard object in the BootBlockRequest
            if(ncinfo->_ioboard && !_diags)
                mcBootBlockRequest._ioCards.push_back(ioCard);
            else if(ncinfo->_ioboard == false)
                mcBootBlockRequest._nodeCards.push_back(nodeCard);
        }
    }

    if(parseDomains(mcBootBlockRequest, domains) == false) {
        reply << FAIL << "Bad domain syntax" << DONE;
        return;
    }

    if(!parseSteps(reply, mcBootBlockRequest, steps))
        return; // Bad step, our reply is already set

    //
    // beginning of critical section
    //
    mutex.Lock(&_mutex);


    // Check if we are disconnecting
    if (isDisconnecting())
    {
        reply << FAIL << disconnectReason() << DONE;
        return;
    }

    // mark node targets as started
    for (unsigned i = 0; i < getNodes().size(); ++i)
    {
        BCNodeInfo *nodeInfo = getNodes()[i];
        if (nodeInfo->_open)
        {
            nodeInfo->_state	       = NST_PROGRAM_RUNNING;
            nodeInfo->_initialized = false;
            nodeInfo->_haltComplete    = false;
            _numNodesStarted   += !(nodeInfo->isIOnode());
            _numNodesActive++;
        }
    }
    _isstarted			   = true; // boot has been started
    mutex.Unlock();
    //
    // end of critical section
    //

    LOG_DEBUG_MSG("Sending boot request");
    timer->stop();

    // send Boot command to mcMonitor
    MCServerMessageSpec::BootBlockReply   mcBootBlockReply;
    try {
        time(&_boot_block_start);
        BOOST_SCOPE_EXIT( (&_boot_block_start) ) {
            _boot_block_start = 0;
        } BOOST_SCOPE_EXIT_END;

        if(!hardWareAccessBlocked()) {
            mcBootBlockRequest._startTime = _boot_block_start;
            _mcServer->bootBlock(mcBootBlockRequest, mcBootBlockReply);
        } else {
            std::ostringstream msg;
            msg << "SubnetMc managing hardware for " << _blockName 
                << " temporarily unavailable.";
            LOG_ERROR_MSG(msg.str());
            reply << FAIL << msg.str() << DONE;
            return;
        }
    }
    catch (exception &e)
    {
        mcBootBlockReply._rc = -1;
        mcBootBlockReply._rt = e.what();
    }

    if (mcBootBlockReply._rc)
        {
            std::ostringstream msg;
            msg <<  "bootBlock: " << mcBootBlockReply._rt;
            LOG_ERROR_MSG(msg.str());
            reply << FAIL << msg.str() << DONE;
            return;
        }
    LOG_INFO_MSG("Received boot reply");
    _block_shut_down = false;
    // Check if we are disconnecting
    if (isDisconnecting())
    {
        reply << FAIL << disconnectReason() << DONE;
        return;
    }
    reply << OK << DONE;
}

void CNBlockController::build_shutdown_req(MCServerMessageSpec::ShutdownBlockRequest& mcShutdownBlockRequest,
                                           MCServerMessageSpec::ShutdownBlockReply& mcShutdownBlockReply,
                                           MMCSCommandReply& reply) {
    LOG_DEBUG_MSG(__FUNCTION__);
    time(&_shutdown_sent_time);

    //
    // initialize the Nodecard vector in the ShutdownBlockRequest
    //
    for (vector<BCIconInfo*>::iterator nciter = getIcons().begin(); nciter != getIcons().end(); ++nciter)
    {
        if (typeid(**nciter) != typeid(BCNodecardInfo))	// only process node cards
            continue;

        // only process connected node cards or node cards with open processor cards
        if ((*nciter)->_open || (_isshared && (*nciter)->nodesOpen()))
        {
            BCNodecardInfo* ncinfo = dynamic_cast<BCNodecardInfo*>(*nciter);

            // create the ShutdownBlockRequest::NodeCard object for this node card
            MCServerMessageSpec::ShutdownBlockRequest::NodeCard nodeCard(ncinfo->location());
            // It might be an IO node card.  We send a trainonly message for connected IO
            MCServerMessageSpec::ShutdownBlockRequest::IoCard ioCard(ncinfo->location());

            // add ShutdownBlockRequest::Node entries for all the nodes in the node card
            for (unsigned i = 0; i < ncinfo->_nodes.size(); ++i)
            {
                BCNodeInfo* ninfo = dynamic_cast<BCNodeInfo*>(ncinfo->_nodes[i]);
                if (ninfo->_open) // only boot connected nodes
                {
                    bool trainOnly = false; // do all boot steps, not just I/O link training
                    int jport = 0;
                    // Use the _linkio flag instead of isIONode within a compute block
                    if(ninfo->_linkio) { // IO nodes are always trainOnly in a compute block.
                        if(ninfo->_iopos.trainOnly()) trainOnly = true;
                        jport = ninfo->_iopos.jtagPort();
                    } else {
                        jport = ninfo->_pos.jtagPort();
                    }

                    string nodeLoc = ninfo->location();
                    string nodeCust = _machine_config_data + _nodeCustomization[nodeLoc];
                    MCServerMessageSpec::ShutdownBlockRequest::Node node(jport, // jtag position of the node
                                                                         ninfo->_linkio,		// is this an I/O node?
                                                                         trainOnly,
                                                                         string((char*) &(ninfo->personality()), sizeof(ninfo->personality())),	// node personality
                                                                         nodeCust, nodeCust.length(),
                                                                         nodeLoc);


                    if(ncinfo->_ioboard)
                        ioCard._node_location.push_back(node);
                    else
                        nodeCard._node_location.push_back(node);
                }
            }

            // Now get all of the link chips on the node card.
            for(
                    std::vector<BCLinkchipInfo*>::iterator chipit = ncinfo->_linkChips.begin();
                    chipit != ncinfo->_linkChips.end();
                    ++chipit
                    )
            {
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
                MCServerMessageSpec::ShutdownBlockRequest::Link link((*chipit)->location());
                // Next, create the chip object.
                MCServerMessageSpec::ShutdownBlockRequest::LinkChip chip((*chipit)->_jtag, pers);
                // And push it in the chip vector.
                if(ncinfo->_ioboard) {
                    ioCard._linkChips.push_back(chip);
                    ioCard._link_location.push_back(link);
                } else {
                    nodeCard._linkChips.push_back(chip);
                    nodeCard._link_location.push_back(link);
                }
            }

            // save the ShutdownBlockRequest::NodeCard object in the ShutdownBlockRequest
            if(ncinfo->_ioboard)
                mcShutdownBlockRequest._ioCards.push_back(ioCard);
            else
                mcShutdownBlockRequest._nodeCards.push_back(nodeCard);
        }
    }
    std::ostringstream os;
    mcShutdownBlockRequest.write(os);
    LOG_TRACE_MSG(os.str());
}

void
CNBlockController::reboot_nodes(BlockControllerTarget* pTarget, deque<string> args, MMCSCommandReply& reply) {
    LOGGING_DECLARE_BLOCK_ID_MDC; LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);;
    reply << FAIL << "reboot_nodes not supported for compute blocks." << DONE;
    return;
}

bool CNBlockController::notify_io(MCServerMessageSpec::NotifyIoRequest& mcNotifyIoRequest,
                                  MCServerMessageSpec::NotifyIoReply& mcNotifyIoReply,
                                  MMCSCommandReply& reply, std::vector<BCNodeInfo*>& targetargs) {
    // Create a block controller target set for the IO.
    BlockPtr io_target(new BlockControllerBase(_machineXML, _userName, "temp_io_target", true));
    io_target->create_nodeset(targetargs, reply);
    BlockControllerTarget bctarget(io_target, "{i}", reply);
    std::deque<std::string> cargs;
    // Connect to the IO target.
    io_target->connect(cargs, reply, &bctarget, true);

    // When we leave scope, we disconnect.
    BOOST_SCOPE_EXIT( ( &io_target ) ) {
        std::deque<std::string> dargs;
        dargs.push_back("no_sysrq");
        io_target->disconnect(dargs);
    } BOOST_SCOPE_EXIT_END;

    if(io_target->getMCServer() == 0) {
        // We probably had a conflicting target set so we can't notify the IO nodes.
        if(reply.getStatus() == 0) {
            reply << FAIL << "Unable to open target set in mc_server.  Block will remain 'ALLOCATED'." << DONE;
        } else {
            std::ostringstream newreply;
            newreply << "Unable to open IO target in mc_server.  mc_server returned \""
                     << reply.str() << "\".  Block will remain 'ALLOCATED'."
                     << " Make sure there are no users redirecting IO on the block and try again.";
            reply.reset();
            reply << FAIL << newreply.str() << DONE;
        }
        return false;
    }

    // Send the message.
    try {
        io_target->getMCServer()->notify_io(mcNotifyIoRequest, mcNotifyIoReply);
    } catch (exception& e) {
        mcNotifyIoReply._rc = -1;
        mcNotifyIoReply._rt = e.what();
    } catch (XML::Exception &e) {
        ostringstream buf; buf << e;
        mcNotifyIoReply._rc = -1;
        mcNotifyIoReply._rt = buf.str();
    }

    if (mcNotifyIoReply._rc) {
        std::ostringstream errmsg;
        errmsg << "Notify IO response: " << mcNotifyIoReply._rt
               << " Notify IO error: " << mcNotifyIoReply._rc;
        LOG_WARN_MSG(errmsg.str());
        // Failure to notify IO is not a block free failure.
        _io_notified = false;
        return true;
    } else
        LOG_INFO_MSG("IO nodes notified.");
    _io_notified = true;
    return true;
}

void
CNBlockController::shutdown_block(MMCSCommandReply& reply, BlockControllerTarget* target, bool no_sysrq)
{
    LOGGING_DECLARE_BLOCK_ID_MDC; LOGGING_DECLARE_BLOCK_USER_MDC;
    PthreadMutexHolder mutex;
    LOG_INFO_MSG(__FUNCTION__);
    mutex.Lock(&_mutex);	// serialize this code

    if(_block_shut_down) {
        LOG_DEBUG_MSG("CN Block already shut down");
        reply << OK << DONE;
        return;
    }

    if (!isCreated())
    {
        reply << FAIL << "block is not created" << DONE;
	return;
    }
    if (!isConnected())
    {
        reply << FAIL << "block is not connected" << DONE;
	return;
    }
    if (!isStarted())
    {
	reply << FAIL << "block is not booted" << DONE;
	return;
    }

    //
    // create the ShutdownBlockRequest
    //
    MCServerMessageSpec::ShutdownBlockRequest mcShutdownBlockRequest(_blockName, _bootCookie, _createId, _diags, "");
    MCServerMessageSpec::ShutdownBlockReply mcShutdownBlockReply;
    build_shutdown_req(mcShutdownBlockRequest, mcShutdownBlockReply, reply);

    MCServerMessageSpec::HaltNodesRequest mcHaltNodesRequest;
    MCServerMessageSpec::HaltNodesReply mcHaltNodesReply;
    MCServerMessageSpec::NotifyIoRequest mcNotifyIoRequest;
    MCServerMessageSpec::NotifyIoReply mcNotifyIoReply;
    mcNotifyIoReply._rc = 0;

    mcNotifyIoRequest._blockid = _createId;
    mcNotifyIoRequest._cookie = _bootCookie;
    mcHaltNodesRequest._cookie = _bootCookie;
    std::vector<BCNodeInfo*> targetargs;

    for (vector<BCIconInfo*>::iterator nciter = getIcons().begin(); nciter != getIcons().end(); ++nciter)
    {
	if (typeid(**nciter) != typeid(BCNodecardInfo))	// only process node cards
	    continue;
        LOG_TRACE_MSG("Processing " << (*nciter)->location() << ". open=" << (*nciter)->_open
                      << " _isshared=" << _isshared << " nodesopen=" << (*nciter)->nodesOpen());
	// only process connected node cards or node cards with open processor cards
	if ((*nciter)->_open || (_isshared && (*nciter)->nodesOpen()))
	{
	    BCNodecardInfo* ncinfo = dynamic_cast<BCNodecardInfo*>(*nciter);
	    // add entries for all the nodes in the node card
            LOG_TRACE_MSG("Processing " << ncinfo->_nodes.size() << " nodes.");
	    for (unsigned i = 0; i < ncinfo->_nodes.size(); ++i)
	    {
		BCNodeInfo* ninfo = dynamic_cast<BCNodeInfo*>(ncinfo->_nodes[i]);
		if (ninfo->_open) // only process connected nodes
		{
                    if(ninfo->_linkio == false) {// Only add it to halt request if it's a compute
                        mcHaltNodesRequest._location.push_back(ninfo->location());
                        LOG_TRACE_MSG( "added node " << ninfo->location() << " to haltnodes request." );
                    }
                    else { // Connected IO needs to know the block is going down.
                        mcNotifyIoRequest._location.push_back(ninfo->location());
                        targetargs.push_back(ninfo);
                        LOG_TRACE_MSG( "added node " << ninfo->location() << " to notify io request" );
                    }
		}
	    }
	}
    }

    std::ostringstream notifystream;
    mcNotifyIoRequest.write(notifystream);
    LOG_TRACE_MSG(notifystream.str());
    std::ostringstream haltnodesstream;
    mcHaltNodesRequest.write(haltnodesstream);
    LOG_TRACE_MSG(haltnodesstream.str());

    // Don't send a halt request if:
    // 1) Somebody said they didn't want one with a no_sysrq option.
    // 2) There are no locations in the request.
    // 3) The block is already shut down.
    // 4) It's a diags block
    if(!no_sysrq && !mcHaltNodesRequest._location.empty() && !_block_shut_down && !_diags) {
        // LLCS wants to do this.  Comment for now, bring back if it fails.
        // try {
        //     getMCServer()->haltnodes(mcHaltNodesRequest, mcHaltNodesReply);
        // } catch (exception &e) {
        //     mcHaltNodesReply._rc = -1;
        //     mcHaltNodesReply._rt = e.what();
        // } catch (XML::Exception &e) {
        //     ostringstream buf; buf << e;
        //     mcHaltNodesReply._rc = -1;
        //     mcHaltNodesReply._rt = buf.str();
        // }

        // if (mcHaltNodesReply._rc) {
        //     LOG_ERROR_MSG("HaltNodes response: " << mcHaltNodesReply._rt
        //                   << " Halt nodes error: " << mcHaltNodesReply._rc);
        //     reply << FAIL << mcHaltNodesReply._rt << DONE;
        //     // Don't return here.  Let it go to FREE and rely on LLCS to set
        //     // hardware in error.
        //     // return;
        // }

        // LOG_INFO_MSG("Halt Nodes sent");
    } else {
        LOG_WARN_MSG("no Halt Nodes sent");
    }

    time(&_shutdown_sent_time);

    reply << OK;
    // Send the shutdown if we're not already done.  Note, diags blocks
    // WILL get shut down even though halt and notify won't happen.
    bool sent = false;
    if(!_block_shut_down) {
        //
        // Send the ShutdownBlockRequest
        try
            {
                if(!hardWareAccessBlocked()) {
                    _mcServer->shutdownBlock(mcShutdownBlockRequest, mcShutdownBlockReply);
                    sent = true;
                } else {
                    std::ostringstream msg;
                    msg << "SubnetMc managing hardware for " << _blockName 
                        << " temporarily unavailable.";
                    LOG_ERROR_MSG(msg.str());
                    reply << ABORT << msg.str();
                }
            }
        catch (exception &e)
            {
                mcShutdownBlockReply._rc = -1;
                mcShutdownBlockReply._rt = e.what();
            }
        if (mcShutdownBlockReply._rc != 0)
            {
                LOG_WARN_MSG("shutdown_block: " << mcShutdownBlockReply._rt);
                reply << ABORT << "shutdown_block: " << mcShutdownBlockReply._rt;
                // Don't return here.  Let it go to FREE and rely on LLCS to set
                // hardware in error.
                // return;
            }

        _isstarted = false;
        if(sent)
            LOG_INFO_MSG("Shutdown sent");
    }
    if(!_diags) {
        quiesceMailbox(0);
        // LLCS wants to do this.  Comment for now, bring back if that fails.
        // if(!notify_io(mcNotifyIoRequest, mcNotifyIoReply, reply, targetargs)) {
        //     // Don't need to add anything to the reply.  notify_io already did.
        //     // We'll only get here if we failed to open the target set.
        //     // We also set _disconnecting to false because we aren't 
        //     // going to shut down the block.  We need to be able to shut
        //     // it down later.
        //     _disconnecting = false;
        //     return;
        // }
    }
    if(sent)
        _block_shut_down = true;
    reply << DONE;
}

void
CNBlockController::show_barrier(MMCSCommandReply& reply) {
    LOGGING_DECLARE_BLOCK_ID_MDC; LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    reply << OK;

    // Spin through all of our nodes.  For each one, find its
    // associated rack and see if its location is set.
    for(std::vector<BCNodeInfo*>::iterator it = _nodes.begin();
        it != _nodes.end(); ++it) {
        std::string loc = (*it)->location();
        std::string rack = loc.substr(0,3);
        if(_rackbits.find(rack) != _rackbits.end() &&
           _rackbits[rack].Flagged(loc) == false)
            reply << loc << std::endl;
    }

    reply << DONE;
    return;
}

bool
CNBlockController::processRASMessage(RasEvent& rasEvent)
{
    LOGGING_DECLARE_BLOCK_ID_MDC; LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_TRACE_MSG("Process RAS message " << rasEvent.getDetails()[RasEvent::MSG_ID]);
    bool filtered = false;
    boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();
    // Barrier RAS message
    if (rasEvent.msgId() == 0x00040096) {
        std::string locstr = rasEvent.getDetails()[RasEvent::LOCATION];
        std::string rack = locstr.substr(0,3);
        if(locstr.find("I") == std::string::npos)
            _rackbits[rack].Set(locstr);
        else {
            LOG_WARN_MSG("Received an IO board barrier RAS for location " << locstr <<
                         " on compute block " << _blockName << ".");
#if 0
            _ioboardbits[rack].Set(locstr);
#endif
        }

        unsigned satisfied_nodes = 0;
        for(std::map<std::string, RackBitset>::iterator it = _rackbits.begin();
            it != _rackbits.end(); ++it) {
            satisfied_nodes += it->second.BitsSet();
        }

        for(std::map<std::string, IOBoardBitset>::iterator it = _ioboardbits.begin();
            it != _ioboardbits.end(); ++it) {
            satisfied_nodes += it->second.BitsSet();
        }

        // NOTE:  IF WE EVER HAVE TO ACCOUNT FOR CONNECTED IO,
        // THIS WILL HAVE TO TEST satisfied_nodes AGAINST _nodes.size()
        if(satisfied_nodes == _numComputesTotal) {
            LOG_INFO_MSG("CN block barrier satisfied");
            // Clear all barrier bits so we can do another.
            for(std::map<std::string, RackBitset>::iterator it = _rackbits.begin();
                it != _rackbits.end(); ++it) {
                it->second.Reset();
            }
            for(std::map<std::string, IOBoardBitset>::iterator it = _ioboardbits.begin();
                it != _ioboardbits.end(); ++it) {
                it->second.Reset();
            }

            _do_barrier_ack = true;
        }
        filtered = true;
        boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
        boost::posix_time::time_duration td = end_time - start;
        _total_barrier_duration += td;
        LOG_TRACE_MSG(td.total_microseconds() << " usec to process barrier.");
    }

    // write the RAS message to one or more log files
    if (!filtered) {
        printRASMessage(rasEvent);
    }

    if (_diags && !_insertDiagsRAS) // filter RAS that occurs from diags unless they request it
        filtered = true;

    return filtered;		// don't filter
}
