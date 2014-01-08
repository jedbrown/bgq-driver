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

#ifndef BGSCHED_REALTIME_CLIENT_EVENT_LISTENER_H_
#define BGSCHED_REALTIME_CLIENT_EVENT_LISTENER_H_

/*!
 *  \file
 *
 *  \brief Class ClientEventListener.
 */


#include <bgsched/realtime/Filter.h>
#include <bgsched/realtime/types.h>

#include <bgsched/Coordinates.h>
#include <bgsched/Dimension.h>
#include <bgsched/Job.h>
#include <bgsched/Hardware.h>
#include <bgsched/Block.h>
#include <bgsched/types.h>

#include <boost/shared_ptr.hpp>

#include <string>


namespace bgsched {
namespace realtime {


/*!
 *  \brief Real-time client event listener interface.
 *
 *  Each of the methods in this class represents a different type of real-time event that can
 *  occur. The application should create a subclass of ClientEventListener and override the
 *  methods for the events that it wants to handle.
 *
 *  The default implementation of the handle* methods just logs a message indicating that the
 *  event was ignored.
 */
class ClientEventListener
{
public:

    /*!
     *  \brief Get the indicator of whether to continue or not.
     *
     *  The Client will call this method after calling any of the methods in this class to
     *  determine if the Client should continue to process real-time events. If this method
     *  returns false then the Client will not continue and receiveMessages() will return.
     *
     *  The default ClientEventListener will return true unless the previous event was
     *  real-time ended.
     */
    virtual bool getRealtimeContinue();


    /*!
     *  \brief Real-time started event information.
     */
    class RealtimeStartedEventInfo {
    public:
        Filter::Id getFilterId() const; //!< The filter ID.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        RealtimeStartedEventInfo( const Pimpl& impl_ptr );

    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle a real-time started event.
     */
    virtual void handleRealtimeStartedRealtimeEvent(
            const RealtimeStartedEventInfo& info //!< Real-time started event information.
        );


    /*!
     *  \brief Real-time ended event information.
     */
    class RealtimeEndedEventInfo {
    public:
        // No methods.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        RealtimeEndedEventInfo( const Pimpl& impl_ptr );

    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle a real-time ended event.
     */
    virtual void handleRealtimeEndedRealtimeEvent(
            const RealtimeEndedEventInfo& info //!< Real-time ended event information.
        );


    /*!
     *  \brief %Block added event information.
     */
    class BlockAddedEventInfo {
    public:
        const std::string& getBlockName() const; //!< The block name.
        Block::Status getStatus() const; //!< The block status.
        SequenceId getSequenceId() const; //!< The status sequence ID.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        BlockAddedEventInfo( const Pimpl& impl_ptr );

    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle a block added real-time event.
     */
    virtual void handleBlockAddedRealtimeEvent(
            const BlockAddedEventInfo& info //!< %Block added event information.
        );


    /*!
     *  \brief %Block state changed event information.
     */
    class BlockStateChangedEventInfo {
    public:
        const std::string& getBlockName() const; //!< The block name.
        Block::Status getStatus() const; //!< The block's current status.
        SequenceId getSequenceId() const; //!< The block's current status sequence ID.
        Block::Status getPreviousStatus() const; //!< The block's previous status.
        SequenceId getPreviousSequenceId() const; //!< The block's previous status sequence ID.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        BlockStateChangedEventInfo( const Pimpl& impl_ptr );

    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle a block state changed real-time event.
     */
    virtual void handleBlockStateChangedRealtimeEvent(
            const BlockStateChangedEventInfo& info //!< %Block state changed event information.
        );


    /*!
     *  \brief %Block deleted event information.
     */
    class BlockDeletedEventInfo {
    public:
        const std::string& getBlockName() const; //!< The block name.
        SequenceId getPreviousSequenceId() const; //!< The block's previous status sequence ID.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        BlockDeletedEventInfo( const Pimpl& impl_ptr );

    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle a block deleted real-time event.
     */
    virtual void handleBlockDeletedRealtimeEvent(
            const BlockDeletedEventInfo& info //!< %Block deleted event information.
        );

