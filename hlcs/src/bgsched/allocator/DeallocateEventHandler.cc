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

#include "bgsched/allocator/DeallocateEventHandler.h"
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
// Members of class DeallocateEventHandler.

DeallocateEventHandler::DeallocateEventHandler(
        LiveModel::Impl* model
) :
    _model(model),
    _isRealtimeEndedEvent(false)
{
    // Nothing to do.
}

DeallocateEventHandler::~DeallocateEventHandler()
{
    // Nothing to do.
}

void
DeallocateEventHandler::handleRealtimeStartedRealtimeEvent(
        const RealtimeStartedEventInfo& /* info */
    )
{
    LOG_TRACE_MSG("Setting deallocate thread ready indicator.");
    _model->setDeallocateThreadReady();
}

void
DeallocateEventHandler::handleRealtimeEndedRealtimeEvent(
        const RealtimeEndedEventInfo& /* info */
    )
{
    _isRealtimeEndedEvent = true;
}

bool
DeallocateEventHandler::getRealtimeContinue()
{
    // Return from receiving real-time messages
    return false;
}

void
DeallocateEventHandler::handleBlockStateChangedRealtimeEvent(
        const BlockStateChangedEventInfo& eventInfo
    )
{
    LOG_TRACE_MSG("Received block status changed real-time event. Block="
            << eventInfo.getBlockName() << " Status=" << eventInfo.getStatus());

    // Ignore anything other than Free requests
    if (eventInfo.getStatus() == Block::Free) {

        // Verify it is for a block we are monitoring on the model
        LiveModel::Impl::DeallocatingBlocks blocks = _model->getDeallocatingBlocks();
        LiveModel::Impl::DeallocatingBlocks::iterator it = blocks.find(eventInfo.getBlockName());
        if (it != blocks.end()) {
            // Found a match so send notification to listeners
            LOG_TRACE_MSG("Notifying registered deallocate listeners of block status change for compute block " << eventInfo.getBlockName());
            realtime::ClientEventListener::BlockStateChangedEventInfo::Impl blockEvent(
                    eventInfo.getBlockName(),
                    eventInfo.getStatus(),
                    eventInfo.getSequenceId(),
                    eventInfo.getPreviousStatus(),
                    eventInfo.getPreviousSequenceId()
            );
            // Post block status event to listeners
            std::string emptyErrorMessage;
            _model->notifyDeallocateListeners(blockEvent, emptyErrorMessage);

            // Remove block from the tracking list
            _model->removeDeallocatingBlock(eventInfo.getBlockName());
        }
    }
}

bool
DeallocateEventHandler::isRealtimeEndedEvent()
{
    return _isRealtimeEndedEvent;
}

} // namespace bgsched::allocator
} // namespace bgsched

