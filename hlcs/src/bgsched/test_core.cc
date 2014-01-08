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
/* (C) Copyright IBM Corp.  2009, 2012                              */
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

#include <bgsched/bgsched.h>
#include <bgsched/Block.h>
#include <bgsched/Coordinates.h>
#include <bgsched/IOBlock.h>

#include <bgsched/core/core.h>

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string>
#include <vector>

#include <stdio.h>
#include <strings.h>
#include <time.h>

using namespace bgsched;
using namespace log4cxx;
using namespace std;

// Forward declares
void printIODrawerInfo(const IODrawer::ConstPtr IODrawerPtr, const bool isVerbose);
void printIONodeInfo(const IONode::ConstPtr IONodePtr, const bool isVerbose);
void printMidplaneInfo(const Midplane::ConstPtr midplanePtr);
void printMidplaneInfoBrief(const Midplane::ConstPtr midplanePtr);
void printSwitchInfo(const Switch::ConstPtr switchPtr);
void printSwitchSettingsInfo(const SwitchSettings::ConstPtr switchSettingsPtr);
void printCableInfo(const Cable::ConstPtr cablePtr);
void printCableInfoBrief(const Cable::ConstPtr cablePtr);
void printCableInfoSuperBrief(const Cable::ConstPtr cablePtr);
void printIOLinkInfo(const IOLink::ConstPtr IOLinkPtr);
void printBlockInfo(const Block::Ptr blockPtr);
void printBlockBrief(const Block::Ptr blockPtr);
void printIOBlockInfo(const IOBlock::Ptr IOBlockPtr);
void printIOBlockBrief(const IOBlock::Ptr IOBlockPtr);
void printJobInfo(const Job::ConstPtr jobPtr);
void printNodeBoardInfo(const NodeBoard::ConstPtr nodeBoardPtr);
void printNodeInfo(const Node::ConstPtr nodePtr);
const string blockStatusToString(EnumWrapper<Block::Status> blockStatus);
const string blockActionToString(EnumWrapper<Block::Action::Value> blockAction);
const string IOBlockStatusToString(EnumWrapper<IOBlock::Status> IOBlockStatus);
const string IOBlockActionToString(EnumWrapper<IOBlock::Action::Value> IOBlockAction);
const string jobStatusToString(EnumWrapper<Job::Status> jobStatus);
const string hardwareStateToString(EnumWrapper<Hardware::State> hardwareState);
const string nodeBoardQuadrantToString(EnumWrapper<NodeBoard::Quadrant> quadrant);
const string midplaneInUseToString(EnumWrapper<Midplane::InUse> inUse);
const string switchInUseToString(EnumWrapper<Switch::InUse> inUse);
const string portsSettingToString(EnumWrapper<SwitchSettings::PortsSetting> portsSetting);
int listMachineSize();
void printHelp();

LOG_DECLARE_FILE("bgsched");

static const string JOB_STATUS_STRINGS[] = {
        "SETUP",
        "LOADING",
        "STARTING",
        "RUNNING",
        "DEBUG",
        "CLEANUP",
        "TERMINATED",
        "ERROR"
};

static const string BLOCK_STATUS_STRINGS[] = {
        "ALLOCATED",
        "BOOTING",
        "FREE",
        "INITIALIZED",
        "TERMINATING"
};

static const string BLOCK_ACTION_STRINGS[] = {
        "NONE",
        "BOOT",
        "FREE"
};

static const string HARDWARE_STATE_STRINGS[] = {
        "AVAILABLE",
        "MISSING",
        "ERROR",
        "SERVICE",
        "SOFTWARE FAILURE"
};

static const string NODEBOARD_QUADRANT_STRINGS[] = {
        "Q1",
        "Q2",
        "Q3",
        "Q4"
};

static const string MIDPLANE_INUSE_STRINGS[] = {
        "No",
        "All",
        "Subdivided"
};

static const string SWITCH_INUSE_STRINGS[] = {
        "Switch is not in use",
        "Switch included, input/output ports in use",
        "Switch included, output port in use",
        "Switch included, input port in use",
        "Switch not included, ports are wrapped",
        "Switch not included, ports used for passthrough",
        "Switch not included, ports are wrapped and used for passthrough"
};

static const string PORTS_SETTING_STRINGS[] = {
        "In",
        "Out",
        "Both"
};

int listMachineSize()
{
    try {
        // Get the size of the system in each dimension
        Coordinates machineSize = core::getMachineSize();
        LOG_INFO_MSG("System dimensions - A:" << machineSize[Dimension::A]
                                     << " B:" << machineSize[Dimension::B]
                                     << " C:" << machineSize[Dimension::C]
                                     << " D:" << machineSize[Dimension::D]);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling getMachineSize");
        return -1;
    }

    return 0;
}

int listIOUsageLimit()
{
    try {
        // Get the I/O limit usage setting
        LOG_INFO_MSG("I/O usage limit . . . . . . . : " << bgsched::getIOUsageLimit());

    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling getIOUsageLimit");
        return -1;
    }

    return 0;
}

int listHardware(const bool isVerbose)
{
    try {
        // Get the compute hardware
        //LOG_INFO_MSG("Calling core::getComputeHardware()");
        ComputeHardware::ConstPtr bgq = core::getComputeHardware();
        LOG_INFO_MSG("Blue Gene system configuration: " <<  bgq->getMachineRows() << " row(s)  X  " << bgq->getMachineColumns() << " column(s)");
        LOG_INFO_MSG("Midplane size in compute nodes: " << bgq->getMidplaneSize(Dimension::A) <<
                "x" << bgq->getMidplaneSize(Dimension::B) << "x" << bgq->getMidplaneSize(Dimension::C) <<
                "x" << bgq->getMidplaneSize(Dimension::D) << "x" << bgq->getMidplaneSize(Dimension::E));
        for (uint32_t a = 0; a < bgq->getMachineSize(Dimension::A); ++a) {
            for (uint32_t b = 0; b < bgq->getMachineSize(Dimension::B); ++b) {
                for (uint32_t c = 0; c < bgq->getMachineSize(Dimension::C); ++c) {
                    for (uint32_t d = 0; d < bgq->getMachineSize(Dimension::D); ++d) {
                        Coordinates coords(a, b, c, d);
                        Midplane::ConstPtr midplane = bgq->getMidplane(coords);
                        LOG_INFO_MSG("==== Midplane at A:" << a << " B:" << b << " C:" << c << " D:" << d << " ====");
                        if (isVerbose) {
                            printMidplaneInfo(midplane);
                            LOG_INFO_MSG(" Total 'Available' node boards  . : " << midplane->getAvailableNodeBoardCount());
                        } else {
                            printMidplaneInfoBrief(midplane);
                        }
                        if (isVerbose) {
                            // Print the node boards on the midplane
                            for (uint32_t i = 0; i <  Midplane::MaxNodeBoards; i++) {
                                NodeBoard::ConstPtr nodeBoard = midplane->getNodeBoard(i);
                                printNodeBoardInfo(nodeBoard);
                            }
                        }
                        // Print switches and cables for the midplane
                        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
                            Switch::ConstPtr switchPtr = midplane->getSwitch(dim);
                            if (isVerbose) {
                                printSwitchInfo(switchPtr);
                            }
                            if (isVerbose) {
                                vector<SwitchSettings::ConstPtr> switchSettings = switchPtr->getSwitchSettings();
                                if (switchSettings.size() == 0) {
                                    LOG_INFO_MSG(" No switch settings in dimension  : " << string(dim));
                                } else {
                                    for (vector<SwitchSettings::ConstPtr>::const_iterator iter = switchSettings.begin(); iter != switchSettings.end(); iter++) {
                                        printSwitchSettingsInfo(*iter);
                                    }
                                }
                            }
                            Cable::ConstPtr cable = switchPtr->getCable();
                            if (!cable) {
                                LOG_INFO_MSG(" No cable in dimension  . . . . . : " << string(dim));
                            } else {
                                if (isVerbose) {
                                    printCableInfo(cable);
                                } else {
                                    printCableInfoBrief(cable);
                                }
                            }
                        }
                        LOG_INFO_MSG("=== E N D   O F   M I D P L A N E ===");
                        LOG_INFO_MSG("=====================================");
                    }
                }
            }
        }
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling core::getComputeHardware");
        return -1;
    }

    return 0;
}

int listIOHardware(const bool isVerbose)
{
    try {
        // Get the I/O hardware
        //LOG_INFO_MSG("Calling core::getIOHardware()");
        IOHardware::ConstPtr bgqIO = core::getIOHardware();
        //LOG_INFO_MSG("Testing out getIONode() on IOHardware object --- find R11-ID-J01");
        //printIONodeInfo(bgqIO->getIONode("R11-ID-J01"),isVerbose);
        //LOG_INFO_MSG("Testing out IOHardware copy ctor");
        //IOHardware::ConstPtr bgqIOCopy = bgqIO;
        //vector<IODrawer::ConstPtr> iodrawers = bgqIOCopy->getIODrawers();
        //LOG_INFO_MSG("Testing out getState() on IOHardware object --- state of R01-ID-J04");
        //LOG_INFO_MSG("     Hardware state . . . . . . . : " << hardwareStateToString(bgqIO->getState("R01-ID-J04")));
        //LOG_INFO_MSG("Testing out getState() on IOHardware object --- state of R11-ID");
        //LOG_INFO_MSG("     Hardware state . . . . . . . : " << hardwareStateToString(bgqIO->getState("R11-ID")));
        //LOG_INFO_MSG("Testing out getState() on IOHardware object --- state of R11-IZ -- should fail");
        //LOG_INFO_MSG("     Hardware state . . . . . . . : " << hardwareStateToString(bgqIO->getState("R11-IZ")));
        //LOG_INFO_MSG("Testing out getState() on IOHardware object --- state of R11 -- should fail");
        //LOG_INFO_MSG("     Hardware state . . . . . . . : " << hardwareStateToString(bgqIO->getState("R11")));
        //LOG_INFO_MSG("Testing out getState() on IOHardware object --- state of R11-ID-JW9 -- should fail");
        //LOG_INFO_MSG("     Hardware state . . . . . . . : " << hardwareStateToString(bgqIO->getState("R11-ID-JW9")));
        //LOG_INFO_MSG("Testing out getState() on IOHardware object --- state of RX1-ID-JW9 -- should fail");
        //LOG_INFO_MSG("     Hardware state . . . . . . . : " << hardwareStateToString(bgqIO->getState("RX1-ID-JW9")));

        vector<IODrawer::ConstPtr> iodrawers = bgqIO->getIODrawers();

        if (iodrawers.size() == 0) {
            LOG_INFO_MSG(" No I/O drawers");
        } else {
            LOG_INFO_MSG("Found " << iodrawers.size() << " I/O drawers");
            for (vector<IODrawer::ConstPtr>::const_iterator iter = iodrawers.begin(); iter != iodrawers.end(); iter++) {
                IODrawer::ConstPtr iodrawerPtr = *(iter);
                printIODrawerInfo(iodrawerPtr, isVerbose);
                IONode::ConstPtrs ionodes = iodrawerPtr->getIONodes();
                for (vector<IONode::ConstPtr>::const_iterator nodeIter = ionodes.begin(); nodeIter != ionodes.end(); nodeIter++) {
                    printIONodeInfo(*(nodeIter), isVerbose);
                }
                LOG_INFO_MSG("========================================");
            }
        }
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling core::getIOHardware");
        return -1;
    }

    return 0;
}

void printIODrawerInfo(const IODrawer::ConstPtr IODrawerPtr, const bool isVerbose)
{
    LOG_INFO_MSG(" I/O drawer location  . . . . . . : " << IODrawerPtr->getLocation());
    LOG_INFO_MSG(" Hardware state . . . . . . . . . : " << hardwareStateToString(IODrawerPtr->getState()));
    if (isVerbose) {
        LOG_INFO_MSG(" Sequence ID  . . . . . . . . . . : " << IODrawerPtr->getSequenceId());
        LOG_INFO_MSG(" Available I/O nodes  . . . . . . : " << IODrawerPtr->getAvailableIONodeCount());
    }
}