    /*!
     *  \brief %Job added event information.
     */
    class JobAddedEventInfo {
    public:
        Job::Id getJobId() const; //!< The job ID.
        const std::string& getComputeBlockName() const; //!< The job's compute block.
        Job::Status getStatus() const; //!< The job's new status.
        SequenceId getSequenceId() const; //!< The jobs's new status sequence ID.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        JobAddedEventInfo( const Pimpl& impl_ptr );

    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle a job added real-time event.
     */
    virtual void handleJobAddedRealtimeEvent(
            const JobAddedEventInfo& info //!< %Job added event information.
        );


    /*!
     *  \brief %Job state changed event information.
     */
    class JobStateChangedEventInfo {
    public:
        Job::Id getJobId() const; //!< The job ID.
        const std::string& getComputeBlockName() const; //!< The job's compute block.
        Job::Status getStatus() const; //!< The job's new status.
        SequenceId getSequenceId() const; //!< The jobs's new status sequence ID.
        Job::Status getPreviousStatus() const; //!< The job's previous status.
        SequenceId getPreviousSequenceId() const; //!< The jobs's previous status sequence ID.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        JobStateChangedEventInfo( const Pimpl& impl_ptr );

    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle a job state changed real-time event.
     */
    virtual void handleJobStateChangedRealtimeEvent(
            const JobStateChangedEventInfo& info //!< %Job state changed event information.
        );


    /*!
     *  \brief %Job deleted event information.
     */
    class JobDeletedEventInfo {
    public:
        Job::Id getJobId() const; //!< The job ID.
        const std::string& getComputeBlockName() const; //!< The job's compute block.
        SequenceId getPreviousSequenceId() const; //!< The jobs's previous status sequence ID.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        JobDeletedEventInfo( const Pimpl& impl_ptr );

    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle a job deleted real-time event.
     */
    virtual void handleJobDeletedRealtimeEvent(
            const JobDeletedEventInfo& info //!< %Job deleted event information.
        );


    /*!
     *  \brief %Midplane state changed event information.
     */
    class MidplaneStateChangedEventInfo {
    public:
        const std::string& getLocation() const; //!< The midplane's location.
        Coordinates getMidplaneCoordinates() const; //!< The midplane's coordinates in the machine.
        Hardware::State getState() const; //!< The current state of the midplane.
        SequenceId getSequenceId() const; //!< The current state's sequence ID.
        Hardware::State getPreviousState() const; //!< The previous state of the midplane.
        SequenceId getPreviousSequenceId() const; //!< The previous state's sequence ID.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        MidplaneStateChangedEventInfo( const Pimpl& impl_ptr );

    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle a midplane state changed real-time event.
     */
    virtual void handleMidplaneStateChangedRealtimeEvent(
            const MidplaneStateChangedEventInfo& info //!< %Midplane state changed event information.
        );


    /*!
     *  \brief %Node board state changed event information.
     */
    class NodeBoardStateChangedEventInfo {
    public:
        const std::string& getLocation() const; //!< The node board's location.
        Coordinates getMidplaneCoordinates() const; //!< The node board's midplane's coordinates in the machine.
        Hardware::State getState() const; //!< The current state of the node board.
        SequenceId getSequenceId() const; //!< The current state's sequence ID.
        Hardware::State getPreviousState() const; //!< The previous state of the node board.
        SequenceId getPreviousSequenceId() const; //!< The current state's sequence ID.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        NodeBoardStateChangedEventInfo( const Pimpl& impl_ptr );

    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle a node board state changed real-time event.
     */
    virtual void handleNodeBoardStateChangedRealtimeEvent(
            const NodeBoardStateChangedEventInfo& info //!< %Node board state changed event information.
        );


    /*!
     *  \brief %Node state changed event information.
     */
    class NodeStateChangedEventInfo {
    public:
        const std::string& getLocation() const; //!< The node's location.
        Coordinates getMidplaneCoordinates() const; //!< The node's midplane's coordinates in the machine.
        Hardware::State getState() const; //!< The current state of the node.
        SequenceId getSequenceId() const; //!< The current state's sequence ID.
        Hardware::State getPreviousState() const; //!< The previous state of the node.
        SequenceId getPreviousSequenceId() const; //!< The current state's sequence ID.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        NodeStateChangedEventInfo( const Pimpl& impl_ptr );

    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle a node state changed real-time event.
     */
    virtual void handleNodeStateChangedRealtimeEvent(
            const NodeStateChangedEventInfo& info //!< %Node state changed event information.
        );


