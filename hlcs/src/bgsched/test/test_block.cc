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

#include <bgsched/bgsched.h>
#include <bgsched/Block.h>
#include <bgsched/BlockFilter.h>
#include <bgsched/InputException.h>
#include <bgsched/Midplane.h>
#include <bgsched/SchedUtil.h>

#include <bgsched/core/core.h>

#include <bgsched/allocator/Allocator.h>
#include <bgsched/allocator/InputException.h>
#include <bgsched/allocator/Midplanes.h>
#include <bgsched/allocator/LiveModel.h>
#include <bgsched/allocator/ResourceAllocationException.h>
#include <bgsched/allocator/ResourceSpec.h>

#include <utility/include/Log.h>

#include <execinfo.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <strings.h>

using namespace bgsched;
using namespace bgsched::allocator;
using namespace log4cxx;
using namespace std;

LOG_DECLARE_FILE("testblocks");

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
failed (void)
{
    LOG_ERROR_MSG("!!!FAILED!!!");
    exit(-1);
}

void printBlockInfo(const Block::Ptr blockPtr)
{
    LOG_INFO_MSG(blockPtr->toString(true));
}

void
freeBlock(Allocator &my_alloc, Block::Ptr& block)
{
    LiveModel &model = my_alloc.getLiveModel();
    // Retrieve the list of plug-ins
    vector<string> plugin = my_alloc.getPluginList();
    // Validate that plug-in exists
    if (plugin.size() == 0) {
        LOG_ERROR_MSG("No allocator plug-in was found");
        return;
    }
    try {
        // Free the block resources
        my_alloc.release(model, block->getName(), plugin[0]);
        LOG_INFO_MSG("Block " << block->getName() << " was freed");
    }
    catch(...)
    {
        LOG_ERROR_MSG("Block " << block->getName() << " was not freed");
    }
}

void
removeBlock(string blockName)
{
    try {
        // Remove the block
        Block::remove(blockName);
        LOG_INFO_MSG("Block " << blockName << " was removed");
    }
    catch(...)
    {
        LOG_ERROR_MSG("Block " << blockName << " was not removed");
    }
}

void
updateBlockDefaults(Block::Ptr& block)
{
    LOG_INFO_MSG("Before Update: Dumping contents of block");
    printBlockInfo(block);

    // Update block without modifying fields
    try {
        block->update();
        LOG_INFO_MSG("Block " << block->getName() << " was updated");
    }
    catch(...)
    {
        LOG_ERROR_MSG("Block " << block->getName() << " was not updated");
        return;
    }
    LOG_INFO_MSG("After Update: Dumping contents of block");
    printBlockInfo(block);
}

void
updateBlock(Block::Ptr& block)
{
    LOG_INFO_MSG("Before Update: Dumping contents of block");
    printBlockInfo(block);

    // Update block with changed settings
    try {
        block->setDescription("New block description");
        block->setOptions("ABCDOptions");
        block->setBootOptions("1234BootOptions");
        block->setMicroLoaderImage("theNewMicroloaderImageFilePath");
        // block->setNodeConfiguration("NodeConfigName");
        LOG_INFO_MSG("After Updating Settings: Dumping contents of block");
        printBlockInfo(block);
        block->update();
        LOG_INFO_MSG("Block " << block->getName() << " was updated");
    }
    catch(...)
    {
        LOG_ERROR_MSG("Block " << block->getName() << " was not updated");
        return;
    }
}

void
addBlock(Allocator &my_alloc, uint32_t computeNodes, Block::Ptr& block)
{
    LiveModel &model = my_alloc.getLiveModel();

    // Retrieve the list of plug-ins
    vector<string> plugin = my_alloc.getPluginList();
    // Validate that plug-in exists
    if (plugin.size() == 0) {
        LOG_ERROR_MSG("No allocator plug-in was found");
        return;
    }

    // Create compute node shape
    Shape shape = my_alloc.createSmallShape(computeNodes);

    // Create ResourceSpec based on the shape
    ResourceSpec resourceSpec(shape);

    // Find block resources matching resource spec
    block = my_alloc.findBlockResources(model, resourceSpec, plugin[0]);
    if (!block) {
        LOG_ERROR_MSG("Could not find any available resources to create block.");
        return;
    }

    // Set the block name
    //ostringstream blockName;
    //blockName << "Test_Small_Block_" << computeNodes << "_Nodes";
    //block->setName(blockName.str());

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

    /*
    try {
        // Reserve the block resources
        my_alloc.prepare(model, block->getName(), plugin[0]);
        LOG_INFO_MSG("Block " << block->getName() << " was allocated");
    }
    catch(...)
    {
        LOG_ERROR_MSG("Block " << block->getName() << " was not allocated");
        return;
    }
    */
}

