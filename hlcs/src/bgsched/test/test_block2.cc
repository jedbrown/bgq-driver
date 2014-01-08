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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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
#include <bgsched/InputException.h>

#include <bgsched/core/core.h>

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>

#include <execinfo.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <strings.h>

using namespace bgsched;
using namespace log4cxx;
using namespace std;

LOG_DECLARE_FILE("testblocks");

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

const string BlockStatusToString(EnumWrapper<Block::Status> BlockStatus)
{
    return BLOCK_STATUS_STRINGS[BlockStatus.toValue()];
}

const string BlockActionToString(EnumWrapper<Block::Action::Value> BlockAction)
{
    return BLOCK_ACTION_STRINGS[BlockAction.toValue()];
}

void printBlockInfo(const Block::Ptr blockPtr)
{
    LOG_INFO_MSG("===================================");
    LOG_INFO_MSG(" Block name . . . . . . . . . . . : " << blockPtr->getName());
    LOG_INFO_MSG("    Creation id . . . . . . . . . : " << blockPtr->getCreationId());
    LOG_INFO_MSG("    Status  . . . . . . . . . . . : " << BlockStatusToString(blockPtr->getStatus()));
    LOG_INFO_MSG("    Action  . . . . . . . . . . . : " << BlockActionToString(blockPtr->getAction()));
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

// Obtain a backtrace and print it to stderr.
void
print_trace (void)
{
    void *array[20];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace (array, 20);
    strings = backtrace_symbols (array, size);

    cerr << "Obtained " << size << " stack frames." << endl;

    for (i = 0; i < size; i++)
        cerr << strings[i] << endl;

    cerr.flush();
    free (strings);
}

void
printBlocks()
{
    BlockFilter blockFilter;

    try {
        Block::Ptrs blocksVector = bgsched::core::getBlocks(blockFilter);

        if (blocksVector.empty()) {
            LOG_WARN_MSG("Found no compute blocks");
        } else {
            LOG_WARN_MSG("Found following " << blocksVector.size() <<  " compute blocks:");
            for (Block::Ptrs::iterator iter = blocksVector.begin(); iter != blocksVector.end(); ++iter) {
                Block::Ptr block = *iter;
                printBlockInfo(block);
            }
        }
    } catch(...)
    {
        LOG_ERROR_MSG("Unexpected error getting compute blocks");
        return;
    }
}

int
main(int argc, const char** argv)
{
    bool isListBlocks = false;
    bool isCreateBlock = false;
    bool isBootBlock = false;
    bool isFreeBlock = false;

    Block::Ptr BlockPtr;

    string properties;
    string createBlock;
    string startLocation;
    uint32_t computeNodeCount = 0;
    string bootBlock;
    string freeBlock;

    // iterate through args
    for (int i = 1; i < argc; ++i) {
        if (!strcasecmp(argv[i], "-properties")) {
            if (++i == argc) {
                cerr << "Please give a file name after " << argv[i-1] << endl;
                exit(EXIT_FAILURE);
            } else {
                properties = argv[i];
            }
        } else if (!strcasecmp(argv[i], "-listblocks")) {
            isListBlocks = true;
        } else if (!strcasecmp(argv[i], "-createblock")) {
            isCreateBlock = true;
            if (++i == argc) {
                cerr << "Please give a compute block name after " << argv[i-1] << endl;
                exit(EXIT_FAILURE);
            } else {
                createBlock = argv[i];
                if (++i == argc) {
                    cerr << "Please give a starting location after " << argv[i-2] << endl;
                    exit(EXIT_FAILURE);
                } else {
                    startLocation = argv[i];
                    if (++i == argc) {
                        cerr << "Please give compute node count after " << argv[i-3] << endl;
                        exit(EXIT_FAILURE);
                    } else {
                        computeNodeCount = boost::lexical_cast<uint32_t>(argv[i]);
                    }
                }
            }
        } else if (!strcasecmp(argv[i], "-bootblock")) {
            isBootBlock = true;
            if (++i == argc) {
                cerr << "Please give a compute block name after " << argv[i-1] << endl;
                exit(EXIT_FAILURE);
            } else {
                bootBlock = argv[i];
            }
        } else if (!strcasecmp(argv[i], "-freeblock")) {
            isFreeBlock = true;
            if (++i == argc) {
                cerr << "Please give a block name after " << argv[i-1] << endl;
                exit(EXIT_FAILURE);
            } else {
                freeBlock = argv[i];
            }
        } else if (!strcasecmp(argv[i], "-help") || !strcasecmp(argv[i], "-h")) {
            cout << argv[0] << ":" << endl;
            cout << "\t -listblocks                                 : Print out compute blocks on system" << endl;
            cout << "\t -createblock name, startlocation, numnodes  : Create small compute block" << endl;
            cout << "\t -bootblock  block                           : Boot compute block" << endl;
            cout << "\t -freeblock I/O block                        : Free compute block" << endl;
            cout << "\t -properties <file>                          : Set bg.properties file name" << endl;
            cout << "\t -help | -h                                  : Print this help text" << endl;
            exit(EXIT_SUCCESS);
        }
    }

    // init library
    init( properties );
    LOG_INFO_MSG( "test_block2 running testcases" );

    try {
        if (isListBlocks) {
            printBlocks();
        }

        if (isCreateBlock) {
            LOG_INFO_MSG("Creating compute block " << createBlock );
            BlockPtr = Block::create(startLocation, computeNodeCount);
            // Use default owner
            string owner;
            BlockPtr->setName(createBlock);
            // Instantiate block
            try {

                BlockPtr->add(owner);
                LOG_INFO_MSG("Block " << BlockPtr->getName() << " was added");
            }
            catch(...)
            {
                LOG_ERROR_MSG("Block " << BlockPtr->getName() << " was not added");
                return -1;
            }
            printBlockInfo(BlockPtr);
        }

        if (isBootBlock) {
            std::vector<std::string> unavailableResources;   // Unavailable I/O resources
            LOG_INFO_MSG("Booting compute block " << bootBlock << ".");
            try {
                Block::initiateBoot(bootBlock, &unavailableResources);
            } catch(...)
            {
                if (unavailableResources.size() > 0) {
                    // Print the unavailable compute resources
                    LOG_INFO_MSG("Compute block " << bootBlock << " has " << unavailableResources.size() << " unavailable resources:");
                    for (unsigned int it = 0; it < unavailableResources.size(); it++) {
                        LOG_INFO_MSG(unavailableResources[it]);
                    }
                }
                LOG_ERROR_MSG("Failed to boot compute " << bootBlock);
                exit(-1);
            }
            if (unavailableResources.empty()) {
                LOG_INFO_MSG("All compute resources are available for compute block " << bootBlock);
            }
        }

        if (isFreeBlock) {
            Block::initiateFree(freeBlock);
        }

        LOG_INFO_MSG( "=== test_block2 Completed ===" );
        return(0);
    } catch(...)
    {
        LOG_ERROR_MSG("Exception caught");
        exit(-1);
    }
}
