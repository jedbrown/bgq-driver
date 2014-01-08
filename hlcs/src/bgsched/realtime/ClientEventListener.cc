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

#include <bgsched/realtime/ClientEventListener.h>

#include "ClientEventListenerImpl.h"

#include <bgsched/core/core.h>

#include "bgq_util/include/Location.h"

#include "utility/include/Log.h"

#include <iostream>


using std::ostream;
using std::string;


LOG_DECLARE_FILE( "bgsched" );


namespace bgsched {
namespace realtime {


static ostream& operator<<(
        ostream& os,
        const ClientEventListener::RealtimeStartedEventInfo& info
    )
{
    return os << "{"
            "filterId=" << info.getFilterId() <<
        "}";
}


static ostream& operator<<(
        ostream& os,
        const ClientEventListener::RealtimeEndedEventInfo& /*info*/
    )
{
    return os << "{}";
}


static ostream& operator<<( ostream& os, Block::Status status )
{
    return os << (Block::Allocated == status ? "Allocated" :
                  Block::Booting == status ? "Booting" :
                  Block::Free == status ? "Free" :
                  Block::Initialized == status ? "Initialized" :
                  Block::Terminating == status ? "Terminating" :
                  "Unexpected");
}


static ostream& operator<<( ostream& os, Job::Status status )
{
    return os << (Job::Cleanup == status ? "Cleanup" :
                  Job::Loading == status ? "Loading" :
                  Job::Setup == status ? "Setup" :
                  Job::Starting == status ? "Starting" :
                  Job::Running == status ? "Running" :
                  Job::Terminated == status ? "Terminated" :
                  Job::Error == status ? "Error" :
                  "Unexpected");
}


static ostream& operator<<( ostream& os, const ClientEventListener::BlockAddedEventInfo& info )
{
    return os << "{"
            "block='" << info.getBlockName() << "'"
           " status=" << info.getStatus() <<
           " sequenceId=" << info.getSequenceId() <<
        "}";
}


static ostream& operator<<( ostream& os, const ClientEventListener::BlockStateChangedEventInfo& info )
{
    return os << "{"
            "block='" << info.getBlockName() << "'"
           " status=" << info.getStatus() <<
           " sequenceId=" << info.getSequenceId() <<
           " previousStatus=" << info.getPreviousStatus() <<
           " previousSequenceId=" << info.getPreviousSequenceId() <<
        "}";
}


static ostream& operator<<( ostream& os, const ClientEventListener::BlockDeletedEventInfo& info )
{
    return os << "{"
            "block='" << info.getBlockName() << "'"
           " previousSequenceId=" << info.getPreviousSequenceId() <<
        "}";
}


static ostream& operator<<( ostream& os, const ClientEventListener::JobAddedEventInfo& info )
{
    return os << "{"
            "job=" << info.getJobId() <<
           " computeBlock='" << info.getComputeBlockName() << "'"
           " status=" << info.getStatus() <<
           " sequenceId=" << info.getSequenceId() <<
        "}";
}


static ostream& operator<<( ostream& os, const ClientEventListener::JobStateChangedEventInfo& info )
{
    return os << "{"
            "job=" << info.getJobId() <<
           " computeBlock='" << info.getComputeBlockName() << "'"
           " status=" << info.getStatus() <<
           " sequenceId=" << info.getSequenceId() <<
           " previousStatus=" << info.getPreviousStatus() <<
           " previousSequenceId=" << info.getPreviousSequenceId() <<
        "}";
}


static ostream& operator<<( ostream& os, const ClientEventListener::JobDeletedEventInfo& info )
{
    return os << "{"
            "job=" << info.getJobId() <<
           " computeBlock='" << info.getComputeBlockName() << "'" <<
           " previousSequenceId=" << info.getPreviousSequenceId() <<
        "}";
}


static ostream& operator<<( ostream& os, const ClientEventListener::MidplaneStateChangedEventInfo& info )
{
    return os << "{"
            "location=" << info.getLocation() <<
           " state=" << info.getState() <<
           " sequenceId=" << info.getSequenceId() <<
           " previousState=" << info.getPreviousState() <<
           " previousSequenceId=" << info.getPreviousSequenceId() <<
        "}";
}


static ostream& operator<<( ostream& os, const ClientEventListener::NodeBoardStateChangedEventInfo& info )
{
    return os << "{"
            "location=" << info.getLocation() <<
           " state=" << info.getState() <<
           " sequenceId=" << info.getSequenceId() <<
           " previousState=" << info.getPreviousState() <<
           " previousSequenceId=" << info.getPreviousSequenceId() <<
        "}";
}


static ostream& operator<<( ostream& os, const ClientEventListener::NodeStateChangedEventInfo& info )
{
    return os << "{"
            "location=" << info.getLocation() <<
           " state=" << info.getState() <<
           " sequenceId=" << info.getSequenceId() <<
           " previousState=" << info.getPreviousState() <<
           " previousSequenceId=" << info.getPreviousSequenceId() <<
        "}";
}


static ostream& operator<<( ostream& os, const ClientEventListener::SwitchStateChangedEventInfo& info )
{
    return os << "{"
            "midplane=" << info.getMidplaneLocation() <<
           " dimension=" << info.getDimension() <<
           " state=" << info.getState() <<
           " sequenceId=" << info.getSequenceId() <<
           " previousState=" << info.getPreviousState() <<
           " previousSequenceId=" << info.getPreviousSequenceId() <<
        "}";
}


static ostream& operator<<( ostream& os, const ClientEventListener::TorusCableStateChangedEventInfo& info )
{
    return os << "{"
            "from=" << info.getFromMidplaneLocation() <<
           " to=" << info.getToMidplaneLocation() <<
           " dimension=" << info.getDimension() <<
           " state=" << info.getState() <<
           " sequenceId=" << info.getSequenceId() <<
           " previousState=" << info.getPreviousState() <<
           " previousSequenceId=" << info.getPreviousSequenceId() <<
        "}";
}


static ostream& operator<<( ostream& os, const ClientEventListener::IoCableStateChangedEventInfo& info )
{
    return os << "{"
            "from=" << info.getFromMidplaneLocation() <<
           " to=" << info.getToLocation() <<
           " state=" << info.getState() <<
           " sequenceId=" << info.getSequenceId() <<
           " previousState=" << info.getPreviousState() <<
           " previousSequenceId=" << info.getPreviousSequenceId() <<
        "}";
}


static ostream& operator<<( ostream& os, RasSeverity::Value severity ) {
    switch ( severity ) {
        case RasSeverity::FATAL: os << "FATAL"; break;
        case RasSeverity::INFO: os << "INFO"; break;
        case RasSeverity::WARN: os << "WARN"; break;
        default: os << "UNKNOWN"; break;
    }
    return os;
}


static ostream& operator<<( ostream& os, const ClientEventListener::RasEventInfo& info )
{
    return os << "{"
            "recid=" << info.getRecordId() <<
           " msgid=" << info.getMessageId() <<
           " severity=" << info.getSeverity() <<
        "}";
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::RealtimeStartedEventInfo.


Filter::Id ClientEventListener::RealtimeStartedEventInfo::getFilterId() const
{
    return _impl_ptr->getFilterId();
}


ClientEventListener::RealtimeStartedEventInfo::RealtimeStartedEventInfo(
        const Pimpl& impl_ptr
    ) :
        _impl_ptr(impl_ptr)
{
    // Nothing to do.
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::RealtimeEndedEventInfo.

ClientEventListener::RealtimeEndedEventInfo::RealtimeEndedEventInfo(
        const Pimpl& impl_ptr
    ) :
        _impl_ptr(impl_ptr)
{
    // Nothing to do.
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::BlockAddedEventInfo.


ClientEventListener::BlockAddedEventInfo::BlockAddedEventInfo(
        const Pimpl& impl_ptr
    ) :
        _impl_ptr(impl_ptr)
{
    // Nothing to do.
}


const string& ClientEventListener::BlockAddedEventInfo::getBlockName() const
{
    return _impl_ptr->getBlockName();
}


Block::Status ClientEventListener::BlockAddedEventInfo::getStatus() const
{
    return _impl_ptr->getStatus();
}


SequenceId ClientEventListener::BlockAddedEventInfo::getSequenceId() const
{
    return _impl_ptr->getSequenceId();
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::BlockStateChangedEventInfo.


ClientEventListener::BlockStateChangedEventInfo::BlockStateChangedEventInfo(
        const Pimpl& impl_ptr
    ) :
        _impl_ptr(impl_ptr)
{
    // Nothing to do.
}


const std::string& ClientEventListener::BlockStateChangedEventInfo::getBlockName() const
{
    return _impl_ptr->getBlockName();
}


Block::Status ClientEventListener::BlockStateChangedEventInfo::getStatus() const
{
    return _impl_ptr->getStatus();
}


SequenceId ClientEventListener::BlockStateChangedEventInfo::getSequenceId() const
{
    return _impl_ptr->getSequenceId();
}

Block::Status ClientEventListener::BlockStateChangedEventInfo::getPreviousStatus() const
{
    return _impl_ptr->getPreviousStatus();
}


SequenceId ClientEventListener::BlockStateChangedEventInfo::getPreviousSequenceId() const
{
    return _impl_ptr->getPreviousSequenceId();
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::BlockDeletedEventInfo.


ClientEventListener::BlockDeletedEventInfo::BlockDeletedEventInfo(
        const Pimpl& impl_ptr
    ) :
        _impl_ptr(impl_ptr)
{
    // Nothing to do.
}


const string& ClientEventListener::BlockDeletedEventInfo::getBlockName() const
{
    return _impl_ptr->getBlockName();
}


SequenceId ClientEventListener::BlockDeletedEventInfo::getPreviousSequenceId() const
{
    return _impl_ptr->getPreviousSequenceId();
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::JobAddedEventInfo.

ClientEventListener::JobAddedEventInfo::JobAddedEventInfo(
        const Pimpl& impl_ptr
    ) :
        _impl_ptr(impl_ptr)
{
    // Nothing to do.
}


Job::Id ClientEventListener::JobAddedEventInfo::getJobId() const
{
    return _impl_ptr->getJobId();
}


const string& ClientEventListener::JobAddedEventInfo::getComputeBlockName() const
{
    return _impl_ptr->getComputeBlockName();
}


Job::Status ClientEventListener::JobAddedEventInfo::getStatus() const
{
    return _impl_ptr->getStatus();
}


SequenceId ClientEventListener::JobAddedEventInfo::getSequenceId() const
{
    return _impl_ptr->getSequenceId();
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::JobStateChangedEventInfo

ClientEventListener::JobStateChangedEventInfo::JobStateChangedEventInfo(
        const Pimpl& impl_ptr
    ) :
        _impl_ptr(impl_ptr)
{
    // Nothing to do.
}


Job::Id ClientEventListener::JobStateChangedEventInfo::getJobId() const
{
    return _impl_ptr->getJobId();
}


const string& ClientEventListener::JobStateChangedEventInfo::getComputeBlockName() const
{
    return _impl_ptr->getComputeBlockName();
}


Job::Status ClientEventListener::JobStateChangedEventInfo::getStatus() const
{
    return _impl_ptr->getStatus();
}


SequenceId ClientEventListener::JobStateChangedEventInfo::getSequenceId() const
{
    return _impl_ptr->getSequenceId();
}


Job::Status ClientEventListener::JobStateChangedEventInfo::getPreviousStatus() const
{
    return _impl_ptr->getPreviousStatus();
}


SequenceId ClientEventListener::JobStateChangedEventInfo::getPreviousSequenceId() const
{
    return _impl_ptr->getPreviousSequenceId();
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::JobDeletedEventInfo

ClientEventListener::JobDeletedEventInfo::JobDeletedEventInfo(
        const Pimpl& impl_ptr
    ) :
        _impl_ptr(impl_ptr)
{
    // Nothing to do.
}


Job::Id ClientEventListener::JobDeletedEventInfo::getJobId() const
{
    return _impl_ptr->getJobId();
}


const string& ClientEventListener::JobDeletedEventInfo::getComputeBlockName() const
{
    return _impl_ptr->getComputeBlockName();
}


SequenceId ClientEventListener::JobDeletedEventInfo::getPreviousSequenceId() const
{
    return _impl_ptr->getPreviousSequenceId();
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::MidplaneStateChangedEventInfo


ClientEventListener::MidplaneStateChangedEventInfo::MidplaneStateChangedEventInfo( const Pimpl& impl_ptr )
    : _impl_ptr(impl_ptr)
{
    // Nothing to do.
}

const std::string& ClientEventListener::MidplaneStateChangedEventInfo::getLocation() const  { return _impl_ptr->getLocation(); }
Coordinates ClientEventListener::MidplaneStateChangedEventInfo::getMidplaneCoordinates() const  { return core::getMidplaneCoordinates( _impl_ptr->getLocation() ); }
Hardware::State ClientEventListener::MidplaneStateChangedEventInfo::getState() const  { return _impl_ptr->getState(); }
SequenceId ClientEventListener::MidplaneStateChangedEventInfo::getSequenceId() const  { return _impl_ptr->getSequenceId(); }
Hardware::State ClientEventListener::MidplaneStateChangedEventInfo::getPreviousState() const  { return _impl_ptr->getPreviousState(); }
SequenceId ClientEventListener::MidplaneStateChangedEventInfo::getPreviousSequenceId() const  { return _impl_ptr->getPreviousSequenceId(); }


//-----------------------------------------------------------------------
// Members of class ClientEventListener::NodeBoardStateChangedEventInfo

ClientEventListener::NodeBoardStateChangedEventInfo::NodeBoardStateChangedEventInfo( const Pimpl& impl_ptr )
    : _impl_ptr(impl_ptr)
{
    // Nothing to do.
}

const std::string& ClientEventListener::NodeBoardStateChangedEventInfo::getLocation() const  { return _impl_ptr->getLocation(); }
Coordinates ClientEventListener::NodeBoardStateChangedEventInfo::getMidplaneCoordinates() const  { return core::getMidplaneCoordinates( bgq::util::Location( _impl_ptr->getLocation() ).getMidplaneLocation() ); }
Hardware::State ClientEventListener::NodeBoardStateChangedEventInfo::getState() const  { return _impl_ptr->getState(); }
SequenceId ClientEventListener::NodeBoardStateChangedEventInfo::getSequenceId() const  { return _impl_ptr->getSequenceId(); }
Hardware::State ClientEventListener::NodeBoardStateChangedEventInfo::getPreviousState() const  { return _impl_ptr->getPreviousState(); }
SequenceId ClientEventListener::NodeBoardStateChangedEventInfo::getPreviousSequenceId() const  { return _impl_ptr->getPreviousSequenceId(); }


//-----------------------------------------------------------------------
// Members of class ClientEventListener::NodeStateChangedEventInfo

ClientEventListener::NodeStateChangedEventInfo::NodeStateChangedEventInfo( const Pimpl& impl_ptr )
    : _impl_ptr(impl_ptr)
{
    // Nothing to do.
}

const std::string& ClientEventListener::NodeStateChangedEventInfo::getLocation() const  { return _impl_ptr->getLocation(); }
Coordinates ClientEventListener::NodeStateChangedEventInfo::getMidplaneCoordinates() const  { return core::getMidplaneCoordinates( bgq::util::Location( _impl_ptr->getLocation() ).getMidplaneLocation() ); }
Hardware::State ClientEventListener::NodeStateChangedEventInfo::getState() const  { return _impl_ptr->getState(); }
SequenceId ClientEventListener::NodeStateChangedEventInfo::getSequenceId() const  { return _impl_ptr->getSequenceId(); }
Hardware::State ClientEventListener::NodeStateChangedEventInfo::getPreviousState() const  { return _impl_ptr->getPreviousState(); }
SequenceId ClientEventListener::NodeStateChangedEventInfo::getPreviousSequenceId() const  { return _impl_ptr->getPreviousSequenceId(); }


//-----------------------------------------------------------------------
// Members of class ClientEventListener::SwitchStateChangedEventInfo

ClientEventListener::SwitchStateChangedEventInfo::SwitchStateChangedEventInfo( const Pimpl& impl_ptr )
    : _impl_ptr(impl_ptr)
{
    // Nothing to do.
}

const std::string& ClientEventListener::SwitchStateChangedEventInfo::getMidplaneLocation() const  { return _impl_ptr->getMidplaneLocation(); }
Coordinates ClientEventListener::SwitchStateChangedEventInfo::getMidplaneCoordinates() const { return core::getMidplaneCoordinates( getMidplaneLocation() ); }
Dimension::Value ClientEventListener::SwitchStateChangedEventInfo::getDimension() const  { return _impl_ptr->getDimension(); }
Hardware::State ClientEventListener::SwitchStateChangedEventInfo::getState() const  { return _impl_ptr->getState(); }
SequenceId ClientEventListener::SwitchStateChangedEventInfo::getSequenceId() const  { return _impl_ptr->getSequenceId(); }
Hardware::State ClientEventListener::SwitchStateChangedEventInfo::getPreviousState() const  { return _impl_ptr->getPreviousState(); }
SequenceId ClientEventListener::SwitchStateChangedEventInfo::getPreviousSequenceId() const  { return _impl_ptr->getPreviousSequenceId(); }


//-----------------------------------------------------------------------
// Members of class ClientEventListener::TorusCableStateChangedEventInfo

ClientEventListener::TorusCableStateChangedEventInfo::TorusCableStateChangedEventInfo( const Pimpl& impl_ptr )
    : _impl_ptr(impl_ptr)
{
    // Nothing to do.
}

static Dimension::Value calcDimension( const std::string& cable_location )
{
    /* From create_table.sql
     case substr(fromlocation,12,3) when 'T06'  then 'A_'|| substr(fromlocation,1,6)
                                    when 'T05' then 'B_'|| substr(fromlocation,1,6)
                                    when 'T10' then 'C_'|| substr(fromlocation,1,6)
                                    when 'T09' then 'D_'|| substr(fromlocation,1,6)
                                    when 'T04'  then 'A_'|| substr(fromlocation,1,6)
                                    when 'T07' then 'B_'|| substr(fromlocation,1,6)
                                    when 'T08' then 'C_'|| substr(fromlocation,1,6)
                                    when 'T11' then 'D_'|| substr(fromlocation,1,6) else 'I_'|| substr(fromlocation,1,14) end  as source,
     */

    string port(cable_location.substr( 11, 3 ));

    return (port == "T06" ? Dimension::A :
            port == "T05" ? Dimension::B :
            port == "T10" ? Dimension::C :
            port == "T09" ? Dimension::D :
            port == "T04" ? Dimension::A :
            port == "T07" ? Dimension::B :
            port == "T08" ? Dimension::C :
            port == "T11" ? Dimension::D :
            Dimension::InvalidDimension);
}

const std::string& ClientEventListener::TorusCableStateChangedEventInfo::getFromLocation() const  { return _impl_ptr->getFromLocation(); }
std::string ClientEventListener::TorusCableStateChangedEventInfo::getFromMidplaneLocation() const  { return getFromLocation().substr( 0, 6 ); }
Coordinates ClientEventListener::TorusCableStateChangedEventInfo::getFromMidplaneCoordinates() const  { return core::getMidplaneCoordinates( getFromMidplaneLocation() ); }
const std::string& ClientEventListener::TorusCableStateChangedEventInfo::getToLocation() const  { return _impl_ptr->getToLocation(); }
std::string ClientEventListener::TorusCableStateChangedEventInfo::getToMidplaneLocation() const  { return getToLocation().substr( 0, 6 ); }
Coordinates ClientEventListener::TorusCableStateChangedEventInfo::getToMidplaneCoordinates() const  { return core::getMidplaneCoordinates( getToMidplaneLocation() ); }
Dimension::Value ClientEventListener::TorusCableStateChangedEventInfo::getDimension() const  { return calcDimension( getFromLocation() ); }
Hardware::State ClientEventListener::TorusCableStateChangedEventInfo::getState() const  { return _impl_ptr->getState(); }
SequenceId ClientEventListener::TorusCableStateChangedEventInfo::getSequenceId() const  { return _impl_ptr->getSequenceId(); }
Hardware::State ClientEventListener::TorusCableStateChangedEventInfo::getPreviousState() const  { return _impl_ptr->getPreviousState(); }
SequenceId ClientEventListener::TorusCableStateChangedEventInfo::getPreviousSequenceId() const  { return _impl_ptr->getPreviousSequenceId(); }


//-----------------------------------------------------------------------
// Members of class ClientEventListener::IoCableStateChangedEventInfo

ClientEventListener::IoCableStateChangedEventInfo::IoCableStateChangedEventInfo( const Pimpl& impl_ptr )
    : _impl_ptr(impl_ptr)
{
    // Nothing to do.
}

const std::string& ClientEventListener::IoCableStateChangedEventInfo::getFromLocation() const  { return _impl_ptr->getFromLocation(); }
std::string ClientEventListener::IoCableStateChangedEventInfo::getFromMidplaneLocation() const  { return getFromLocation().substr( 0, 6 ); }
Coordinates ClientEventListener::IoCableStateChangedEventInfo::getFromMidplaneCoordinates() const  { return core::getMidplaneCoordinates( getFromMidplaneLocation() ); }
const std::string& ClientEventListener::IoCableStateChangedEventInfo::getToLocation() const  { return _impl_ptr->getToLocation(); }
Hardware::State ClientEventListener::IoCableStateChangedEventInfo::getState() const  { return _impl_ptr->getState(); }
SequenceId ClientEventListener::IoCableStateChangedEventInfo::getSequenceId() const  { return _impl_ptr->getSequenceId(); }
Hardware::State ClientEventListener::IoCableStateChangedEventInfo::getPreviousState() const  { return _impl_ptr->getPreviousState(); }
SequenceId ClientEventListener::IoCableStateChangedEventInfo::getPreviousSequenceId() const  { return _impl_ptr->getPreviousSequenceId(); }


//-----------------------------------------------------------------------
// Members of class ClientEventListener::RasEventInfo.

ClientEventListener::RasEventInfo::RasEventInfo(
        const Pimpl& impl_ptr
    ) :
        _impl_ptr(impl_ptr)
{
    // Nothing to do.
}


RasRecordId ClientEventListener::RasEventInfo::getRecordId() const
{
    return _impl_ptr->getRecordId();
}


const string& ClientEventListener::RasEventInfo::getMessageId() const
{
    return _impl_ptr->getMessageId();
}


RasSeverity::Value ClientEventListener::RasEventInfo::getSeverity() const
{
    return _impl_ptr->getSeverity();
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener.

bool ClientEventListener::getRealtimeContinue()
{
    return true;
}


void ClientEventListener::handleRealtimeStartedRealtimeEvent(
        const RealtimeStartedEventInfo& info
    )
{
    LOG_DEBUG_MSG("Ignoring real-time started event. info=" << info);
}


void ClientEventListener::handleRealtimeEndedRealtimeEvent(
        const RealtimeEndedEventInfo& info
    )
{
    LOG_DEBUG_MSG("Ignoring real-time ended event. info=" << info);
}


void ClientEventListener::handleBlockAddedRealtimeEvent(
        const BlockAddedEventInfo& info
    )
{
    LOG_DEBUG_MSG("Ignoring block added real-time event. info=" << info);
}


void ClientEventListener::handleBlockStateChangedRealtimeEvent(
        const BlockStateChangedEventInfo& info
    )
{
    LOG_DEBUG_MSG("Ignoring block state changed real-time event. info=" << info);
}


void ClientEventListener::handleBlockDeletedRealtimeEvent(
      const BlockDeletedEventInfo& info
    )
{
    LOG_DEBUG_MSG("Ignoring block deleted real-time event. info=" << info);
}


void ClientEventListener::handleJobAddedRealtimeEvent(
        const JobAddedEventInfo& info
    )
{
    LOG_DEBUG_MSG("Ignoring job added real-time event. info=" << info);
}


void ClientEventListener::handleJobStateChangedRealtimeEvent(
        const JobStateChangedEventInfo& info
    )
{
    LOG_DEBUG_MSG("Ignoring job state changed real-time event. info=" << info);
}


void ClientEventListener::handleJobDeletedRealtimeEvent(
        const JobDeletedEventInfo& info
    )
{
    LOG_DEBUG_MSG("Ignoring job deleted real-time event. info=" << info);
}

void ClientEventListener::handleMidplaneStateChangedRealtimeEvent(
          const MidplaneStateChangedEventInfo& info
      )
{
    LOG_DEBUG_MSG("Ignoring midplane state changed real-time event. info=" << info);
}


void ClientEventListener::handleNodeBoardStateChangedRealtimeEvent(
        const NodeBoardStateChangedEventInfo& info
    )
{
    LOG_DEBUG_MSG("Ignoring node board state changed real-time event. info=" << info);
}


void ClientEventListener::handleNodeStateChangedRealtimeEvent(
        const NodeStateChangedEventInfo& info
    )
{
    LOG_DEBUG_MSG("Ignoring node state changed real-time event. info=" << info);
}


void ClientEventListener::handleSwitchStateChangedRealtimeEvent(
        const SwitchStateChangedEventInfo& info
    )
{
    LOG_DEBUG_MSG("Ignoring switch state changed real-time event. info=" << info);
}


void ClientEventListener::handleTorusCableStateChangedRealtimeEvent(
        const TorusCableStateChangedEventInfo& info
    )
{
    LOG_DEBUG_MSG("Ignoring torus cable state changed real-time event. info=" << info);
}


void ClientEventListener::handleIoCableStateChangedRealtimeEvent(
        const IoCableStateChangedEventInfo& info
    )
{
    LOG_DEBUG_MSG("Ignoring I/O cable state changed real-time event. info=" << info);
}


void ClientEventListener::handleRasRealtimeEvent(
        const RasEventInfo& info
    )
{
    LOG_DEBUG_MSG("Ignoring RAS real-time event. info=" << info);
}


ClientEventListener::~ClientEventListener()
{
    // Nothing to do.
}


} // namespace bgsched::realtime
} // namespace bgsched
