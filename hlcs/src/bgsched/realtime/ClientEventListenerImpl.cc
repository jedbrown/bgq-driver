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


#include "ClientEventListenerImpl.h"

#include "AbstractDatabaseChangeVisitor.h"

#include "../BlockImpl.h"
#include "../HardwareImpl.h"
#include "../JobImpl.h"

#include <utility/include/Log.h>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/export.hpp>


using namespace std;


LOG_DECLARE_FILE( "bgsched" );


BOOST_CLASS_EXPORT_GUID( bgsched::realtime::AbstractDatabaseChange, "AbstractDatabaseChange" )
BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListener::RealtimeStartedEventInfo::Impl, "RealtimeStartedEventInfo" )
BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListener::RealtimeEndedEventInfo::Impl, "RealtimeEndedEventInfo" )
BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListener::BlockAddedEventInfo::Impl, "BlockAddedEventInfo" )
BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListener::BlockStateChangedEventInfo::Impl, "BlockStateChangedEventInfo" )
BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListener::BlockDeletedEventInfo::Impl, "BlockDeletedEventInfo" )
BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListener::JobAddedEventInfo::Impl, "JobAddedEventInfo" )
BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListener::JobStateChangedEventInfo::Impl, "JobStateChangedEventInfo" )
BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListener::JobDeletedEventInfo::Impl, "JobDeletedEventInfo" )
BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListener::MidplaneStateChangedEventInfo::Impl, "MidplaneStateChangedEventInfo" )
BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListener::NodeBoardStateChangedEventInfo::Impl, "NodeBoardStateChangedEventInfo" )
BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListener::NodeStateChangedEventInfo::Impl, "NodeStateChangedEventInfo" )
BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListener::SwitchStateChangedEventInfo::Impl, "SwitchStateChangedEventInfo" )
BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListener::TorusCableStateChangedEventInfo::Impl, "TorusCableStateChangedEventInfo" )
BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListener::IoCableStateChangedEventInfo::Impl, "IoCableStateChangedEventInfo" )
BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListener::RasEventInfo::Impl, "RasEventInfo" )


