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


#include "ClientEventListenerV2Impl.h"

#include "AbstractDatabaseChangeVisitor.h"

#include <utility/include/Log.h>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/export.hpp>


BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListenerV2::IoDrawerStateChangedEventInfo::Impl, "IoDrawerStateChangedEventInfo" )
BOOST_CLASS_EXPORT_GUID( bgsched::realtime::ClientEventListenerV2::IoNodeStateChangedEventInfo::Impl, "IoNodeStateChangedEventInfo" )


LOG_DECLARE_FILE( "bgsched" );


namespace bgsched {
namespace realtime {


//------------------------------------------------------------
// ClientEventListenerV2::IoDrawerStateChangedEventInfo::Impl


ClientEventListenerV2::IoDrawerStateChangedEventInfo::Impl::Impl(
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


bool ClientEventListenerV2::IoDrawerStateChangedEventInfo::Impl::merge( const AbstractDatabaseChange& other )
{
    // if the new change is node change and the location is the same then merge

    const Impl *chg_p(dynamic_cast<const Impl*>(&other));
    if ( ! chg_p ) {
        return false;
    }
    if ( getLocation() != chg_p->getLocation() ) {
        return false;
    }

    LOG_DEBUG_MSG( "Merging i/o drawer update" );

    _state = chg_p->getState();
    _seq_id = chg_p->getSequenceId();

    return true;
}


void ClientEventListenerV2::IoDrawerStateChangedEventInfo::Impl::accept( AbstractDatabaseChangeVisitor& v )
{
    v.visitIoDrawerStateChanged( *this );
}


//-----------------------------------------------------------
// ClientEventListenerV2::IoNodeStateChangedEventInfo::Impl


ClientEventListenerV2::IoNodeStateChangedEventInfo::Impl::Impl(
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


bool ClientEventListenerV2::IoNodeStateChangedEventInfo::Impl::merge( const AbstractDatabaseChange& other )
{
    // if the new change is node change and the location is the same then merge

    const Impl *chg_p(dynamic_cast<const Impl*>(&other));
    if ( ! chg_p ) {
        return false;
    }
    if ( getLocation() != chg_p->getLocation() ) {
        return false;
    }

    LOG_DEBUG_MSG( "Merging i/o node update" );

    _state = chg_p->getState();
    _seq_id = chg_p->getSequenceId();

    return true;
}


void ClientEventListenerV2::IoNodeStateChangedEventInfo::Impl::accept( AbstractDatabaseChangeVisitor& v )
{
    v.visitIoNodeStateChanged( *this );
}


} } // namespace bgsched::realtime
