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
#include "server/job/RankMapping.h"

#include "server/job/CopyMappingFile.h"

#include "server/block/Compute.h"
#include "server/block/ComputeNode.h"
#include "server/block/IoNode.h"
#include "server/block/Midplane.h"

#include "server/Job.h"
#include "server/Options.h"
#include "server/Server.h"

#include "common/JobInfo.h"
#include "common/logging.h"
#include "common/Mapping.h"

#include <spi/include/mu/RankMap.h>

#include <ramdisk/include/services/JobctlMessages.h>

#include <utility/include/Properties.h>

#include <boost/filesystem.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <boost/throw_exception.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

RankMapping::RankMapping(
        const Job::Ptr& job,
        runjob::tool::Proctable& proctable
        ) :
    _proctable( proctable )
{
    bool deleteArchiveFile = false;
    std::string archiveFilename;
    // Start with original mapping from runjob command as default (will be permutation like ABCDET or file name)
    std::string map = job->info().getMapping();

    // No need to find/copy mapping file if permutation
    if (job->info().getMapping().type() == Mapping::Type::Permutation) {
        LOG_INFO_MSG("Rank mapping permutation specified is " << map);
    } else { // Using mapping file
        // Need to get properties settings to find the archive directory
        const Server::Ptr server( job->_server.lock() );
        if ( !server ) {
            BOOST_THROW_EXCEPTION(std::runtime_error("Unexpected internal error retrieving properties settings."));
        }

        const bgq::utility::Properties::ConstPtr& properties = server->getOptions().getProperties();

        try {
            archiveFilename = properties->getValue("runjob.server","mapping_file_archive");
        } catch ( const std::exception& e ) {
            // Key not found, don't copy the mapping file
            LOG_ERROR_MSG( e.what() );
            // Clear the string which will force usage of original mapping file
            archiveFilename.clear();
        }

        // If properties file is mis-configured just use original mapping file and not one from archive directory
        if (!archiveFilename.empty()) {
            // Append job id to archive directory
            archiveFilename += "/";
            archiveFilename += boost::lexical_cast<std::string>(job->id());
            // Check if file already exists in archive directory
            if ( boost::filesystem::exists(archiveFilename) ) {
                // Map file already exists so use it
                map = archiveFilename;
            } else { // Archive file doesn't exist, need to create it.
                try {
                    archiveFilename = CopyMappingFile( job->id(), job->info(), properties).result();
                } catch ( const std::exception& e ) {
                    // Error creating archive file, use original mapping file
                    LOG_ERROR_MSG( e.what() );
                    // Clear the string which will force usage of original mapping file
                    archiveFilename.clear();
                }

                if ( !archiveFilename.empty() ) {
                    map = archiveFilename;
                    // Copied mapping file to archive so delete it later
                    deleteArchiveFile = true;
                }
            }
        }

        LOG_INFO_MSG("Rank mapping file specified is " << map);
    }

    _proctable.clear();

    const BG_JobCoords_t job_shape = this->getJobShape( job );
    
    unsigned buf[2048];

    const unsigned np( job->info().getNp() );
    boost::scoped_array<BG_CoordinateMapping_t> output(
            new BG_CoordinateMapping_t[np]
            );

    const int rc = MUSPI_GenerateCoordinates(
            map.c_str(),
            &job_shape,
            NULL, // rank's coordinates
            job->info().getRanksPerNode(),
            np,
            sizeof(buf),
            buf,
            output.get(),
            NULL, // rank
            NULL  // mpmdFound
            );

    // Check if mapping file archive should be deleted
    if ( deleteArchiveFile ) {
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
                    "could not generate mapping: " +
                    boost::lexical_cast<std::string>( rc)
                    ) 
                );
    }

    for ( unsigned i = 0; i < np; ++i ) {
        this->findLocation( job, i, output[i] );
    }
}

RankMapping::~RankMapping()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}