void printIONodeInfo(const IONode::ConstPtr IONodePtr, const bool isVerbose)
{
    LOG_INFO_MSG("   I/O node location  . . . . . . : " << IONodePtr->getLocation());
    LOG_INFO_MSG("     Hardware state . . . . . . . : " << hardwareStateToString(IONodePtr->getState()));
    if (isVerbose) {
        if (IONodePtr->isInUse()) {
            LOG_INFO_MSG("     In use . . . . . . . . . . . : Yes");
            LOG_INFO_MSG("     In use I/O block . . . . . . : " << IONodePtr->getIOBlockName());
        } else {
            LOG_INFO_MSG("     In Use . . . . . . . . . . . : No");
        }
        LOG_INFO_MSG("     Sequence ID  . . . . . . . . : " << IONodePtr->getSequenceId());
    }
}

int listHardwareWiring()
{
    try {
        // Print the machine size
        listMachineSize();
        // Get the compute hardware
        ComputeHardware::ConstPtr bgq = core::getComputeHardware();
        for (uint32_t d = 0; d < bgq->getMachineSize(Dimension::D); ++d) {
            for (uint32_t c = 0; c < bgq->getMachineSize(Dimension::C); ++c) {
                for (uint32_t b = 0; b < bgq->getMachineSize(Dimension::B); ++b) {
                    for (uint32_t a = 0; a < bgq->getMachineSize(Dimension::A); ++a) {
                        Coordinates coords(a, b, c, d);
                        Midplane::ConstPtr midplane = bgq->getMidplane(coords);
                        LOG_INFO_MSG("==== Midplane " << midplane->getLocation() << " at " << a << b << c << d << " ====");
                        // Print switches and cables for the midplane
                        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
                            Switch::ConstPtr switchPtr = midplane->getSwitch(dim);
                            Cable::ConstPtr cable = switchPtr->getCable();
                            if (!cable) {
                                LOG_INFO_MSG(" No cable in dimension  . . . . . : " << string(dim));
                            } else {
                                    printCableInfoSuperBrief(cable);
                            }
                        }
                        LOG_INFO_MSG("=====================================");
                    }
                }
            }
        }
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error");
        return -1;
    }

    return 0;
}

void printMidplaneInfo(const Midplane::ConstPtr midplanePtr)
{
    //Coordinates mpCoords = core::getMidplaneCoordinates(midplanePtr->getLocation());
    LOG_INFO_MSG(" Midplane location  . . . . . . . : " << midplanePtr->getLocation());
    //LOG_INFO_MSG("    Coordinates . . . . . . . . . : " << mpCoords[Dimension::A] << "," << mpCoords[Dimension::B] << "," << mpCoords[Dimension::C] << "," << mpCoords[Dimension::D]);
    LOG_INFO_MSG("    In use  . . . . . . . . . . . : " << midplaneInUseToString(midplanePtr->getInUse()));
    LOG_INFO_MSG("    In use compute block  . . . . : " << midplanePtr->getComputeBlockName());
    LOG_INFO_MSG("    Hardware state  . . . . . . . : " << hardwareStateToString(midplanePtr->getState()));
    LOG_INFO_MSG("    Sequence ID . . . . . . . . . : " << midplanePtr->getSequenceId());
    LOG_INFO_MSG("    Compute node memory (MB)  . . : " << midplanePtr->getComputeNodeMemory());
}

void printMidplaneInfoBrief(const Midplane::ConstPtr midplanePtr)
{
    LOG_INFO_MSG(" Midplane location  . . . . . . . : " << midplanePtr->getLocation());
    LOG_INFO_MSG("    In use  . . . . . . . . . . . : " << midplaneInUseToString(midplanePtr->getInUse()));
    LOG_INFO_MSG("    In use compute block  . . . . : " << midplanePtr->getComputeBlockName());
}

void printSwitchInfo(const Switch::ConstPtr switchPtr)
{
    //Coordinates mpCoords = switchPtr->getMidplaneCoordinates();
    LOG_INFO_MSG(" Switch location  . . . . . . . . : " << switchPtr->getLocation());
    //LOG_INFO_MSG("    Associated midplane . . . . . : " << switchPtr->getMidplaneLocation());
    //LOG_INFO_MSG("    Associated midplane coords  . : " << mpCoords[Dimension::A] << "," << mpCoords[Dimension::B] << "," << mpCoords[Dimension::C] << "," << mpCoords[Dimension::D]);
    LOG_INFO_MSG("    In use  . . . . . . . . . . . : " << switchInUseToString(switchPtr->getInUse()));
    LOG_INFO_MSG("    Hardware state  . . . . . . . : " << hardwareStateToString(switchPtr->getState()));
    LOG_INFO_MSG("    Sequence ID . . . . . . . . . : " << switchPtr->getSequenceId());
}

void printSwitchSettingsInfo(const SwitchSettings::ConstPtr switchSettingsPtr)
{
    string block = switchSettingsPtr->getComputeBlockName();

    LOG_INFO_MSG(" Switch settings  . . . . . . . . : " << switchSettingsPtr->getLocation());
    LOG_INFO_MSG("    Compute block . . . . . . . . : " << block);
    if (switchSettingsPtr->isPassthrough()) {
        LOG_INFO_MSG("    Passthrough . . . . . . . . . : Yes");
    } else {
        LOG_INFO_MSG("    Passthrough . . . . . . . . . : No");
    }
    if (switchSettingsPtr->isIncludedMidplane()) {
        LOG_INFO_MSG("    Included  . . . . . . . . . . : Yes");
    } else {
        LOG_INFO_MSG("    Included  . . . . . . . . . . : No");
    }
    LOG_INFO_MSG("    Ports setting . . . . . . . . : " << portsSettingToString(switchSettingsPtr->getPortsSetting()));
}

void printCableInfo(const Cable::ConstPtr cablePtr)
{
    LOG_INFO_MSG(" Cable location . . . . . . . . . : " << cablePtr->getLocation());
    LOG_INFO_MSG("    Destination location  . . . . : " << cablePtr->getDestinationLocation());
    LOG_INFO_MSG("    Hardware state  . . . . . . . : " << hardwareStateToString(cablePtr->getState()));
    LOG_INFO_MSG("    Sequence ID . . . . . . . . . : " << cablePtr->getSequenceId());
}

void printCableInfoBrief(const Cable::ConstPtr cablePtr)
{
    LOG_INFO_MSG(" Cable location . . . . . . . . . : " << cablePtr->getLocation());
    LOG_INFO_MSG("    Destination location  . . . . : " << cablePtr->getDestinationLocation());
}

void printCableInfoSuperBrief(const Cable::ConstPtr cablePtr)
{
    LOG_INFO_MSG(" Destination location . . . . . . : " << cablePtr->getDestinationLocation());
}

int listIOLinks(const string& midplane)
{
    vector<IOLink::ConstPtr> IOLinkVector; // Vector of I/O links returned by core::getIOLinks

    try {
        IOLinkVector = core::getIOLinks(midplane);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling core::getIOLinks");
        return -1;
    }

    // Check if we got any I/O links back
    if (IOLinkVector.empty()) {
        LOG_INFO_MSG("No I/O links found");
        return 0; // Leave
    }

    // Print the I/O link info
    if (midplane.empty()) {
        LOG_INFO_MSG("Number of I/O links returned for entire BG/Q system: " << IOLinkVector.size());
    } else {
        LOG_INFO_MSG("Number of I/O links returned for midplane " << midplane << ": " << IOLinkVector.size());
    }

    for (vector<IOLink::ConstPtr>::iterator IOLinkVectorIter = IOLinkVector.begin(); IOLinkVectorIter != IOLinkVector.end(); IOLinkVectorIter++) {
        printIOLinkInfo(*(IOLinkVectorIter));
    }

    return 0;
}

int listAvailableIOLinks(const string& midplane)
{
    vector<IOLink::ConstPtr> IOLinkVector; // Vector of I/O links returned by core::getAvailableIOLinks

    try {
        IOLinkVector = core::getAvailableIOLinks(midplane);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling core::getAvailableIOLinks");
        return -1;
    }

    // Check if we got any I/O links back
    if (IOLinkVector.empty()) {
        LOG_INFO_MSG("No I/O links found");
        return 0; // Leave
    }

    // Print the I/O link info
    if (midplane.empty()) {
        LOG_INFO_MSG("Number of available I/O links returned for entire BG/Q system: " << IOLinkVector.size());
    } else {
        LOG_INFO_MSG("Number of available I/O links returned for midplane " << midplane << ": " << IOLinkVector.size());
    }

    for (vector<IOLink::ConstPtr>::iterator IOLinkVectorIter = IOLinkVector.begin(); IOLinkVectorIter != IOLinkVector.end(); IOLinkVectorIter++) {
        printIOLinkInfo(*(IOLinkVectorIter));
    }

    return 0;
}

int listBlockIOLinks(const string& block)
{
    vector<IOLink::ConstPtr> IOLinkVector; // Vector of I/O links returned by Block::checkIOLinks

    try {
        IOLinkVector = Block::checkIOLinks(block);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling Block::checkIOLinks");
        return -1;
    }

    // Check if we got any I/O links back
    if (IOLinkVector.empty()) {
        LOG_INFO_MSG("No I/O links found for block " << block);
    } else {
        // Print the I/O link info
        LOG_INFO_MSG("Number of I/O links returned for block " << block << ": " << IOLinkVector.size());

        for (vector<IOLink::ConstPtr>::iterator IOLinkVectorIter = IOLinkVector.begin(); IOLinkVectorIter != IOLinkVector.end(); IOLinkVectorIter++) {
            printIOLinkInfo(*(IOLinkVectorIter));
        }
    }

    return 0;
}

int listBlockAvailableIOLinks(const string& block)
{
    vector<IOLink::ConstPtr> IOLinkVector; // Vector of I/O links returned by Block::checkAvailableIOLinks

    try {
        IOLinkVector = Block::checkAvailableIOLinks(block);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling Block::checkAvaiableIOLinks");
        return -1;
    }

    // Check if we got any I/O links back
    if (IOLinkVector.empty()) {
        LOG_INFO_MSG("No available I/O links found for block " << block);
    } else {
        // Print the I/O link info
        LOG_INFO_MSG("Number of available I/O links returned for block " << block << ": " << IOLinkVector.size());

        for (vector<IOLink::ConstPtr>::iterator IOLinkVectorIter = IOLinkVector.begin(); IOLinkVectorIter != IOLinkVector.end(); IOLinkVectorIter++) {
            printIOLinkInfo(*(IOLinkVectorIter));
        }
    }

    return 0;
}

int listBlockIOLinksBrief(const string& block)
{

    uint32_t availableIOLinks = 0;
    uint32_t unavailableIOLinks = 0;
    try {
        Block::checkIOLinksSummary(block, &availableIOLinks, &unavailableIOLinks);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling Block::checkIOLinksSummary");
        return -1;
    }

    // Print the I/O link summary info
    LOG_INFO_MSG("Number of available I/O links for block " << block << ": " << availableIOLinks);
    LOG_INFO_MSG("Number of unavailable I/O links for block " << block << ": " << unavailableIOLinks);

    return 0;
}

int checkBlockIO(const string& block)
{
    vector<string> unconnectedIONodes; // Vector of unconnected I/O nodes returned
    vector<string> midplanesFailingIORules; // Vector of midplanes failing I/O requirements returned

    try {
        Block::checkIO(block, &unconnectedIONodes, &midplanesFailingIORules);
        if (midplanesFailingIORules.empty()) {
            LOG_INFO_MSG("All midplanes meet I/O requirements for block " << block);
        } else {
                // Print the failing midplanes
                LOG_INFO_MSG("Block " << block << " has " << midplanesFailingIORules.size() << " midplanes not meeting I/O requirements: ");
                for (unsigned int it = 0; it < midplanesFailingIORules.size(); it++) {
                    LOG_INFO_MSG(midplanesFailingIORules[it]);
                }
        }
        if (unconnectedIONodes.empty()) {
            LOG_INFO_MSG("All I/O nodes are connected for block " << block);
        } else {
                // Print the count of unconnected I/O nodes and names of I/O nodes
                LOG_INFO_MSG("Block " << block << " has " << unconnectedIONodes.size() << " unconnected I/O node(s)");
                for (unsigned int it = 0; it < unconnectedIONodes.size(); it++) {
                    LOG_INFO_MSG(unconnectedIONodes[it]);
                }
        }
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling Block::checkIO");
        return -1;
    }

    return 0;
}

