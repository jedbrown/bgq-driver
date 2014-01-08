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

#ifndef BGSCHED_ALLOCATOR_ALLOCATE_EVENT_HANDLER_H_
#define BGSCHED_ALLOCATOR_ALLOCATE_EVENT_HANDLER_H_

/*!
 *  \file bgsched/allocator/AllocateEventHandler.h
 *  \brief Class AllocateEventHandler.
 */

#include <bgsched/realtime/ClientEventListener.h>

#include "bgsched/allocator/LiveModelImpl.h"

namespace bgsched {
namespace allocator {

/*!
 *  \brief Handle compute block status changes as a result of a block allocate.
 */
class AllocateEventHandler: public bgsched::realtime::ClientEventListener
{
public:

    /*!
     * \brief
     */
    AllocateEventHandler(
            LiveModel::Impl* model //!< Pointer to LiveModel implementation
    );

    /*!
     * \brief
     */
    ~AllocateEventHandler();

    /*!
     *  \brief Handle a real-time started event.
     */
    void handleRealtimeStartedRealtimeEvent(
            const RealtimeStartedEventInfo& info //!< Real-time started event information.
    );

    /*!
     *  \brief Handle a real-time ended event.
     */
    void handleRealtimeEndedRealtimeEvent(
            const RealtimeEndedEventInfo& info //!< Real-time ended event information.
    );

    /*!
     *  \brief Get the indicator of whether to continue receiving messages or not.
     *
     *  The real-time client will call this method after calling any of the methods in this
     *  class to determine if the client should continue to process real-time events. If this
     *  method returns false then the client will not continue and receiveMessages() will return.
     */
    bool getRealtimeContinue();

    /*!
     *  \brief Handle a block state changed real-time event.
     */
    void handleBlockStateChangedRealtimeEvent(
            const BlockStateChangedEventInfo& eventInfo  //!< Compute block state changed event information
    );

    /*!
     *  \brief Get the indicator of whether a real-time ended event occurred.
     */
    bool isRealtimeEndedEvent();

protected:

    LiveModel::Impl*  _model;                //!< Pointer to LiveModel implementation
    bool              _isRealtimeEndedEvent; //!< Indicator that real-time ended event occurred

};

} // namespace bgsched::allocator
} // namespace bgsched

#endif
