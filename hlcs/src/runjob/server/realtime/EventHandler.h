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

#ifndef RUNJOB_SERVER_REALTIME_EVENT_HANDLER_H
#define RUNJOB_SERVER_REALTIME_EVENT_HANDLER_H

#include "common/error.h"

#include "server/realtime/fwd.h"
#include "server/fwd.h"

#include <bgsched/realtime/ClientEventListener.h>
#include <bgsched/Block.h>
#include <bgsched/types.h>

#include <boost/asio/strand.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <queue>

namespace runjob {
namespace server {
namespace realtime {

/*!
 * \brief Handles events from the real-time server
 *
 * \see block::Reconnect
 * \see Connection
 * \see Polling
 */
class EventHandler : public bgsched::realtime::ClientEventListener
{
public:
    /*!
     * \brief Callback type.
     */
    typedef boost::function<void()> Callback;

public:
    /*!
     * \brief
     */
    EventHandler(
            const boost::shared_ptr<Server>& server    //!< [in]
            );

    /*!
     * \brief
     */
    void start(
            const Callback& callback    //!< [in]
            );

    /*!
     * \brief
     */
    void poll();

private:
    void reconnectCallback(
            const bgsched::SequenceId sequence,
            const Callback& callback
            );

    void handlePollingEnded(
            bgsched::SequenceId sequence
            );

    void handleRealtimeStartedRealtimeEvent(
            const RealtimeStartedEventInfo& info
            );

    void handleRealtimeEndedRealtimeEvent(
            const RealtimeEndedEventInfo& info
            );
    
    void handleBlockStateChangedRealtimeEvent(
            const BlockStateChangedEventInfo& eventInfo
            );
    
    void blockCallback(
            const BlockStateChangedEventInfo& event,
            error_code::rc error,
            const std::string& message
            );

    void add(
            const BlockStateChangedEventInfo& event
            );

private:
    const boost::weak_ptr<Server> _server;
    boost::weak_ptr<Polling> _polling;
    bgsched::SequenceId _pollingSequence;
    bgsched::SequenceId _sequence;
    boost::asio::io_service::strand _strand;
    std::queue<BlockStateChangedEventInfo> _queue;
};

} // realtime
} // server
} // runjob

#endif