BG_JobCoords_t
RankMapping::getJobShape(
        const Job::Ptr& job
        )
{
    BG_JobCoords_t result;
    memset( &result, '\0', sizeof ( result ) );

    const JobInfo& info = job->info();
    if ( info.getSubBlock().isValid() ) {
        const Shape& shape = info.getSubBlock().shape();
        result.shape.a = boost::numeric_cast<uint8_t>( shape.a() );
        result.shape.b = boost::numeric_cast<uint8_t>( shape.b() );
        result.shape.c = boost::numeric_cast<uint8_t>( shape.c() );
        result.shape.d = boost::numeric_cast<uint8_t>( shape.d() );
        result.shape.e = boost::numeric_cast<uint8_t>( shape.e() );

        const SubBlock& subBlock = info.getSubBlock();
        const Corner& corner = subBlock.corner();
        LOG_TRACE_MSG( 
                "sub-block corner within block " << corner.getBlockCoordinates()
                );
        LOG_TRACE_MSG( 
                "sub-block corner within midplane " << corner.getMidplaneCoordinates()
                );
    } else {
        const block::Compute::Ptr block = job->block();
        result.shape.a = boost::numeric_cast<uint8_t>( block->a() );
        result.shape.b = boost::numeric_cast<uint8_t>( block->b() );
        result.shape.c = boost::numeric_cast<uint8_t>( block->c() );
        result.shape.d = boost::numeric_cast<uint8_t>( block->d() );
        result.shape.e = boost::numeric_cast<uint8_t>( block->e() );
    }

    LOG_TRACE_MSG( 
            "job shape " << 
            static_cast<unsigned>( result.shape.a ) << "x" <<
            static_cast<unsigned>( result.shape.b ) << "x" <<
            static_cast<unsigned>( result.shape.c ) << "x" <<
            static_cast<unsigned>( result.shape.d ) << "x" <<
            static_cast<unsigned>( result.shape.e ) << "x" <<
            info.getRanksPerNode()
            );

    return result;
}

void
RankMapping::findLocation(
        const Job::Ptr& job,
        const unsigned rank,
        BG_CoordinateMapping_t& coordinates
        )
{
    const JobInfo& info = job->info();
    const SubBlock& subBlock = info.getSubBlock();
    const Corner& corner = subBlock.corner();

    // get node's midplane coordinates by dividing by the number of nodes per midplane
    const unsigned midplane_a = coordinates.a / BGQTopology::MAX_A_NODE;
    const unsigned midplane_b = coordinates.b / BGQTopology::MAX_B_NODE;
    const unsigned midplane_c = coordinates.c / BGQTopology::MAX_C_NODE;
    const unsigned midplane_d = coordinates.d / BGQTopology::MAX_D_NODE;
   
    // get midplane config using the coordinates
    const block::Compute::Ptr& block = job->block();
    const BGQMidplaneNodeConfig* result = block->config()->midplaneNodeConfig( midplane_a, midplane_b, midplane_c, midplane_d );
    BOOST_ASSERT( result );

    // find the midplane using its name
    const block::Compute::Midplanes& midplanes = block->midplanes();
    const block::Compute::Midplanes::const_iterator i = midplanes.find( result->posInMachine() );
    BOOST_ASSERT( i != midplanes.end() );
    const block::Midplane& midplane = i->second;

    // find the node within the midplane
    const block::ComputeNode::Ptr& node = midplane.node(
            Coordinates(
                (coordinates.a + corner.getMidplaneCoordinates().a()) % BGQTopology::MAX_A_NODE,
                (coordinates.b + corner.getMidplaneCoordinates().b()) % BGQTopology::MAX_B_NODE,
                (coordinates.c + corner.getMidplaneCoordinates().c()) % BGQTopology::MAX_C_NODE,
                (coordinates.d + corner.getMidplaneCoordinates().d()) % BGQTopology::MAX_D_NODE,
                coordinates.e + corner.getMidplaneCoordinates().e()
                )
            );
    BOOST_ASSERT( node );

    // calculate pid using this rank's node coordinates
    const unsigned pid = bgcios::jobctl::coordsToNodeId(
            corner.getBlockCoordinates().a() + coordinates.a,
            corner.getBlockCoordinates().b() + coordinates.b,
            corner.getBlockCoordinates().c() + coordinates.c,
            corner.getBlockCoordinates().d() + coordinates.d,
            corner.getBlockCoordinates().e() + coordinates.e
            );

    LOG_TRACE_MSG( 
            std::setw(5) << rank << ": " << std::setw(1) <<
            node->getLocation() << " <-> " <<
            node->getConnectedIo()->getLocation() <<
            " (" <<
            coordinates.a << "," <<
            coordinates.b << "," <<
            coordinates.c << "," <<
            coordinates.d << "," <<
            coordinates.e << "," <<
            coordinates.t <<
            ") " <<
            "pid 0x" << std::hex << std::setw(8) << std::setfill('0') << pid
            );

    _proctable.push_back(
            runjob::tool::Rank( 
                node->getConnectedIo()->getLocation(),
                rank,
                pid
                )
            );
}

} // job
} // server
} // runjob

