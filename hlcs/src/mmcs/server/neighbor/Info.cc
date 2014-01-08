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


#include "Info.h"

#include "ComputeNode.h"
#include "Direction.h"
#include "IoLink.h"
#include "IoNode.h"
#include "LinkChip.h"
#include "utility.h"
#include "../BCNodeInfo.h"
#include "../BlockControllerBase.h"

#include <firmware/include/personality.h>

#include <utility/include/Log.h>

#include <boost/assign/list_of.hpp>


LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace neighbor {

std::auto_ptr<Info>
Info::create(
        const boost::shared_ptr<BlockControllerBase>& block,
        int msgid,
        const bgq::util::Location& location,
        const char* rawdata
        )
{
    std::auto_ptr<Info> result;
    if (
            location.getType() == bgq::util::Location::ComputeCardOnNodeBoard ||
            location.getType() == bgq::util::Location::ComputeCardOnIoBoard
       )
    {
        const Direction direction( rawdata );
        if ( direction.dim() == Dimension::IO ) {
            result.reset( new IoLink(block, msgid, location) );
        } else if ( direction.dim() == Dimension::D && location.getType() == bgq::util::Location::ComputeCardOnIoBoard ) {
            result.reset( new IoLink(block, msgid, location) );
        } else if (location.getType() == bgq::util::Location::ComputeCardOnNodeBoard ) {
            result.reset( new ComputeNode(block, msgid, location) );
        } else if ( location.getType() == bgq::util::Location::ComputeCardOnIoBoard ) {
            result.reset( new IoNode(block, msgid, location) );
        } else {
            BOOST_ASSERT( !"shouldn't get here" );
        }
    } else if ( 
            location.getType() == bgq::util::Location::LinkModuleOnNodeBoard ||
            location.getType() == bgq::util::Location::LinkModuleOnIoBoardInIoRack ||
            location.getType() == bgq::util::Location::LinkModuleOnIoBoardInComputeRack
            )
    {
        result.reset( new LinkChip(block, msgid, location) );
    } else {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    std::string(location) + " is not a compute node or link chip location"
                    )
                );
    }

    result->impl( rawdata );

    return result;
}

Info::Info(
        const boost::shared_ptr<BlockControllerBase>& block,
        const int msgId,
        const bgq::util::Location& location
        ) :
    _details(),
    _msgId( msgId ),
    _location( location ),
    _block( block )
{
    LOG_TRACE_MSG( location );
}

Info::~Info()
{
    const std::string details( this->getDetails() );
    if ( details.empty() ) return;

    LOG_DEBUG_MSG( _location << ": " << details );
}

BCNodeInfo*
Info::getNeighbor(
        BCNodeInfo* const node,
        const Direction& direction
        ) const
{
    LOG_TRACE_MSG( __FUNCTION__ );

    // get torus coordinates from personality
    const std::vector<int> coords = boost::assign::list_of
            (node->personality().Network_Config.Acoord)
            (node->personality().Network_Config.Bcoord)
            (node->personality().Network_Config.Ccoord)
            (node->personality().Network_Config.Dcoord)
            (node->personality().Network_Config.Ecoord)
            ;
    LOG_DEBUG_MSG(
            _location << " (" << coords[0] << "," << coords[1] << "," << coords[2] << "," << coords[3] << "," << coords[4] << ")"
            );
   
    // get block size from personality
    const std::vector<int> dimensions = boost::assign::list_of
            (node->personality().Network_Config.Anodes)
            (node->personality().Network_Config.Bnodes)
            (node->personality().Network_Config.Cnodes)
            (node->personality().Network_Config.Dnodes)
            (node->personality().Network_Config.Enodes)
            ;
    LOG_DEBUG_MSG(
            _block->getBlockName() << " (" << 
            dimensions[0] << "," << 
            dimensions[1] << "," << 
            dimensions[2] << "," << 
            dimensions[3] << "," << 
            dimensions[4] << ")"
            );
   
    // get network flags for each dimension from personality
    const std::vector<uint64_t> flags = boost::assign::list_of
            (node->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_A)
            (node->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_B)
            (node->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_C)
            (node->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_D)
            (node->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_E)
            ;
    LOG_DEBUG_MSG(
            "flags (" << 
            std::hex << flags[0] << "," << 
            std::hex << flags[1] << "," << 
            std::hex << flags[2] << "," <<
            std::hex << flags[3] << "," << 
            std::hex << flags[4] << ")"
            );

    // use dimension as index into the arrays built previously to get the next or previous
    // coordinate in the direction provided
    const unsigned index = direction.dim();
    LOG_DEBUG_MSG( "index " << index );
    const int neighbor_coord(
            direction.orientation() == Orientation::Plus ?
            next_coord( coords[index], dimensions[index], flags[index] ) :
            prev_coord( coords[index], dimensions[index], flags[index] )
            );

    LOG_DEBUG_MSG( direction << " neighbor coordinate " << neighbor_coord );

    // find our neighbor using the direction provided
    return _block->findNodeInfo(
            direction.dim() == Dimension::A ? neighbor_coord : coords[0],
            direction.dim() == Dimension::B ? neighbor_coord : coords[1],
            direction.dim() == Dimension::C ? neighbor_coord : coords[2],
            direction.dim() == Dimension::D ? neighbor_coord : coords[3],
            direction.dim() == Dimension::E ? neighbor_coord : coords[4]
            );
}

bool
Info::isWrapped(
        const Direction& direction,
        BCNodeInfo* const node,
        BCNodeInfo* const neighbor
        ) const
{
    BOOST_ASSERT( node );
    BOOST_ASSERT( neighbor );
        bool result = false;

    if ( direction.orientation() == Orientation::Minus ) {
        // minus orientation means the link chip will wrap if our
        // coordinate in this dimension is zero
        const std::vector<int> coords = boost::assign::list_of
            (node->personality().Network_Config.Acoord)
            (node->personality().Network_Config.Bcoord)
            (node->personality().Network_Config.Ccoord)
            (node->personality().Network_Config.Dcoord)
            (node->personality().Network_Config.Ecoord)
            ;
        result = (coords[direction.dim()] == 0);
    } else if ( direction.orientation() == Orientation::Plus ) {
        // plus orientation means the link chip will wrap if our neighbor's
        // coordinate in this dimension is zero
        const std::vector<int> coords = boost::assign::list_of
            (neighbor->personality().Network_Config.Acoord)
            (neighbor->personality().Network_Config.Bcoord)
            (neighbor->personality().Network_Config.Ccoord)
            (neighbor->personality().Network_Config.Dcoord)
            (neighbor->personality().Network_Config.Ecoord)
            ;
        result = (coords[direction.dim()] == 0);
    }

    return result;    
}

} } } // namespace mmcs::server::neighbor
