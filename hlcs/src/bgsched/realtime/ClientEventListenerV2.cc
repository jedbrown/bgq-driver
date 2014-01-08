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

#include <bgsched/realtime/ClientEventListenerV2.h>

#include "ClientEventListenerV2Impl.h"

#include <utility/include/Log.h>

#include <iostream>


using std::ostream;


LOG_DECLARE_FILE( "bgsched" );




namespace bgsched {
namespace realtime {


static ostream& operator<<(
        ostream& os,
        const ClientEventListenerV2::IoDrawerStateChangedEventInfo& info
    )
{
    return os << "{"
            "location=" << info.getLocation() <<
           " state=" << info.getState() <<
           " sequenceId=" << info.getSequenceId() <<
           " previousState=" << info.getPreviousState() <<
           " previousSequenceId=" << info.getPreviousSequenceId() <<
        "}";
}


static ostream& operator<<(
        ostream& os,
        const ClientEventListenerV2::IoNodeStateChangedEventInfo& info
    )
{
    return os << "{"
            "location=" << info.getLocation() <<
           " state=" << info.getState() <<
           " sequenceId=" << info.getSequenceId() <<
           " previousState=" << info.getPreviousState() <<
           " previousSequenceId=" << info.getPreviousSequenceId() <<
        "}";
}


//---------------------------------------------------------------------
// Members of class ClientEventListenerV2::IoDrawerStateChangedEventInfo

const std::string& ClientEventListenerV2::IoDrawerStateChangedEventInfo::getLocation() const  { return _impl_ptr->getLocation(); }
Hardware::State ClientEventListenerV2::IoDrawerStateChangedEventInfo::getState() const  { return _impl_ptr->getState(); }
SequenceId ClientEventListenerV2::IoDrawerStateChangedEventInfo::getSequenceId() const  { return _impl_ptr->getSequenceId(); }
Hardware::State ClientEventListenerV2::IoDrawerStateChangedEventInfo::getPreviousState() const  { return _impl_ptr->getPreviousState(); }
SequenceId ClientEventListenerV2::IoDrawerStateChangedEventInfo::getPreviousSequenceId() const  { return _impl_ptr->getPreviousSequenceId(); }

ClientEventListenerV2::IoDrawerStateChangedEventInfo::IoDrawerStateChangedEventInfo( const Pimpl& impl_ptr )
    : _impl_ptr(impl_ptr)
{
    // Nothing to do.
}


//---------------------------------------------------------------------
// Members of class ClientEventListenerV2::IoNodeStateChangedEventInfo

const std::string& ClientEventListenerV2::IoNodeStateChangedEventInfo::getLocation() const  { return _impl_ptr->getLocation(); }
Hardware::State ClientEventListenerV2::IoNodeStateChangedEventInfo::getState() const  { return _impl_ptr->getState(); }
SequenceId ClientEventListenerV2::IoNodeStateChangedEventInfo::getSequenceId() const  { return _impl_ptr->getSequenceId(); }
Hardware::State ClientEventListenerV2::IoNodeStateChangedEventInfo::getPreviousState() const  { return _impl_ptr->getPreviousState(); }
SequenceId ClientEventListenerV2::IoNodeStateChangedEventInfo::getPreviousSequenceId() const  { return _impl_ptr->getPreviousSequenceId(); }

ClientEventListenerV2::IoNodeStateChangedEventInfo::IoNodeStateChangedEventInfo( const Pimpl& impl_ptr )
    : _impl_ptr(impl_ptr)
{
    // Nothing to do.
}


//----------------------------------------
// Members of class ClientEventListenerV2


void ClientEventListenerV2::handleIoDrawerStateChangedEvent(
        const IoDrawerStateChangedEventInfo& info //!< I/O drawer state changed event information.
    )
{
    LOG_DEBUG_MSG( "Ignoring I/O drawer state changed real-time event. info=" << info );
}


void ClientEventListenerV2::handleIoNodeStateChangedEvent(
        const IoNodeStateChangedEventInfo& info //!< I/O node state changed event information.
    )
{
    LOG_DEBUG_MSG( "Ignoring I/O node state changed real-time event. info=" << info );
}


} } // namespace bgsched::realtime
