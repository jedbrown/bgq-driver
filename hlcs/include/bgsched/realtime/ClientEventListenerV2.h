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


#ifndef BGSCHED_REALTIME_CLIENT_EVENT_LISTENER_V2_H_
#define BGSCHED_REALTIME_CLIENT_EVENT_LISTENER_V2_H_


#include "ClientEventListener.h"


namespace bgsched {
namespace realtime {


/*!
 *  \file
 *
 *  \brief Class ClientEventListenerV2.
 */


/*!
 *  \brief Real-time client event listener interface, version 2.
 *
 *  This event listener contains new functions introduced in V1R2M0.
 *
 *  Each of the methods in this class represents a different type of real-time event that can
 *  occur. The application should create a subclass of ClientEventListener and override the
 *  methods for the events that it wants to handle.
 *
 *  The default implementation of the handle* methods just logs a message indicating that the
 *  event was ignored.
 *
 *  \ingroup V1R2
 */
class ClientEventListenerV2 : public ClientEventListener
{
public:

    /*!
     *  \brief I/O drawer state changed event information.
     */
    class IoDrawerStateChangedEventInfo {
    public:
        const std::string& getLocation() const; //!< The I/O drawer's location.
        Hardware::State getState() const; //!< The current state of the I/O drawer.
        SequenceId getSequenceId() const; //!< The current state's sequence ID.
        Hardware::State getPreviousState() const; //!< The previous state of the I/O drawer.
        SequenceId getPreviousSequenceId() const; //!< The current state's sequence ID.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        IoDrawerStateChangedEventInfo( const Pimpl& impl_ptr );

    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle an I/O drawer state change real-time event.
     */
    virtual void handleIoDrawerStateChangedEvent(
            const IoDrawerStateChangedEventInfo& info //!< I/O drawer state changed event information.
        );


    /*!
     *  \brief I/O node state changed event information.
     */
    class IoNodeStateChangedEventInfo {
    public:
        const std::string& getLocation() const; //!< The I/O node's location.
        Hardware::State getState() const; //!< The current state of the I/O node.
        SequenceId getSequenceId() const; //!< The current state's sequence ID.
        Hardware::State getPreviousState() const; //!< The previous state of the I/O node.
        SequenceId getPreviousSequenceId() const; //!< The current state's sequence ID.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        IoNodeStateChangedEventInfo( const Pimpl& impl_ptr );

    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle an I/O node state change real-time event.
     */
    virtual void handleIoNodeStateChangedEvent(
            const IoNodeStateChangedEventInfo& info //!< I/O node state changed event information.
        );

};

} // namespace bgsched::realtime
} // namespace bgsched

#endif
