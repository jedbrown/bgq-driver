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

#include <bgsched/realtime/Client.h>
#include <bgsched/realtime/ClientEventListener.h>

#include <utility/include/Log.h>

using namespace bgsched;
using namespace bgsched::realtime;
using namespace std;

LOG_DECLARE_FILE("realtime.tester");

static const string BLOCK_STATUS_STRINGS[] = {
        "ALLOCATED",
        "BOOTING",
        "FREE",
        "INITIALIZED",
        "TERMINATING"
};

const string blockStatusToString(Block::Status blockStatus)
{
    return BLOCK_STATUS_STRINGS[blockStatus];
}

/*!
 *  \brief Handle compute block status changes
 */
class EventHandler: public bgsched::realtime::ClientEventListener
{
public:

    EventHandler()  { }

    ~EventHandler() { }

    /*!
     *  \brief Handle a real-time started event.
     */
    void handleRealtimeStartedRealtimeEvent(const RealtimeStartedEventInfo& /*info*/)  { }

    /*!
     *  \brief Get the indicator of whether to continue receiving messages or not.
     *
     *  The real-time client will call this method after calling any of the methods in this
     *  class to determine if the client should continue to process real-time events. If this
     *  method returns false then the client will not continue and receiveMessages() will return.
     */
    bool getRealtimeContinue()
    {
        LOG_INFO_MSG("Setting indicator to continue receiving messages from real-time.");
        return true;
    }

    /*!
     *  \brief Handle a block state changed real-time event.
     */
    void handleBlockStateChangedRealtimeEvent(const BlockStateChangedEventInfo& eventInfo)
    {
        LOG_INFO_MSG("Received block status changed real-time event. Block=" <<
                eventInfo.getBlockName() <<
                " Old status=" << blockStatusToString(eventInfo.getPreviousStatus()) <<
                " New Status=" << blockStatusToString(eventInfo.getStatus())
                );
    }

    /*!
     *  \brief Handle a node status changed real-time event.
     */
    void handleNodeStateChangedRealtimeEvent(const NodeStateChangedEventInfo& eventInfo)
    {
        LOG_INFO_MSG("Received node status changed real-time event. Node=" <<
                eventInfo.getLocation() <<
                " Old state=" << eventInfo.getPreviousState() <<
                " New state=" << eventInfo.getState()
                );
    }
};

int main(int /*argc*/, char** /*argv[]*/)
{
    string properties;
    // Initialize logger and database
    bgsched::init(properties);

    LOG_INFO_MSG("Creating real-time client to handle block events.");
    Client rt_client;

    EventHandler event_hand; // Event handler

    Filter::BlockStatuses block_statuses;

    Filter rt_filter(Filter::createNone());
    rt_filter.setBlocks(true); // Filter on blocks
    rt_filter.setNodes(true); // Filter on nodes

    // Filter on following status changes
    block_statuses.insert(Block::Free);
    block_statuses.insert(Block::Booting);
    block_statuses.insert(Block::Initialized);
    block_statuses.insert(Block::Terminating);
    rt_filter.setBlockStatuses(&block_statuses);

    // Set the event handler that receives block status change messages
    rt_client.addListener(event_hand);

    LOG_DEBUG_MSG("Connecting real-time client to real-time server.");
    rt_client.connect();

    bgsched::realtime::Filter::Id filter_id; // Assigned filter id

    rt_client.setFilter(rt_filter, &filter_id, NULL);

    // Request real-time updates from the server. The client attempts to send the request. If the
    // send is  interrupted then the client will repeatedly attempt to send the request until it's
    // not interrupted.
    LOG_INFO_MSG("Requesting updates on the real-time client for block status changes.");
    rt_client.requestUpdates(NULL);

    LOG_TRACE_MSG("Receive real-time messages.");
    // If the real-time server closes the connection then the "end" indicator is set
    // to true and the client is disconnected.
    bool end = false;
    rt_client.receiveMessages(NULL, NULL, &end);
}
