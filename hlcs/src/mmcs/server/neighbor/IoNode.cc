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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#include "IoNode.h"

#include "Direction.h"
#include "../BCNodeInfo.h"
#include "../BlockControllerBase.h"

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace neighbor {

IoNode::IoNode(
        const BlockPtr& block,
        const int msgid,
        const bgq::util::Location& location
        ) :
    Info( block, msgid, location )
{
    LOG_TRACE_MSG( _location );
}

void
IoNode::impl(
        const char* rawdata
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );

    BCNodeInfo* const nodeInfo( _block->findNodeInfo(_location) );
    if ( !nodeInfo ) {
        LOG_WARN_MSG( "Could not find node info for " << _location );
        return;
    }
    const Direction direction( rawdata );
    if ( direction.dim() == Dimension::Invalid ) return;
    switch ( direction.dim() ) {
        case Dimension::A:
        case Dimension::B:
        case Dimension::C:
            // these are valid I/O torus dimensions, fall through
            break;
        case Dimension::D:
        case Dimension::IO:
            // should have already handled this, so fall through
        default:
            BOOST_THROW_EXCEPTION(
                    std::invalid_argument( "Invalid I/O torus dimension: " + direction.buf() )
                    );
    }

    BCNodeInfo* const neighbor( this->getNeighbor(nodeInfo, direction) );
    if ( !neighbor ) {
        LOG_WARN_MSG( "Could not find " << direction << " neighbor for " << _location );
        _details << "NEIGHBOR=N/A";
        return;
    }

    LOG_DEBUG_MSG( _location << " " << direction << " neighbor is " << neighbor->location() );
    _details << "NEIGHBOR=" << neighbor->location();

    // special case for ND Sender events, we need to swap the from and to locations
    // to correctly callout the link chip and cables used in this path
    const bool swap( _msgId == 0x00080038 );

    // append extra link chip and optical cable details as needed
    this->extra( 
            swap ? neighbor : nodeInfo,
            swap ? nodeInfo : neighbor,
            direction
            );
}

void
IoNode::extra(
        BCNodeInfo* const me,
        BCNodeInfo* const neighbor,
        const Direction& direction
        )
{
    BOOST_ASSERT( neighbor );
    switch ( direction.dim() ) {
        case Dimension::A:
        case Dimension::B:
        case Dimension::C:
            // fall through, these are ok
            break;
        default:
            LOG_WARN_MSG( "Unsupported direction dimension: " << direction );
            return;
    }
    switch( direction.orientation() ) {
        case Orientation::Plus:
        case Orientation::Minus:
            // fall through, these are ok
            break;
        default:
            LOG_WARN_MSG( "Unsupported direction orientation: " << direction );
            return;
    }

    // these containers map a jtag port (0 - 7) to a link chip, they
    // are based on the BG/Q naming conventions document
    static const std::vector<std::string> linkChipA = boost::assign::list_of
        ("U01")("U01")("U01")("U00")("U00")("U00")("U00")("U00");

    static const std::vector<std::string> linkChipB = boost::assign::list_of
        ("U01")("U01")("U00")("U00")("U01")("U01")("U00")("U00");

    static const std::vector<std::string> linkChipC = boost::assign::list_of
        ("U03")("U03")("U03")("U03")("U02")("U02")("U02")("U02");

    const std::string fromRack( _location.getLocation().substr(0,3) );
    const std::string toRack( neighbor->location().substr(0,3) );
    const std::string fromDrawer( _location.getLocation().substr(4,2) );
    const std::string toDrawer( neighbor->location().substr(4,2) );
    const bool sameDrawer( (fromRack + fromDrawer) == (toRack + toDrawer) );

    _details << " Connects through ";
    if ( sameDrawer ) {
        if ( this->isWrapped(direction, me, neighbor) ) {
            _details << "link chip (wrap) ";
            _details << fromRack << "-" << fromDrawer << "-";
            switch ( direction.dim() ) {
                case Dimension::A: _details << linkChipA.at( me->_iopos.jtagPort() ); break;
                case Dimension::B: _details << linkChipB.at( me->_iopos.jtagPort() ); break;
                case Dimension::C: _details << linkChipC.at( me->_iopos.jtagPort() ); break;
                default: LOG_WARN_MSG( "Unhandled wrapped dimension: " << direction );
            }
        } else {
            _details << "I/O drawer";
        }

        return;
    }

    // neighbor is in a different drawer, so two link chips and a cable are used
    const std::string linkChip(
        direction.dim() == Dimension::A ? linkChipA[ me->_iopos.jtagPort() ] :
        direction.dim() == Dimension::B ? linkChipB[ me->_iopos.jtagPort() ] :
        linkChipC[ me->_iopos.jtagPort() ]
        );

    _details << "link chips " <<
        (direction.orientation() == Orientation::Plus ? fromRack : toRack) << "-" <<
        (direction.orientation() == Orientation::Plus ? fromDrawer : toDrawer) << "-" << linkChip <<
        " and " <<
        (direction.orientation() == Orientation::Plus ? toRack : fromRack) << "-" <<
        (direction.orientation() == Orientation::Plus ? toDrawer : fromDrawer) << "-" << linkChip;

    // a combination of the direction and link chip will give us the port in use
    // this information comes from the naming convention document
    static const std::map<std::string,std::string> ports = boost::assign::map_list_of
        ("A+U00","T01")("A+U01","T09")
        ("A-U00","T05")("A-U01","T13")
        ("B+U00","T07")("B+U01","T15")
        ("B-U00","T03")("B-U01","T11")
        ("C+U03","T20")("C+U02","T17")
        ("C-U03","T16")("C-U02","T21");

    _details << " using cable from port " <<
        (direction.orientation() == Orientation::Plus ? toRack : fromRack) << "-" <<
        (direction.orientation() == Orientation::Plus ? toDrawer : fromDrawer) << "-" <<
        ports.at( direction.buf() + linkChip );

    _details << " to port " <<
        (direction.orientation() == Orientation::Plus ? fromRack : toRack) << "-" <<
        (direction.orientation() == Orientation::Plus ? fromDrawer : toDrawer) << "-" <<
        ports.at(
                boost::lexical_cast<std::string>(direction.dim()) +
                // reverse the direction given to find the other end of the cable
                (direction.orientation() == Orientation::Plus ? "-" : "+") +
                linkChip
                );
}

} } } // namespace mmcs::server::neighbor
