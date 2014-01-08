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
#include <bgsched/IOBlock.h>
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

LOG_DECLARE_FILE("testioblocks");

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

const string IOBlockStatusToString(EnumWrapper<IOBlock::Status> IOBlockStatus)
{
    return BLOCK_STATUS_STRINGS[IOBlockStatus.toValue()];
}

const string IOBlockActionToString(EnumWrapper<IOBlock::Action::Value> IOBlockAction)
{
    return BLOCK_ACTION_STRINGS[IOBlockAction.toValue()];
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
updateIOBlock(IOBlock::Ptr& block)
{
    LOG_INFO_MSG("Before update: Dumping contents of I/O block");
    printIOBlockInfo(block);

    // Update block with changed settings
    try {
        block->setDescription("New block description");
        block->setBootOptions("1234BootOptions");
        block->setMicroLoaderImage("theNewMicroloaderImageFilePath");
        // block->setNodeConfiguration("NodeConfigName");
        LOG_INFO_MSG("After updating settings: Dumping contents of I/O block");
        printIOBlockInfo(block);
        block->update();
        LOG_INFO_MSG("I/O block " << block->getName() << " was updated");
    }
    catch(...)
    {
        LOG_ERROR_MSG("I/O block " << block->getName() << " was not updated");
        return;
    }
}

void
testBadValues(IOBlock::Ptr& block)
{
    LOG_INFO_MSG("Before update: Dumping contents of I/O block");
    printIOBlockInfo(block);

    // Try setting bad description (max 1024 chars)
    try {
        string desc;
        block->setDescription(desc); // Empty string
    }
    catch(...)
    {
        LOG_ERROR_MSG("Set description failed on block " << block->getName());
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

    LOG_INFO_MSG("After updating settings: Dumping contents of I/O block");
    printIOBlockInfo(block);

    try {
        block->update();
    }
    catch(...)
    {
        LOG_ERROR_MSG("I/O block " << block->getName() << " was not updated");
        return;
    }

    LOG_INFO_MSG("I/O block " << block->getName() << " was updated");
}

void
printIOBlocks()
{
    IOBlockFilter blockFilter;

    try {
        IOBlock::Ptrs blocksVector = bgsched::core::getIOBlocks(blockFilter);

        if (blocksVector.empty()) {
            LOG_WARN_MSG("Found no I/O blocks");
        } else {
            LOG_WARN_MSG("Found following " << blocksVector.size() <<  " I/O blocks:");
            for (IOBlock::Ptrs::iterator iter = blocksVector.begin(); iter != blocksVector.end(); ++iter) {
                IOBlock::Ptr block = *iter;
                printIOBlockInfo(block);
            }
        }
    } catch(...)
    {
        LOG_ERROR_MSG("Unexpected error getting I/O blocks");
        return;
    }
}

int
main(int argc, const char** argv)
{
    bool isListBlocks = false;
    bool isCreateBlock = false;
    bool isBootBlock = false;
    bool isBootBlockHoles = false;
    bool isFreeBlock = false;

    IOBlock::Ptr IOBlockPtr;

    string properties;
    string createBlock;
    string startLocation;
    uint32_t IONodeCount = 0;
    string bootBlock;
    string bootBlockHoles;
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
                cerr << "Please give an I/O block name after " << argv[i-1] << endl;
                exit(EXIT_FAILURE);
            } else {
                createBlock = argv[i];
                if (++i == argc) {
                    cerr << "Please give a starting location after " << argv[i-2] << endl;
                    exit(EXIT_FAILURE);
                } else {
                    startLocation = argv[i];
                    if (++i == argc) {
                        cerr << "Please give I/O node count after " << argv[i-3] << endl;
                        exit(EXIT_FAILURE);
                    } else {
                        IONodeCount = boost::lexical_cast<uint32_t>(argv[i]);
                    }
                }
            }
        } else if (!strcasecmp(argv[i], "-bootblock")) {
            isBootBlock = true;
            if (++i == argc) {
                cerr << "Please give an I/O block name after " << argv[i-1] << endl;
                exit(EXIT_FAILURE);
            } else {
                bootBlock = argv[i];
            }
        } else if (!strcasecmp(argv[i], "-bootblockholes")) {
            isBootBlockHoles = true;
            if (++i == argc) {
                cerr << "Please give an I/O block name after " << argv[i-1] << endl;
                exit(EXIT_FAILURE);
            } else {
                bootBlockHoles = argv[i];
            }
        } else if (!strcasecmp(argv[i], "-freeblock")) {
            isFreeBlock = true;
            if (++i == argc) {
                cerr << "Please give an I/O block name after " << argv[i-1] << endl;
                exit(EXIT_FAILURE);
            } else {
                freeBlock = argv[i];
            }
        } else if (!strcasecmp(argv[i], "-help") || !strcasecmp(argv[i], "-h")) {
            cout << argv[0] << ":" << endl;
            cout << "\t -listblocks                                 : Print out I/O blocks on system" << endl;
            cout << "\t -createblock name, startlocation, numnodes  : Create I/O block" << endl;
            cout << "\t -bootblock I/O block                        : Boot I/O block" << endl;
            cout << "\t -bootblockholes I/O block                   : Boot I/O block, allow holes" << endl;
            cout << "\t -freeblock I/O block                        : Free I/O block" << endl;
            cout << "\t -properties <file>                          : Set bg.properties file name" << endl;
            cout << "\t -help | -h                                  : Print this help text" << endl;
            exit(EXIT_SUCCESS);
        }
    }

    // init library
    init( properties );
    LOG_INFO_MSG( "test_ioblock running testcases" );

    try {
        if (isListBlocks) {
            printIOBlocks();
        }

        if (isCreateBlock) {
            LOG_INFO_MSG("Creating I/O block " << createBlock );
            IOBlockPtr = IOBlock::create(createBlock, startLocation, IONodeCount, "created by test pgm");
            printIOBlockInfo(IOBlockPtr);
        }

        if (isBootBlock) {
            std::vector<std::string> unavailableResources;   // Unavailable I/O resources
            std::vector<std::string> IONodesInError;         // I/O nodes in error
            LOG_INFO_MSG("Booting I/O block " << bootBlock << ", no holes allowed.");
            try {
                IOBlock::initiateBoot(bootBlock, false, &unavailableResources, &IONodesInError);
            } catch(...)
            {
                if (unavailableResources.empty()) {
                    // LOG_INFO_MSG("All I/O resources are available for I/O block " << bootBlock);
                } else {
                    // Print the unavailable I/O resources
                    LOG_INFO_MSG("I/O block " << bootBlock << " has " << unavailableResources.size() << " unavailable resources:");
                    for (unsigned int it = 0; it < unavailableResources.size(); it++) {
                        LOG_INFO_MSG(unavailableResources[it]);
                    }
                }
                LOG_ERROR_MSG("Failed to boot I/O block " << bootBlock);
                exit(-1);
            }
            if (unavailableResources.empty()) {
                LOG_INFO_MSG("All I/O resources are available for I/O block " << bootBlock);
            }
        }

        if (isBootBlockHoles) {
            std::vector<std::string> unavailableResources;   // Unavailable I/O resources
            std::vector<std::string> IONodesInError;         // I/O nodes in error
            LOG_INFO_MSG("Booting I/O block " << bootBlockHoles << ", will allow holes.");
            IOBlock::initiateBoot(bootBlockHoles, true, &unavailableResources, &IONodesInError);
            if (IONodesInError.empty()) {
                LOG_INFO_MSG("All I/O nodes are available for I/O block " << bootBlockHoles);
            } else {
                // Print the count of I/O nodes in error and names of I/O nodes
                LOG_INFO_MSG("I/O block " << bootBlockHoles << " has " << IONodesInError.size() << " in error:");
                for (unsigned int it = 0; it < IONodesInError.size(); it++) {
                    LOG_INFO_MSG(IONodesInError[it]);
                }
            }
        }

        if (isFreeBlock) {
            IOBlock::initiateFree(freeBlock);
        }

        LOG_INFO_MSG( "=== test_ioblock Completed ===" );
        return(0);
    } catch(...)
    {
        LOG_ERROR_MSG("Exception caught");
        exit(-1);
    }
}
