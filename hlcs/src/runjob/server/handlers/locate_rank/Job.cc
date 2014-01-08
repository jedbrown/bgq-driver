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

#include "server/handlers/locate_rank/Job.h"

#include "server/handlers/locate_rank/ActiveJob.h"
#include "server/handlers/locate_rank/Block.h"
#include "server/handlers/locate_rank/Exception.h"
#include "server/handlers/locate_rank/HistoryJob.h"

#include "server/job/RankMapping.h"

#include "server/Server.h"

#include "common/error.h"
#include "common/Exception.h"
#include "common/logging.h"
#include "common/Mapping.h"

#include <spi/include/mu/RankMap.h>

#include <boost/make_shared.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace handlers {
namespace locate_rank {

Job::Ptr
Job::create(
        const cxxdb::ConnectionPtr& db,
        BGQDB::job::Id job
        )
{
    Ptr result;
    try {
        result = boost::make_shared<ActiveJob>( db, job );
    } catch ( const Exception& e ) {
        if ( e.error() == runjob::commands::error::job_not_found ) {
            result = boost::make_shared<HistoryJob>( db, job );
        } else {
            throw;
        }
    } 

    result->_block = Block::create( db, result );

    return result;
}

Uci
Job::find(
        uint32_t rank
        )
{
    if ( rank >= _np ) {
        BOOST_THROW_EXCEPTION(
                Exception(
                    "rank " +
                    boost::lexical_cast<std::string>( rank ) + " " +
                    boost::lexical_cast<std::string>(
                        ( dynamic_cast<HistoryJob*>(this) ? "did" : "does" )  // use correct verb tense for history jobs
                        ) +
                    " not participate in job " + 
                    boost::lexical_cast<std::string>( _id ),
                    runjob::commands::error::rank_invalid
                    )
                );
    }

    return this->findCoordinates( rank );
}

int
Job::blockCreationId() const
{
    return _block->creationId(); 
}

Job::Job(
        BGQDB::job::Id id
        ) :
    _block(),
    _id( id ),
    _shape(),
    _blockId(),
    _mapping(),
    _corner(),
    _np( 0 )
{

}

Job::~Job()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

Uci
Job::findCoordinates(
        uint32_t rank
        )
{
    LOG_TRACE_MSG( "finding coordinates for rank " << rank );

    BG_JobCoords_t coordinates;
    coordinates.shape.a = boost::numeric_cast<uint8_t>( _shape[0] );
    coordinates.shape.b = boost::numeric_cast<uint8_t>( _shape[1] );
    coordinates.shape.c = boost::numeric_cast<uint8_t>( _shape[2] );
    coordinates.shape.d = boost::numeric_cast<uint8_t>( _shape[3] );
    coordinates.shape.e = boost::numeric_cast<uint8_t>( _shape[4] );

    unsigned buf[2048];

    boost::scoped_array<BG_CoordinateMapping_t> output(
            new BG_CoordinateMapping_t[_np]
            );

    const int rc = MUSPI_GenerateCoordinates(
            _mapping.value().c_str(),
            &coordinates,
            NULL, // rank's coordinates
            _shape[5],
            _np,
            sizeof(buf),
            buf,
            output.get(),
            NULL, // rank
            NULL  // mpmd Found
            );

    if ( rc ) {
        BOOST_THROW_EXCEPTION(
                std::runtime_error(
                    "could not generate mapping: rc=" +
                    boost::lexical_cast<std::string>(rc)
                    )
                );
    }

    LOG_TRACE_MSG(
            "(" <<
            output[rank].a << "," <<
            output[rank].b << "," <<
            output[rank].c << "," <<
            output[rank].d << "," <<
            output[rank].e << "," <<
            output[rank].t << ")"
            );

    return _block->findNodeLocation( output[rank] );
}

} // locate_rank
} // handlers
} // server
} // runjob
