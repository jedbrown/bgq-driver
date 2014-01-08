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

#include "TestAllocatorEventListener.h"

#include <bgsched/bgsched.h>
#include <bgsched/Block.h>
#include <bgsched/BlockFilter.h>
#include <bgsched/Coordinates.h>
#include <bgsched/Midplane.h>
#include <bgsched/SchedUtil.h>

#include <bgsched/allocator/Allocator.h>
#include <bgsched/allocator/InputException.h>
#include <bgsched/allocator/Midplanes.h>
#include <bgsched/allocator/LiveModel.h>
#include <bgsched/allocator/ResourceAllocationException.h>
#include <bgsched/allocator/ResourceSpec.h>

#include <bgsched/core/core.h>

#include <utility/include/Log.h>

#include <execinfo.h>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <strings.h>

using namespace bgsched;
using namespace bgsched::allocator;
using namespace log4cxx;
using namespace std;

// Forward declares
void printHelp();

LOG_DECLARE_FILE("testallocator");

static const string HARDWARE_STATE_STRINGS[] = {
        "AVAILABLE",
        "MISSING",
        "ERROR",
        "SERVICE",
        "SOFTWARE FAILURE"
};

// Global count of allocates
uint32_t allocateRequests = 0;

// Global plugin name
vector<string> plugin;

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

const string
hardwareStateToString(EnumWrapper<Hardware::State> hardwareState)
{
    return HARDWARE_STATE_STRINGS[hardwareState.toValue()];
}

void
printMidplaneInfo(const Midplane::ConstPtr midplanePtr)
{
    LOG_INFO_MSG(" Midplane location  . . . . . . . : " << midplanePtr->getLocation());
    LOG_INFO_MSG("    State . . . . . . . . . . . . : " << hardwareStateToString(midplanePtr->getState()));
    LOG_INFO_MSG("    Sequence ID . . . . . . . . . : " << midplanePtr->getSequenceId());
}

void
failed (void)
{
    LOG_ERROR_MSG("===FAILED===");
    exit(-1);
}

void
freeAndRemoveBlock(Allocator &my_alloc, Model& model, bool isVirtualBlock, Block::Ptr& block)
{
    try {
        if (block->getStatus() != Block::Free) {
            // Free the block resources.
            my_alloc.release(model, block->getName(), plugin[0]);
            if (isVirtualBlock) {
                LOG_INFO_MSG("Block " << block->getName() << " was freed.");
                LOG_INFO_MSG("Removing block " << block->getName());
                model.removeBlock(block->getName());
            } else {
                // The "release" event handler will remove block after it is free.
                LOG_INFO_MSG("Block " << block->getName() << " free request initiated");
            }
        } else {
            LOG_INFO_MSG("Block " << block->getName() << " already free. Removing the block.");
            model.removeBlock(block->getName());
        }
    }
    catch(...)
    {
        LOG_ERROR_MSG("Block " << block->getName() << " was not freed or removed");
    }
}

void
printModelMidplanes(const Model& model)
{
    Midplanes::ConstPtr midplanelist = model.getMidplanes();
    Coordinates coords = midplanelist->getMachineSize();
    for (uint32_t a = 0; a <  coords[Dimension::A]; ++a) {
        for (uint32_t b = 0; b < coords[Dimension::B]; ++b) {
            for (uint32_t c = 0; c < coords[Dimension::C]; ++c) {
                for (uint32_t d = 0; d < coords[Dimension::D]; ++d) {
                    Coordinates coord(a, b, c, d);
                    Midplane::ConstPtr midplane = midplanelist->get(coord);
                    LOG_INFO_MSG("==== Midplane at A:" << a << " B:" << b << " C:" << c << " D:" << d << " ====");
                    printMidplaneInfo(midplane);
                    LOG_INFO_MSG("=== E N D   O F   M I D P L A N E ===");
                    LOG_INFO_MSG("=====================================");
                }
            }
        }
    }
}