void
testBadValues(Block::Ptr& block)
{
    LOG_INFO_MSG("Before Update: Dumping contents of block");
    printBlockInfo(block);

    // Try setting bad description (max 1024 chars)
    try {
        string desc;
        block->setDescription(desc); // Empty string
    }
    catch(...)
    {
        LOG_ERROR_MSG("Set description failed on block " << block->getName());
    }

    // Try setting bad options (max 16 chars)
    try {
        block->setOptions("1234567890ABCDEFGH");
    }
    catch(...)
    {
        LOG_ERROR_MSG("Set options failed on block " << block->getName());
    }

    // Try setting bad boot options (max 256 chars)
    try {
        block->setBootOptions("123456789012345678921234567893123456789412345678951234567896123456789712345678981234567899123456789012345678901234567892123456789312345678941234567895123456789612345678971234567898123456789912345678901234567890123456789212345678931234567894123456789512345678961234567897123456789812345678991234567890");
    }
    catch(...)
    {
        LOG_ERROR_MSG("Set boot options failed on block " << block->getName());
    }

    // Try setting bad micro-loader image (max 256 chars)
    try {
        block->setMicroLoaderImage("123456789012345678921234567893123456789412345678951234567896123456789712345678981234567899123456789012345678901234567892123456789312345678941234567895123456789612345678971234567898123456789912345678901234567890123456789212345678931234567894123456789512345678961234567897123456789812345678991234567890");
    }
    catch(...)
    {
        LOG_ERROR_MSG("Set micro-loader image failed on block " << block->getName());
    }

    // Try setting bad node configuration (max 32 chars)
    try {
        block->setNodeConfiguration("123456789012345678921234567893123");
    }
    catch(...)
    {
        LOG_ERROR_MSG("Set node configuration failed on block " << block->getName());
    }

    LOG_INFO_MSG("After Updating Settings: Dumping contents of block");
    printBlockInfo(block);

    try {
        block->update();
    }
    catch(...)
    {
        LOG_ERROR_MSG("Block " << block->getName() << " was not updated");
        return;
    }

    LOG_INFO_MSG("Block " << block->getName() << " was updated");
}

void
removeDupBlocks()
{
    BlockFilter blockFilter;
    string prefix("IBMTest");

    try {
        Block::Ptrs blocksVector = bgsched::core::getBlocks(blockFilter);

        if (blocksVector.empty()) {
            LOG_WARN_MSG("Found no blocks to remove.");
        } else {
            for (Block::Ptrs::iterator iter = blocksVector.begin(); iter != blocksVector.end(); ++iter) {
                Block::Ptr block = *iter;
                string blockName = block->getName();
                size_t found = blockName.find(prefix);
                if (found != string::npos) {
                    Block::remove(blockName);
                }
            }
        }
    } catch(...)
    {
        LOG_ERROR_MSG("Unexpected error deleting blocks");
        return;
    }
}

void
dupBlocks()
{
    LOG_INFO_MSG("Retrieving large blocks and duplicating first 5 found.");
    BlockFilter blockFilter;
    string prefix("IBMTest");

    try {

        blockFilter.setExtendedInfo(true);
        Block::Ptrs blocksVector = bgsched::core::getBlocks(blockFilter);

        int maxBlocks = 0;
        if (blocksVector.empty()) {
            LOG_WARN_MSG("Found no blocks to duplicate.");
        } else {
            for (Block::Ptrs::iterator iter = blocksVector.begin(); iter != blocksVector.end(); ++iter) {
                Block::Ptr block = *iter;
                if (block->isLarge()) {
                    maxBlocks++;
                    string blockName = SchedUtil::createBlockName(prefix);
                    block->setName(blockName);
                    block->add(block->getOwner());
                    printBlockInfo(block);
                }
                if (maxBlocks >= 5) {
                    break;
                }
            }
        }
    } catch(...)
    {
        LOG_ERROR_MSG("Error duplicating blocks");
        return;
    }
}

