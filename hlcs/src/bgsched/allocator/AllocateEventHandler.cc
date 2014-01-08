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

#include "bgsched/allocator/AllocateEventHandler.h"
#include "bgsched/allocator/LiveModelImpl.h"

#include "bgsched/BlockImpl.h"

#include "bgsched/realtime/ClientEventListenerImpl.h"

#include <utility/include/Log.h>

using namespace bgsched;
using namespace bgsched::allocator;
using namespace bgsched::realtime;

LOG_DECLARE_FILE("bgsched");

namespace bgsched {
namespace allocator {

//-----------------------------------------------------------------------
// Members of class AllocateEventHandler.

AllocateEventHandler::AllocateEventHandler(
        LiveModel::Impl* model
) :
    _model(model),
    _isRealtimeEndedEvent(false)
{
    // Nothing to do
}

AllocateEventHandler::~AllocateEventHandler()
{
    // Nothing to do.
}

void
AllocateEventHandler::handleRealtimeStartedRealtimeEvent(
        const RealtimeStartedEventInfo& /* info */
    )
{
    LOG_TRACE_MSG("Setting allocate thread ready indicator.");
    _model->setAllocateThreadReady();
}

void
AllocateEventHandler::handleRealtimeEndedRealtimeEvent(
        const RealtimeEndedEventInfo& /* info */
    )
{
    _isRealtimeEndedEvent = true;
}

bool
AllocateEventHandler::getRealtimeContinue()
{
    // Return from receiving real-time messages
    return false;
}

void
AllocateEventHandler::handleBlockStateChangedRealtimeEvent(
        const BlockStateChangedEventInfo& eventInfo
    )
{
    LOG_TRACE_MSG("Received block status changed real-time event. Block="
            << eventInfo.getBlockName() << " Status=" << eventInfo.getStatus());

    // Ignore anything other than Free or Initialized requests
    if ((eventInfo.getStatus() == Block::Free) || (eventInfo.getStatus() == Block::Initialized)) {

        // Verify it is for a block we are monitoring on the model
        LiveModel::Impl::AllocatingBlocks blocks = _model->getAllocatingBlocks();
        LiveModel::Impl::AllocatingBlocks::iterator it = blocks.find(eventInfo.getBlockName());
        if (it != blocks.end()) {
            // Found a match so send notification to listeners
            LOG_TRACE_MSG("Notifying registered allocate listeners of block status change for compute block " << eventInfo.getBlockName());

            realtime::ClientEventListener::BlockStateChangedEventInfo::Impl blockEvent(
                    eventInfo.getBlockName(),
                    eventInfo.getStatus(),
                    eventInfo.getSequenceId(),
                    eventInfo.getPreviousStatus(),
                    eventInfo.getPreviousSequenceId()
                );

            // Post block status event to listeners
            std::string emptyErrorMessage;
            _model->notifyAllocateListeners(blockEvent, emptyErrorMessage);

            // Remove block from the tracking list
            _model->removeAllocatingBlock(eventInfo.getBlockName());
        }
    }
}

bool
AllocateEventHandler::isRealtimeEndedEvent()
{
    return _isRealtimeEndedEvent;
}

} // namespace bgsched::allocator
} // namespace bgsched
