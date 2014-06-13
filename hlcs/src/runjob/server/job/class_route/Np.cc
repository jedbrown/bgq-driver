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

#include "server/job/class_route/Coordinates.h"
#include "server/job/class_route/Mapping.h"

#include "server/Job.h"

#include "common/defaults.h"
#include "common/Environment.h"
#include "common/JobInfo.h"
#include "common/logging.h"
#include "common/SubBlock.h"

#include <fcntl.h> // need open() in RankMap.h
#include <spi/include/mu/RankMap.h>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/scoped_array.hpp>
#include <boost/foreach.hpp>

#include <set>
#include <string.h>
#include <unistd.h>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job  {
namespace class_route {

Np::Np(
        const JobInfo& info,
        Rectangle* world,
        Mapping* mapping,
        const std::string& mapArchiveFile,
        bool permutationMappingType,
        bool retainMappingFiles
        ) :
    _container(),
    _size( 0 ),
    _rectangle(),
    _primaryDimension( 0 ),
    _includeCount( 0 ),
    _mapArchiveFile(mapArchiveFile),
    _permutationMappingType(permutationMappingType),
    _retainMappingFiles(retainMappingFiles)
{
    const unsigned size = world->size();
    LOG_TRACE_MSG( "size           : " << size << " node" << (size == 1 ? "" : "s") );

    const unsigned ranksPerNode = info.getRanksPerNode();
    LOG_TRACE_MSG( "ranks per node : " << ranksPerNode );

    const unsigned np_ranks = info.getNp().get();
    LOG_TRACE_MSG( "np             : " << np_ranks << " rank" << (np_ranks == 1 ? "" : "s") );

    _includeCount = this->countIncludedNodes(info, world);
    const unsigned excludeCount = size - _includeCount;

    LOG_TRACE_MSG( "exclude        : " << excludeCount << " node" << (excludeCount == 1 ? "" : "s") );
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

unsigned
Np::countIncludedNodes(
        const JobInfo& info,
        const Rectangle* world
        ) const
{
    const JobInfo::EnvironmentVector& envs = info.getEnvs();
    bool disabled = false;
    BOOST_FOREACH( const Environment& i, envs ) {
        disabled = ( i.getKey() == "RUNJOB_USE_OLD_NP_INCLUDE_COUNT" );
        if ( disabled ) break;
    }

    if ( disabled ) {
        // this count will be incorrect for certain mappings, see issue 7342. That's why this is
        // disabled by default.
        LOG_DEBUG_MSG( "using old np include count" );
        const unsigned excludeCount = (world->size() * info.getRanksPerNode() - info.getNp()) / info.getRanksPerNode();
        return world->size() - excludeCount;
    }

    // need to calculate how many nodes will be included in the np rectangle
    // this requires calculating the coordinates for ranks 0 through np, and
    // counting the number of unique a,b,c,d,e coordinates in the resulting set
    std::set<Coordinates> result;

    BG_JobCoords_t coordinates;
    coordinates.shape.a = boost::numeric_cast<uint8_t>( world->size(Dimension::A) );
    coordinates.shape.b = boost::numeric_cast<uint8_t>( world->size(Dimension::B) );
    coordinates.shape.c = boost::numeric_cast<uint8_t>( world->size(Dimension::C) );
    coordinates.shape.d = boost::numeric_cast<uint8_t>( world->size(Dimension::D) );
    coordinates.shape.e = boost::numeric_cast<uint8_t>( world->size(Dimension::E) );

    unsigned buf[2048];
    memset(buf,0,sizeof(buf));

    const unsigned np = info.getNp().get();
    boost::scoped_array<BG_CoordinateMapping_t> output(
            new BG_CoordinateMapping_t[np]
            );

    std::string map = info.getMapping();
    if (map.empty()) {
        LOG_INFO_MSG("No mapping permutation or file specified.");
    } else {
        if ( _permutationMappingType ) {
            LOG_INFO_MSG("Mapping permutation specified is " << map);
        } else {
            map = _mapArchiveFile;
            LOG_INFO_MSG("Mapping file specified is " << map);
        }
    }

    const int rc = MUSPI_GenerateCoordinates(
            map.c_str(),
            &coordinates,
            NULL, // rank's coordinates
            info.getRanksPerNode(),
            np,
            sizeof(buf),
            buf,
            output.get(),
            NULL, // rank
            NULL  // mpmd Found
            );

    // Check if mapping file archive should be deleted
    if ( (_permutationMappingType == false) && (_retainMappingFiles == false) ) {
        int rc2  = unlink(map.c_str());
        if ( rc2 == 0 ) {
            LOG_DEBUG_MSG("Deleted temporary mapping file " << map);
        } else {
            LOG_WARN_MSG("Failed to delete temporary mapping file " << map);
        }
    }

    if ( rc ) {
        BOOST_THROW_EXCEPTION(
                std::runtime_error(
                    "Could not generate mapping: rc=" +
                    boost::lexical_cast<std::string>(rc)
                    )
                );
    }

    for ( unsigned i = 0; i < np; ++i ) {
        LOG_TRACE_MSG(
                "(" <<
                output[i].a << "," <<
                output[i].b << "," <<
                output[i].c << "," <<
                output[i].d << "," <<
                output[i].e << "," <<
                output[i].t << ") " << i
                );

        result.insert(
                Coordinates(
                    output[i].a,
                    output[i].b,
                    output[i].c,
                    output[i].d,
                    output[i].e
                    )
                );
    }

    return static_cast<unsigned>(result.size());
}

} // class_route
} // job
} // server
} // runjob

