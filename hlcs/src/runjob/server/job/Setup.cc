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
#include "server/job/Setup.h"

#include "server/block/Compute.h"

#include "server/cios/Message.h"

#include "server/job/SubNodePacing.h"
#include "server/job/ValidateMappingFile.h"

#include "server/Job.h"

#include "common/Exception.h"
#include "common/logging.h"
#include "common/Mapping.h"
#include "common/SubBlock.h"

#include <bgq_util/include/Location.h>

#include <boost/numeric/conversion/cast.hpp>

#include <cstring>
#include <fstream>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

void
Setup::create(
        const Job::Ptr& job
        )
{
    job->status().set( Status::Setup, job );

    try {
        const Ptr result(
                new Setup( job )
                );
    } catch ( const std::exception& e ) {
        job->setError(
                e.what(),
                error_code::job_failed_to_start
                );
        job->remove();
    }
}

Setup::Setup(
        const Job::Ptr& job
        ) :
    _job( job ),
    _classRoute( job )
{
    this->validateMapping();
    
    // each I/O node gets a separate setup message due to the class route information
    BOOST_FOREACH( IoNode::Map::value_type& i, _job->io() ) {
        const auto location = i.first;
        IoNode& node = i.second;
        this->populate( location, node );
    }
}

void
Setup::validateMapping() const
{
    // short circuit if we are not validating a mapping file
    const Mapping& mapping = _job->info().getMapping();
    if ( mapping.type() != Mapping::Type::File ) return;

    LOG_DEBUG_MSG( mapping.value() );

    std::ifstream file( mapping.value() );
    if ( !file ) {
        boost::system::system_error error(
                boost::system::error_code(
                    errno,
                    boost::system::get_system_category()
                    )
            );

        LOG_RUNJOB_EXCEPTION( 
                error_code::mapping_file_invalid,
                "Could not open mapping file: " << mapping.value() << ". " << error.what()
                );
    }

    BG_JobCoords_t jobSize;
    jobSize.corner.a = 0;
    jobSize.corner.b = 0;
    jobSize.corner.c = 0;
    jobSize.corner.d = 0;
    jobSize.corner.e = 0;
    if ( _job->info().getSubBlock().isValid() ) {
        const SubBlock& subBlock = _job->info().getSubBlock();
        jobSize.shape.a = boost::numeric_cast<uint8_t>(subBlock.shape().a());
        jobSize.shape.b = boost::numeric_cast<uint8_t>(subBlock.shape().b());
        jobSize.shape.c = boost::numeric_cast<uint8_t>(subBlock.shape().c());
        jobSize.shape.d = boost::numeric_cast<uint8_t>(subBlock.shape().d());
        jobSize.shape.e = boost::numeric_cast<uint8_t>(subBlock.shape().e());
    } else {
        jobSize.shape.a = boost::numeric_cast<uint8_t>(_job->block()->a());
        jobSize.shape.b = boost::numeric_cast<uint8_t>(_job->block()->b());
        jobSize.shape.c = boost::numeric_cast<uint8_t>(_job->block()->c());
        jobSize.shape.d = boost::numeric_cast<uint8_t>(_job->block()->d());
        jobSize.shape.e = boost::numeric_cast<uint8_t>(_job->block()->e());
    }

    ValidateMappingFile( _job->id(), _job->info(), file, jobSize );
}

void
Setup::populate(
        const Uci& location,
        IoNode& node
        )
{
    const cios::Message::Ptr message = cios::Message::create(
            bgcios::jobctl::SetupJob,
            _job->id()
            );
    const boost::shared_ptr<bgcios::jobctl::SetupJobMessage> setup( 
            message->as<bgcios::jobctl::SetupJobMessage>()
            );

    const JobInfo& info = _job->info();

    // add block ID and  ranks per node
    setup->blockId = _job->block()->id();
    setup->numRanksPerNode = boost::numeric_cast<uint16_t>( info.getRanksPerNode().getValue() );

    if ( info.getMapping().type() == Mapping::Type::Permutation ) {
        (void)strncpy( setup->mapping, info.getMapping().value().c_str(), sizeof(setup->mapping) );
    } else {
        (void)strncpy( setup->mapFilePath, info.getMapping().value().c_str(), sizeof(setup->mapFilePath) );
    }

    // add job corner
    const runjob::SubBlock& subBlock = info.getSubBlock();
    const runjob::Corner& corner = subBlock.corner();
    if ( corner.isValid() ) {
        const Coordinates& coords = corner.getBlockCoordinates();
        setup->corner.aCoord = boost::numeric_cast<uint8_t>( coords.a() );
        setup->corner.bCoord = boost::numeric_cast<uint8_t>( coords.b() );
        setup->corner.cCoord = boost::numeric_cast<uint8_t>( coords.c() );
        setup->corner.dCoord = boost::numeric_cast<uint8_t>( coords.d() );
        setup->corner.eCoord = boost::numeric_cast<uint8_t>( coords.e() );
    } else {
        setup->corner.aCoord = 0;
        setup->corner.bCoord = 0;
        setup->corner.cCoord = 0;
        setup->corner.dCoord = 0;
        setup->corner.eCoord = 0;
    }

    // add job core for sub-node jobs
    if ( corner.isValid() && corner.isCoreSet() ) {
        setup->corner.core = boost::numeric_cast<uint8_t>( corner.getCore() );
        setup->shape.core = 1;
    } else {
        // sub-block or full block jobs use all 16 cores
        setup->shape.core = bgq::util::Location::ComputeCardCoresOnBoard - 1; 
    }

    // add job shape
    const runjob::Shape& shape = subBlock.shape();
    if ( shape.isValid() ) {
        setup->shape.aCoord = boost::numeric_cast<uint8_t>( shape.a() );
        setup->shape.bCoord = boost::numeric_cast<uint8_t>( shape.b() );
        setup->shape.cCoord = boost::numeric_cast<uint8_t>( shape.c() );
        setup->shape.dCoord = boost::numeric_cast<uint8_t>( shape.d() );
        setup->shape.eCoord = boost::numeric_cast<uint8_t>( shape.e() );
    } else {
        setup->shape.aCoord = boost::numeric_cast<uint8_t>( _job->block()->a() );
        setup->shape.bCoord = boost::numeric_cast<uint8_t>( _job->block()->b() );
        setup->shape.cCoord = boost::numeric_cast<uint8_t>( _job->block()->c() );
        setup->shape.dCoord = boost::numeric_cast<uint8_t>( _job->block()->d() );
        setup->shape.eCoord = boost::numeric_cast<uint8_t>( _job->block()->e() );
    }

    // add np
    setup->numRanks = info.getNp().get();

    // get class route for this I/O node's compute nodes
    _classRoute.add( location, setup );

    if ( _job->pacing() ) {
        _job->pacing()->add( message, _job );
    } else {
        node.writeControl( message );
    }
}

} // job
} // server
} // runjob