int checkBlockIOConnected(const string& block)
{
    vector<string> unconnectedIONodes; // Vector of unconnected I/O nodes returned

    try {
        if (Block::isIOConnected(block, &unconnectedIONodes)) {
            LOG_INFO_MSG("All I/O nodes are connected for block " << block);
        } else {
            // Check if we got any unconnected resources back
            if (unconnectedIONodes.empty()) {
                LOG_ERROR_MSG("Unexpected error: unconnected I/O nodes not returned for block " << block);
            } else {
                // Print the unconnected resources
                LOG_INFO_MSG("Block " << block << " has the following " << unconnectedIONodes.size() << " unconnected I/O nodes:");
                for (unsigned int it = 0; it < unconnectedIONodes.size(); it++) {
                    LOG_INFO_MSG(unconnectedIONodes[it]);
                }
            }
        }
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling Block::isIOConnected");
        return -1;
    }

    return 0;
}

int listconnectedcomputeblocks(const string& ioblock)
{
    IOBlock::ConnectedComputeBlocks connectedcomputeblocks; // Vector of connected compute blocks returned

    try {
        connectedcomputeblocks = IOBlock::getConnectedComputeBlocks(ioblock);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling getConnectedComputeBlocks");
        return -1;
    }

    // Check if we got any compute blocks back
    if (connectedcomputeblocks.empty()) {
        LOG_INFO_MSG("Found no compute blocks connected to I/O block " << ioblock);
        return 0; // Leave
    }

    // Print the connected compute blocks
    LOG_INFO_MSG("I/O block " << ioblock << " has the following " << connectedcomputeblocks.size() << " connected compute blocks:");
    for (unsigned int it = 0; it < connectedcomputeblocks.size(); it++) {
        LOG_INFO_MSG(connectedcomputeblocks[it]);
    }
    return 0;
}

void printIOLinkInfo(const IOLink::ConstPtr IOLinkPtr)
{
    LOG_INFO_MSG("===================================");
    LOG_INFO_MSG("    I/O link location . . . . . . : " << IOLinkPtr->getLocation());
    LOG_INFO_MSG("    I/O link hardware state . . . : " << hardwareStateToString(IOLinkPtr->getState()));
    LOG_INFO_MSG("    I/O link sequence ID  . . . . : " << IOLinkPtr->getSequenceId());
    LOG_INFO_MSG("    Destination I/O node location : " << IOLinkPtr->getDestinationLocation());
    LOG_INFO_MSG("    I/O node hardware state . . . : " << hardwareStateToString(IOLinkPtr->getIONodeState()));
}

int listBlocks(const BlockFilter& blockFilter, const core::BlockSort& blockSort, const bool isVerboseBlock)
{
    Block::Ptrs blockVector; // Vector of blocks returned by getBlocks

    //LOG_INFO_MSG("Calling core::getBlocks()");
    try {
        blockVector = core::getBlocks(blockFilter, blockSort);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling getBlocks");
        return -1;
    }
    //LOG_INFO_MSG("Completed call to core::getBlocks()");


    // Check if we got any blocks back
    if (blockVector.empty()) {
        LOG_INFO_MSG("Found no matching blocks");
        return 0; // Leave
    }

    // Print the block info
    for (Block::Ptrs::iterator blockVectorIter = blockVector.begin(); blockVectorIter != blockVector.end(); blockVectorIter++) {
        if (isVerboseBlock) {
            printBlockInfo(*(blockVectorIter));
        }
        else {
            printBlockBrief(*(blockVectorIter));
        }
    }

    LOG_INFO_MSG("Number of blocks returned: " << blockVector.size());

    return 0;
}

void printBlockInfo(const Block::Ptr blockPtr)
{
    LOG_INFO_MSG("===================================");
    LOG_INFO_MSG(" Block name . . . . . . . . . . . : " << blockPtr->getName());
    LOG_INFO_MSG("    Creation id . . . . . . . . . : " << blockPtr->getCreationId());
    LOG_INFO_MSG("    Status  . . . . . . . . . . . : " << blockStatusToString(blockPtr->getStatus()));
    LOG_INFO_MSG("    Action  . . . . . . . . . . . : " << blockActionToString(blockPtr->getAction()));
    LOG_INFO_MSG("    Compute nodes . . . . . . . . : " << blockPtr->getComputeNodeCount());
    LOG_INFO_MSG("    Microloader image . . . . . . : " << blockPtr->getMicroLoaderImage());
    LOG_INFO_MSG("    Node configuration  . . . . . : " << blockPtr->getNodeConfiguration());
    LOG_INFO_MSG("    Boot options  . . . . . . . . : " << blockPtr->getBootOptions());
    LOG_INFO_MSG("    Options . . . . . . . . . . . : " << blockPtr->getOptions());
    LOG_INFO_MSG("    Owner . . . . . . . . . . . . : " << blockPtr->getOwner());
    LOG_INFO_MSG("    User (block booter) . . . . . : " << blockPtr->getUser());
    LOG_INFO_MSG("    Sequence ID . . . . . . . . . : " << blockPtr->getSequenceId());
    LOG_INFO_MSG("    Description . . . . . . . . . : " << blockPtr->getDescription());

    // Print all job Ids associated with the block
    vector<Job::Id> jobIdVector = blockPtr->getJobIds();
    // Check if we got any job ids back
    if (jobIdVector.empty()) {
        LOG_INFO_MSG("    Job id  . . . . . . . . . . . : ");
    } else { // Print any job IDs
        for (vector<Job::Id>::iterator jobIdVectorIter = jobIdVector.begin(); jobIdVectorIter != jobIdVector.end(); jobIdVectorIter++) {
            LOG_INFO_MSG("    Job id  . . . . . . . . . . . : " << *(jobIdVectorIter));
        }
    }

    if (blockPtr->isSmall()) {
        // This is a small block.
        LOG_INFO_MSG("    Block type  . . . . . . . . . : Small");
        Block::NodeBoards nodeBoards(blockPtr->getNodeBoards());
        if (!nodeBoards.empty()) { // Dump the node boards
            for ( Block::NodeBoards::const_iterator i = nodeBoards.begin(); i != nodeBoards.end(); ++i ) {
                LOG_INFO_MSG("    Node board  . . . . . . . . . : " << (*i));
            }
        }
    } else {
        LOG_INFO_MSG("    Block type  . . . . . . . . . : Large");
        // Print block shape
        LOG_INFO_MSG("    Shape . . . . . . . . . . . . : " << blockPtr->getDimensionSize(Dimension::A) << "x"
            << blockPtr->getDimensionSize(Dimension::B) << "x" << blockPtr->getDimensionSize(Dimension::C) << "x"
            << blockPtr->getDimensionSize(Dimension::D));

        // Print block connectivity
        LOG_INFO_MSG("    Connectivity  . . . . . . . . : ");
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
            if (blockPtr->isMesh(dim) == true) {
                LOG_INFO_MSG("       " << string(dim) << " dimension  . . . . . . . : Mesh");
            } else {
                LOG_INFO_MSG("       " << string(dim) << " dimension  . . . . . . . : Torus");
            }
        }

        // Check if any midplanes
        Block::Midplanes midplanes(blockPtr->getMidplanes());
        if (!midplanes.empty()) {
            // Print the midplane locations for the block
            for (Block::Midplanes::const_iterator i = midplanes.begin(); i != midplanes.end(); ++i) {
                LOG_INFO_MSG("    Midplane  . . . . . . . . . . : " << (*i));
            }
            // Print any pass-through midplanes for the block
            Block::PassthroughMidplanes  passthroughMidplanes(blockPtr->getPassthroughMidplanes());
            for (Block::PassthroughMidplanes::const_iterator i = passthroughMidplanes.begin(); i != passthroughMidplanes.end(); ++i) {
                LOG_INFO_MSG("    Pass-through midplane . . . . : " << (*i));
            }
        }
    }

    vector<string> blockUsers = Block::getUsers(blockPtr->getName());
    if (blockUsers.size() == 0) {
        LOG_INFO_MSG("    Users authorized to run jobs  : None");
    } else {
        for (unsigned int i = 0; i < blockUsers.size(); i++) {
            LOG_INFO_MSG("    User authorized to run jobs . : " << blockUsers[i]);
        }
    }
}

void printBlockBrief(const Block::Ptr blockPtr)
{
    LOG_INFO_MSG("===================================");
    LOG_INFO_MSG(" Block name . . . . . . . . . . . : " << blockPtr->getName());
    LOG_INFO_MSG("    Status  . . . . . . . . . . . : " << blockStatusToString(blockPtr->getStatus()));
    LOG_INFO_MSG("    Action  . . . . . . . . . . . : " << blockActionToString(blockPtr->getAction()));
    LOG_INFO_MSG("    Owner . . . . . . . . . . . . : " << blockPtr->getOwner());
    LOG_INFO_MSG("    User (block booter) . . . . . : " << blockPtr->getUser());
    LOG_INFO_MSG("    Compute nodes . . . . . . . . : " << blockPtr->getComputeNodeCount());
    LOG_INFO_MSG("    Description . . . . . . . . . : " << blockPtr->getDescription());
}

const string blockStatusToString(EnumWrapper<Block::Status> blockStatus)
{
    return BLOCK_STATUS_STRINGS[blockStatus.toValue()];
}

const string blockActionToString(EnumWrapper<Block::Action::Value> blockAction)
{
    return BLOCK_ACTION_STRINGS[blockAction.toValue()];
}

int listIOBlocks(const IOBlockFilter& IOBlockFilter, const core::IOBlockSort& IOBlockSort, const bool isVerboseIOBlock)
{
    IOBlock::Ptrs IOBlockVector; // Vector of I/O blocks returned by getIOBlocks

    //LOG_INFO_MSG("Calling core::getIOBlocks()");
    try {
        IOBlockVector = core::getIOBlocks(IOBlockFilter, IOBlockSort);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling getIOBlocks");
        return -1;
    }
    //LOG_INFO_MSG("Completed call to core::getIOBlocks()");


    // Check if we got any I/O blocks back
    if (IOBlockVector.empty()) {
        LOG_INFO_MSG("Found no matching I/O blocks");
        return 0; // Leave
    }

    // Print the I/O block info
    for (IOBlock::Ptrs::iterator IOBlockVectorIter = IOBlockVector.begin(); IOBlockVectorIter != IOBlockVector.end(); IOBlockVectorIter++) {
        if (isVerboseIOBlock) {
            printIOBlockInfo(*(IOBlockVectorIter));
        }
        else {
            printIOBlockBrief(*(IOBlockVectorIter));
        }
    }

    LOG_INFO_MSG("Number of I/O blocks returned: " << IOBlockVector.size());

    return 0;
}

void printIOBlockInfo(const IOBlock::Ptr IOBlockPtr)
{
    LOG_INFO_MSG("===================================");
    LOG_INFO_MSG(" I/O block name . . . . . . . . . : " << IOBlockPtr->getName());
    LOG_INFO_MSG("    Creation id . . . . . . . . . : " << IOBlockPtr->getCreationId());
    LOG_INFO_MSG("    Status  . . . . . . . . . . . : " << IOBlockStatusToString(IOBlockPtr->getStatus()));
    LOG_INFO_MSG("    Action  . . . . . . . . . . . : " << IOBlockActionToString(IOBlockPtr->getAction()));
    LOG_INFO_MSG("    I/O nodes . . . . . . . . . . : " << IOBlockPtr->getIONodeCount());
    LOG_INFO_MSG("    Microloader image . . . . . . : " << IOBlockPtr->getMicroLoaderImage());
    LOG_INFO_MSG("    Node configuration  . . . . . : " << IOBlockPtr->getNodeConfiguration());
    LOG_INFO_MSG("    Boot options  . . . . . . . . : " << IOBlockPtr->getBootOptions());
    LOG_INFO_MSG("    Owner . . . . . . . . . . . . : " << IOBlockPtr->getOwner());
    LOG_INFO_MSG("    User (block booter) . . . . . : " << IOBlockPtr->getUser());
    LOG_INFO_MSG("    Sequence ID . . . . . . . . . : " << IOBlockPtr->getSequenceId());
    LOG_INFO_MSG("    Description . . . . . . . . . : " << IOBlockPtr->getDescription());
    // Check if any I/O locations
    IOBlock::IOLocations locations(IOBlockPtr->getIOLocations());
    if (!locations.empty()) {
        // Print the I/O locations for the I/O block
        for (IOBlock::IOLocations::const_iterator i = locations.begin(); i != locations.end(); ++i ) {
            LOG_INFO_MSG("    I/O location  . . . . . . . . : " << (*i));
        }
    }
}

