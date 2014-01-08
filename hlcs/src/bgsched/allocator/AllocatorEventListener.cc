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

#include <bgsched/allocator/AllocatorEventListener.h>

#include "bgsched/allocator/AllocatorEventListenerImpl.h"

#include <utility/include/Log.h>

using namespace std;

LOG_DECLARE_FILE("bgsched");

namespace bgsched {
namespace allocator {

static ostream& operator<<(
        ostream& os,
        const AllocatorEventListener::AllocateCompleted& info
        )
{
    return os << "{"
            "block='" << info.getBlockName() << "'"
           " status=" << info.getStatus() <<
           " sequenceId=" << info.getSequenceId() <<
           " successful=" << (info.successful()?"true":"false") <<
        "}";
}

static ostream& operator<<(
        ostream& os,
        const AllocatorEventListener::DeallocateCompleted& info
        )
{
    return os << "{"
            "block='" << info.getBlockName() << "'"
           " status=" << info.getStatus() <<
           " sequenceId=" << info.getSequenceId() <<
           " successful=" << (info.successful()?"true":"false") <<
        "}";
}

AllocatorEventListener::BlockEvent::BlockEvent(
        Pimpl impl
        ) :
    _impl(impl)
{

}

const string&
AllocatorEventListener::BlockEvent::getBlockName() const
{
    return _impl->getBlockName();
}


Block::Status
AllocatorEventListener::BlockEvent::getStatus() const
{
    return _impl->getStatus();
}


SequenceId
AllocatorEventListener::BlockEvent::getSequenceId() const
{
    return _impl->getSequenceId();
}

bool
AllocatorEventListener::BlockEvent::successful() const
{
    return _impl->successful();
}

/*
const string&
AllocatorEventListener::BlockEvent::getErrorMessage() const
{
    return _impl->getErrorMessage();
}
*/

AllocatorEventListener::AllocateCompleted::AllocateCompleted(
        const Pimpl impl
        ) :
    BlockEvent(impl)
{
    // Nothing to do.
}

AllocatorEventListener::DeallocateCompleted::DeallocateCompleted(
        const Pimpl impl
        ) :
    BlockEvent(impl)
{
    // Nothing to do.
}

void
AllocatorEventListener::handleAllocate(
        const AllocateCompleted& info
        )
{
    LOG_INFO_MSG("Ignoring block allocate completed event. info=" << info);
}


void
AllocatorEventListener::handleDeallocate(
        const DeallocateCompleted& info
        )
{
    LOG_INFO_MSG("Ignoring block deallocate completed event. info=" << info);
}


AllocatorEventListener::~AllocatorEventListener()
{
    // Nothing to do.
}

} // namespace bgsched::allocator
} // namespace bgsched