void
createBadSmallBlocks()
{
    //  The valid node board and count combinations are:
    //  all positions -- 1
    //  N00, N02, N04, N06, N08, N10, N12, N14 -- 2
    //  N00, N04, N08, N12 -- 4
    //  N00, N08 -- 8
    string owner;
    Block::NodeBoards nodeBoards;
    nodeBoards.push_back("R00-M0-N02");
    // Attempt to create bad (no I/O links) 32 and  64 small blocks
    LOG_INFO_MSG( "=== Create 32 cnode block without I/O link: should FAIL ===" );
    try {
        Block::Ptr block32  = Block::create(nodeBoards); // Create 32 cnode block
        block32->setName("IBMTestBad32"); // Set the name
        block32->setDescription("Bad 32 cnode block"); // Set the description
        block32->add(owner); // Add to db
        printBlockInfo(block32); // Print out the contents
    } catch(...) {
        LOG_ERROR_MSG("Error creating 32 node small block");
    }

    nodeBoards.push_back("R00-M0-N03");
    LOG_INFO_MSG( "=== Create 64 cnode block without I/O link: should FAIL ===" );
    try {
        Block::Ptr block64  = Block::create(nodeBoards);  // Create 64 cnode block
        block64->setName("IBMTestBad64"); // Set the name
        block64->setDescription("bad 64 cnode block"); // Set the description
        block64->add(owner); // Add to db
        printBlockInfo(block64); // Print out the contents
    } catch(...) {
        LOG_ERROR_MSG("Error creating 64 node small block");
    }

    nodeBoards.clear();
    nodeBoards.push_back("R00-M0-N99");
    nodeBoards.push_back("R00-M0-N9A");
    LOG_INFO_MSG( "=== Create 64 cnode block with bad node board location: should FAIL ===" );
    try {
        Block::Ptr block64  = Block::create(nodeBoards);  // Create 64 cnode block
        block64->setName("IBMTestBad64"); // Set the name
        block64->setDescription("bad 64 cnode block"); // Set the description
        block64->add(owner); // Add to db
        printBlockInfo(block64); // Print out the contents
    } catch(...) {
        LOG_ERROR_MSG("Error creating 64 node small block");
    }

    // Attempt to create bad (no I/O links) 32 and  64 small blocks
    LOG_INFO_MSG( "=== Create 32 cnode block without I/O link: should FAIL ===" );
    try {
        Block::Ptr block32  = Block::create("R00-M0-N02", 1); // Create 32 cnode block
        block32->setName("IBMTestBadx32"); // Set the name
        block32->setDescription("32 cnode block"); // Set the description
        block32->add(owner); // Add to db
        printBlockInfo(block32); // Print out the contents
    } catch(...) {
        LOG_ERROR_MSG("Error creating 32 node small block");
    }

    LOG_INFO_MSG( "=== Create 64 cnode block without I/O link: should FAIL ===" );
    try {
        Block::Ptr block64  = Block::create("R00-M0-N02", 2);  // Create 64 cnode block
        block64->setName("IBMTestBadx64"); // Set the name
        block64->setDescription("64 cnode block"); // Set the description
        block64->add(owner); // Add to db
        printBlockInfo(block64); // Print out the contents
    } catch(...) {
        LOG_ERROR_MSG("Error creating 64 node small block");
    }

    LOG_INFO_MSG( "=== Create 64 cnode block with bad node board location: should FAIL ===" );
    try {
        Block::Ptr block64  = Block::create("R00-M0-N0A", 2);  // Create 64 cnode block
        block64->setName("IBMTestBadxx64"); // Set the name
        block64->setDescription("64 cnode block"); // Set the description
        block64->add(owner); // Add to db
        printBlockInfo(block64); // Print out the contents
    } catch(...) {
        LOG_ERROR_MSG("Error creating 64 node small block");
    }
}