void
findAndAllocateSmallBlock(Allocator &my_alloc, Model& model, bool isVirtualBlock, uint32_t computeNodes, bool allocate, Block::Ptr& block)
{
    // Create compute node shape
    Shape shape = my_alloc.createSmallShape(computeNodes);

    // Create ResourceSpec based on the shape
    ResourceSpec resourceSpec(shape);

    // Add drained midplanes
    //resourceSpec.addDrainedMidplane("R00-M0");

    // Find block resources matching resource spec
    block = my_alloc.findBlockResources(model, resourceSpec, plugin[0]);
    if (!block) {
        LOG_ERROR_MSG("Could not find any available resources to create block.");
        return;
    }

    // Set the block name
    string prefix = "IBMTest_";
    string blockName = SchedUtil::createBlockName(prefix);
    block->setName(blockName);

    // Set block description
    string description = "Created by IBM test_allocator";
    block->setDescription(description);

    LOG_DEBUG_MSG("Dumping contents of block " << block->toString(true));

    // Instantiate block
    try {
        // Use default owner
        string owner;
        model.addBlock(block, owner);
        LOG_INFO_MSG("Block " << block->getName() << " was added");
    }
    catch(...)
    {
        LOG_ERROR_MSG("Block " << block->getName() << " was not added");
        return;
    }

    // Check if allocate was requested
    if (allocate) {
        try {
            // Reserve the block resources
            my_alloc.prepare(model, block->getName(), plugin[0]);
            LOG_INFO_MSG("Block " << block->getName() << " was allocated");
            // For LiveModel the allocates are asynchronous, only track LiveModel allocates
            if (isVirtualBlock == false) {
                allocateRequests++;
            }
        }
        catch(...)
        {
            LOG_ERROR_MSG("Block " << block->getName() << " was not allocated");
            return;
        }
    }
}

void
findAndAllocateLargeBlock(Allocator &my_alloc, Model& model, bool isVirtualBlock, uint32_t a, uint32_t b, uint32_t c, uint32_t d, ResourceSpec::ConnectivitySpec::Value conn_a,
        ResourceSpec::ConnectivitySpec::Value conn_b, ResourceSpec::ConnectivitySpec::Value conn_c, ResourceSpec::ConnectivitySpec::Value conn_d,
        bool canRotateShape, bool canUsePassthrough, bool allocate, Block::Ptr& block)
{
    // Create midplane shape
    Shape shape = my_alloc.createMidplaneShape(a, b, c, d);

    // Create ResourceSpec based on the shape
    ResourceSpec resourceSpec(shape, canRotateShape, canUsePassthrough);

    // Setup connectivity spec
    resourceSpec.setConnectivitySpec(Dimension::A, conn_a);
    resourceSpec.setConnectivitySpec(Dimension::B, conn_b);
    resourceSpec.setConnectivitySpec(Dimension::C, conn_c);
    resourceSpec.setConnectivitySpec(Dimension::D, conn_d);

    // Add drained midplanes
    //resourceSpec.addDrainedMidplane("R00-M0");
    //resourceSpec.addDrainedMidplane("R13-M1");

    // Find block resources matching resource spec
    LOG_INFO_MSG("Finding block resources.");
    block = my_alloc.findBlockResources(model, resourceSpec, plugin[0]);
    if (!block) {
        LOG_ERROR_MSG("Could not find resources to create block.");
        return;
    }
    LOG_INFO_MSG("Found block resources.");

    // Set the block name
    string prefix = "IBMTest_";
    string blockName = SchedUtil::createBlockName(prefix);
    // string blockName = "IBMTest_Full";
    block->setName(blockName);

    // Set block description
    string description = "Created by IBM test_allocator";
    block->setDescription(description);

    LOG_DEBUG_MSG("Dumping contents of block " << block->toString(true));

    // Instantiate block
    try {
        // Use default owner
        string owner;
        LOG_INFO_MSG("Requesting block add for block " << block->getName());
        model.addBlock(block, owner);
        LOG_INFO_MSG("Block " << block->getName() << " was added");
    }
    catch(...)
    {
        LOG_ERROR_MSG("Block " << block->getName() << " was not added");
        return;
    }

    // Check if allocate was requested
    if (allocate) {
        try {
            // Reserve the block resources
            my_alloc.prepare(model, block->getName(), plugin[0]);
            LOG_INFO_MSG("Block " << block->getName() << " was allocated");
            // For LiveModel the allocates are asynchronous, so only track LiveModel allocates
            if (isVirtualBlock == false) {
                allocateRequests++;
            }
        }
        catch(...)
        {
            LOG_ERROR_MSG("Block " << block->getName() << " was not allocated");
            return;
        }
    }
}