namespace bgsched {
namespace realtime {

//-----------------------------------------------------------------------
// Members of class ClientEventListener::RealtimeStartedEventInfo::Impl


ClientEventListener::RealtimeStartedEventInfo::Impl::Impl( Filter::Id filter_id )
    : _filter_id(filter_id)
{
    // Nothing to do.
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::RealtimeEndedEventInfo::Impl


ClientEventListener::RealtimeEndedEventInfo::Impl::Impl()
{
    // Nothing to do.
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::BlockAddedEventInfo::Impl

ClientEventListener::BlockAddedEventInfo::Impl::Impl(
        const std::string& block_id,
        Block::Status status,
        SequenceId sequence_id
    ) :
        _block_id(block_id),
        _status(status),
        _seq_id(sequence_id)
{
    // Nothing to do.
}


bool ClientEventListener::BlockAddedEventInfo::Impl::merge( const AbstractDatabaseChange& other )
{
    // if the new change is BLOCK_STATE_CHG and the block id is the same then can merge

    const ClientEventListener::BlockStateChangedEventInfo::Impl *chg_p(dynamic_cast<const ClientEventListener::BlockStateChangedEventInfo::Impl*>(&other));
    if ( ! chg_p ) {
        return false;
    }
    if ( getBlockName() != chg_p->getBlockName() ) {
        return false;
    }

    LOG_DEBUG_MSG( "Merging block update to block add" );

    _status = chg_p->getStatus();
    _seq_id = chg_p->getSequenceId();

    return true;
}


void ClientEventListener::BlockAddedEventInfo::Impl::accept( AbstractDatabaseChangeVisitor& v )
{
    v.visitBlockAdded( *this );
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::BlockStateChangedEventInfo::Impl

ClientEventListener::BlockStateChangedEventInfo::Impl::Impl(
        const std::string& block_id,
        Block::Status status,
        SequenceId seq_id,
        Block::Status prev_status,
        SequenceId prev_seq_id
    ) :
        _block_id(block_id),
        _status(status),
        _seq_id(seq_id),
        _prev_status(prev_status),
        _prev_seq_id(prev_seq_id)
{
    // Nothing to do.
}


bool ClientEventListener::BlockStateChangedEventInfo::Impl::merge( const AbstractDatabaseChange& other )
{
    // if the new change is BLOCK_STATE_CHG and the block id is the same then can merge

    const ClientEventListener::BlockStateChangedEventInfo::Impl *chg_p(dynamic_cast<const ClientEventListener::BlockStateChangedEventInfo::Impl*>(&other));
    if ( ! chg_p ) {
        return false;
    }
    if ( getBlockName() != chg_p->getBlockName() ) {
        return false;
    }

    _status = chg_p->getStatus();
    _seq_id = chg_p->getSequenceId();

    LOG_DEBUG_MSG( "Merging block update to block update. status=" << _status );

    return true;
}


void ClientEventListener::BlockStateChangedEventInfo::Impl::accept( AbstractDatabaseChangeVisitor& v )
{
    v.visitBlockStateChanged( *this );
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::BlockDeletedEventInfo::Impl

ClientEventListener::BlockDeletedEventInfo::Impl::Impl(
        const std::string& block_id,
        SequenceId prev_seq_id
    ) :
        _block_id(block_id),
        _prev_seq_id(prev_seq_id)
{
    // Nothing to do.
}


void ClientEventListener::BlockDeletedEventInfo::Impl::accept( AbstractDatabaseChangeVisitor& v )
{
    v.visitBlockDeleted( *this );
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::JobAddedEventInfo::Impl.

ClientEventListener::JobAddedEventInfo::Impl::Impl(
        Job::Id job_id,
        const std::string compute_block_id,
        Job::Status status,
        SequenceId seq_id
    ) :
        _job_id(job_id),
        _compute_block_id(compute_block_id),
        _status(status),
        _seq_id(seq_id)
{
    // Nothing to do.
}


bool ClientEventListener::JobAddedEventInfo::Impl::merge( const AbstractDatabaseChange& other )
{
    // if the new change is JOB_STATE_CHG and the db job id is the same then can merge

    const ClientEventListener::JobStateChangedEventInfo::Impl *chg_p(dynamic_cast<const ClientEventListener::JobStateChangedEventInfo::Impl*>(&other));
    if ( ! chg_p ) {
        return false;
    }
    if ( getJobId() != chg_p->getJobId() ) {
        return false;
    }

    LOG_DEBUG_MSG( "Merging job update" );

    _status = chg_p->getStatus();
    _seq_id = chg_p->getSequenceId();

    return true;
}


void ClientEventListener::JobAddedEventInfo::Impl::accept( AbstractDatabaseChangeVisitor& v )
{
    v.visitJobAdded( *this );
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::JobStateChangedEventInfo::Impl.

ClientEventListener::JobStateChangedEventInfo::Impl::Impl(
        Job::Id job_id,
        const std::string& compute_block_id,
        Job::Status status,
        SequenceId seq_id,
        Job::Status prev_status,
        SequenceId prev_seq_id
    ) :
        _job_id(job_id),
        _compute_block_id(compute_block_id),
        _status(status),
        _seq_id(seq_id),
        _prev_status(prev_status),
        _prev_seq_id(prev_seq_id)
{
    // Nothing to do.
}


bool ClientEventListener::JobStateChangedEventInfo::Impl::merge( const AbstractDatabaseChange& other )
{
    // if the new change is JOB_STATE_CHG and the db job id is the same then can merge

    const ClientEventListener::JobStateChangedEventInfo::Impl *chg_p(dynamic_cast<const ClientEventListener::JobStateChangedEventInfo::Impl*>(&other));
    if ( ! chg_p ) {
        return false;
    }
    if ( getJobId() != chg_p->getJobId() ) {
        return false;
    }

    LOG_DEBUG_MSG( "Merging job update" );

    _status = chg_p->getStatus();
    _seq_id = chg_p->getSequenceId();

    return true;
}


void ClientEventListener::JobStateChangedEventInfo::Impl::accept( AbstractDatabaseChangeVisitor& v )
{
    v.visitJobStateChanged( *this );
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::JobDeletedEventInfo::Impl.

ClientEventListener::JobDeletedEventInfo::Impl::Impl(
        Job::Id job_id,
        const std::string& block_id,
        SequenceId seq_id
    ) :
        _job_id(job_id),
        _block_id(block_id),
        _prev_seq_id(seq_id)
{
    // Nothing to do.
}


void ClientEventListener::JobDeletedEventInfo::Impl::accept( AbstractDatabaseChangeVisitor& v )
{
    v.visitJobDeleted( *this );
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::MidplaneStateChangedEventInfo::Impl.

ClientEventListener::MidplaneStateChangedEventInfo::Impl::Impl(
        const std::string& location,
        Hardware::State state_code,
        SequenceId seq_id,
        Hardware::State prev_state,
        SequenceId prev_seq_id
    ) :
        _location(location),
        _state(state_code),
        _seq_id(seq_id),
        _prev_state(prev_state),
        _prev_seq_id(prev_seq_id)
{
    // Nothing to do.
}


bool ClientEventListener::MidplaneStateChangedEventInfo::Impl::merge( const AbstractDatabaseChange& other )
{
    // if the new change is midplane change and the location is the same then merge

    const ClientEventListener::MidplaneStateChangedEventInfo::Impl *chg_p(dynamic_cast<const ClientEventListener::MidplaneStateChangedEventInfo::Impl*>(&other));
    if ( ! chg_p ) {
        return false;
    }
    if ( getLocation() != chg_p->getLocation() ) {
        return false;
    }

    LOG_DEBUG_MSG( "Merging midplane update" );

    _state = chg_p->getState();
    _seq_id = chg_p->getSequenceId();

    return true;
}


void ClientEventListener::MidplaneStateChangedEventInfo::Impl::accept( AbstractDatabaseChangeVisitor& v )
{
    v.visitMidplaneStateChanged( *this );
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::NodeBoardStateChangedEventInfo::Impl.

ClientEventListener::NodeBoardStateChangedEventInfo::Impl::Impl(
        const std::string& location,
        Hardware::State state,
        SequenceId seq_id,
        Hardware::State prev_state,
        SequenceId prev_seq_id
    ) :
        _location(location),
        _state(state),
        _seq_id(seq_id),
        _prev_state(prev_state),
        _prev_seq_id(prev_seq_id)
{
    // Nothing to do.
}


bool ClientEventListener::NodeBoardStateChangedEventInfo::Impl::merge( const AbstractDatabaseChange& other )
{
    // if the new change is node board change and the location is the same then merge

    const ClientEventListener::NodeBoardStateChangedEventInfo::Impl *chg_p(dynamic_cast<const ClientEventListener::NodeBoardStateChangedEventInfo::Impl*>(&other));
    if ( ! chg_p ) {
        return false;
    }
    if ( getLocation() != chg_p->getLocation() ) {
        return false;
    }

    LOG_DEBUG_MSG( "Merging node board update" );

    _state = chg_p->getState();
    _seq_id = chg_p->getSequenceId();

    return true;
}


void ClientEventListener::NodeBoardStateChangedEventInfo::Impl::accept( AbstractDatabaseChangeVisitor& v )
{
    v.visitNodeBoardStateChanged( *this );
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::NodeStateChangedEventInfo::Impl.

ClientEventListener::NodeStateChangedEventInfo::Impl::Impl(
        const std::string& location,
        Hardware::State state,
        SequenceId seq_id,
        Hardware::State prev_state,
        SequenceId prev_seq_id
    ) :
        _location(location),
        _state(state),
        _seq_id(seq_id),
        _prev_state(prev_state),
        _prev_seq_id(prev_seq_id)
{
    // Nothing to do.
}


bool ClientEventListener::NodeStateChangedEventInfo::Impl::merge( const AbstractDatabaseChange& other )
{
    // if the new change is node change and the location is the same then merge

    const ClientEventListener::NodeStateChangedEventInfo::Impl *chg_p(dynamic_cast<const ClientEventListener::NodeStateChangedEventInfo::Impl*>(&other));
    if ( ! chg_p ) {
        return false;
    }
    if ( getLocation() != chg_p->getLocation() ) {
        return false;
    }

    LOG_DEBUG_MSG( "Merging node update" );

    _state = chg_p->getState();
    _seq_id = chg_p->getSequenceId();

    return true;
}


void ClientEventListener::NodeStateChangedEventInfo::Impl::accept( AbstractDatabaseChangeVisitor& v )
{
    v.visitNodeStateChanged( *this );
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::SwitchStateChangedEventInfo::Impl

ClientEventListener::SwitchStateChangedEventInfo::Impl::Impl(
        const std::string& midplane_location,
        Dimension::Value dim,
        Hardware::State state,
        SequenceId seq_id,
        Hardware::State prev_state,
        SequenceId prev_seq_id
    ) :
        _midplane_location(midplane_location),
        _dimension(dim),
        _state(state),
        _seq_id(seq_id),
        _prev_state(prev_state),
        _prev_seq_id(prev_seq_id)
{
    // Nothing to do.
}


bool ClientEventListener::SwitchStateChangedEventInfo::Impl::merge( const AbstractDatabaseChange& other )
{
    // if the new change is switch change and the location and dimension is the same then merge

    const ClientEventListener::SwitchStateChangedEventInfo::Impl *chg_p(dynamic_cast<const ClientEventListener::SwitchStateChangedEventInfo::Impl*>(&other));
    if ( ! chg_p ) {
        return false;
    }
    if ( getMidplaneLocation() != chg_p->getMidplaneLocation() || getDimension() != chg_p->getDimension() ) {
        return false;
    }

    LOG_DEBUG_MSG( "Merging switch update" );

    _state = chg_p->getState();
    _seq_id = chg_p->getSequenceId();

    return true;
}


void ClientEventListener::SwitchStateChangedEventInfo::Impl::accept( AbstractDatabaseChangeVisitor& v )
{
    v.visitSwitchStateChanged( *this );
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::TorusCableStateChangedEventInfo::Impl


ClientEventListener::TorusCableStateChangedEventInfo::Impl::Impl(
        const std::string& from_location,
        const std::string& to_location,
        Hardware::State state,
        SequenceId seq_id,
        Hardware::State prev_state,
        SequenceId prev_seq_id
    ) :
        _from_location(from_location),
        _to_location(to_location),
        _state(state),
        _seq_id(seq_id),
        _prev_state(prev_state),
        _prev_seq_id(prev_seq_id)
{
    // Nothing to do.
}


bool ClientEventListener::TorusCableStateChangedEventInfo::Impl::merge( const AbstractDatabaseChange& other )
{
    // if the new change is cable change and the locations are the same then merge

    const ClientEventListener::TorusCableStateChangedEventInfo::Impl *chg_p(dynamic_cast<const ClientEventListener::TorusCableStateChangedEventInfo::Impl*>(&other));
    if ( ! chg_p ) {
        return false;
    }
    if ( getFromLocation() != chg_p->getFromLocation() || getToLocation() != chg_p->getToLocation() ) {
        return false;
    }

    LOG_DEBUG_MSG( "Merging torus cable update" );

    _state = chg_p->getState();
    _seq_id = chg_p->getSequenceId();

    return true;
}


void ClientEventListener::TorusCableStateChangedEventInfo::Impl::accept( AbstractDatabaseChangeVisitor& v )
{
    v.visitTorusCableStateChanged( *this );
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::IoCableStateChangedEventInfo::Impl


ClientEventListener::IoCableStateChangedEventInfo::Impl::Impl(
        const std::string& from_location,
        const std::string& to_location,
        Hardware::State state,
        SequenceId seq_id,
        Hardware::State prev_state,
        SequenceId prev_seq_id
    ) :
        _from_location(from_location),
        _to_location(to_location),
        _state(state),
        _seq_id(seq_id),
        _prev_state(prev_state),
        _prev_seq_id(prev_seq_id)
{
    // Nothing to do.
}


bool ClientEventListener::IoCableStateChangedEventInfo::Impl::merge( const AbstractDatabaseChange& other )
{
    // if the new change is cable change and the locations are the same then merge

    const ClientEventListener::IoCableStateChangedEventInfo::Impl *chg_p(dynamic_cast<const ClientEventListener::IoCableStateChangedEventInfo::Impl*>(&other));
    if ( ! chg_p ) {
        return false;
    }
    if ( getFromLocation() != chg_p->getFromLocation() || getToLocation() != chg_p->getToLocation() ) {
        return false;
    }

    LOG_DEBUG_MSG( "Merging I/O cable update" );

    _state = chg_p->getState();
    _seq_id = chg_p->getSequenceId();

    return true;
}


void ClientEventListener::IoCableStateChangedEventInfo::Impl::accept( AbstractDatabaseChangeVisitor& v )
{
    v.visitIoCableStateChanged( *this );
}


//-----------------------------------------------------------------------
// Members of class ClientEventListener::RasEventInfo::Impl.


ClientEventListener::RasEventInfo::Impl::Impl(
        RasRecordId rec_id,
        const std::string& msg_id,
        RasSeverity::Value severity,
        const std::string& block_id,
        Job::Id job_id
    ) :
        _rec_id(rec_id),
        _msg_id(msg_id),
        _severity(severity),
        _block_id(block_id),
        _job_id(job_id)
{
    // Nothing to do.
}


void ClientEventListener::RasEventInfo::Impl::accept( AbstractDatabaseChangeVisitor& v )
{
    v.visitRasEvent( *this );
}


} // namespace bgsched::realtime
} // namespace bgsched
