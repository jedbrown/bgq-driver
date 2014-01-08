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

#include "bgsched/CableImpl.h"
#include "bgsched/SwitchImpl.h"
#include "bgsched/utility.h"

#include <utility/include/Log.h>
#include <utility/include/XMLEntity.h>

#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE("bgsched");

using namespace bgsched;
using namespace std;

namespace bgsched {

Cable::Impl::Impl(
        const string& location,
        const XMLEntity* XMLEntityPtr
        ) :
    Hardware::Impl(location),
    _destination()
{
    _destination.set(string(XMLEntityPtr->attrByName("destination")));
    _state = convertDatabaseState(XMLEntityPtr->attrByName("status"));

    try {
        _sequenceId = boost::lexical_cast<SequenceId>(XMLEntityPtr->attrByName("statusSeqID"));
    } catch (const boost::bad_lexical_cast& e) {
        LOG_INFO_MSG( e.what() );
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::XMLParseError,
                "Unexpected error parsing XML."
        );
    }
}

Cable::Impl::Impl(
        const Cable::Pimpl fromCable
        ) :
    Hardware::Impl(fromCable->getLocation()), // Set hardware location
    _destination(fromCable->getDestinationLocation()) // Set destination
{
    _state = fromCable->getState(); // Set cable hardware state
    _isLiveState = fromCable->isLiveState(); // Set live state indicator
    _sequenceId = fromCable->getSequenceId(); // Set cable sequence ID
}

void
Cable::Impl::dump(
        ostream& os
        ) const
{
    os << "       Cable location . . .  . . : " << getLocation() << endl;
    os << "       Destination location  . . : " << getDestinationLocation() << endl;
    os << "       Hardware state  . . . . . : " << _state << endl;
    if (isLiveState()) {
        os << "       Hardware state from db  . : Yes" << endl;
    } else {
        os << "       Hardware state from db  . : No" << endl;
    }
    os << "       Sequence ID . . . . . . . : " << getSequenceId() << endl;
}

const string&
Cable::Impl::getDestinationLocation() const
{
    return _destination.getLocation();
}

} // namespace bgsched