void printIOBlockBrief(const IOBlock::Ptr IOBlockPtr)
{
    LOG_INFO_MSG("===================================");
    LOG_INFO_MSG(" I/O block name . . . . . . . . . : " << IOBlockPtr->getName());
    LOG_INFO_MSG("    Status  . . . . . . . . . . . : " << IOBlockStatusToString(IOBlockPtr->getStatus()));
    LOG_INFO_MSG("    Action  . . . . . . . . . . . : " << IOBlockActionToString(IOBlockPtr->getAction()));
    LOG_INFO_MSG("    Owner . . . . . . . . . . . . : " << IOBlockPtr->getOwner());
    LOG_INFO_MSG("    User (block booter) . . . . . : " << IOBlockPtr->getUser());
    LOG_INFO_MSG("    I/O nodes . . . . . . . . . . : " << IOBlockPtr->getIONodeCount());
    LOG_INFO_MSG("    Description . . . . . . . . . : " << IOBlockPtr->getDescription());
}

const string IOBlockStatusToString(EnumWrapper<IOBlock::Status> IOBlockStatus)
{
    return BLOCK_STATUS_STRINGS[IOBlockStatus.toValue()];
}

const string IOBlockActionToString(EnumWrapper<IOBlock::Action::Value> IOBlockAction)
{
    return BLOCK_ACTION_STRINGS[IOBlockAction.toValue()];
}

int listJobs(const JobFilter& jobFilter, const core::JobSort& jobSort) {
    Job::ConstPtrs jobVector; // Vector of jobs returned by getJobs

    try {
        //LOG_INFO_MSG("Calling core::getJobs()");
        jobVector = core::getJobs(jobFilter, jobSort);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling getJobs");
        return -1;
    }

    // Check if we got any jobs back
    if (jobVector.empty()) {
        LOG_INFO_MSG("Found no matching jobs");
        return 0; // Leave
    }

     // Print the job info
    for (vector<Job::ConstPtr>::iterator jobVectorIter = jobVector.begin(); jobVectorIter != jobVector.end(); jobVectorIter++) {
        printJobInfo(*(jobVectorIter));
    }

    LOG_INFO_MSG("Number of jobs returned: " << jobVector.size());

    return 0;
}

void printJobInfo(const Job::ConstPtr jobPtr)
{
    LOG_INFO_MSG("===================================");
    LOG_INFO_MSG(" Job identifier . . . . . . . . . : " << jobPtr->getId());
    LOG_INFO_MSG("    Status  . . . . . . . . . . . : " << jobStatusToString(jobPtr->getStatus()));
    LOG_INFO_MSG("    Scheduler data  . . . . . . . : " << jobPtr->getSchedulerData());
    LOG_INFO_MSG("    Compute block . . . . . . . . : " << jobPtr->getComputeBlockName());
    LOG_INFO_MSG("    User  . . . . . . . . . . . . : " << jobPtr->getUser());
    LOG_INFO_MSG("    Executable  . . . . . . . . . : " << jobPtr->getExecutable());
    LOG_INFO_MSG("    Working directory . . . . . . : " << jobPtr->getWorkingDirectory());
    LOG_INFO_MSG("    Arguments . . . . . . . . . . : " << jobPtr->getArgs());
    LOG_INFO_MSG("    Environmentals  . . . . . . . : " << jobPtr->getEnvs());
    LOG_INFO_MSG("    Start time  . . . . . . . . . : " << jobPtr->getStartTime());
    LOG_INFO_MSG("    Ranks per node  . . . . . . . : " << jobPtr->getRanksPerNode());
    LOG_INFO_MSG("    Sequence ID . . . . . . . . . : " << jobPtr->getSequenceId());
    LOG_INFO_MSG("    Corner (for sub-block job). . : " << jobPtr->getCorner());
    LOG_INFO_MSG("    Shape (for sub-block job) . . : " << jobPtr->getShape());
    LOG_INFO_MSG("    Compute nodes used  . . . . . : " << jobPtr->getComputeNodesUsed());
    LOG_INFO_MSG("    Mapping . . . . . . . . . . . : " << jobPtr->getMapping());
    LOG_INFO_MSG("    Client info . . . . . . . . . : " << jobPtr->getClientInfo());
    LOG_INFO_MSG("    Number of processes (np). . . : " << jobPtr->getNP());

    if (jobPtr->isInHistory()) {
        LOG_INFO_MSG("    Compute block creation Id . . : " << jobPtr->getComputeBlockCreationId());
        LOG_INFO_MSG("    End time  . . . . . . . . . . : " << jobPtr->getEndTime());
        LOG_INFO_MSG("    Run time (in seconds) . . . . : " << jobPtr->getRunTime());
        LOG_INFO_MSG("    Exit status . . . . . . . . . : " << jobPtr->getExitStatus());
        LOG_INFO_MSG("    Error text  . . . . . . . . . : " << jobPtr->getErrorText());
    }
}

const string jobStatusToString(EnumWrapper<Job::Status> jobStatus)
{
    return JOB_STATUS_STRINGS[jobStatus.toValue()];
}

int listNodeBoards(const string& midplane)
{
    vector<NodeBoard::ConstPtr> nodeBoardVector; // Vector of node boards returned by core::getNodeBoards

    try {
        nodeBoardVector = core::getNodeBoards(midplane);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling core::getNodeBoards");
        return -1;
    }

    // Check if we got any node boards back
    if (nodeBoardVector.empty()) {
        LOG_INFO_MSG("Found no node boards for midplane " << midplane);
        return 0; // Leave
    }

    LOG_INFO_MSG("Number of node boards returned: " << nodeBoardVector.size());

    // Print the node board info
    for (vector<NodeBoard::ConstPtr>::iterator nodeBoardVectorIter = nodeBoardVector.begin(); nodeBoardVectorIter != nodeBoardVector.end(); nodeBoardVectorIter++) {
        printNodeBoardInfo(*(nodeBoardVectorIter));
    }

    return 0;
}

void printNodeBoardInfo(const NodeBoard::ConstPtr nodeBoardPtr)
{
    //Coordinates mpCoords = nodeBoardPtr->getMidplaneCoordinates();
    LOG_INFO_MSG(" Node board location  . . . . . . : " << nodeBoardPtr->getLocation());
    if (nodeBoardPtr->isInUse()) {
        LOG_INFO_MSG("    In use by compute block . . . : " <<  nodeBoardPtr->getComputeBlockName());
    } else {
        LOG_INFO_MSG("    In use  . . . . . . . . . . . : No, unless midplane indicates 'All'" );
    }
    LOG_INFO_MSG("    Hardware state  . . . . . . . : " << hardwareStateToString(nodeBoardPtr->getState()));
    if (nodeBoardPtr->isMetaState()) {
        LOG_INFO_MSG("       Meta-state . . . . . . . . : Yes");
    } else {
        LOG_INFO_MSG("       Meta-state . . . . . . . . : No");
    }
    LOG_INFO_MSG("    Number of available cnodes  . : " << nodeBoardPtr->getAvailableNodeCount());
    LOG_INFO_MSG("    Quadrant  . . . . . . . . . . : " << nodeBoardQuadrantToString(nodeBoardPtr->getQuadrant()));
    LOG_INFO_MSG("    Sequence ID . . . . . . . . . : " << nodeBoardPtr->getSequenceId());
    //LOG_INFO_MSG("    Associated midplane . . . . . : " << nodeBoardPtr->getMidplaneLocation());
    //LOG_INFO_MSG("    Associated midplane coords  . : " << mpCoords[Dimension::A] << "," << mpCoords[Dimension::B] << "," << mpCoords[Dimension::C] << "," << mpCoords[Dimension::D]);
}

int listMidplaneNodes(const string& midplane)
{
    vector<Node::ConstPtr> nodeVector; // Vector of nodes returned by core::getMidplaneNodes

    try {
        LOG_INFO_MSG("About to call core::getMidplaneNodes()for midplane " << midplane);
        nodeVector = core::getMidplaneNodes(midplane);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling core::getMidplaneNodes");
        return -1;
    }

    // Check if we got any nodes back
    if (nodeVector.empty()) {
        LOG_INFO_MSG("Found no nodes for midplane " << midplane);
        return 0; // Leave
    }

    LOG_INFO_MSG("Number of nodes returned: " << nodeVector.size());

    // Print the node info
    for (vector<Node::ConstPtr>::iterator nodeVectorIter = nodeVector.begin(); nodeVectorIter != nodeVector.end(); nodeVectorIter++) {
        printNodeInfo(*(nodeVectorIter));
    }

    return 0;
}

int listNodes(const string& nodeBoard)
{
    vector<Node::ConstPtr> nodeVector; // Vector of nodes returned by core::getNodes

    try {
        nodeVector = core::getNodes(nodeBoard);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling core::getNodes");
        return -1;
    }

    // Check if we got any nodes back
    if (nodeVector.empty()) {
        LOG_INFO_MSG("Found no nodes for node board " << nodeBoard);
        return 0; // Leave
    }

    LOG_INFO_MSG("Number of nodes returned: " << nodeVector.size());

    // Print the node info
    for (vector<Node::ConstPtr>::iterator nodeVectorIter = nodeVector.begin(); nodeVectorIter != nodeVector.end(); nodeVectorIter++) {
        printNodeInfo(*(nodeVectorIter));
    }

    return 0;
}

void printNodeInfo(const Node::ConstPtr nodePtr)
{
    LOG_INFO_MSG(" Node location  . . . . . . . . . : " << nodePtr->getLocation());
    LOG_INFO_MSG("    Hardware state  . . . . . . . : " << hardwareStateToString(nodePtr->getState()));
    LOG_INFO_MSG("    Sequence ID . . . . . . . . . : " << nodePtr->getSequenceId());
}

int iterateAllNodes()
{
    try {
        // Get the compute hardware
        ComputeHardware::ConstPtr bgq = core::getComputeHardware();
        LOG_INFO_MSG("Blue Gene system configuration: " <<  bgq->getMachineRows() << " row(s)  X  " << bgq->getMachineColumns() << " column(s)");
        LOG_INFO_MSG("Start iterating on all nodes");
        for (uint32_t a = 0; a < bgq->getMachineSize(Dimension::A); ++a) {
            for (uint32_t b = 0; b < bgq->getMachineSize(Dimension::B); ++b) {
                for (uint32_t c = 0; c < bgq->getMachineSize(Dimension::C); ++c) {
                    for (uint32_t d = 0; d < bgq->getMachineSize(Dimension::D); ++d) {
                        Coordinates coords(a, b, c, d);
                        Midplane::ConstPtr midplane = bgq->getMidplane(coords);
                        for (uint32_t i = 0; i <  Midplane::MaxNodeBoards; i++) {
                            NodeBoard::ConstPtr nodeBoard = midplane->getNodeBoard(i);
                            vector<Node::ConstPtr> nodeVector; // Vector of nodes returned by core::getNodes
                            try {
                                nodeVector = core::getNodes(nodeBoard->getLocation());
                            } catch (...) { // Handle all exceptions
                                LOG_ERROR_MSG("Unexpected error calling core::getNodes()");
                                return -1;
                            }

                            // Check if we got any nodes back
                            if (nodeVector.empty()) {
                                LOG_INFO_MSG("Found no nodes for node board " << nodeBoard->getLocation());
                            }
                            //LOG_INFO_MSG("Number of nodes returned for node board " << nodeBoard->getLocation() << ": " << nodeVector.size());
                        }
                    }
                }
            }
        }
        LOG_INFO_MSG("Stop iterating on all nodes");
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling core::getComputeHardware");
        return -1;
    }

    return 0;
}

