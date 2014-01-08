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

#include "bgsched/test/TestAllocatorEventListener.h"

#include <utility/include/Log.h>

using namespace bgsched;
using namespace bgsched::allocator;

LOG_DECLARE_FILE("testallocator");

//-----------------------------------------------------------------------
// Members of class TestAllocatorEventListener.

TestAllocatorEventListener::TestAllocatorEventListener() :
       _deallocatesProcessed(0),
       _allocatesProcessed(0)
{
    // Nothing to do
}

void
TestAllocatorEventListener::handleAllocate(
        const AllocateCompleted& info
    )
{
    _allocatesProcessed++;
    if (info.successful()) {
        LOG_INFO_MSG("Block " << info.getBlockName() << " allocate successful");
    } else {
        LOG_INFO_MSG("Block " << info.getBlockName() << " allocate failed.");
    }
}

void
TestAllocatorEventListener::handleDeallocate(
        const DeallocateCompleted& info
    )
{
    _deallocatesProcessed++;
    if (info.successful()) {
        LOG_INFO_MSG("Block " << info.getBlockName() << " deallocate successful");
        try {
            Block::remove(info.getBlockName());
        }
        catch(...)
        {
            LOG_ERROR_MSG("Block " << info.getBlockName() << " could not be removed");
        }
    } else {
        LOG_INFO_MSG("Block " << info.getBlockName() << " deallocate failed.");
    }
}

uint32_t
TestAllocatorEventListener::getDeallocateCount()
{
    return _deallocatesProcessed;
}

uint32_t
TestAllocatorEventListener::getAllocateCount()
{
    return _allocatesProcessed;
}

TestAllocatorEventListener::~TestAllocatorEventListener()
{
    // Nothing to do.
}
