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

#include <bgsched/Dimension.h>
#include <bgsched/DatabaseException.h>
#include <bgsched/InternalException.h>

#include "bgsched/NodeBoardImpl.h"
#include "bgsched/utility.h"

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/Exception.h>
#include <db/include/api/genblock.h>

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>

#include <sstream>
#include <string.h>

using namespace bgsched;
using namespace std;

LOG_DECLARE_FILE("bgsched");

namespace bgsched {

NodeBoard::Impl::Impl(
        const string& location,
        const XMLEntity* XMLEntityPtr
        ) :
    Hardware::Impl(location),
    _inUse(false),
    _computeBlockName(),
    _quadrant(NodeBoard::Q1),
    _isMetaState(false),
    _availableNodeCount(0)
{
    _state = convertDatabaseState(XMLEntityPtr->attrByName("status"));

    // Set meta-state indicator
    if (strcmp(XMLEntityPtr->attrByName("isMetaState"),"Y") == 0) {
        _isMetaState = true;
    } else {
        _isMetaState = false;
    }

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

    // Set node board quadrant
    if (strcmp(XMLEntityPtr->attrByName("quarter"),"Q1") == 0) {
        _quadrant = NodeBoard::Q1;
    } else if (strcmp(XMLEntityPtr->attrByName("quarter"),"Q2") == 0) {
        _quadrant = NodeBoard::Q2;
    } else if (strcmp(XMLEntityPtr->attrByName("quarter"),"Q3") == 0) {
        _quadrant = NodeBoard::Q3;
    } else {
        _quadrant = NodeBoard::Q4;
    }

    try {
        _availableNodeCount = boost::lexical_cast<uint32_t>(XMLEntityPtr->attrByName("nodeCount"));
    } catch (const boost::bad_lexical_cast& e) {
        LOG_INFO_MSG(e.what());
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::XMLParseError,
                "Unexpected error parsing XML."
        );
    }

    // Set "in use" indicator
    if (strcmp(XMLEntityPtr->attrByName("inUse"),"Y") == 0) {
        _inUse = true;
    } else {
        _inUse = false;
    }

    // Set "in use" compute block name
    _computeBlockName = string(XMLEntityPtr->attrByName("blockId"));

}

NodeBoard::Impl::Impl(
        const NodeBoard::Pimpl fromNodeBoard
        ) :
    Hardware::Impl(fromNodeBoard->getLocationString()),
    _inUse(fromNodeBoard->_inUse),
    _computeBlockName(fromNodeBoard->_computeBlockName),
    _quadrant(fromNodeBoard->_quadrant),
    _isMetaState(fromNodeBoard->_isMetaState),
    _availableNodeCount(fromNodeBoard->_availableNodeCount)

{
    _state = fromNodeBoard->getState(); // Set node board hardware state
    _isLiveState = fromNodeBoard->isLiveState(); // Set live state indicator
    _sequenceId = fromNodeBoard->getSequenceId(); // Set node board sequence ID
}

void
NodeBoard::Impl::dump(
        ostream& os
        )
{
    os << "     Node board location . . . . : " << getLocation() << endl;
    if (isInUse()) {
        os << "     In use  . . . . . . . . . . : Yes, by block " << _computeBlockName << endl;
    } else {
        os << "     In use  . . . . . . . . . . : No, unless midplane indicates 'All'" << endl;
    }
    os << "     Hardware state. . . . . . . : " << _state << endl;
    if (isLiveState()) {
        os << "     Hardware state from database: Yes" << endl;
    } else {
        os << "     Hardware state from database: No" << endl;
    }
    if (isMetaState()) {
        os << "     Meta-state  . . . . . . . . : Yes" << endl;
    } else {
        os << "     Meta-state  . . . . . . . . : No" << endl;
    }
    os << "     Total 'Available' nodes . . : " << getAvailableNodeCount() << endl;
    os << "     Sequence ID . . . . . . . . : " << getSequenceId() << endl;
    os << "     Quadrant  . . . . . . . . . : " << _quadrant << endl;
}

bool
NodeBoard::Impl::isInUse() const
{
    return _inUse;
}

const string&
NodeBoard::Impl::getComputeBlockName() const
{
    return _computeBlockName;
}

const EnumWrapper<NodeBoard::Quadrant>
NodeBoard::Impl::getQuadrant() const
{
    return _quadrant;
}

void
NodeBoard::Impl::setInUse(
        bool inUse
        )
{
    _inUse = inUse;
    if (_inUse) {
        LOG_TRACE_MSG("Node board " << getLocationString() << " 'in use' state was set to true.");
    } else {
        LOG_TRACE_MSG("Node board " << getLocationString() << " 'in use' state was set to false.");
    }
}

void
NodeBoard::Impl::setComputeBlockName(
        const string& computeBlockName
        )
{
    _computeBlockName = computeBlockName;
    if (computeBlockName.empty()) {
        LOG_TRACE_MSG("Node board " << getLocationString() << " 'in use' block name was cleared.");
    } else {
        LOG_TRACE_MSG("Node board " << getLocationString() << " set to 'in use' by block " << computeBlockName);
    }
}


void
NodeBoard::Impl::setQuadrant(
        NodeBoard::Quadrant quadrant
        )
{
    _quadrant = quadrant;
}

const Coordinates
NodeBoard::Impl::getMidplaneCoordinates() const
{
    Coordinates midplaneCoordinates(0,0,0,0);
    BGQDB::MidplaneCoordinate mp_coord;
    try {
        BGQDB::torusCoordinateForMidplane(_location.getMidplaneLocation(), mp_coord);
        midplaneCoordinates[Dimension::A] = mp_coord[Dimension::A];
        midplaneCoordinates[Dimension::B] = mp_coord[Dimension::B];
        midplaneCoordinates[Dimension::C] = mp_coord[Dimension::C];
        midplaneCoordinates[Dimension::D] = mp_coord[Dimension::D];
    } catch (const BGQDB::Exception& e) {
        ostringstream os;
        os << "Unable to get midplane coordinates for midplane location " << _location.getMidplaneLocation() << ". Error is: " << e.what();
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                os.str()
        );
    }
    return midplaneCoordinates;
}

const string
NodeBoard::Impl::getMidplaneLocation() const
{
    return _location.getMidplaneLocation();
}

bool
NodeBoard::Impl::isMetaState() const
{
    return _isMetaState;
}

void
NodeBoard::Impl::setMetaState(
        bool metaState
        )
{
    _isMetaState = metaState;
}

uint32_t
NodeBoard::Impl::getAvailableNodeCount() const
{
    return _availableNodeCount;
}

void
NodeBoard::Impl::setAvailableNodeCount(
        const uint32_t nodeCount
        )
{
    _availableNodeCount = nodeCount;
}

ostream&
operator<<(
        ostream& os,
        NodeBoard::Quadrant quadrant)
{
    switch(quadrant)
    {
        case NodeBoard::Q1:
            os << "Q1";
            break;
        case NodeBoard::Q2:
            os << "Q2";
            break;
        case NodeBoard::Q3:
            os << "Q3";
            break;
        case NodeBoard::Q4:
            os << "Q4";
            break;
        default:
            os << "Unknown quadrant";
            break;
    }
    return os;
}

} // namespace bgsched






