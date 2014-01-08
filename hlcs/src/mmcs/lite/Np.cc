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

#include "Np.h"

#include "JobInfo.h"

#include "server/CNBlockController.h"
#include "server/IOBlockController.h"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>

#include <stdexcept>


LOG_DECLARE_FILE( "mmcs.lite" );


namespace mmcs {
namespace lite {


Np::Np(
        const JobInfo& info,
        const server::BlockPtr& block
      ) :
    _info( info ),
    _block( block ),
    _container(),
    _world(),
    _rectangle(),
    _primaryDimension( 0 ),
    _includeCount( 0 ),
    _mapping(),
    _size( 0 )
{
    this->defaultMapping();

    this->createWorldRectangle();

    this->createRectangle();
}

void
Np::defaultMapping()
{
    // default EDCBA mapping for now
    char buf[6];
    char* s = buf;
    for ( unsigned x = 1; x <= CR_NUM_DIMS; ++x ) {
        _mapping[ x - 1 ] = CR_NUM_DIMS - x;
        *s++ = CR_DIM_NAMES[ CR_NUM_DIMS - x ];
    }
    buf[5] = '\0';
    LOG_TRACE_MSG( "mapping: " << buf );
}

void
Np::createWorldRectangle()
{
    *CR_RECT_LL(&_world) = (CR_COORD_T) {{0,0,0,0,0}};
    if ( boost::dynamic_pointer_cast<server::CNBlockController>(_block) ) {
        *CR_RECT_UR(&_world) = (CR_COORD_T) {{
            _block->getBlockNodeConfig()->aNodeSize() - 1,
            _block->getBlockNodeConfig()->bNodeSize() - 1,
            _block->getBlockNodeConfig()->cNodeSize() - 1,
            _block->getBlockNodeConfig()->dNodeSize() - 1,
            _block->getBlockNodeConfig()->eNodeSize() - 1
        }};
    } else if ( boost::dynamic_pointer_cast<server::IOBlockController>(_block) ) {
        *CR_RECT_UR(&_world) = (CR_COORD_T) {{
            _block->getBlockNodeConfig()->aIONodeSize() - 1,
            _block->getBlockNodeConfig()->bIONodeSize() - 1,
            _block->getBlockNodeConfig()->cIONodeSize() - 1,
            _block->getBlockNodeConfig()->dIONodeSize() - 1,
            1 // I/O blocks have no E dimension
        }};
    }
    LOG_TRACE_MSG(
            "(" <<
            _world.ll.coords[0] << "," <<
            _world.ll.coords[1] << "," <<
            _world.ll.coords[2] << "," <<
            _world.ll.coords[3] << "," <<
            _world.ll.coords[4] <<
            "):(" <<
            _world.ur.coords[0] << "," <<
            _world.ur.coords[1] << "," <<
            _world.ur.coords[2] << "," <<
            _world.ur.coords[3] << "," <<
            _world.ur.coords[4] <<
            ")"
            );
}

void
Np::createRectangle()
{
    // calculate size of world
    int world_size = 1;
    for ( unsigned dimension = 0; dimension < CR_NUM_DIMS; ++dimension) {
        world_size *= (CR_COORD_DIM(CR_RECT_UR(&_world),dimension) - CR_COORD_DIM(CR_RECT_LL(&_world),dimension) + 1);
    }
    LOG_TRACE_MSG( "world size     : " << world_size << " node" << (world_size == 1 ? "" : "s") );

    // validate --np fits into world size
    if ( _info.getNp() > world_size * _info.getRanksPerNode() ) {
        BOOST_THROW_EXCEPTION(
                std::logic_error(
                    "--np value of " + boost::lexical_cast<std::string>( _info.getNp() ) +
                    " with --ranks-per-node value of " + boost::lexical_cast<std::string>( _info.getRanksPerNode() ) +
                    " is greater than block size of " + boost::lexical_cast<std::string>( world_size ) +
                    " nodes."
                    )
                );
    }

    const unsigned ranksPerNode = _info.getRanksPerNode();
    LOG_TRACE_MSG( "ranks per node : " << ranksPerNode );

    const unsigned np_ranks = _info.getNp() ? _info.getNp() : world_size * ranksPerNode;
    LOG_TRACE_MSG( "np             : " << np_ranks << " rank" << (np_ranks == 1 ? "" : "s") );
    const unsigned excludeCount = (world_size * ranksPerNode - np_ranks) / ranksPerNode;
    LOG_TRACE_MSG( "exclude        : " << excludeCount << " node" << (excludeCount == 1 ? "" : "s") );
    _includeCount = world_size - excludeCount;
    LOG_TRACE_MSG( "np             : " << _includeCount << " node" << (_includeCount == 1 ? "" : "s") );

    // the container needs enough storage to hold up to the number of excluded nodes
    _container.reset( new CR_COORD_T[ excludeCount ] );
    memset( _container.get(), 0, sizeof(CR_COORD_T) * excludeCount );

    MUSPI_MakeNpRectMap(
            &_world,
            _includeCount,
            _mapping,
            &_rectangle,
            _container.get(),
            &_size,
            &_primaryDimension
            );

    LOG_TRACE_MSG( _size << " nodes excluded from circumscribing rectangle" );
}

} } // namespace mmcs::lite