void
cleanupBlocks(Allocator &my_alloc, Model& model, bool waitOnCleanup)
{
    Block::Ptrs blocksVector;
    BlockFilter blockFilter;
    string prefix("IBMTest");
    uint32_t sleep_sec(3);

    try {
        blocksVector = bgsched::core::getBlocks(blockFilter);

        if (blocksVector.empty()) {
            LOG_WARN_MSG("Found no blocks to remove.");
        } else {
            for (Block::Ptrs::iterator iter = blocksVector.begin(); iter != blocksVector.end(); ++iter) {
                Block::Ptr block = *iter;
                string blockName = block->getName();
                size_t found = blockName.find(prefix);
                if (found != string::npos) {
                    // The allocate event listener for deallocate will remove the block
                    freeAndRemoveBlock(my_alloc, model, false, block);
                }
            }
        }
    } catch(...)
    {
        LOG_ERROR_MSG("Unexpected error removing blocks");
        return;
    }

    // Loop until all blocks have been freed and removed
    if (waitOnCleanup) {
        bool cleanupDone = false;
        uint32_t waitCount = 1;
        // Max time we will wait for cleanup is 90 seconds
        while (!cleanupDone && waitCount < 30) {
            blocksVector = bgsched::core::getBlocks(blockFilter);
            if (blocksVector.empty()) {
              cleanupDone = true;
            } else {
                bool blockFound = false;
                for (Block::Ptrs::iterator iter = blocksVector.begin(); iter != blocksVector.end(); ++iter) {
                    Block::Ptr block = *iter;
                    string blockName = block->getName();
                    size_t found = blockName.find(prefix);
                    if (found != string::npos) {
                        blockFound = true;
                    }
                }
                if (blockFound) {
                    // Sleep before checking blocks again
                    sleep(sleep_sec);
                    waitCount++; }
                else {
                    cleanupDone = true;
                }
            }
        }
    }
}