    /*!
     *  \brief %Switch state changed event information.
     */
    class SwitchStateChangedEventInfo {
    public:
        const std::string& getMidplaneLocation() const; //!< The location of the midplane.
        Coordinates getMidplaneCoordinates() const; //!< The switch's midplane's coordinates in the machine.
        Dimension::Value getDimension() const; //!< The dimension.
        Hardware::State getState() const; //!< The current state of the switch.
        SequenceId getSequenceId() const; //!< The current state's sequence ID.
        Hardware::State getPreviousState() const; //!< The previous state of the switch.
        SequenceId getPreviousSequenceId() const; //!< The previous state's sequence ID.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        SwitchStateChangedEventInfo( const Pimpl& impl_ptr );

    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle a switch state changed real-time event.
     */
    virtual void handleSwitchStateChangedRealtimeEvent(
            const SwitchStateChangedEventInfo& info //!< %Switch state changed event information.
        );


    /*!
     *  \brief Torus cable state changed event information.
     */
    class TorusCableStateChangedEventInfo {
    public:
        const std::string& getFromLocation() const; //!< The cable source.
        std::string getFromMidplaneLocation() const; //!< The cable source midplane location.
        Coordinates getFromMidplaneCoordinates() const; //!< The source midplane's coordinates in the machine.

        const std::string& getToLocation() const; //!< The cable destination.
        std::string getToMidplaneLocation() const; //!< The cable destination midplane location.
        Coordinates getToMidplaneCoordinates() const; //!< The source midplane's coordinates in the machine.

        Dimension::Value getDimension() const; //!< The cable dimension.

        Hardware::State getState() const; //!< The current state of the cable.
        SequenceId getSequenceId() const; //!< The current state's sequence ID.
        Hardware::State getPreviousState() const; //!< The previous state of the cable.
        SequenceId getPreviousSequenceId() const; //!< The previous state's sequence ID.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        TorusCableStateChangedEventInfo( const Pimpl& impl_ptr );

    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle a torus cable state changed real-time event.
     */
    virtual void handleTorusCableStateChangedRealtimeEvent(
            const TorusCableStateChangedEventInfo& info //!< Torus cable state changed event information.
        );


    /*!
     *  \brief I/O cable state changed event information.
     */
    class IoCableStateChangedEventInfo {
    public:
        const std::string& getFromLocation() const; //!< The cable source.
        std::string getFromMidplaneLocation() const; //!< The cable source midplane location.
        Coordinates getFromMidplaneCoordinates() const; //!< The source midplane's coordinates in the machine.

        const std::string& getToLocation() const; //!< The cable destination.

        Hardware::State getState() const; //!< The current state of the cable.
        SequenceId getSequenceId() const; //!< The current state's sequence ID.
        Hardware::State getPreviousState() const; //!< The previous state of the cable.
        SequenceId getPreviousSequenceId() const; //!< The previous state's sequence ID.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        IoCableStateChangedEventInfo( const Pimpl& impl_ptr );

    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle an I/O cable state changed real-time event.
     */
    virtual void handleIoCableStateChangedRealtimeEvent(
            const IoCableStateChangedEventInfo& info //!< I/O cable state changed event information.
        );


    /*!
     *  \brief RAS event information.
     */
    class RasEventInfo {
    public:
        RasRecordId getRecordId() const; //!< The RAS event's record ID.
        const std::string& getMessageId() const; //!< The RAS event's message ID.
        RasSeverity::Value getSeverity() const; //!< The RAS event's severity.

        class Impl;
        typedef boost::shared_ptr<const Impl> Pimpl;

        RasEventInfo( const Pimpl& impl_ptr );
    private:
        Pimpl _impl_ptr;
    };

    /*!
     *  \brief Handle a RAS real-time event.
     */
    virtual void handleRasRealtimeEvent(
            const RasEventInfo& info //!< RAS event information.
        );


    /*!
     *  \brief Virtual destructor.
     */
    virtual ~ClientEventListener();
};


} // namespace bgsched::realtime
} // namespace bgsched


#endif
