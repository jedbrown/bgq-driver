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

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <queue>

namespace runjob {
namespace server {
namespace realtime {

/*!
 * \brief
 */
class EventHandler : public bgsched::realtime::ClientEventListener
{
public:
    /*!
     * \brief
     */
    EventHandler(
            const boost::shared_ptr<Server>& server,    //!< [in]
            bgsched::SequenceId sequence                //!< [in]
            );

    /*!
     * \brief
     */
    void poll();

private:
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

private:
    const boost::weak_ptr<Server> _server;
    boost::weak_ptr<Polling> _polling;
    bgsched::SequenceId _pollingSequence;
    bgsched::SequenceId _sequence;
    std::queue<BlockStateChangedEventInfo> _queue;
};

} // realtime
} // server
} // runjob

#endif