int
main(int argc, char *argv[])
{
    bool isListLiveModelMidplanes = false;
    bool isDumpShapes = false;
    bool isDumpHardware = false;
    bool isDumpBlocks = false;
    bool isDumpBlocksExtended = false;
    bool isDumpDrainList = false;
    bool isWaitOnCleanup = true;
    bool isWaitOnAllocate = true;

    // iterate through args first to get -properties
    string properties;

    for (int i = 1; i < argc; ++i) {
        if (!strcasecmp(argv[i], "-properties")) {
            if (++i == argc) {
                printf("-properties specified without an argument value\n");
                exit(EXIT_FAILURE);
            } else {
                properties = argv[i];
            }
        } else if (!strcasecmp(argv[i], "-help") || !strcasecmp(argv[i], "-h")) {
            printHelp();
            exit(EXIT_SUCCESS);
        }
    }

    // init library
    init(properties);

    // collect any parameters
    int argNbr = 1;
    char* argKey;
    char* argVal;
    while (argNbr < argc) {
        argKey = argv[argNbr];
        argNbr++;
        if (argNbr < argc) {
            argVal = argv[argNbr];
        } else {
            argVal = NULL;
        }

        if (strcasecmp(argKey, "-listLiveModelMidplanes") == 0) {
            isListLiveModelMidplanes = true;
        } else if (strcasecmp(argKey, "-dumpShapes") == 0) {
            isDumpShapes = true;
        } else if (strcasecmp(argKey, "-dumpHardware") == 0) {
            isDumpHardware = true;
        } else if (strcasecmp(argKey, "-dumpBlocks") == 0) {
            isDumpBlocks = true;
        } else if (strcasecmp(argKey, "-dumpBlocksExtended") == 0) {
            isDumpBlocksExtended = true;
        } else if (strcasecmp(argKey, "-dumpDrainList") == 0) {
            isDumpDrainList = true;
        } else if (strcasecmp(argKey, "-waitOnCleanup") == 0) {
            if (argVal != NULL) {
                while (argVal != NULL) {
                    if (strcasecmp(argVal, "Y") == 0) {
                        isWaitOnCleanup = true;
                    } else if (strcasecmp(argVal, "N") == 0)  {
                        isWaitOnCleanup = false;
                    } else {
                        printf("-waitOnCleanup argument value, %s, is not valid\n", argVal);
                        printf("Valid values are: Y or N\n");
                        string badArg = argVal;
                        LOG_ERROR_MSG(__FUNCTION__ << " -waitOnCleanup argument value " << badArg << " is not valid");
                        return -1;
                    }
                    argNbr++;
                    argVal = NULL;
                    if (argNbr < argc && argv[argNbr][0] != '-') {
                        argVal = argv[argNbr];
                    }
                }
            } else {
                printf("-waitOnCleanup specified without an argument value\n");
                LOG_ERROR_MSG(__FUNCTION__ << " -waitOnCleanup specified without an argument value");
                return -1;
            }
        } else if (strcasecmp(argKey, "-waitOnAllocate") == 0) {
            if (argVal != NULL) {
                while (argVal != NULL) {
                    if (strcasecmp(argVal, "Y") == 0) {
                        isWaitOnAllocate = true;
                    } else if (strcasecmp(argVal, "N") == 0)  {
                        isWaitOnAllocate = false;
                    } else {
                        printf("-waitOnAllocate argument value, %s, is not valid\n", argVal);
                        printf("Valid values are: Y or N\n");
                        string badArg = argVal;
                        LOG_ERROR_MSG(__FUNCTION__ << " -waitOnAllocate argument value " << badArg << " is not valid");
                        return -1;
                    }
                    argNbr++;
                    argVal = NULL;
                    if (argNbr < argc && argv[argNbr][0] != '-') {
                        argVal = argv[argNbr];
                    }
                }
            } else {
                printf("-waitOnAllocate specified without an argument value\n");
                LOG_ERROR_MSG(__FUNCTION__ << " -waitOnAllocate specified without an argument value");
                return -1;
            }
         } else if (strcasecmp(argKey, "-properties") == 0) {
            // ignore, we've already parsed this guy
            ++argNbr;
        } else {
            printf("parameter %s not recognized\n", argKey);
            printHelp();
            return 0;
        }
    }

    LOG_INFO_MSG( "test_allocator running testcases (Note: requires 4x4 configuration)" );

    try
    {

        // Get machine size
        Coordinates coords = core::getMachineSize();

        LOG_DEBUG_MSG( "Creating allocator framework" );
        Allocator my_alloc;

        // Retrieve the list of plug-ins
        plugin = my_alloc.getPluginList();

        // Validate that plug-in exists
        if (plugin.size() == 0) {
            LOG_ERROR_MSG("No allocator plug-in was found");
            return 0;
        }

        LOG_TRACE_MSG( "Obtaining reference to LiveModel from allocator" );
        LiveModel &model(my_alloc.getLiveModel());

        TestAllocatorEventListener blockEventListener;
        LOG_TRACE_MSG( "Registering block event listener for LiveModel" );
        model.registerListener(blockEventListener);
/*
        // while (true) {
            // Cleanup (free and remove) any LiveModel blocks from previous runs
            LOG_DEBUG_MSG( "Cleaning up blocks from previous runs" );
            cleanupBlocks(my_alloc, model, isWaitOnCleanup);

            // Sync up the Live Model after block cleanup step
            model.syncState();

            if (isListLiveModelMidplanes) {
                LOG_INFO_MSG( "=== Print list of midplanes from LiveModel: should complete OK ===" );
                printModelMidplanes(model);
            }

            //model.addDrainedMidplane("R00-M0");
            //model.addDrainedMidplane("R00-M1");
            //model.addDrainedMidplane("R01-M0");

            LOG_INFO_MSG( "=== findBlockResources() for 32,64,128,256 cnodes on LiveModel ===" );
            LOG_INFO_MSG( "=== 32 cnodes on LiveModel: should complete OK ===" );
            Block::Ptr block32;
            findAndAllocateSmallBlock(my_alloc, model, false, 32, true, block32);
            model.syncState();
            LOG_INFO_MSG( "=== 64 cnodes on LiveModel: should complete OK ===" );
            Block::Ptr block64;
            findAndAllocateSmallBlock(my_alloc, model, false, 64, true, block64);
            model.syncState();
            LOG_INFO_MSG( "=== 128 cnodes on LiveModel: should complete OK ===" );
            Block::Ptr block128;
            findAndAllocateSmallBlock(my_alloc, model, false, 128, true, block128);
            model.syncState();
            LOG_INFO_MSG( "=== 256 cnodes on LiveModel: should complete OK ===" );
            Block::Ptr block256;
            findAndAllocateSmallBlock(my_alloc, model, false, 256, true, block256);
            model.syncState();
        //}

*/
/*
        // Cleanup (free and remove) any LiveModel blocks from previous runs
        LOG_DEBUG_MSG( "Cleaning up blocks from previous runs" );
        cleanupBlocks(my_alloc, model, isWaitOnCleanup);

        // Sync up the Live Model after block cleanup step
        model.syncState();

        if (isListLiveModelMidplanes) {
            LOG_INFO_MSG( "=== Print list of midplanes from LiveModel: should complete OK ===" );
            printModelMidplanes(model);
        }

        //model.addDrainedMidplane("R00-M0");
        //model.addDrainedMidplane("R00-M1");
        //model.addDrainedMidplane("R01-M0");

        LOG_INFO_MSG( "=== findBlockResources() for 32,64,128,256 cnodes on LiveModel ===" );
        LOG_INFO_MSG( "=== 32 cnodes on LiveModel: should complete OK ===" );
        Block::Ptr block32;
        findAndAllocateSmallBlock(my_alloc, model, false, 32, true, block32);
        model.syncState();
        LOG_INFO_MSG( "=== 64 cnodes on LiveModel: should complete OK ===" );
        Block::Ptr block64;
        findAndAllocateSmallBlock(my_alloc, model, false, 64, true, block64);
        model.syncState();
        LOG_INFO_MSG( "=== 128 cnodes on LiveModel: should complete OK ===" );
        Block::Ptr block128;
        findAndAllocateSmallBlock(my_alloc, model, false, 128, true, block128);
        model.syncState();
        LOG_INFO_MSG( "=== 256 cnodes on LiveModel: should complete OK ===" );
        Block::Ptr block256;
        findAndAllocateSmallBlock(my_alloc, model, false, 256, true, block256);
        model.syncState();
*/

/*
        LOG_INFO_MSG( "=== Test empty block on LiveModel: should FAIL ===" );
        Block::Ptr live_block;
        // Use default owner
        string dft_owner;
        model.addBlock(live_block, dft_owner);
*/

        // Cleanup (free and remove) any LiveModel blocks from previous runs
        LOG_DEBUG_MSG( "Cleaning up blocks from previous runs" );
        cleanupBlocks(my_alloc, model, isWaitOnCleanup);
        LOG_INFO_MSG( "=== findBlockResources() and allocate for all midplanes on LiveModel ===" );
        Block::Ptr block512;
        uint32_t midplaneCount = coords[Dimension::A] * coords[Dimension::B] * coords[Dimension::C] * coords[Dimension::D];
        // Create and allocate a midplane sized block for every midplane on the system (plus fail last request)
        for (uint32_t i = 1; i <= midplaneCount+1; ++i) {
            findAndAllocateLargeBlock(
                    my_alloc,
                    model,
                    false, // Live block
                    1,     // Midplanes in A
                    1,     // Midplanes in B
                    1,     // Midplanes in C
                    1,     // Midplanes in D
                    ResourceSpec::ConnectivitySpec::Torus, // Connectivity in A
                    ResourceSpec::ConnectivitySpec::Torus, // Connectivity in B
                    ResourceSpec::ConnectivitySpec::Torus, // Connectivity in C
                    ResourceSpec::ConnectivitySpec::Torus, // Connectivity in D
                    false, // No rotation
                    false, // No pass-through midplanes
                    true,  // Allocate
                    block512);
            model.syncState();
        }

/*
        LOG_INFO_MSG( "=== findBlockResources() and allocate all small blocks (128 cnodes each) on LiveModel ===" );
        Block::Ptr smallblock128;
        // Create and allocate a 128 cnode for all hardware on the system (plus fail last request)
        for (uint32_t i = 1; i <= 100; ++i) {
            findAndAllocateSmallBlock(
                    my_alloc,
                    model,
                    false, // Live block
                    128,   // Size in cnodes
                    true,  // Allocate
                    smallblock128);
            model.syncState();
        }
*/


        LOG_INFO_MSG( "=== findBlockResources() for full block on LiveModel ===" );
        Block::Ptr block_full;
        findAndAllocateLargeBlock(
                my_alloc,
                model,
                false,  // Live block
                coords[Dimension::A],
                coords[Dimension::B],
                coords[Dimension::C],
                coords[Dimension::D],
                ResourceSpec::ConnectivitySpec::Torus, // Connectivity in A
                ResourceSpec::ConnectivitySpec::Torus, // Connectivity in B
                ResourceSpec::ConnectivitySpec::Torus, // Connectivity in C
                ResourceSpec::ConnectivitySpec::Torus, // Connectivity in D
                false, // No rotation
                false, // No pass-through midplanes
                true,  // Allocate
                block_full);

/*
        LOG_INFO_MSG( "=== findBlockResources() for 1x4x1x2 shape with rotation on LiveModel ===" );
        Block::Ptr large_block;
        findAndAllocateLargeBlock(
                my_alloc,
                model,
                false, // Live block
                1,
                4,
                1,
                2,
                ResourceSpec::ConnectivitySpec::Torus, // Connectivity in A
                ResourceSpec::ConnectivitySpec::Torus, // Connectivity in B
                ResourceSpec::ConnectivitySpec::Torus, // Connectivity in C
                ResourceSpec::ConnectivitySpec::Torus, // Connectivity in D
                true,  // Allow rotation
                true,  // Allow pass-through midplanes
                false,  // Allocate
                large_block);

        model.syncState();
*/
/*

        LOG_INFO_MSG( "=== findBlockResources() for 2x1x2x1 shape with rotation on LiveModel ===" );
        findAndAllocateLargeBlock(
                my_alloc,
                model,
                false, // Live block
                2,
                1,
                2,
                1,
                ResourceSpec::ConnectivitySpec::Either, // Connectivity in A
                ResourceSpec::ConnectivitySpec::Either, // Connectivity in B
                ResourceSpec::ConnectivitySpec::Either, // Connectivity in C
                ResourceSpec::ConnectivitySpec::Either, // Connectivity in D
                true,  // Allow rotation
                true,  // Allow pass-through midplanes
                true,  // Allocate
                large_block);
*/

/*
        LOG_INFO_MSG( "=== findBlockResources() for 3x1x2x1 shape with rotation on LiveModel ===" );
        Block::Ptr large_block;
        findAndAllocateLargeBlock(
                my_alloc,
                model,
                false, // Live block
                3,
                1,
                2,
                1,
                ResourceSpec::ConnectivitySpec::Mesh,  // Connectivity in A
                ResourceSpec::ConnectivitySpec::Torus, // Connectivity in B
                ResourceSpec::ConnectivitySpec::Mesh,  // Connectivity in C
                ResourceSpec::ConnectivitySpec::Torus, // Connectivity in D
                true,  // Allow rotation
                false, // Allow pass-through midplanes
                true,  // Allocate
                large_block);
*/
/*
        LOG_INFO_MSG( "=== findBlockResources() for all large shape sizes on LiveModel ===" );
        Block::Ptr block;
        Allocator::Shapes shapes(my_alloc.getShapes());
        for (Allocator::Shapes::iterator iter = shapes.begin(); iter != shapes.end(); ++iter) {
            Shape shape = *iter;
            try {
                findAndAllocateLargeBlock(
                        my_alloc,
                        model,
                        false, // Live block
                        shape.getMidplaneSize(Dimension::A),
                        shape.getMidplaneSize(Dimension::B),
                        shape.getMidplaneSize(Dimension::C),
                        shape.getMidplaneSize(Dimension::D),
                        ResourceSpec::ConnectivitySpec::Either, // Connectivity in A
                        ResourceSpec::ConnectivitySpec::Either, // Connectivity in B
                        ResourceSpec::ConnectivitySpec::Either, // Connectivity in C
                        ResourceSpec::ConnectivitySpec::Either,  // Connectivity in D
                        true,  // Allow rotation
                        true,  // Allow pass-through midplanes
                        false, // Allocate
                        block);
                // Sync up the Live Model after block cleanup step
                //model.syncState();
            } catch(...)
            {
                // Do nothing
            }
        }
*/

        if (isDumpHardware) {
            model.dump(std::cout, Model::DumpVerbosity::Hardware);
        }

        //model.addDrainedMidplane("R00-M0");
        if (isDumpDrainList) {
            model.dump(std::cout, Model::DumpVerbosity::DrainList);
        }

        if (isDumpShapes) {
            my_alloc.dump(std::cout, Allocator::DumpVerbosity::BlockShapes);
        }

        // Wait on allocates if requested
        if (isWaitOnAllocate) {
            LOG_INFO_MSG( "Waiting up to 500 seconds for blocks to allocate" );
            uint32_t sleep_sec(10);
            // Check if all allocate requests completed
            bool allocatesDone = false;
            uint32_t waitCount = 1;
            // Max time we will wait for allocates to complete is 120 seconds
            while (!allocatesDone && waitCount < 50) {
                if (blockEventListener.getAllocateCount() == allocateRequests) {
                    allocatesDone = true;
                } else {
                    // Sleep before checking blocks again
                    sleep(sleep_sec);
                    waitCount++;
                }
            }
        }

        LOG_INFO_MSG( "=== ENDING TEST_ALLOCATOR ===" );
    }
    catch(...)
    {
        LOG_ERROR_MSG( "Exception caught" );
        print_trace();
        throw;
    }
}

void printHelp()
{
    printf("Parameters:\n");
    printf("  -listLiveModelMidplanes             - Displays the midplanes from LiveModel\n");
    printf("  -dumpShapes                         - Displays the valid block shapes for the machine\n");
    printf("  -dumpHardware                       - Displays a summary of the model hardware\n");
    printf("  -dumpBlocks                         - Displays a summary (basic info) of the model blocks\n");
    printf("  -dumpBlocksExtended                 - Displays a summary (extended info) of the model blocks\n");
    printf("  -dumpDrainList                      - Displays a list of midplanes in drain list of the model\n");
    printf("  -waitOnCleanup Y|N                  - Wait until all blocks are cleaned up\n");
    printf("  -waitOnAllocate Y|N                 - Wait until blocks are allocated before ending program\n");
    printf("  -properties                         - Set the bg.properties file name\n");
    printf("  -help                               - Print this help text\n");
}
