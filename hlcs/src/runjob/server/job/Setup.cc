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
#include "server/job/CopyMappingFile.h"
#include "server/job/SubNodePacing.h"
#include "server/job/ValidateMappingFile.h"
#include "server/Job.h"
#include "server/Options.h"
#include "server/Server.h"

#include "common/Exception.h"
#include "common/logging.h"
#include "common/Mapping.h"
#include "common/SubBlock.h"

#include <bgq_util/include/Location.h>
#include <db/include/api/tableapi/gensrc/DBTJob.h>
#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/cxxdb/cxxdb.h>
#include <utility/include/ScopeGuard.h>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/bind.hpp>

#include <cstring>
#include <fstream>

#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>

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
    _classRoute(),
    _copiedMapping(),
    _retainMappingFiles(true)
{

    const JobInfo& info = _job->info();
    bool isPermutationMappingType = false;

    if ( info.getMapping().type() == Mapping::Type::Permutation ) {
        isPermutationMappingType = true;
    }

    const Server::Ptr server( _job->_server.lock() );
    if ( !server ) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Unexpected internal error retrieving properties settings."));
    }

    const std::string keyName("retain_mapping_files");
    const std::string propertiesSection("runjob.server");
    const bgq::utility::Properties::ConstPtr& properties = server->getOptions().getProperties();
    try {
        const std::string value(properties->getValue(propertiesSection, keyName));
        std::istringstream is(value);
        is >> std::boolalpha >> _retainMappingFiles;
        if (!is) {
             LOG_WARN_MSG( "Invalid " << keyName << " value from properties file, using default value of 'true'" );
             _retainMappingFiles = defaults::ServerRetainMappingFiles;
         } else {
             LOG_DEBUG_MSG( "Using " << keyName <<  " properties value: " << std::boolalpha << _retainMappingFiles );
         }
    } catch ( const std::exception& e ) {
        // Key not found, retain the mapping file by default
        _retainMappingFiles = defaults::ServerRetainMappingFiles;
        LOG_WARN_MSG( "Missing " << keyName << " key from " << propertiesSection << " section in properties file, using default value of 'true'" );
    }

    this->validateMapping();
    LOG_INFO_MSG( "Successfully validated mapping for job " <<  _job->id() );

    _classRoute.reset(
            new class_route::Generate( job, _copiedMapping, isPermutationMappingType, _retainMappingFiles )
           );

    // each I/O node gets a separate setup message due to the class route information
    BOOST_FOREACH( IoNode::Map::value_type& i, _job->io() ) {
        const auto location = i.first;
        IoNode& node = i.second;
        this->populate( location, node );
    }
}

void
Setup::validateMapping()
{
    // short circuit if we are not validating a mapping file
    const Mapping& mapping = _job->info().getMapping();
    if ( mapping.type() != Mapping::Type::File ) return;

    LOG_DEBUG_MSG( mapping.value() );

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

    ValidateMappingFile( _job->id(), _job->info(), jobSize );

    const Server::Ptr server( _job->_server.lock() );
    if ( !server ) return;

    // If a mapping file is specified for the job it will always be copied temporarily into the mapping archive directory.
    // If the properties setting retain_mapping_files = false then the database should not be updated with the archive mapping name.
    _copiedMapping = CopyMappingFile( _job->id(), _job->info(), server->getOptions().getProperties() ).result();
    if ( !_copiedMapping.empty() ) {
        if ( _retainMappingFiles ) {
            LOG_INFO_MSG( "Job " <<  _job->id() << " mapping file copied to " << _copiedMapping );
            this->updateDatabaseMapping( _copiedMapping );
        } else {
            LOG_DEBUG_MSG( "Job " <<  _job->id() << " mapping file was temporarily written to " << _copiedMapping << ". Will be deleted later." );
        }
    }
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
        setup->mapFilePath[0] = 0;
    } else {
        (void)strncpy( setup->mapFilePath, info.getMapping().value().c_str(), sizeof(setup->mapFilePath) );
        setup->mapping[0] = 0;
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
    _classRoute->add( location, setup );

    if ( _job->pacing() ) {
        _job->pacing()->add( message, _job );
    } else {
        node.writeControl( message );
    }
}

void
Setup::updateDatabaseMapping(
        const std::string& mapping
        ) const
{
    bgq::utility::ScopeGuard removeGuard(
            boost::bind( &unlink, mapping.c_str() )
            );

    const cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    if ( !connection ) {
        LOG_RUNJOB_EXCEPTION(
                error_code::mapping_file_invalid,
                "Could not get database connection"
                );
    }

    const cxxdb::UpdateStatementPtr update = connection->prepareUpdate(
            "UPDATE BGQJob SET mapping=? WHERE id=?",
            { BGQDB::DBTJob::MAPPING_COL, BGQDB::DBTJob::ID_COL }
            );
    update->parameters()[ "id" ].cast( _job->id() );
    bool truncated;
    update->parameters()[ "mapping" ].set( mapping, &truncated );
    if ( truncated ) {
        const BGQDB::DBTJob j;
        LOG_RUNJOB_EXCEPTION(
                error_code::mapping_file_invalid,
                "Copied mapping file path of " << mapping << " is too long to fit in " << sizeof(j._mapping) << " characters"
                );
    }

    update->execute();
    LOG_DEBUG_MSG( "Updated mapping file path to " << mapping );
    removeGuard.dismiss();

    this->updateModificationTime( mapping, connection );
}

void
Setup::updateModificationTime(
        const std::string& mapping,
        const cxxdb::ConnectionPtr& connection
        ) const
{
    const cxxdb::QueryStatementPtr query = connection->prepareQuery(
            "SELECT " + BGQDB::DBTJob::STARTTIME_COL + " FROM BGQJob WHERE id=?",
            { BGQDB::DBTJob::ID_COL }
            );
    query->parameters()[ "id" ].cast( _job->id() );
    const cxxdb::ResultSetPtr results( query->execute() );
    if ( !results->fetch() ) {
        LOG_WARN_MSG( "could not find job " << _job->id() );
        return;
    }

    // get job's start time and calculate seconds since epoch
    const boost::posix_time::ptime startTime( results->columns()[ BGQDB::DBTJob::STARTTIME_COL ].getTimestamp() );
    LOG_DEBUG_MSG( "start time: " << startTime );
    const boost::posix_time::ptime epoch( boost::gregorian::date(1970,1,1) );
    const boost::posix_time::time_duration duration( startTime - epoch );
    LOG_DEBUG_MSG( "since epoch: " << duration.total_seconds() );

    // get the file's stat buffer so we can retain the access time
    struct stat statBuf;
    if ( stat(mapping.c_str(), &statBuf) ) {
        char buf[256];
        LOG_WARN_MSG( "Could not stat " << mapping << ": " << strerror_r(errno, buf, sizeof(buf)) );
        return;
    }
    LOG_DEBUG_MSG( "file's modification time: " << statBuf.st_mtime );

    // change the file's modification time to be the same as the job's start time
    struct utimbuf newTime;
    newTime.actime = statBuf.st_atime;
    newTime.modtime = duration.total_seconds();
    if ( utime(mapping.c_str(), &newTime) ) {
        char buf[256];
        LOG_WARN_MSG( "Could not update modification time of " << mapping << ": " << strerror_r(errno, buf, sizeof(buf)) );
    }
}

} // job
} // server
} // runjob
