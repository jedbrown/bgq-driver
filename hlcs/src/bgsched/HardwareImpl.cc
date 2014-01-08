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

#include <bgsched/InternalException.h>

#include "bgsched/HardwareImpl.h"
#include "bgsched/utility.h"

#include <db/include/api/BGQDBlib.h>

#include <utility/include/Log.h>

LOG_DECLARE_FILE("bgsched");

using namespace bgsched;
using namespace std;

namespace bgsched {

Hardware::Impl::Impl(
        const string& location
        ) :
    _state(Hardware::Available),
    _isLiveState(true), // Indicate state comes directly from database and not overridden by "setState"
    _location(location),
    _sequenceId(0)
{
    // Nothing to do
}

Hardware::Impl::~Impl()
{
    // Nothing to do
}

Hardware::State
Hardware::Impl::getState( ) const
{
    return _state;
}

SequenceId
Hardware::Impl::getSequenceId() const
{
    return _sequenceId;
}

void
Hardware::Impl::setState(
        Hardware::State state,
        bool fromDatabase
        )
{
    _state = state;
    _isLiveState = fromDatabase;
}

bool
Hardware::Impl::isLiveState() const
{
    return _isLiveState;
}


void
Hardware::Impl::setSequenceId(
        SequenceId sequenceId
        )
{
    _sequenceId = sequenceId;
}

const string&
Hardware::Impl::getLocationString () const
{
    return _location.getLocation();
}


const bgq::util::Location
Hardware::Impl::getLocation () const
{
    return _location;
}

Hardware::State
Hardware::Impl::convertDatabaseState(
        const char *state
        )
{
    if (strcmp(state,BGQDB::HARDWARE_AVAILABLE) == 0) {
        return Hardware::Available;
    } else if (strcmp(state,BGQDB::HARDWARE_SERVICE) == 0) {
        return Hardware::Service;
    } else if (strcmp(state,BGQDB::HARDWARE_MISSING) == 0) {
        return Hardware::Missing;
    } else if (strcmp(state,BGQDB::HARDWARE_ERROR) == 0) {
        return Hardware::Error;
    } else if (strcmp(state,BGQDB::SOFTWARE_FAILURE) == 0) {
        return Hardware::SoftwareFailure;
    }

    // If get to this point the database state is unknown
    THROW_EXCEPTION(
            bgsched::InternalException,
            bgsched::InternalErrors::InconsistentDataError,
            "Unexpected hardware state found."
    );
}

ostream&
operator<<(
        ostream& os,
        const Hardware::Impl& hardware
        )
{
    os << hardware.getLocation() << ": " << hardware.getState();
    return os;
}

ostream&
operator<<(
        ostream& os,
        Hardware::State state
        )
{
    switch (state)
    {
        case Hardware::Available:
            os << "Available";
            break;
        case Hardware::Missing:
            os << "Missing";
            break;
        case Hardware::Error:
            os << "Error";
            break;
        case Hardware::Service:
            os << "Service";
            break;
        case Hardware::SoftwareFailure:
            os << "Software Failure";
            break;
        default:
            os << "Unknown";
            break;
    }

    return os;
}

} // namespace bgsched