void
createSmallBlocks()
{
    try {
        string owner;
        //  The valid node board and count combinations are:
        //  all positions -- 1
        //  N00, N02, N04, N06, N08, N10, N12, N14 -- 2
        //  N00, N04, N08, N12 -- 4
        //  N00, N08 -- 8
        // Create 32, 64, 128, 256 cnode small blocks
        LOG_INFO_MSG( "=== Create 32 cnode block: should complete OK ===" );
        Block::Ptr block32  = Block::create("R00-M0-N00", 1); // Create 32 cnode block
        block32->setName("IBMTest32"); // Set the name
        //block32->setDescription("32 cnode block"); // Set the description
        block32->add(owner); // Add to db
        printBlockInfo(block32); // Print out the contents

        LOG_INFO_MSG( "=== Create 64 cnode block: should complete OK ===" );
        Block::Ptr block64  = Block::create("R00-M0-N04", 2);  // Create 64 cnode block
        block64->setName("IBMTest64"); // Set the name
        //block64->setDescription("64 cnode block"); // Set the description
        block64->add(owner); // Add to db
        printBlockInfo(block64); // Print out the contents

        LOG_INFO_MSG( "=== Create 128 cnode block: should complete OK ===" );
        Block::Ptr block128 = Block::create("R00-M0-N12", 4); // Create 128 cnode block
        block128->setName("IBMTest128"); // Set the name
        //block128->setDescription("128 cnode block"); // Set the description
        block128->add(owner); // Add to db
        printBlockInfo(block128); // Print out the contents

        LOG_INFO_MSG( "=== Create 256 cnode block: should complete OK ===" );
        Block::Ptr block256 = Block::create("R00-M0-N08", 8); // Create 256 cnode block
        block256->setName("IBMTest256"); // Set the name
        //block256->setDescription("256 cnode block"); // Set the description
        block256->add(owner); // Add to db
        printBlockInfo(block256); // Print out the contents

        Block::NodeBoards nodeBoards;
        nodeBoards.push_back("R00-M0-N00");
        // Attempt to create good (with I/O links) 32 and  64 small blocks
        LOG_INFO_MSG( "=== Create 32 cnode block with I/O link: should complete OK ===" );
        Block::Ptr block32Good  = Block::create(nodeBoards); // Create 32 cnode block
        block32Good->setName("IBMTestGood32"); // Set the name
        //block32Good->setDescription("Good 32 cnode block"); // Set the description
        block32Good->add(owner); // Add to db
        printBlockInfo(block32Good); // Print out the contents

        nodeBoards.push_back("R00-M0-N01");
        LOG_INFO_MSG( "=== Create 64 cnode block with I/O link: should complete OK ===" );
        Block::Ptr block64Good  = Block::create(nodeBoards); // Create 64 cnode block
        block64Good->setName("IBMTestGood64"); // Set the name
        //block64Good->setDescription("Good 64 cnode block"); // Set the description
        block64Good->add(owner); // Add to db
        printBlockInfo(block64Good); // Print out the contents
    } catch(...)
    {
        LOG_ERROR_MSG("Unexpected error creating small blocks");
        return;
    }
}
void
createLargeBlocks()
{
    try {
        string owner;
        Block::Ptr block;
        Block::Midplanes midplanes;
        Block::PassthroughMidplanes passthroughMidplanes;
        Block::DimensionConnectivity dimensionConnectivity;
/*
        LOG_INFO_MSG( "=== Create 1024 compute node block... 1x2x1x1 TEST" );
        midplanes.clear();
        passthroughMidplanes.clear();
        dimensionConnectivity.clear();

        dimensionConnectivity[Dimension::A] = Block::Connectivity::Torus;
        dimensionConnectivity[Dimension::B] = Block::Connectivity::Mesh;
        dimensionConnectivity[Dimension::C] = Block::Connectivity::Torus;
        dimensionConnectivity[Dimension::D] = Block::Connectivity::Torus;

        // Set midplanes
        midplanes.push_back("R00-M0");
        midplanes.push_back("R20-M0");

        LOG_INFO_MSG("Midplane size: " << midplanes.size() << " pass size: " << passthroughMidplanes.size());
        block = Block::create(midplanes, passthroughMidplanes, dimensionConnectivity);

        LOG_INFO_MSG("after block create call..");
*/

/*
        LOG_INFO_MSG( "=== Create 8192 cnode block: should complete OK ===" );
        //------------------------------------------------
        // Create 8192 cnode large block (any configuration)
        //-------------------------------------------------
        // Set connectivity to Torus in all dimensions
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim)
        {
            dimensionConnectivity[dim] = Block::Connectivity::Torus;
        }
        // Set midplane
        midplanes.push_back("R02-M0");
        midplanes.push_back("R02-M1");
        midplanes.push_back("R03-M0");
        midplanes.push_back("R03-M1");
        midplanes.push_back("R22-M0");
        midplanes.push_back("R22-M1");
        midplanes.push_back("R23-M0");
        midplanes.push_back("R23-M1");
        midplanes.push_back("R32-M0");
        midplanes.push_back("R32-M1");
        midplanes.push_back("R33-M0");
        midplanes.push_back("R33-M1");
        midplanes.push_back("R12-M0");
        midplanes.push_back("R12-M1");
        midplanes.push_back("R13-M0");
        midplanes.push_back("R13-M1");
        block = Block::create(midplanes, passthroughMidplanes, dimensionConnectivity);
        block->setName("IBMTestSlurm"); // Set the name
        block->setDescription("8192 slurm boot failed block"); // Set the description
        block->add(owner); // Add to db
        printBlockInfo(block); // Print out the contents
        LOG_INFO_MSG( "=== Boot slurm block ===" );
        Block::initiateBoot(block->getName());
*/

        LOG_INFO_MSG( "=== Create 512 cnode block: should complete OK ===" );
        //------------------------------------------------
        // Create 512 cnode large block (any configuration)
        //-------------------------------------------------
        // Set connectivity to Torus in all dimensions
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim)
        {
            dimensionConnectivity[dim] = Block::Connectivity::Torus;
        }
        // Set midplane
        midplanes.push_back("R00-M1");
        block = Block::create(midplanes, passthroughMidplanes, dimensionConnectivity);
        block->setName("IBMTest512"); // Set the name
        //block->setDescription("512 cnode block"); // Set the description
        block->add(owner); // Add to db
        printBlockInfo(block); // Print out the contents

/*
        LOG_INFO_MSG( "=== Create 1024 cnode block: should complete OK ===" );
        //------------------------------------------------
        // Create 1024 cnode large block (any configuration)
        //------------------------------------------------
        midplanes.clear();
        passthroughMidplanes.clear();
        dimensionConnectivity.clear();
        // Set connectivity to Torus in all dimensions
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim)
        {
            dimensionConnectivity[dim] = Block::Connectivity::Torus;
        }
        dimensionConnectivity[Dimension::D] = Block::Connectivity::Mesh;
        // Set midplanes
        midplanes.push_back("R00-M0");
        midplanes.push_back("R00-M1");
        block = Block::create(midplanes, passthroughMidplanes, dimensionConnectivity);
        block->setName("IBMTest1024"); // Set the name
        block->setDescription("1024 cnode block"); // Set the description
        block->add(owner); // Add to db
        printBlockInfo(block); // Print out the contents

        LOG_INFO_MSG( "=== Create 2048 cnode block: should complete OK ===" );
        //------------------------------------------------
        // Create 2048 cnode large block (1x2 or larger configuration)
        //------------------------------------------------
        midplanes.clear();
        passthroughMidplanes.clear();
        dimensionConnectivity.clear();
        // Set connectivity to Torus in all dimensions
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim)
        {
            dimensionConnectivity[dim] = Block::Connectivity::Torus;
        }
        // Set midplanes
        midplanes.push_back("R00-M0");
        midplanes.push_back("R00-M1");
        midplanes.push_back("R01-M0");
        midplanes.push_back("R01-M1");
        block = Block::create(midplanes, passthroughMidplanes, dimensionConnectivity);
        block->setName("IBMTest2048"); // Set the name
        block->setDescription("2048 cnode block"); // Set the description
        block->add(owner); // Add to db
        printBlockInfo(block); // Print out the contents

        LOG_INFO_MSG( "=== Create 2048 cnode block in B dim: should complete OK ===" );
        //------------------------------------------------
        // Create 2048 cnode large block in B dim (4x4 configuration)
        //------------------------------------------------
        midplanes.clear();
        passthroughMidplanes.clear();
        dimensionConnectivity.clear();
        // Set connectivity to Torus in all dimensions
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim)
        {
            dimensionConnectivity[dim] = Block::Connectivity::Torus;
        }
        // Set midplanes
        midplanes.push_back("R00-M0");
        midplanes.push_back("R10-M0");
        midplanes.push_back("R20-M0");
        midplanes.push_back("R30-M0");

        block = Block::create(midplanes, passthroughMidplanes, dimensionConnectivity);
        block->setName("IBMTest20484x4Bdim"); // Set the name
        block->setDescription("2048 cnode block in B dim"); // Set the description
        block->add(owner); // Add to db
        printBlockInfo(block); // Print out the contents

        LOG_INFO_MSG( "=== Create 1536 cnode block with single midplane passthru: should complete OK ===" );
        //------------------------------------------------
        // Create a 1536 cnode large block with single midplane pass-through in B dim (4x4 configuration)
        //------------------------------------------------
        midplanes.clear();
        passthroughMidplanes.clear();
        dimensionConnectivity.clear();
        // Set connectivity to Torus in all dimensions
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim)
        {
            dimensionConnectivity[dim] = Block::Connectivity::Torus;
        }
        // Set midplanes
        midplanes.push_back("R00-M0");
        midplanes.push_back("R30-M0");
        midplanes.push_back("R10-M0");
        // Set passthrough midplanes
        passthroughMidplanes.push_back("R20-M0");
        block = Block::create(midplanes, passthroughMidplanes, dimensionConnectivity);
        block->setName("IBMTest15364x4BdimPassthruR20M0"); // Set the name
        block->setDescription("1536 cnode block in B dim with R20-M0 passthru"); // Set the description
        block->add(owner); // Add to db
        printBlockInfo(block); // Print out the contents

        LOG_INFO_MSG( "=== Create 1024 cnode block with two midplane passthru in B dim: should complete OK ===" );
        //------------------------------------------------
        // Create a 1024 cnode large block with two midplane pass-through in B dim (4x4 configuration)
        //------------------------------------------------
        midplanes.clear();
        passthroughMidplanes.clear();
        dimensionConnectivity.clear();
        // Set connectivity to Torus in all dimensions
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim)
        {
            dimensionConnectivity[dim] = Block::Connectivity::Torus;
        }
        // Set midplanes
        midplanes.push_back("R00-M0");
        midplanes.push_back("R30-M0");
        // Set pass-through midplanes
        passthroughMidplanes.push_back("R10-M0");
        passthroughMidplanes.push_back("R20-M0");
        block = Block::create(midplanes, passthroughMidplanes, dimensionConnectivity);
        block->setName("IBMTest10244x4BdimPTR10M0R20M0"); // Set the name
        block->setDescription("1024 cnode block in B dim with R10-M0 & R20-M0 passthru"); // Set the description
        block->add(owner); // Add to db
        printBlockInfo(block); // Print out the contents

        LOG_INFO_MSG( "=== Create 1536 cnode block as a mesh in B dim: should complete OK ===" );
        //------------------------------------------------
        // Create a 1536 cnode large block as a mesh in B dim (4x4 configuration)
        //------------------------------------------------
        midplanes.clear();
        passthroughMidplanes.clear();
        dimensionConnectivity.clear();
        // Set connectivity
        dimensionConnectivity[Dimension::A] = Block::Connectivity::Torus;
        dimensionConnectivity[Dimension::B] = Block::Connectivity::Mesh;
        dimensionConnectivity[Dimension::C] = Block::Connectivity::Torus;
        dimensionConnectivity[Dimension::D] = Block::Connectivity::Torus;
        // Set midplanes
        midplanes.push_back("R00-M0");
        midplanes.push_back("R20-M0");
        midplanes.push_back("R30-M0");
        block = Block::create(midplanes, passthroughMidplanes, dimensionConnectivity);
        block->setName("IBMTest15364x4BdimMesh"); // Set the name
        block->setDescription("1536 cnode block with mesh in B dim"); // Set the description
        block->add(owner); // Add to db
        printBlockInfo(block); // Print out the contents

        LOG_INFO_MSG( "=== Create 1024 cnode block as a mesh in B dim: should FAIL ===" );
        //------------------------------------------------
        // Create a 1024 cnode large block as a mesh in B dim (4x4 configuration) - should FAIL
        // because missing a midplane in a mesh
        //------------------------------------------------
        midplanes.clear();
        passthroughMidplanes.clear();
        dimensionConnectivity.clear();
        // Set connectivity
        dimensionConnectivity[Dimension::A] = Block::Connectivity::Torus;
        dimensionConnectivity[Dimension::B] = Block::Connectivity::Mesh;
        dimensionConnectivity[Dimension::C] = Block::Connectivity::Torus;
        dimensionConnectivity[Dimension::D] = Block::Connectivity::Torus;
        // Set midplanes
        midplanes.push_back("R00-M0");
        midplanes.push_back("R30-M0");
        block = Block::create(midplanes, passthroughMidplanes, dimensionConnectivity);
        block->setName("IBMTestshouldfail"); // Set the name
        block->setDescription("should fail"); // Set the description
        block->add(owner); // Add to db
        printBlockInfo(block); // Print out the contents
 */

    } catch (const bgsched::InputException& e) {
        switch (e.getError().toValue())
        {
            case bgsched::InputErrors::InvalidMidplanes:
                LOG_INFO_MSG("Invalid midplanes given for Block::Create()");
                return;
            case  bgsched::InputErrors::InvalidConnectivity:
                LOG_INFO_MSG("Invalid connectivity given for Block::Create()");
                return;
            case bgsched::InputErrors::BlockNotCreated:
                LOG_INFO_MSG("Block::Create() can not create block from input arguments");
                return;
            default:
                LOG_INFO_MSG("Unexpected exception value from Block::Create()");
                return;
        }
    }
}