int iterateAllMidplaneNodes()
{
    Midplane::ConstPtr midplane;

    try {
        // Get the compute hardware
        ComputeHardware::ConstPtr bgq = core::getComputeHardware();
        LOG_INFO_MSG("Blue Gene system configuration: " <<  bgq->getMachineRows() << " row(s)  X  " << bgq->getMachineColumns() << " column(s)");
        LOG_INFO_MSG("Start iterating on all midplane nodes");
        for (uint32_t a = 0; a < bgq->getMachineSize(Dimension::A); ++a) {
            for (uint32_t b = 0; b < bgq->getMachineSize(Dimension::B); ++b) {
                for (uint32_t c = 0; c < bgq->getMachineSize(Dimension::C); ++c) {
                    for (uint32_t d = 0; d < bgq->getMachineSize(Dimension::D); ++d) {
                        Coordinates coords(a, b, c, d);
                        midplane = bgq->getMidplane(coords);
                        vector<Node::ConstPtr> nodeVector; // Vector of nodes returned by core::getMidplaneNodes
                        try {
                            nodeVector = core::getMidplaneNodes(midplane->getLocation());
                        } catch (...) { // Handle all exceptions
                            LOG_ERROR_MSG("Unexpected error calling core::getMidplaneNodes()");
                            return -1;
                        }

                        // Check if we got any nodes back
                        if (nodeVector.empty()) {
                            LOG_INFO_MSG("Found no nodes for midplane " << midplane->getLocation());
                        }
                        //LOG_INFO_MSG("Number of nodes returned for midplane " << midplane->getLocation() << ": " << nodeVector.size());
                    }
                }
            }
        }
        LOG_INFO_MSG("Stop iterating on all midplane nodes");
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling core::getComputeHardware");
        return -1;
    }

    return 0;
}

const string midplaneInUseToString(EnumWrapper<Midplane::InUse> inUse)
{
    return MIDPLANE_INUSE_STRINGS[inUse.toValue()];
}

const string switchInUseToString(EnumWrapper<Switch::InUse> inUse)
{
    return SWITCH_INUSE_STRINGS[inUse.toValue()];
}

const string portsSettingToString(EnumWrapper<SwitchSettings::PortsSetting> portsSetting)
{
    return PORTS_SETTING_STRINGS[portsSetting.toValue()];
}

const string hardwareStateToString(EnumWrapper<Hardware::State> hardwareState)
{
    return HARDWARE_STATE_STRINGS[hardwareState.toValue()];
}

const string nodeBoardQuadrantToString(EnumWrapper<NodeBoard::Quadrant> quadrant)
{
    return NODEBOARD_QUADRANT_STRINGS[quadrant.toValue()];
}

static void printNodeMidplaneCoordinates( const std::string& nodeLocation )
{
    // Get the size of the system in each dimension
    Coordinates nodeCoordinates = core::getNodeMidplaneCoordinates( nodeLocation );
    LOG_INFO_MSG("Node coordinates - A:" << nodeCoordinates[Dimension::A]
                                << " B:" << nodeCoordinates[Dimension::B]
                                << " C:" << nodeCoordinates[Dimension::C]
                                << " D:" << nodeCoordinates[Dimension::D]
                                << " E:" << nodeCoordinates[Dimension::E]);
}


