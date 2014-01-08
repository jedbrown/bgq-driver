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
#include "server/job/class_route/Np.h"

#include "server/job/class_route/Mapping.h"

#include "common/JobInfo.h"
#include "common/logging.h"
#include "common/SubBlock.h"

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job  {
namespace class_route {

Np::Np(
        const JobInfo& info,
        Rectangle* world,
        Mapping* mapping
        ) :
    _container(),
    _size( 0 ),
    _rectangle(),
    _primaryDimension( 0 ),
    _includeCount( 0 )
{
    const unsigned size = world->size();
    LOG_TRACE_MSG( "size           : " << size << " node" << (size == 1 ? "" : "s") );

    const unsigned ranksPerNode = info.getRanksPerNode();
    LOG_TRACE_MSG( "ranks per node : " << ranksPerNode );

    const unsigned np_ranks = info.getNp().get();
    LOG_TRACE_MSG( "np             : " << np_ranks << " rank" << (np_ranks == 1 ? "" : "s") );
    const unsigned excludeCount = (size * ranksPerNode - np_ranks) / ranksPerNode;
    LOG_TRACE_MSG( "exclude        : " << excludeCount << " node" << (excludeCount == 1 ? "" : "s") );
    _includeCount = size - excludeCount;
    LOG_TRACE_MSG( "np             : " << _includeCount << " node" << (_includeCount == 1 ? "" : "s") );

    // the container needs enough storage to hold up to the number of excluded nodes
    _container.reset( new CR_COORD_T[ excludeCount ] );
    memset( _container.get(), 0, sizeof(CR_COORD_T) * excludeCount );

    // generate a rectangle suitable for use with the --np value provided
    CR_RECT_T npworld;
    MUSPI_MakeNpRectMap(
            &world->impl(),
            _includeCount,
            mapping->impl(),
            &npworld,
            _container.get(),
            &_size,
            &_primaryDimension
            );

    LOG_TRACE_MSG( "primary dim    : " << CR_DIM_NAMES[_primaryDimension] );

    // remember the circumscribing rectangle
    _rectangle = Rectangle( &npworld );

    LOG_TRACE_MSG( _size << " nodes excluded from circumscribing rectangle" );
    LOG_DEBUG_MSG( _rectangle );

    // log coordinates of nodes outside the rectangle
    for ( int i = 0; i < _size; ++i ) {
        LOG_TRACE_MSG(
                "exclude " << i+1 << " of " << _size << 
                " (" <<
                _container[i].coords[0] << "," <<
                _container[i].coords[1] << "," <<
                _container[i].coords[2] << "," <<
                _container[i].coords[3] << "," <<
                _container[i].coords[4] <<
                ")"
                );
    }
}

} // class_route
} // job
} // server
} // runjob