void
createPassthruBlocks()
{
    try {
        string owner;
        Block::Ptr block;
        Block::Midplanes midplanes;
        Block::PassthroughMidplanes passthroughMidplanes;
        Block::DimensionConnectivity dimensionConnectivity;

        LOG_INFO_MSG( "=== Create 8192 cnode block with passthru: should complete OK ===" );
        //------------------------------------------------
        // Create 8192 cnode large block with passthru
        //-------------------------------------------------
        // Set connectivity to Torus in all dimensions
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim)
        {
            dimensionConnectivity[dim] = Block::Connectivity::Torus;
        }
        // Set midplanes
        midplanes.push_back("R09-M1");
        midplanes.push_back("R09-M0");
        midplanes.push_back("R0B-M0");
        midplanes.push_back("R0B-M1");
        midplanes.push_back("R0D-M0");
        midplanes.push_back("R0D-M1");
        midplanes.push_back("R0F-M0");
        midplanes.push_back("R0F-M1");
        midplanes.push_back("R31-M0");
        midplanes.push_back("R31-M1");
        midplanes.push_back("R33-M0");
        midplanes.push_back("R33-M1");
        midplanes.push_back("R35-M0");
        midplanes.push_back("R35-M1");
        midplanes.push_back("R37-M0");
        midplanes.push_back("R37-M1");
        // Set passthru midplanes
        passthroughMidplanes.push_back("R01-M0");
        passthroughMidplanes.push_back("R01-M1");
        passthroughMidplanes.push_back("R03-M0");
        passthroughMidplanes.push_back("R03-M1");
        passthroughMidplanes.push_back("R05-M0");
        passthroughMidplanes.push_back("R05-M1");
        passthroughMidplanes.push_back("R07-M0");
        passthroughMidplanes.push_back("R07-M1");
        passthroughMidplanes.push_back("R08-M0");
        passthroughMidplanes.push_back("R08-M1");
        passthroughMidplanes.push_back("R0A-M0");
        passthroughMidplanes.push_back("R0A-M1");
        passthroughMidplanes.push_back("R0C-M0");
        passthroughMidplanes.push_back("R0C-M1");
        passthroughMidplanes.push_back("R0E-M0");
        passthroughMidplanes.push_back("R0E-M1");
        passthroughMidplanes.push_back("R30-M0");
        passthroughMidplanes.push_back("R30-M1");
        passthroughMidplanes.push_back("R32-M0");
        passthroughMidplanes.push_back("R32-M1");
        passthroughMidplanes.push_back("R34-M0");
        passthroughMidplanes.push_back("R34-M1");
        passthroughMidplanes.push_back("R36-M0");
        passthroughMidplanes.push_back("R36-M1");
        passthroughMidplanes.push_back("R39-M0");
        passthroughMidplanes.push_back("R39-M1");
        passthroughMidplanes.push_back("R3B-M0");
        passthroughMidplanes.push_back("R3B-M1");
        passthroughMidplanes.push_back("R3D-M0");
        passthroughMidplanes.push_back("R3D-M1");
        passthroughMidplanes.push_back("R3F-M0");
        passthroughMidplanes.push_back("R3F-M1");
        block = Block::create(midplanes, passthroughMidplanes, dimensionConnectivity);
        block->setName("IBMTestSlurmPassthru"); // Set the name
        block->setDescription("8192 passthru slurm boot failed block"); // Set the description
        block->add(owner); // Add to db
        printBlockInfo(block); // Print out the contents
        LOG_INFO_MSG( "=== Boot slurm block ===" );
        Block::initiateBoot(block->getName());
    } catch (const bgsched::InputException& e) {
        switch (e.getError().toValue())
        {
            case bgsched::InputErrors::InvalidMidplanes:
                LOG_INFO_MSG("Invalid midplanes given for Block::Create()");
                return;
            case  bgsched::InputErrors::InvalidConnectivity:
                LOG_INFO_MSG("Invalid connectivity given for Block::Create()");
                return;
            case bgsched::InputErrors::BlockNotCreated:
                LOG_INFO_MSG("Block::Create() can not create block from input arguments");
                return;
            default:
                LOG_INFO_MSG("Unexpected exception value from Block::Create()");
                return;
        }
    }
}