int main(int argc, char *argv[])
{
    //time_t mytime = time(0);
    //printf("Time: %s", ctime(&mytime));

    char* argKey;
    char* argVal;

    JobFilter jobFilter;
    BlockFilter blockFilter;
    IOBlockFilter IOblockFilter;
    core::JobSort jobSort;
    core::BlockSort blockSort;
    core::IOBlockSort IOblockSort;

    bool isListJobs = false;
    bool isListHardware = false;
    bool isListHardwareBrief = false;
    bool isListIOHardware = false;
    bool isListIOHardwareBrief = false;
    bool isListHardwareWiring = false;
    bool isListMachineSize = false;
    bool isListIOUsageLimit = false;
    bool isListBlocks = false;
    bool isListBlocksBrief = false;
    bool isListIOBlocks = false;
    bool isListIOBlocksBrief = false;
    bool isListMidplaneNodeBoards = false;
    bool isListNodeBoardNodes = false;
    bool isListMidplaneNodes = false;
    bool isListConnectedComputeBlocks = false;
    bool isListIOLinks = false;
    bool isListAvailableIOLinks = false;
    bool isListBlockIOLinks = false;
    bool isListBlockAvailableIOLinks = false;
    bool isListBlockIOLinksBrief = false;
    bool ischeckBlockIO = false;
    bool ischeckBlockIOConnected = false;

    Job::Id jobFilterId = 0;
    JobFilter::Statuses jobStatuses;
    int jobFilterExitStatus = 0;
    bgsched::TimeInterval startTimeInterval;
    bgsched::TimeInterval endTimeInterval;
    BlockFilter::Statuses blockStatuses;
    IOBlockFilter::Statuses IOBlockStatuses;
    string midplane;
    string IOLinksMidplane;
    string IOLinksAvailableMidplane;
    string IOLinksBlock;
    string IOLinksAvailableBlock;
    string IOLinksBlockBrief;
    string checkIOBlock;
    string IOConnectedBlock;
    string IOBlock;
    string nodeBoard;
    string nodesMidplane;
    string nodeLocation;

    int rc = 0;
    // Print help text if no arguments passed
    if (argc == 1) {
        printHelp();
        return 0;
    }

    // iterate through args first to get -properties and -verbose
    string properties;

    // Default logging to Info
    uint32_t verboseLevel = 5;
    for (int i = 1; i < argc; ++i) {
        if (strcasecmp(argv[i], "-properties")  == 0 || strcasecmp(argv[i], "--properties")  == 0) {
            if (++i == argc) {
                printf("properties keyword specified without an argument value\n");
                exit(EXIT_FAILURE);
            } else {
                properties = argv[i];
            }
        } else if (strcasecmp(argv[i], "-verbose")  == 0 || strcasecmp(argv[i], "--verbose") == 0 || strcasecmp(argv[i], "-v")  == 0) {
            if (++i == argc) {
                printf("verbose keyword specified without an argument value\n");
                exit(EXIT_FAILURE);
            } else {
                try {
                    verboseLevel = boost::lexical_cast<uint32_t>(argv[i]);
                } catch (const boost::bad_lexical_cast& e) {
                    printf("verbose keyword specified with a bad value\n");
                    exit(EXIT_FAILURE);
                }
            }
        } else if (strcasecmp(argv[i], "-help")  == 0 || strcasecmp(argv[i], "--help")  == 0 || strcasecmp(argv[i], "-h")  == 0) {
            printHelp();
            exit(EXIT_SUCCESS);
        }
    }

    // Initialize logger and database
    bgsched::init(properties);

    log4cxx::LoggerPtr logger(Logger::getLogger("ibm.bgsched"));

    // 1=OFF,2=FATAL,3=ERROR,4=WARN,5=INFO,6=DEBUG,7=TRACE
    switch (verboseLevel) {
      case 1:
        logger->setLevel(log4cxx::Level::getOff());
        break;
      case 2:
        logger->setLevel(log4cxx::Level::getFatal());
        break;
      case 3:
        logger->setLevel(log4cxx::Level::getError());
        break;
      case 4:
        logger->setLevel(log4cxx::Level::getWarn());
        break;
      case 5:
        logger->setLevel(log4cxx::Level::getInfo());
        break;
      case 6:
        logger->setLevel(log4cxx::Level::getDebug());
        break;
      case 7:
      case 0: // Conversion error on input - use default
      default:
        logger->setLevel(log4cxx::Level::getTrace());
        break;
    }

    int argNbr = 1;
    while (argNbr < argc) {
        argKey = argv[argNbr];
        argNbr++;
        if (argNbr < argc) {
            argVal = argv[argNbr];
        } else {
            argVal = NULL;
        }

        if (strcasecmp(argKey, "-listjobs") == 0 || strcasecmp(argKey, "--listjobs") == 0) {
            isListJobs = true;
        } else if (strcasecmp(argKey, "-listhardwarebrief") == 0 || strcasecmp(argKey, "--listhardwarebrief") == 0) {
            isListHardwareBrief = true;
        } else if (strcasecmp(argKey, "-listhardware") == 0  || strcasecmp(argKey, "--listhardware") == 0) {
            isListHardware = true;
        } else if (strcasecmp(argKey, "-listIOhardwarebrief") == 0 || strcasecmp(argKey, "--listIOhardwarebrief") == 0) {
            isListIOHardwareBrief = true;
        } else if (strcasecmp(argKey, "-listIOhardware") == 0  || strcasecmp(argKey, "--listIOhardware") == 0) {
            isListIOHardware = true;
        } else if (strcasecmp(argKey, "-listhardwarewiring") == 0 || strcasecmp(argKey, "--listhardwarewiring") == 0) {
            isListHardwareWiring = true;
        } else if (strcasecmp(argKey, "-listmachinesize") == 0 || strcasecmp(argKey, "--listmachinesize") == 0) {
            isListMachineSize = true;
        } else if (strcasecmp(argKey, "-listIOusagelimit") == 0 || strcasecmp(argKey, "--listIOusagelimit") == 0) {
            isListIOUsageLimit = true;
        } else if (strcasecmp(argKey, "-listblocksbrief") == 0 || strcasecmp(argKey, "--listblocksbrief") == 0) {
            isListBlocksBrief = true;
        } else if (strcasecmp(argKey, "-listblocks") == 0 || strcasecmp(argKey, "--listblocks") == 0) {
            isListBlocks = true;
        } else if (strcasecmp(argKey, "-listIOblocksbrief") == 0 || strcasecmp(argKey, "--listIOblocksbrief") == 0) {
            isListIOBlocksBrief = true;
        } else if (strcasecmp(argKey, "-listIOblocks") == 0 || strcasecmp(argKey, "--listIOblocks") == 0) {
            isListIOBlocks = true;
        } else if (strcasecmp(argKey, "-listmidplanenodeboards") == 0 || strcasecmp(argKey, "--listmidplanenodeboards") == 0) {
            isListMidplaneNodeBoards = true;
            if (argVal != NULL) {
                midplane.assign(argVal);
                argNbr++;
            } else {
                printf("listmidplanenodeboards keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-listnodeboardnodes") == 0) {
            isListNodeBoardNodes = true;
            if (argVal != NULL) {
                nodeBoard.assign(argVal);
                argNbr++;
            } else {
                printf("listnodeboardnodes keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-listmidplanenodes") == 0) {
            isListMidplaneNodes = true;
            if (argVal != NULL) {
                nodesMidplane.assign(argVal);
                argNbr++;
            } else {
                printf("listmidplanenodes keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-listIOlinks") == 0 || strcasecmp(argKey, "--listIOlinks") == 0) {
            isListIOLinks = true;
            if (argVal != NULL) {
                if (strcasecmp(argVal, "*ALL") == 0) {
                    IOLinksMidplane.clear(); // Return I/O links on all midplanes
                } else {
                    IOLinksMidplane.assign(argVal);
                }
                argNbr++;
            } else {
                printf("listIOlinks keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-listAvailableIOlinks") == 0 || strcasecmp(argKey, "--listAvailableIOlinks") == 0) {
            isListAvailableIOLinks = true;
            if (argVal != NULL) {
                if (strcasecmp(argVal, "*ALL") == 0) {
                    IOLinksAvailableMidplane.clear(); // Return I/O links on all midplanes
                } else {
                    IOLinksAvailableMidplane.assign(argVal);
                }
                argNbr++;
            } else {
                printf("listAvailableIOlinks keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-listconnectedcomputeblocks") == 0 || strcasecmp(argKey, "--listconnectedcomputeblocks") == 0) {
            isListConnectedComputeBlocks = true;
            if (argVal != NULL) {
                IOBlock.assign(argVal);
                argNbr++;
            } else {
                printf("listconnectedcomputeblocks keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-listblockIOlinks") == 0 || strcasecmp(argKey, "--listblockIOlinks") == 0) {
            isListBlockIOLinks = true;
            if (argVal != NULL) {
                IOLinksBlock.assign(argVal);
                argNbr++;
            } else {
                printf("listblockIOlinks keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-listblockAvailableIOlinks") == 0 || strcasecmp(argKey, "--listblockAvailableIOlinks") == 0) {
            isListBlockAvailableIOLinks = true;
            if (argVal != NULL) {
                IOLinksAvailableBlock.assign(argVal);
                argNbr++;
            } else {
                printf("listblockAvailableIOlinks keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-listblockIOlinksbrief") == 0 || strcasecmp(argKey, "--listblockIOlinksbrief") == 0) {
            isListBlockIOLinksBrief = true;
            if (argVal != NULL) {
                IOLinksBlockBrief.assign(argVal);
                argNbr++;
            } else {
                printf("listblockIOlinks keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-checkblockIO") == 0 || strcasecmp(argKey, "--checkblockIO") == 0) {
            ischeckBlockIO = true;
            if (argVal != NULL) {
                checkIOBlock.assign(argVal);
                argNbr++;
            } else {
                printf("checkblockIO keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-checkblockIOconnected") == 0 || strcasecmp(argKey, "--checkblockIOconnected") == 0) {
            ischeckBlockIOConnected = true;
            if (argVal != NULL) {
                IOConnectedBlock.assign(argVal);
                argNbr++;
            } else {
                printf("checkblockIOconnected keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-blocksort") == 0 || strcasecmp(argKey, "--blocksort") == 0) {
            if (argVal != NULL) {
                while (argVal != NULL) {
                    if (strcasecmp(argVal, "NAME") == 0) {
                        LOG_INFO_MSG("Sorting blocks by NAME");
                        blockSort.setSort(core::BlockSort::Field::Name, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "OWNER") == 0) {
                        LOG_INFO_MSG("Sorting blocks by OWNER");
                        blockSort.setSort(core::BlockSort::Field::Owner, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "USER") == 0) {
                        LOG_INFO_MSG("Sorting blocks by USER");
                        blockSort.setSort(core::BlockSort::Field::User, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "STATUS") == 0) {
                        LOG_INFO_MSG("Sorting blocks by STATUS");
                        blockSort.setSort(core::BlockSort::Field::Status, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "COMPUTENODECOUNT") == 0) {
                        LOG_INFO_MSG("Sorting blocks by COMPUTENODECOUNT");
                        blockSort.setSort(core::BlockSort::Field::ComputeNodeCount, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "CREATEDATE") == 0) {
                        LOG_INFO_MSG("Sorting blocks by CREATEDATE");
                        blockSort.setSort(core::BlockSort::Field::CreateDate, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "STATUSLASTMODIFIED") == 0) {
                        LOG_INFO_MSG("Sorting blocks by STATUSLASTMODIFIED");
                        blockSort.setSort(core::BlockSort::Field::StatusLastModified, core::SortOrder::Ascending);
                    } else {
                        printf("blocksort argument value, %s, is not valid\n", argVal);
                        printf("Valid values are: NAME OWNER USER STATUS COMPUTENODECOUNT CREATEDATE STATUSLASTMODIFIED\n");
                        return -1;
                    }
                    argNbr++;
                    argVal = NULL;
                    if (argNbr < argc && argv[argNbr][0] != '-') {
                        argVal = argv[argNbr];
                    }
                }
            } else {
                printf("blocksort keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-blockfilterstatus") == 0 || strcasecmp(argKey, "--blockfilterstatus") == 0) {
            if (argVal != NULL) {
                while (argVal != NULL) {
                    if (strcasecmp(argVal, "FREE") == 0) {
                        LOG_INFO_MSG("Filtering on blocks in FREE state");
                        blockStatuses.insert(Block::Free);
                    } else if (strcasecmp(argVal, "BOOTING") == 0) {
                        LOG_INFO_MSG("Filtering on blocks in BOOTING state");
                        blockStatuses.insert(Block::Booting);
                    } else if (strcasecmp(argVal, "ALLOCATED") == 0) {
                        LOG_INFO_MSG("Filtering on blocks in ALLOCATED state");
                        blockStatuses.insert(Block::Allocated);
                    } else if (strcasecmp(argVal, "INITIALIZED") == 0) {
                        LOG_INFO_MSG("Filtering on blocks in INITIALIZED state");
                        blockStatuses.insert(Block::Initialized);
                    } else if (strcasecmp(argVal, "TERMINATING") == 0) {
                        LOG_INFO_MSG("Filtering on blocks in TERMINATING state");
                        blockStatuses.insert(Block::Terminating);
                    } else {
                        printf("blockfilterstatus argument value, %s, is not valid\n", argVal);
                        printf("Valid values are: FREE BOOTING ALLOCATED INITIALIZED TERMINATING\n");
                        return -1;
                    }
                    argNbr++;
                    argVal = NULL;
                    if (argNbr < argc && argv[argNbr][0] != '-') {
                        argVal = argv[argNbr];
                    }
                }
                blockFilter.setStatuses(&blockStatuses);
            } else {
                printf("blockfilterstatus keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-blockfiltername") == 0 || strcasecmp(argKey, "--blockfiltername") == 0) {
            if (argVal != NULL) {
                LOG_INFO_MSG("Filtering on blocks named " << argVal);
                blockFilter.setName(string(argVal));
                argNbr++;
            } else {
                printf("blockfiltername keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-blockfilterowner") == 0 || strcasecmp(argKey, "--blockfilterowner") == 0) {
            if (argVal != NULL) {
                LOG_INFO_MSG("Filtering on blocks owned by " << argVal);
                blockFilter.setOwner(string(argVal));
                argNbr++;
            } else {
                printf("blockfilterowner keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-blockfilteruser") == 0 || strcasecmp(argKey, "--blockfilteruser") == 0) {
            if (argVal != NULL) {
                LOG_INFO_MSG("Filtering on blocks booted by " << argVal);
                blockFilter.setUser(string(argVal));
                argNbr++;
            } else {
                printf("blockfilteruser keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-blockfilterincludejobs") == 0 || strcasecmp(argKey, "--blockfilterincludejobs") == 0) {
            if (argVal != NULL) {
                while (argVal != NULL) {
                    if (strcasecmp(argVal, "Y") == 0) {
                        blockFilter.setIncludeJobs(true);
                    } else if (strcasecmp(argVal, "N") == 0) {
                        blockFilter.setIncludeJobs(false);
                    } else {
                        printf("blockfilterincludejobs argument value, %s, is not valid\n", argVal);
                        printf("Valid values are: Y or N\n");
                        return -1;
                    }
                    argNbr++;
                    argVal = NULL;
                    if (argNbr < argc && argv[argNbr][0] != '-') {
                        argVal = argv[argNbr];
                    }
                }
            } else {
                printf("blockfilterincludejobs keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-blockfilterextendedinfo") == 0 || strcasecmp(argKey, "--blockfilterextendedinfo") == 0) {
            if (argVal != NULL) {
                while (argVal != NULL) {
                    if (strcasecmp(argVal, "Y") == 0) {
                        blockFilter.setExtendedInfo(true);
                    } else if (strcasecmp(argVal, "N") == 0)  {
                        blockFilter.setExtendedInfo(false);
                    } else {
                        printf("blockfilterextendedinfo argument value, %s, is not valid\n", argVal);
                        printf("Valid values are: Y or N\n");
                        return -1;
                    }
                    argNbr++;
                    argVal = NULL;
                    if (argNbr < argc && argv[argNbr][0] != '-') {
                        argVal = argv[argNbr];
                    }
                }
            } else {
                printf("blockfilterextendedinfo keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-blockfiltersize") == 0 || strcasecmp(argKey, "--blockfiltersize") == 0) {
            if (argVal != NULL) {
                while (argVal != NULL) {
                    if (strcasecmp(argVal, "LARGE") == 0) {
                        blockFilter.setSize(BlockFilter::BlockSize::Large);
                    } else if (strcasecmp(argVal, "SMALL") == 0)  {
                        blockFilter.setSize(BlockFilter::BlockSize::Small);
                    } else if (strcasecmp(argVal, "ALL") == 0)  {
                        blockFilter.setSize(BlockFilter::BlockSize::All);
                    } else {
                        printf("blockfiltersize argument value, %s, is not valid\n", argVal);
                        printf("Valid values are: LARGE, SMALL or ALL\n");
                        return -1;
                    }
                    argNbr++;
                    argVal = NULL;
                    if (argNbr < argc && argv[argNbr][0] != '-') {
                        argVal = argv[argNbr];
                    }
                }
            } else {
                printf("blockfiltersize keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-IOblocksort") == 0 || strcasecmp(argKey, "--IOblocksort") == 0) {
            if (argVal != NULL) {
                while (argVal != NULL) {
                    if (strcasecmp(argVal, "NAME") == 0) {
                        LOG_INFO_MSG("Sorting I/O blocks by NAME");
                        IOblockSort.setSort(core::IOBlockSort::Field::Name, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "OWNER") == 0) {
                        LOG_INFO_MSG("Sorting I/O blocks by OWNER");
                        IOblockSort.setSort(core::IOBlockSort::Field::Owner, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "USER") == 0) {
                        LOG_INFO_MSG("Sorting I/O blocks by USER");
                        IOblockSort.setSort(core::IOBlockSort::Field::User, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "STATUS") == 0) {
                        LOG_INFO_MSG("Sorting I/O blocks by STATUS");
                        IOblockSort.setSort(core::IOBlockSort::Field::Status, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "IONODECOUNT") == 0) {
                        LOG_INFO_MSG("Sorting I/O blocks by IONODECOUNT");
                        IOblockSort.setSort(core::IOBlockSort::Field::IONodeCount, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "CREATEDATE") == 0) {
                        LOG_INFO_MSG("Sorting I/O blocks by CREATEDATE");
                        IOblockSort.setSort(core::IOBlockSort::Field::CreateDate, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "STATUSLASTMODIFIED") == 0) {
                        LOG_INFO_MSG("Sorting I/O blocks by STATUSLASTMODIFIED");
                        IOblockSort.setSort(core::IOBlockSort::Field::StatusLastModified, core::SortOrder::Ascending);
                    } else {
                        printf("IOblocksort argument value, %s, is not valid\n", argVal);
                        printf("Valid values are: NAME OWNER USER STATUS IONODECOUNT CREATEDATE STATUSLASTMODIFIED\n");
                        return -1;
                    }
                    argNbr++;
                    argVal = NULL;
                    if (argNbr < argc && argv[argNbr][0] != '-') {
                        argVal = argv[argNbr];
                    }
                }
            } else {
                printf("IOblocksort keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-IOblockfilterstatus") == 0 || strcasecmp(argKey, "--IOblockfilterstatus") == 0) {
            if (argVal != NULL) {
                while (argVal != NULL) {
                    if (strcasecmp(argVal, "FREE") == 0) {
                        LOG_INFO_MSG("Filtering on I/O blocks in FREE state");
                        IOBlockStatuses.insert(IOBlock::Free);
                    } else if (strcasecmp(argVal, "BOOTING") == 0) {
                        LOG_INFO_MSG("Filtering on I/O blocks in BOOTING state");
                        IOBlockStatuses.insert(IOBlock::Booting);
                    } else if (strcasecmp(argVal, "ALLOCATED") == 0) {
                        LOG_INFO_MSG("Filtering on I/O blocks in ALLOCATED state");
                        IOBlockStatuses.insert(IOBlock::Allocated);
                    } else if (strcasecmp(argVal, "INITIALIZED") == 0) {
                        LOG_INFO_MSG("Filtering on I/O blocks in INITIALIZED state");
                        IOBlockStatuses.insert(IOBlock::Initialized);
                    } else if (strcasecmp(argVal, "TERMINATING") == 0) {
                        LOG_INFO_MSG("Filtering on I/O blocks in TERMINATING state");
                        IOBlockStatuses.insert(IOBlock::Terminating);
                    } else {
                        printf("IOblockfilterstatus argument value, %s, is not valid\n", argVal);
                        printf("Valid values are: FREE BOOTING ALLOCATED INITIALIZED TERMINATING\n");
                        return -1;
                    }
                    argNbr++;
                    argVal = NULL;
                    if (argNbr < argc && argv[argNbr][0] != '-') {
                        argVal = argv[argNbr];
                    }
                }
                IOblockFilter.setStatuses(&IOBlockStatuses);
            } else {
                printf("IOblockfilterstatus keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-IOblockfiltername") == 0 || strcasecmp(argKey, "--IOblockfiltername") == 0) {
            if (argVal != NULL) {
                LOG_INFO_MSG("Filtering on I/O blocks named " << argVal);
                IOblockFilter.setName(string(argVal));
                argNbr++;
            } else {
                printf("IOblockfiltername keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-IOblockfilterowner") == 0 || strcasecmp(argKey, "--IOblockfilterowner") == 0) {
            if (argVal != NULL) {
                LOG_INFO_MSG("Filtering on I/O blocks owned by " << argVal);
                IOblockFilter.setOwner(string(argVal));
                argNbr++;
            } else {
                printf("IOblockfilterowner keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-IOblockfilteruser") == 0 || strcasecmp(argKey, "--IOblockfilteruser") == 0) {
            if (argVal != NULL) {
                LOG_INFO_MSG("Filtering on I/O blocks booted by " << argVal);
                IOblockFilter.setUser(string(argVal));
                argNbr++;
            } else {
                printf("IOblockfilteruser keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-IOblockfilterextendedinfo") == 0 || strcasecmp(argKey, "--IOblockfilterextendedinfo") == 0) {
            if (argVal != NULL) {
                while (argVal != NULL) {
                    if (strcasecmp(argVal, "Y") == 0) {
                        IOblockFilter.setExtendedInfo(true);
                    } else if (strcasecmp(argVal, "N") == 0)  {
                        IOblockFilter.setExtendedInfo(false);
                    } else {
                        printf("IOblockfilterextendedinfo argument value, %s, is not valid\n", argVal);
                        printf("Valid values are: Y or N\n");
                        return -1;
                    }
                    argNbr++;
                    argVal = NULL;
                    if (argNbr < argc && argv[argNbr][0] != '-') {
                        argVal = argv[argNbr];
                    }
                }
            } else {
                printf("IOblockfilterextendedinfo keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-jobsort") == 0 || strcasecmp(argKey, "--jobsort") == 0) {
            if (argVal != NULL) {
                while (argVal != NULL) {
                    if (strcasecmp(argVal, "ID") == 0) {
                        LOG_INFO_MSG("Sorting jobs by ID");
                        jobSort.setSort(core::JobSort::Field::Id, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "USER") == 0) {
                        LOG_INFO_MSG("Sorting jobs by USER");
                        jobSort.setSort(core::JobSort::Field::User, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "BLOCK") == 0) {
                        LOG_INFO_MSG("Sorting jobs by BLOCK");
                        jobSort.setSort(core::JobSort::Field::Block, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "STATUS") == 0) {
                        LOG_INFO_MSG("Sorting jobs by STATUS");
                        jobSort.setSort(core::JobSort::Field::Status, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "COMPUTENODESUSED") == 0) {
                        LOG_INFO_MSG("Sorting jobs by COMPUTENODESUSED");
                        jobSort.setSort(core::JobSort::Field::ComputeNodesUsed, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "STARTTIME") == 0) {
                        LOG_INFO_MSG("Sorting jobs by STARTTIME");
                        jobSort.setSort(core::JobSort::Field::StartTime, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "ENDTIME") == 0) {
                        LOG_INFO_MSG("Sorting jobs by ENDTIME");
                        jobSort.setSort(core::JobSort::Field::EndTime, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "EXECUTABLE") == 0) {
                        LOG_INFO_MSG("Sorting jobs by EXECUTABLE");
                        jobSort.setSort(core::JobSort::Field::Executable, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "EXITSTATUS") == 0) {
                        LOG_INFO_MSG("Sorting jobs by EXITSTATUS");
                        jobSort.setSort(core::JobSort::Field::ExitStatus, core::SortOrder::Ascending);
                    } else if (strcasecmp(argVal, "RANKSPERNODE") == 0) {
                        LOG_INFO_MSG("Sorting jobs by RANKSPERNODE");
                        jobSort.setSort(core::JobSort::Field::RanksPerNode, core::SortOrder::Ascending);
                    } else {
                        printf("jobSort argument value, %s, is not valid\n", argVal);
                        printf("Valid values are: ID USER BLOCK STARTTIME ENDTIME EXITSTATUS EXECUTABLE STATUS COMPUTENODESUSED RANKSPERNODE\n");
                        return -1;
                    }
                    argNbr++;
                    argVal = NULL;
                    if (argNbr < argc && argv[argNbr][0] != '-') {
                        argVal = argv[argNbr];
                    }
                }
            } else {
                printf("jobSort keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-jobfiltertype") == 0 || strcasecmp(argKey, "--jobfiltertype") == 0) {
            if (argVal != NULL) {
                while (argVal != NULL) {
                    if (strcasecmp(argVal, "ACTIVE") == 0) {
                        LOG_INFO_MSG("Filtering on ACTIVE jobs");
                        jobFilter.setJobType(JobFilter::JobType::Active);
                    } else if (strcasecmp(argVal, "COMPLETED") == 0) {
                        LOG_INFO_MSG("Filtering on COMPLETED jobs");
                        jobFilter.setJobType(JobFilter::JobType::Completed);
                    } else if (strcasecmp(argVal, "ALL") == 0) {
                        LOG_INFO_MSG("Filtering on ALL jobs (Active and Completed)");
                        jobFilter.setJobType(JobFilter::JobType::All);
                    } else {
                        printf("jobfiltertype argument value, %s, is not valid\n", argVal);
                        printf("Valid values are: ACTIVE COMPLETED ALL\n");
                        return -1;
                    }
                    argNbr++;
                    argVal = NULL;
                    if (argNbr < argc && argv[argNbr][0] != '-') {
                        argVal = argv[argNbr];
                    }
                }
            } else {
                printf("jobfiltertype keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-jobfilterstatus") == 0 || strcasecmp(argKey, "--jobfilterstatus") == 0) {
            if (argVal != NULL) {
                while (argVal != NULL) {
                    if (strcasecmp(argVal, "SETUP") == 0) {
                        LOG_INFO_MSG("Filtering on jobs with SETUP status");
                        jobStatuses.insert(Job::Setup);
                    } else if (strcasecmp(argVal, "LOADING") == 0) {
                        LOG_INFO_MSG("Filtering on jobs with LOADING status");
                        jobStatuses.insert(Job::Loading);
                    } else if (strcasecmp(argVal, "STARTING") == 0) {
                        LOG_INFO_MSG("Filtering on jobs with STARTING status");
                        jobStatuses.insert(Job::Starting);
                    } else if (strcasecmp(argVal, "RUNNING") == 0) {
                        LOG_INFO_MSG("Filtering on jobs with RUNNING status");
                        jobStatuses.insert(Job::Running);
                    } else if (strcasecmp(argVal, "DEBUG") == 0) {
                        LOG_INFO_MSG("Filtering on jobs with DEBUG status");
                        jobStatuses.insert(Job::Debug);
                    } else if (strcasecmp(argVal, "CLEANUP") == 0) {
                        LOG_INFO_MSG("Filtering on jobs with CLEANUP status");
                        jobStatuses.insert(Job::Cleanup);
                    } else if (strcasecmp(argVal, "TERMINATED") == 0) {
                        LOG_INFO_MSG("Filtering on jobs with TERMINATED status");
                        jobStatuses.insert(Job::Terminated);
                    } else if (strcasecmp(argVal, "ERROR") == 0) {
                        LOG_INFO_MSG("Filtering on jobs with ERROR status");
                        jobStatuses.insert(Job::Error);
                    } else {
                        printf("jobfilterstatus argument value, %s, is not valid\n", argVal);
                        printf("Valid values are: SETUP LOADING STARTING RUNNING DEBUG CLEANUP TERMINATED ERROR\n");
                        return -1;
                    }
                    argNbr++;
                    argVal = NULL;
                    if (argNbr < argc && argv[argNbr][0] != '-') {
                        argVal = argv[argNbr];
                    }
                }
                jobFilter.setStatuses(&jobStatuses);
            } else {
                printf("jobfilterstatus keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-jobfilterid") == 0 || strcasecmp(argKey, "--jobfilterid") == 0) {
            if (argVal != NULL) {
                try {
                    jobFilterId = boost::lexical_cast<Job::Id>(argVal);
                } catch (const boost::bad_lexical_cast& e) {
                    printf("jobfilterid specified with a bad value\n");
                    return -1;
                }
                LOG_INFO_MSG("Filtering on jobs with job ID " << jobFilterId);
                jobFilter.setJobId(jobFilterId);
                argNbr++;
            } else {
                printf("jobfilterid keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-jobfilterschedulerdata") == 0 || strcasecmp(argKey, "--jobfilterschedulerdata") == 0) {
            if (argVal != NULL) {
                LOG_INFO_MSG("Filtering on jobs with scheduler data " << string(argVal));
                jobFilter.setSchedulerData(string(argVal));
                argNbr++;
            } else {
                printf("jobfilterschedulerdata keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-jobfiltercomputeblockname") == 0 || strcasecmp(argKey, "--jobfiltercomputeblockname") == 0) {
            if (argVal != NULL) {
                LOG_INFO_MSG("Filtering on jobs running under compute block " << string(argVal));
                jobFilter.setComputeBlockName(string(argVal));
                argNbr++;
            } else {
                printf("jobfiltercomputeblockname keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-jobfilteruser") == 0 || strcasecmp(argKey, "--jobfilteruser") == 0) {
            if (argVal != NULL) {
                LOG_INFO_MSG("Filtering on jobs running under user " << string(argVal));
                jobFilter.setUser(string(argVal));
                argNbr++;
            } else {
                printf("jobfilteruser keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-jobfilterexecutable") == 0 || strcasecmp(argKey, "--jobfilterexecutable") == 0) {
            if (argVal != NULL) {
                LOG_INFO_MSG("Filtering on jobs running executable named " << string(argVal));
                jobFilter.setExecutable(string(argVal));
                argNbr++;
            } else {
                printf("jobfilterexecutable keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-jobfilterexitstatus") == 0 || strcasecmp(argKey, "--jobfilterexitstatus") == 0) {
            if (argVal != NULL) {
                try {
                    jobFilterExitStatus = boost::lexical_cast<int>(argVal);
                } catch (const boost::bad_lexical_cast& e) {
                    LOG_ERROR_MSG("jobfilterexitstatus keyword specified with a bad value\n");
                    return -1;
                }
                LOG_INFO_MSG("Filtering on history jobs with exit status " << jobFilterExitStatus);
                jobFilter.setExitStatus(jobFilterExitStatus);
                argNbr++;
            } else {
                printf("jobfilterexitstatus keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-jobfilterstarttime") == 0 || strcasecmp(argKey, "--jobfilterstarttime") == 0) {
            if (argVal != NULL) {
                bool isStartTimeHandled = false;
                bool isEndTimeHandled = false;
                string starttime, endtime;
                while (argVal != NULL) {
                    if (!isStartTimeHandled) {
                        isStartTimeHandled = true;
                        starttime = string(argVal);
                    } else {
                        if (!isEndTimeHandled) {
                            isEndTimeHandled = true;
                            endtime = string(argVal);
                        } else {
                            LOG_ERROR_MSG("jobfilterstarttime keyword specified with a bad argument\n");
                            return -1;
                        }
                    }
                    argNbr++;
                    argVal = NULL;
                    if (argNbr < argc && argv[argNbr][0] != '-') {
                        argVal = argv[argNbr];
                    }
                }

                if (isStartTimeHandled && isEndTimeHandled) {
                    try {
                        startTimeInterval = TimeInterval(starttime, endtime);
                        jobFilter.setStartTimeInterval(startTimeInterval);
                        LOG_INFO_MSG("Filtering on jobs starting between " << boost::posix_time::to_simple_string(startTimeInterval.getStart()) <<
                                " and " << boost::posix_time::to_simple_string(startTimeInterval.getEnd()));
                    } catch (...) {
                        printf("jobfilterstarttime keyword specified with a bad value\n");
                        return -1;
                    }
                } else {
                    printf("jobfilterstarttime keyword specified with a bad argument\n");
                    return -1;
                }
            } else {
                printf("jobfilterstarttime keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-jobfilterendtime") == 0 || strcasecmp(argKey, "--jobfilterendtime") == 0) {
            if (argVal != NULL) {
                bool isStartTimeHandled = false;
                bool isEndTimeHandled = false;
                string starttime, endtime;
                while (argVal != NULL) {
                    if (!isStartTimeHandled) {
                        isStartTimeHandled = true;
                        starttime = string(argVal);
                    } else {
                        if (!isEndTimeHandled) {
                            isEndTimeHandled = true;
                            endtime = string(argVal);
                        } else {
                            printf("jobfilterendtime keyword specified with a bad argument\n");
                            return -1;
                        }
                    }
                    argNbr++;
                    argVal = NULL;
                    if (argNbr < argc && argv[argNbr][0] != '-') {
                        argVal = argv[argNbr];
                    }
                }

                if (isStartTimeHandled && isEndTimeHandled) {
                    try {
                        endTimeInterval = TimeInterval(starttime, endtime);
                        jobFilter.setEndTimeInterval(endTimeInterval);
                        LOG_INFO_MSG("Filtering on jobs ending between " << boost::posix_time::to_simple_string(endTimeInterval.getStart()) <<
                                " and " << boost::posix_time::to_simple_string(endTimeInterval.getEnd()));
                    } catch (...) {
                        printf("jobfilterendtime keyword specified with a bad value\n");
                        return -1;
                    }
                } else {
                    printf("jobfilterendtime keyword specified with a bad argument\n");
                    return -1;
                }
            } else {
                printf("jobfilterendtime keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-dumpxml") == 0 || strcasecmp(argKey, "--dumpxml") == 0) {
            LOG_TRACE_MSG("Setting BG_DUMP_XML=true");
            setenv("BG_DUMP_XML", "true", 1);
        } else if (strcasecmp(argKey, "-verbose") == 0 || strcasecmp(argKey, "--verbose") == 0 || strcasecmp(argKey, "-v") == 0) {
            // ignore, we've already parsed this guy
            ++argNbr;
        } else if (strcasecmp(argKey, "-properties") == 0 || strcasecmp(argKey, "--properties") == 0) {
            // ignore, we've already parsed this guy
            ++argNbr;
        } else if (strcasecmp(argKey, "-getNodeMidplaneCoordinates") == 0 || strcasecmp(argKey, "--getNodeMidplaneCoordinates") ==0) {
            nodeLocation = argVal;
            ++argNbr;
        } else {
            printf("parameter %s not recognized\n", argKey);
            printHelp();
            return 0;
        }
    }

    if (isListHardware) {
        rc = listHardware(true);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listHardware");
           return -1;
        }
    }

    if (isListHardwareBrief) {
        rc = listHardware(false);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listHardware");
           return -1;
        }
    }

    if (isListIOHardware) {
        rc = listIOHardware(true);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listIOHardware");
           return -1;
        }
    }

    if (isListIOHardwareBrief) {
        rc = listIOHardware(false);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listIOHardware");
           return -1;
        }
    }

    if (isListHardwareWiring) {
        rc = listHardwareWiring();
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listHardwareWiring");
           return -1;
        }
    }

    if (isListMachineSize) {
        rc = listMachineSize();
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listMachineSize");
           return -1;
        }
    }

    if (isListIOUsageLimit) {
        rc = listIOUsageLimit();
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listIOUsageLimit");
           return -1;
        }
    }

    if (isListBlocks) {
        rc = listBlocks(blockFilter, blockSort, true);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listBlocks");
           return -1;
        }
    }

    if (isListBlocksBrief) {
        rc = listBlocks(blockFilter, blockSort, false);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listBlocks");
           return -1;
        }
    }

    if (isListIOBlocks) {
        rc = listIOBlocks(IOblockFilter, IOblockSort, true);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listIOBlocks");
           return -1;
        }
    }

    if (isListIOBlocksBrief) {
        rc = listIOBlocks(IOblockFilter, IOblockSort, false);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listIOBlocks");
           return -1;
        }
    }

    if (isListJobs) {
        rc = listJobs(jobFilter, jobSort);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listJobs");
           return -1;
        }
    }

    if (isListMidplaneNodeBoards) {
        rc = listNodeBoards(midplane);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listNodeBoards");
           return -1;
        }
    }

    if (isListNodeBoardNodes) {
        //rc = iterateAllNodes();
        rc = listNodes(nodeBoard);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listNodes");
           return -1;
        }
    }

    if (isListMidplaneNodes) {
        //rc = iterateAllMidplaneNodes();
        rc = listMidplaneNodes(nodesMidplane);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listMidplaneNodes");
           return -1;
        }
    }

    if (isListConnectedComputeBlocks) {
        rc = listconnectedcomputeblocks(IOBlock);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listconnectedcomputeblocks");
           return -1;
        }
    }

    if (isListIOLinks) {
        rc = listIOLinks(IOLinksMidplane);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listIOLinks");
           return -1;
        }
    }

    if (isListAvailableIOLinks) {
        rc = listAvailableIOLinks(IOLinksAvailableMidplane);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listAvailableIOLinks");
           return -1;
        }
    }

    if (isListBlockIOLinks) {
        rc = listBlockIOLinks(IOLinksBlock);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listBlockIOLinks");
           return -1;
        }
    }

    if (isListBlockAvailableIOLinks) {
        rc = listBlockAvailableIOLinks(IOLinksAvailableBlock);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listBlockAvailableIOLinks");
           return -1;
        }
    }

    if (isListBlockIOLinksBrief) {
        rc = listBlockIOLinksBrief(IOLinksBlockBrief);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling listBlockIOLinksBrief");
           return -1;
        }
    }

    if (ischeckBlockIO) {
        rc = checkBlockIO(checkIOBlock);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling checkBlockIO");
           return -1;
        }
    }

    if (ischeckBlockIOConnected) {
        rc = checkBlockIOConnected(IOConnectedBlock);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling checkBlockIOConnected");
           return -1;
        }
    }

    if ( ! nodeLocation.empty() ) {
        printNodeMidplaneCoordinates( nodeLocation );
        return 0;
    }

    LOG_DEBUG_MSG("Completed Successfully");
    return 0;
}

void printHelp()
{
    printf("Parameters:\n");
    printf("  -listhardware                       - Display a list of midplanes, node boards, cables and switches\n");
    printf("  -listhardwarebrief                  - Displays brief info on midplanes, cables and switches\n");
    printf("  -listhardwarewiring                 - Displays hardware wiring\n");
    printf("  -listIOhardware                     - Display a list of I/O drawers and I/O nodes\n");
    printf("  -listIOhardwarebrief                - Displays brief info on I/O drawers and I/O nodes\n");
    printf("  -listmachinesize                    - Displays machine size in ABCD dimensions\n");
    printf("  -listIOusagelimit                   - Display the I/O usage limit from bg.properties\n");
    printf("  -listblocks                         - Display a list of compute blocks with full info\n");
    printf("  -listblocksbrief                    - Display a list of compute blocks with brief info\n");
    printf("  -blocksort sort                     - Optional sort order for -listblocks and -listblocksbrief\n");
    printf("                                          Specify one of the following:\n");
    printf("                                          NAME OWNER USER STATUS COMPUTENODECOUNT\n");
    printf("                                          CREATEDATE STATUSLASTMODIFIED\n");
    printf("  -blockfilterstatus statuses         - Optional status filter for -listblocks and -listblocksbrief\n");
    printf("                                          Specify one or more of the following:\n");
    printf("                                          FREE BOOTING ALLOCATED INITIALIZED TERMINATING\n");
    printf("  -blockfiltername block              - Optional block name filter for -listblocks and -listblocksbrief\n");
    printf("  -blockfilterowner owner             - Optional block owner filter for -listblocks and -listblocksbrief\n");
    printf("  -blockfilteruser user               - Optional block user filter for -listblocks and -listblocksbrief\n");
    printf("  -blockfilterincludejobs Y|N         - Optional include jobs filter for -listblocks and -listblocksbrief\n");
    printf("                                          Specify Y or N\n");
    printf("  -blockfilterextendedinfo Y|N        - Optional extended info filter for -listblocks and -listblocksbrief\n");
    printf("                                          Specify Y or N\n");
    printf("  -blockfiltersize LARGE|SMALL|ALL    - Optional block size filter for -listblocks and -listblocksbrief\n");
    printf("                                          Specify LARGE, SMALL or ALL\n");
    printf("  -listIOblocks                       - Display a list of I/O blocks with full info\n");
    printf("  -listIOblocksbrief                  - Display a list of I/O blocks with brief info\n");
    printf("  -IOblocksort sort                   - Optional sort order for -listIOblocks and -listIOblocksbrief\n");
    printf("                                          Specify one of the following:\n");
    printf("                                          NAME OWNER USER STATUS IONODECOUNT\n");
    printf("                                          CREATEDATE STATUSLASTMODIFIED\n");
    printf("  -IOblockfilterstatus statuses       - Optional status filter for -listIOblocks and -listIOblocksbrief\n");
    printf("                                          Specify one or more of the following:\n");
    printf("                                          FREE BOOTING ALLOCATED INITIALIZED TERMINATING\n");
    printf("  -IOblockfiltername block            - Optional I/O block name filter for -listIOblocks and -listIOblocksbrief\n");
    printf("  -IOblockfilterowner owner           - Optional I/O block owner filter for -listIOblocks and -listIOblocksbrief\n");
    printf("  -IOblockfilteruser user             - Optional I/O block user filter for -listIOblocks and -listIOblocksbrief\n");
    printf("  -IOblockfilterextendedinfo Y|N      - Optional extended info filter for -listIOblocks and -listIOblocksbrief\n");
    printf("                                          Specify Y or N\n");
    printf("  -listjobs                           - Display a list of jobs - default is Active jobs\n");
    printf("  -jobsort sort                       - Optional sort order for -listjobs\n");
    printf("                                          Specify one of the following:\n");
    printf("                                          ID USER BLOCK STARTTIME ENDTIME EXITSTATUS\n");
    printf("                                          EXECUTABLE STATUS COMPUTENODESUSED RANKSPERNODE\n");
    printf("  -jobfiltertype ACTIVE|COMPLETED|ALL - Optional job type filter for -listjobs\n");
    printf("                                          Specify ACTIVE, COMPLETED or ALL\n");
    printf("  -jobfilterstatus statuses           - Optional status filter for -listjobs\n");
    printf("                                          Specify one or more of the following:\n");
    printf("                                          Active Jobs: SETUP LOADING STARTING DEBUG RUNNING CLEANUP\n");
    printf("                                          Completed Jobs: TERMINATED ERROR\n");
    printf("  -jobfilterid id                     - Optional job ID filter for -listjobs\n");
    printf("  -jobfilterschedulerdata scheddata   - Optional scheduler data filter for -listjobs\n");
    printf("  -jobfiltercomputeblockname block    - Optional compute block name filter for -listjobs\n");
    printf("  -jobfilteruser user                 - Optional user name filter for -listjobs\n");
    printf("  -jobfilterexecutable exec           - Optional executable filter for -listjobs\n");
    printf("  -jobfilterexitstatus exitstatus     - Optional exit status filter for -listjobs\n");
    printf("  -jobfilterstarttime start end       - Optional start time interval filter (YYYYMMDDThhmmss format) for -listjobs\n");
    printf("  -jobfilterendtime start end         - Optional end time interval filter (YYYYMMDDThhmmss format) for -listjobs\n");
    printf("  -listmidplanenodeboards  midplane   - Display a list of node boards for a midplane (e.g. R00-M0)\n");
    printf("  -listnodeboardnodes  nodeboard      - Display a list of nodes for a node board (e.g. R00-M1-N14)\n");
    printf("  -listmidplanenodes  midplane        - Display a list of nodes for a midplane (e.g. R00-M1)\n");
    printf("  -listIOlinks  midplane or *ALL      - Display a list of the I/O links for a midplane\n");
    printf("                                          For all midplanes specify *ALL for midplane argument\n");
    printf("  -listAvailableIOlinks  mp or *ALL   - Display a list of the available I/O links for a midplane\n");
    printf("                                          For all midplanes specify *ALL for midplane argument\n");
    printf("  -getNodeMidplaneCoordinates loc     - Display coordinate for the node relative to the midplane.\n");
    printf("  -listconnectedcomputeblocks ioblock - Display a list of compute blocks connected to I/O block\n");
    printf("  -listblockIOlinks block             - Display a list of all the I/O links for a compute block\n");
    printf("  -listblockAvailableIOlinks block    - Display a list of the available I/O links for a compute block\n");
    printf("  -listblockIOlinksbrief block        - Display a summary count of available/unavailable I/O links for a compute block\n");
    printf("  -checkblockIO block                 - Indicates unconnected I/O nodes and midplanes failing I/O rules for compute block\n");
    printf("  -checkblockIOconnected block        - Indicates if all I/O nodes are available for compute block\n");
    printf("  -dumpxml                            - Set BG_DUMP_XML=true envvar\n");
    printf("                                          This option causes Core API internal XML files to\n");
    printf("                                          be dumped to files in /tmp\n");
    printf("  -verbose 1|2|3|4|5|6|7              - Set the logging verbose level (1..7) (Defaults to INFO)\n");
    printf("                                          1=OFF,2=FATAL,3=ERROR,4=WARN,5=INFO,6=DEBUG,7=TRACE\n");
    printf("  -properties                         - Set the bg.properties file name\n");
    printf("  -help                               - Print this help text\n");
}
