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

#include "bgsched/IOLinkImpl.h"
#include "bgsched/utility.h"

#include <bgq_util/include/Location.h>

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE("bgsched");

using namespace bgsched;
using namespace std;

namespace bgsched {

IOLink::Impl::Impl(
        const string& location,
        const XMLEntity* XMLEntityPtr
        ) :
    Hardware::Impl(location),
    _destination()
{
    _destination = XMLEntityPtr->attrByName("destination");
    _state = convertDatabaseState(XMLEntityPtr->attrByName("status"));

    try {
        _sequenceId = boost::lexical_cast<SequenceId>(XMLEntityPtr->attrByName("statusSeqID"));
    } catch (const boost::bad_lexical_cast& e) {
        LOG_INFO_MSG(e.what());
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::XMLParseError,
                "Unexpected error parsing XML."
                );
    }
    _IONodeState = convertDatabaseState(XMLEntityPtr->attrByName("IONstatus"));
}

IOLink::Impl::Impl(
        const IOLink::Pimpl fromIOLink
        ) :
    Hardware::Impl(fromIOLink->getLocation()),          // Set I/O link hardware location
    _destination(fromIOLink->getDestinationLocation()), // Set I/O node destination location
    _IONodeState(fromIOLink->getIONodeState())          // Set I/O node hardware state
{
    _state = fromIOLink->getState();                    // Set I/O link hardware state
    _isLiveState = fromIOLink->isLiveState();           // Set live state indicator
    _sequenceId = fromIOLink->getSequenceId();          // Set I/O link sequence ID
}

void
IOLink::Impl::dump(
        ostream& os
        ) const
{
    os << "       I/O link location  .  . . : " << getLocation() << endl;
    os << "       I/O link hardware state . : " << _state << endl;
    if (isLiveState()) {
        os << "       Hardware state from db  . : Yes" << endl;
    } else {
        os << "       Hardware state from db  . : No" << endl;
    }
    os << "       I/O link sequence ID  . . : " << getSequenceId() << endl;
    os << "       I/O node location  .  . . : " << getDestinationLocation() << endl;
    os << "       I/O node hardware state . : " << _IONodeState << endl;
}

const string&
IOLink::Impl::getDestinationLocation() const
{
    return _destination;
}

Hardware::State
IOLink::Impl::getIONodeState() const
{
    return _IONodeState;
}


} // namespace bgsched
