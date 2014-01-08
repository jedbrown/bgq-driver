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

#include <bgsched/Coordinates.h>
#include <bgsched/InputException.h>
#include <bgsched/InternalException.h>

#include "bgsched/MidplaneImpl.h"
#include "bgsched/utility.h"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>

#include <string.h>

LOG_DECLARE_FILE("bgsched");

using namespace bgsched;
using namespace std;
using namespace bgq::util;

namespace bgsched {

Midplane::Impl::Impl(
        const string& location,
        const XMLEntity* XMLEntityPtr
        ) :
    Hardware::Impl(location),
    _inUse(Midplane::NotInUse),
    _computeBlockName(),
    _switches(),
    _nodeBoards(),
    _machineCoordinates(0,0,0,0),
    _computeNodeMemory(0)
{
    _state = convertDatabaseState(XMLEntityPtr->attrByName("status"));

    try {
        _sequenceId = boost::lexical_cast<SequenceId>(XMLEntityPtr->attrByName("statusSeqID"));
        _computeNodeMemory = boost::lexical_cast<uint32_t>(XMLEntityPtr->attrByName("computeNodeMemory"));
        _machineCoordinates[Dimension::A] = boost::lexical_cast<uint32_t>(XMLEntityPtr->attrByName("torusA"));
        _machineCoordinates[Dimension::B] = boost::lexical_cast<uint32_t>(XMLEntityPtr->attrByName("torusB"));
        _machineCoordinates[Dimension::C] = boost::lexical_cast<uint32_t>(XMLEntityPtr->attrByName("torusC"));
        _machineCoordinates[Dimension::D] = boost::lexical_cast<uint32_t>(XMLEntityPtr->attrByName("torusD"));
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
        _inUse = Midplane::AllInUse;
    } else {
        if (strcmp(XMLEntityPtr->attrByName("inUse"),"N") == 0) {
            _inUse = Midplane::NotInUse;
        } else {
            _inUse = Midplane::SubdividedInUse;
        }
    }

    // Set "in use" compute block name
    _computeBlockName = string(XMLEntityPtr->attrByName("blockId"));
}

Midplane::Impl::Impl(
        const Midplane::Pimpl fromMidplane
        ) :
    Hardware::Impl(fromMidplane->getLocationString()),
    _inUse(fromMidplane->_inUse),
    _computeBlockName(fromMidplane->_computeBlockName),
    _switches(),
    _nodeBoards(),
    _machineCoordinates(fromMidplane->_machineCoordinates),
    _computeNodeMemory(fromMidplane->_computeNodeMemory)
{
    _state = fromMidplane->getState(); // Set midplane hardware state
    _isLiveState = fromMidplane->isLiveState(); // Set live state indicator
    _sequenceId = fromMidplane->getSequenceId(); // Set midplane sequence ID
    // Set midplane switches
    for(Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
        _switches[dim] = Switch::Pimpl(new Switch::Impl(fromMidplane->_switches[dim]));
    }
    // Set midplane node boards
    for (uint32_t i = 0; i < MaxNodeBoards; i++) {
        _nodeBoards[i] = NodeBoard::Pimpl(new NodeBoard::Impl(fromMidplane->_nodeBoards[i]));
    }
}

void
Midplane::Impl::dump(
        ostream& os
        )
{
    os << "   === S T A R T   O F   M I D P L A N E ===" << endl;
    os << "   Midplane location . . . . . . : " << getLocation() << endl;
    if (getInUse() == Midplane::AllInUse) {
        os << "   In use  . . . . . . . . . . . : 'All': by large block " << _computeBlockName << endl;
    } else {
        if (getInUse() == Midplane::SubdividedInUse){
            os << "   In use  . . . . . . . . . . . : 'Subdivided': check node boards for small block names" << endl;
        } else {
            os << "   In use  . . . . . . . . . . . : 'No': midplane is available for use" << endl;
        }
    }
    os << "   Hardware state  . . . . . . . : " << _state << endl;
    if (isLiveState()) {
        os << "   Hardware state from database  : Yes" << endl;
    } else {
        os << "   Hardware state from database  : No" << endl;
    }
    os << "   Sequence ID . . . . . . . . . : " << getSequenceId() << endl;
    os << "   Compute node memory (MB)  . . : " << getComputeNodeMemory() << endl;
    os << "   Total 'Available' node boards : " << getAvailableNodeBoardCount() << endl;
    os << "   === S T A R T   O F   N O D E B O A R D S  ===" << endl;
    // Dump midplane node boards
    for (uint32_t i = 0; i < MaxNodeBoards; i++) {
        _nodeBoards[i]->dump(os);
    }
    os << "   === E N D   O F   N O D E B O A R D S  ===" << endl;
    os << "   === S T A R T   O F   S W I T C H E S  ===" << endl;
    // Dump midplane switches
    for(Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
        _switches[dim]->dump(os);
    }
    os << "   === E N D   O F   S W I T C H E S  ===" << endl;
    os << "   === E N D   O F   M I D P L A N E ===" << endl;
}

uint32_t
Midplane::Impl::getComputeNodeMemory() const
{
    return _computeNodeMemory;
}

uint32_t
Midplane::Impl::getAvailableNodeBoardCount() const
{
    uint32_t nodeboardCount = 0;
    for (uint32_t i = 0; i < MaxNodeBoards; i++) {
        NodeBoard::Pimpl nodeboard = _nodeBoards[i];
        if (nodeboard->getState() == Hardware::Available) {
            nodeboardCount++;
        }
    }
    return nodeboardCount;
}

NodeBoard::Pimpl
Midplane::Impl::getNodeBoard(
        const uint32_t position
        ) const
{
    if (position < MaxNodeBoards) {
        return _nodeBoards[position];
    } else {
        // Node board position is outside valid range
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoardPosition,
                "Node board position argument must be in range 0 to 15"
                );
    }
}