int
main(int argc, const char** argv)
{
    // iterate through args first to get -properties
    string properties;
    for (int i = 1; i < argc; ++i) {
        if (!strcasecmp(argv[i], "-properties")) {
            if (++i == argc) {
                cerr << "Please give a file name after " << argv[i-1] << endl;
                exit(EXIT_FAILURE);
            } else {
                properties = argv[i];
            }
        } else if (!strcasecmp(argv[i], "-help") || !strcasecmp(argv[i], "-h")) {
            cerr << argv[0] << ":" << endl;
            cerr << "\t -properties <file>     : Set bg.properties file name" << endl;
            cerr << "\t -help | -h             : Print this help text" << endl;
            exit(EXIT_SUCCESS);
        }
    }

    // init library
    init( properties );
    LOG_INFO_MSG( "test_block running testcases (Note: requires 4x4 configuration)" );

    try
    {

        LOG_TRACE_MSG( "Creating allocator framework" );
        Allocator my_alloc;

/*
        LOG_TRACE_MSG( "Obtaining reference to LiveModel from allocator" );
        LiveModel &model(my_alloc.getLiveModel());


        LOG_INFO_MSG( "=== findBlockResources() for 32,64,128,256 cnodes on LiveModel ===" );
        LOG_INFO_MSG( "=== 32 cnodes on LiveModel: should complete OK ===" );
        Block::Ptr block32;
        addBlock(my_alloc, 32, block32);
        model.syncState();
        LOG_INFO_MSG( "=== 64 cnodes on LiveModel: should complete OK ===" );
        Block::Ptr block64;
        addBlock(my_alloc, 64, block64);
        model.syncState();
        LOG_INFO_MSG( "=== 128 cnodes on LiveModel: should complete OK ===" );
        Block::Ptr block128;
        addBlock(my_alloc, 128, block128);
        model.syncState();
        LOG_INFO_MSG( "=== 256 cnodes on LiveModel: should complete OK ===" );
        Block::Ptr block256;
        addBlock(my_alloc, 256, block256);
        model.syncState();
        // Free and remove blocks from testcase
        if (block32) {
            //freeBlock(my_alloc, block32);
            updateBlockDefaults(block32);
            removeBlock(block32->getName());
        }
        if (block64) {
            //freeBlock(my_alloc, block64);
            updateBlock(block64);
            removeBlock(block64->getName());
        }
        if (block128) {
            //freeBlock(my_alloc, block128);
            testBadValues(block128);
            removeBlock(block128->getName());
        }
        if (block256) {
            freeBlock(my_alloc, block256);
            removeBlock(block256->getName());
        }

        // Remove any blocks created from last run of testcase
        removeDupBlocks();

        //LOG_INFO_MSG( "=== Add blocks from existing large blocks ===" );
        //dupBlocks();

*/

        LOG_INFO_MSG( "=== Create small blocks without I/O links ===" );
        createBadSmallBlocks();

        LOG_INFO_MSG( "=== Create small blocks ===" );
        createSmallBlocks();

/*
        LOG_INFO_MSG( "=== Create large blocks ===" );
        createLargeBlocks();

        LOG_INFO_MSG( "=== Create passthru blocks ===" );
        createPassthruBlocks();

        Block::Ptr IBMTestBoot;
        addBlock(my_alloc, 32, IBMTestBoot);

        // Check if block was created
        if (!IBMTestBoot) {
            LOG_ERROR_MSG("No resources to create block so skipping boot test.");
        } else {

            // If block add successful then run boot/free test as well as add/remove user test
            if (model.getBlock(IBMTestBoot->getName())) {
                // Add user to block
                Block::addUser(IBMTestBoot->getName(), "FredFlintstone");
                Block::addUser(IBMTestBoot->getName(), "MickeyMouse");
                Block::addUser(IBMTestBoot->getName(), "MickeyMouse");

                // Get users authorized with "Execute" permissions to block
                vector<string> users;
                users = Block::getUsers(IBMTestBoot->getName());

                // Print out authorized users
                if (users.size() == 0) {
                    LOG_INFO_MSG( "No users with 'execute' authority to block " << IBMTestBoot->getName());
                } else {
                    for (unsigned int it =0; it < users.size(); it++) {
                        LOG_INFO_MSG( "User " << users[it] << " has 'execute' authority to block " << IBMTestBoot->getName());
                    }
                }

                LOG_INFO_MSG( "=== Boot block ===" );
                Block::initiateBoot(IBMTestBoot->getName());
                // Sleep for a short time to allow MMCS to pick-up the boot request before freeing
                uint32_t sleep_sec(10);
                sleep(sleep_sec);
                LOG_INFO_MSG( "=== Free block ===" );
                try {
                    Block::initiateFree(IBMTestBoot->getName());
                    LOG_INFO_MSG( "=== Duplicate free request ===" );
                    sleep(sleep_sec);
                    Block::initiateFree(IBMTestBoot->getName());
                }  catch(...)
                {
                    LOG_ERROR_MSG("Free block request failed ... continuing.");
                }

                // Remove user from block
                Block::removeUser(IBMTestBoot->getName(), "FredFlintstone");
                Block::removeUser(IBMTestBoot->getName(), "FredFlintstone");

                users = Block::getUsers(IBMTestBoot->getName());
                // Print out authorized users
                if (users.size() == 0) {
                    LOG_INFO_MSG( "No users with 'execute' authority to block " << IBMTestBoot->getName());
                } else {
                    for (unsigned int it = 0; it < users.size(); it++) {
                        LOG_INFO_MSG( "User " << users[it] << " has 'execute' authority to block " << IBMTestBoot->getName());
                    }
                }
            }
        }
*/
        // Test failures
        // Block::initiateBoot("NonexistingBlock");
        LOG_INFO_MSG( "=== Test_Block Completed ===" );
        return(0);
    } catch(...)
    {
        LOG_ERROR_MSG( "Exception caught" );
        print_trace();
        exit(-1);
    }
}
