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
#include "server/job/class_route/Generate.h"

#include "server/job/class_route/Mapping.h"
#include "server/job/class_route/Np.h"
#include "server/job/class_route/Parameters.h"
#include "server/job/class_route/Rectangle.h"

#include "server/block/Compute.h"
#include "server/block/ComputeNode.h"
#include "server/block/IoNode.h"
#include "server/block/Midplane.h"

#include "server/job/ClassRoute.h"

#include "server/Job.h"

#include "common/JobInfo.h"
#include "common/logging.h"
#include "common/SubBlock.h"

#include <control/include/bgqconfig/BGQMidplaneNodeConfig.h>
#include <control/include/bgqconfig/BGQNodePos.h>
#include <control/include/bgqconfig/BGQTopology.h>

#include <ramdisk/include/services/JobctlMessages.h>

#include <spi/include/mu/Classroute_inlines.h>

#include <boost/foreach.hpp>
#include <boost/throw_exception.hpp>

#include <vector>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job  {
namespace class_route {

Generate::Generate(
        const Job::Ptr& job,
        const std::string& archiveMapFile,
        bool isPermutationMappingType,
        bool retainMappingFiles
        ) :
    _container(),
    _jobLeader(),
    _job( job ),
    _np(),
    _world(),
    _mapping( new Mapping ),
    _archiveMapFile(archiveMapFile),
    _isPermutationMappingType(isPermutationMappingType),
    _retainMappingFiles(retainMappingFiles)
{
    const JobInfo& info = job->info();
    const SubBlock& subBlock = info.getSubBlock();

    this->calculateJobShape();

    this->pickJobLeader();

    if ( subBlock.isValid() ) {
        this->subBlock();
    } else {
        this->fullBlock();
    }
}

Generate::~Generate()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

void
Generate::calculateJobShape()
{
    const JobInfo& info = _job->info();
    const SubBlock& subBlock = info.getSubBlock();

    CR_RECT_T world;
    *CR_RECT_LL(&world) = (CR_COORD_T) {{0,0,0,0,0}};

    if ( subBlock.isValid() ) {
        const Shape& shape = subBlock.shape();
        *CR_RECT_UR(&world) = (CR_COORD_T) {{ shape.a() - 1, shape.b() - 1, shape.c() - 1, shape.d() - 1, shape.e() - 1 }};
    } else {
        const block::Compute::Ptr block = _job->block();
        *CR_RECT_UR(&world) = (CR_COORD_T) {{ block->a() - 1, block->b() - 1, block->c() - 1, block->d() - 1, block->e() - 1 }};
    }

    _world.reset( new Rectangle(&world) );

    _np.reset(
           new Np( info, _world.get(), _mapping.get(), _archiveMapFile, _isPermutationMappingType, _retainMappingFiles )
           );
}

void
Generate::pickJobLeader()
{
    // Find a root node for comm-world using the rectangle calculated previously
    CR_COORD_T leader;
    int primaryDimension;
    CR_RECT_T& rectangle = _np->rectangle().impl();
    MUSPI_PickWorldRoot( &rectangle, _mapping->impl(), &leader, &primaryDimension );

    _jobLeader.reset( new Coordinates(&leader) );
}

void
Generate::subBlock()
{
    const JobInfo& info = _job->info();
    const SubBlock& subBlock = info.getSubBlock();
    const Corner& corner = subBlock.corner();
    LOG_TRACE_MSG( "corner " << corner.getBlockCoordinates() );

    // find corner's midplane
    const block::Compute::Ptr& block = _job->block();
    const block::Compute::Midplanes& midplanes = block->midplanes();
    const block::Compute::Midplanes::const_iterator i = midplanes.find( subBlock.corner().getMidplane() );
    if ( i == midplanes.end() ) {
        BOOST_THROW_EXCEPTION(
                std::runtime_error(
                    "could not find midplane '" + subBlock.corner().getMidplane() + "' in block configuration."
                    )
                );
    }
    const block::Midplane& midplane = i->second;

    for ( unsigned a = 0; a < _world->size( Dimension::A );  ++a ) {
        for ( unsigned b = 0; b < _world->size( Dimension::B );  ++b ) {
            for ( unsigned c = 0; c < _world->size( Dimension::C );  ++c ) {
                for ( unsigned d = 0; d < _world->size( Dimension::D );  ++d ) {
                    for ( unsigned e = 0; e < _world->size( Dimension::E );  ++e ) {
                        // get compute node location in midplane based on offset from corner
                        const runjob::Coordinates offset( 
                                a + corner.getMidplaneCoordinates().a(),
                                b + corner.getMidplaneCoordinates().b(),
                                c + corner.getMidplaneCoordinates().c(),
                                d + corner.getMidplaneCoordinates().d(),
                                e + corner.getMidplaneCoordinates().e()
                                );
                        const block::ComputeNode::Ptr& node = midplane.node( offset );

                        // node's coordinates in the job are relative to the sub-block
                        const Coordinates coordinates( a, b, c, d, e );

                        this->set( node, coordinates );
                    }
                }
            }
        }
    }
}

void
Generate::fullBlock()
{
    const block::Compute::Ptr block = _job->block();
    const block::Compute::Midplanes& midplanes = block->midplanes();
    for ( auto i = midplanes.begin(); i != midplanes.end(); ++i ) {
        this->midplane( i->second );
    }
}

void
Generate::midplane(
        const block::Midplane& midplane
        )
{
    LOGGING_DECLARE_LOCATION_MDC( midplane.location() );

    const block::Compute::Ptr& block = _job->block();
    BGQBlockNodeConfig::midplane_iterator config = block->config()->midplaneBegin();
    do {
        if ( config->posInMachine() == boost::lexical_cast<std::string>(midplane.location()) ) break;
    } while ( ++config != block->config()->midplaneEnd() );
    if ( config == block->config()->midplaneEnd() ) {
        BOOST_THROW_EXCEPTION(
                std::runtime_error(
                    "could not find midplane '" + 
                    boost::lexical_cast<std::string>( midplane.location() ) +
                    "' in block configuration."
                    )
                );
    }

    // get shape of midplane
    const unsigned sizeA = config->fullMidplaneUsed() ? BGQTopology::MAX_A_NODE : config->partialAsize();
    const unsigned sizeB = config->fullMidplaneUsed() ? BGQTopology::MAX_B_NODE : config->partialBsize();
    const unsigned sizeC = config->fullMidplaneUsed() ? BGQTopology::MAX_C_NODE : config->partialCsize();
    const unsigned sizeD = config->fullMidplaneUsed() ? BGQTopology::MAX_D_NODE : config->partialDsize();
    const unsigned sizeE = config->fullMidplaneUsed() ? BGQTopology::MAX_E_NODE : config->partialEsize();
    if ( midplane.small() ) {
        LOG_TRACE_MSG(
                "sub-midplane " << midplane.location() << " size " <<
                sizeA << "x" <<
                sizeB << "x" <<
                sizeC << "x" <<
                sizeD << "x" <<
                sizeE
                );
    } else {
        LOG_TRACE_MSG(
                "midplane coordinates (" <<
                config.a() << "," <<
                config.b() << "," <<
                config.c() << "," <<
                config.d() <<
                ")"
                );
    }

    for ( unsigned a = 0; a < sizeA; ++a ) {
        for ( unsigned b = 0; b < sizeB; ++b ) {
            for ( unsigned c = 0; c < sizeC; ++c ) {
                for ( unsigned d = 0; d < sizeD; ++d ) {
                    for ( unsigned e = 0; e < sizeE; ++e ) {
                        const runjob::Coordinates midplane_coordinates( a, b, c, d, e );
                        const block::ComputeNode::Ptr& node = midplane.node( midplane_coordinates );

                        // offset coordinates based on the midplane's position in the block
                        const Coordinates block_coordinates = Coordinates(
                                a + config.a() * BGQTopology::MAX_A_NODE,
                                b + config.b() * BGQTopology::MAX_B_NODE,
                                c + config.c() * BGQTopology::MAX_C_NODE,
                                d + config.d() * BGQTopology::MAX_D_NODE,
                                e 
                                );
                        this->set( node, block_coordinates );
                    }
                }
            }
        }
    }
}

void
Generate::set(
        const block::ComputeNode::Ptr& node,
        const Coordinates& coordinates
        )
{
    Parameters parameters = Parameters::create().
        // named parameter idiom
        world( _world.get() ).
        np( _np.get() ).
        coordinates( coordinates ).
        jobLeader( _jobLeader.get() ).
        mapping( _mapping.get() )
        ;

    // look for job leader to log its location
    if ( coordinates == *_jobLeader ) {
        LOG_DEBUG_MSG( "job leader " << *_jobLeader << " " << node->getLocation() );
        LOG_DEBUG_MSG( "comm world " << *_world );
    }

    const ClassRoute cr( parameters );

    _container.insert(
            Container::value_type(
                node->getConnectedIo()->getLocation(),
                cr
                )
            );
}

void
Generate::add(
        const Uci& node,
        const boost::shared_ptr<bgcios::jobctl::SetupJobMessage>& message
        ) const
{
    LOGGING_DECLARE_LOCATION_MDC( node );
    typedef std::pair<Container::const_iterator, Container::const_iterator> Range;
    Range range = _container.equal_range( node );

    const size_t count = std::distance( range.first, range.second );
    if ( count >= bgcios::jobctl::MaxComputeNodes ) {
        LOG_WARN_MSG( 
                "I/O node " << node << " controls " << count << " compute nodes"
                );
        LOG_WARN_MSG(
                "which is more than maximum supported value of " << bgcios::jobctl::MaxComputeNodes
                );

        return;
    }

    // get corner
    const JobInfo& info = _job->info();
    const SubBlock& subBlock = info.getSubBlock();
    const runjob::Coordinates& corner = subBlock.corner().getBlockCoordinates();
    LOG_TRACE_MSG( "corner: " << corner );

    for ( Container::const_iterator i = range.first; i != range.second; ++i ) {
        const ClassRoute& cr = i->second;
        const size_t index = std::distance( range.first, i );

        // offset classroute coordinates with the corner in the block
        message->nodesInJob[ index ].coords.aCoord = boost::numeric_cast<uint8_t>( cr.coordinates().get(Dimension::A) + corner.a() );
        message->nodesInJob[ index ].coords.bCoord = boost::numeric_cast<uint8_t>( cr.coordinates().get(Dimension::B) + corner.b() );
        message->nodesInJob[ index ].coords.cCoord = boost::numeric_cast<uint8_t>( cr.coordinates().get(Dimension::C) + corner.c() );
        message->nodesInJob[ index ].coords.dCoord = boost::numeric_cast<uint8_t>( cr.coordinates().get(Dimension::D) + corner.d() );
        message->nodesInJob[ index ].coords.eCoord = boost::numeric_cast<uint8_t>( cr.coordinates().get(Dimension::E) + corner.e() );
        message->nodesInJob[ index ].subBlockClassRoute = cr.allNodes();
        message->nodesInJob[ index ].npClassRoute = cr.npNodes();
        LOG_TRACE_MSG(
                "(" << 
                static_cast<unsigned>( message->nodesInJob[ index ].coords.aCoord ) << "," <<
                static_cast<unsigned>( message->nodesInJob[ index ].coords.bCoord ) << "," <<
                static_cast<unsigned>( message->nodesInJob[ index ].coords.cCoord ) << "," <<
                static_cast<unsigned>( message->nodesInJob[ index ].coords.dCoord ) << "," <<
                static_cast<unsigned>( message->nodesInJob[ index ].coords.eCoord ) <<
                ") " <<
                // left align class route data
                std::left <<
                ": " << cr
                );
    }
    message->numNodes = boost::numeric_cast<unsigned>( count );

    // add job leader, offset coordinates based on corner in the block
    message->jobLeader.aCoord = boost::numeric_cast<uint8_t>( _jobLeader->get(Dimension::A) + corner.a() );
    message->jobLeader.bCoord = boost::numeric_cast<uint8_t>( _jobLeader->get(Dimension::B) + corner.b() );
    message->jobLeader.cCoord = boost::numeric_cast<uint8_t>( _jobLeader->get(Dimension::C) + corner.c() );
    message->jobLeader.dCoord = boost::numeric_cast<uint8_t>( _jobLeader->get(Dimension::D) + corner.d() );
    message->jobLeader.eCoord = boost::numeric_cast<uint8_t>( _jobLeader->get(Dimension::E) + corner.e() );
}

} // class_route
} // job
} // server
} // runjob