Switch::Pimpl
Midplane::Impl::getSwitch(
        const Dimension& dimension
        ) const
{
    if (dimension <= Dimension::D) {
        return _switches[dimension];
    } else {
        // Dimension is outside valid range
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidDimension,
                "Dimension must be A, B, C or D"
                );
    }
}

Switch::Pimpl
Midplane::Impl::getSwitch(
        const Location::Dimension::Value dimension
        ) const
{
    switch (dimension) {
    case Location::Dimension::A:
        return _switches[0];
        break;
    case Location::Dimension::B:
        return _switches[1];
        break;
    case Location::Dimension::C:
        return _switches[2];
        break;
    case Location::Dimension::D:
        return _switches[3];
        break;
    default:
        // Dimension is outside valid range
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidDimension,
                "Dimension must be A, B, C or D"
        );
    }
}

void
Midplane::Impl::addNodeBoard(
        const uint32_t position,
        const NodeBoard::Pimpl n
        )
{
    if (position < MaxNodeBoards) {
        _nodeBoards[position] = n;
    } else {
        // Node board position is outside valid range
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidNodeBoardPosition,
                "Node board position argument must be in range 0 to 15"
                );
    }
}

void
Midplane::Impl::addSwitch(
        const uint32_t dimension,
        const Switch::Pimpl s
        )
{
    if (dimension <= Dimension::D) {
        _switches[dimension] = s;
    } else {
        // Dimension is outside valid range
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidDimension,
                "Dimension must be A, B, C or D"
        );
    }
}

const Coordinates&
Midplane::Impl::getCoordinates( ) const
{
    return _machineCoordinates;
}

EnumWrapper<Midplane::InUse>
Midplane::Impl::getInUse() const
{
    return _inUse;
}

void
Midplane::Impl::setInUse(
        Midplane::InUse inUse
        )
{
    _inUse = inUse;
    if (_inUse == Midplane::AllInUse) {
        LOG_DEBUG_MSG("Midplane " << getLocationString() << " 'in use' state was set to 'All'.");
    } else {
        if (_inUse == Midplane::SubdividedInUse){
            LOG_DEBUG_MSG("Midplane " << getLocationString() << " 'in use' state was set to 'Subdivided'.");
        } else {
            LOG_DEBUG_MSG("Midplane " << getLocationString() << " 'in use' state was set to 'No'.");
        }
    }
}

const string&
Midplane::Impl::getComputeBlockName() const
{
    return _computeBlockName;
}

void
Midplane::Impl::setComputeBlockName(
        const string& computeBlockName
        )
{
    _computeBlockName = computeBlockName;
    if (computeBlockName.empty()) {
        LOG_DEBUG_MSG("Midplane " << getLocationString() << " 'in use' block name was cleared.");
    } else {
        LOG_DEBUG_MSG("Midplane " << getLocationString() << " set to 'in use' by block " << computeBlockName);
    }
}

ostream&
operator<<(
        ostream& os,
        const Coordinates& coordinates
        )
{
    os << "< " << coordinates[Dimension::A] << "," << coordinates[Dimension::B] << "," << coordinates[Dimension::C] << "," << coordinates[Dimension::D] << " >";

    return os;
}

} // namespace bgsched
