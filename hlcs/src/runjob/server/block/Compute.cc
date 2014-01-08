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
#include "server/block/Compute.h"

#include "common/logging.h"

#include "server/block/IoLink.h"
#include "server/block/IoNode.h"
#include "server/block/Midplane.h"

#include "server/database/Init.h"
#include "server/database/NodeStatus.h"

#include "server/job/Container.h"
#include "server/job/Create.h"

#include "server/performance/Counters.h"

#include "server/Job.h"
#include "server/Server.h"

#include <bgq_util/include/Location.h>

#include <control/include/bgqconfig/BGQBlockNodeConfig.h>
#include <control/include/bgqconfig/BGQMidplaneNodeConfig.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace block {

Compute::Ptr
Compute::create(
        const std::string& name,
        const boost::shared_ptr<BGQBlockNodeConfig>& block,
        const Server::Ptr& server,
        const Io::Links& links
        )
{
    const Compute::Ptr result(
            new Compute(
                name,
                block,
                server,
                links
                )
            );

    return result;
}

Compute::Compute(
        const std::string& name,
        const boost::shared_ptr<BGQBlockNodeConfig>& block,
        const Server::Ptr& server,
        const Io::Links& links
        ) :
    Block( name, block, server ),
    _strand( server->getIoService() ),
    _midplanes(),
    _size( block->aNodeSize() * block->bNodeSize() * block->cNodeSize() * block->dNodeSize() * block->eNodeSize() ),
    _io()
{
    BOOST_ASSERT( !links.empty() );
    LOG_TRACE_MSG( "creating" );

    // iterate through midplanes
    for (
            BGQBlockNodeConfig::midplane_iterator i = block->midplaneBegin();
            i != block->midplaneEnd();
            ++i
        )
    {
        // get midplane object from bgqconfig
        BGQMidplaneNodeConfig& midplane = *i;

        // create Midplane object
        const Midplane mp( 
                _name,
                midplane.posInMachine(),
                midplane,
                links
                );

        // and insert into container
        BOOST_ASSERT(
                _midplanes.insert(
                    Midplanes::value_type(
                        midplane.posInMachine(),
                        mp
                        )
                    ).second
                );
    }

    // collect I/O ratios
    BOOST_FOREACH( auto i, _midplanes ) {
        const Midplane& midplane = i.second;
        const IoRatio& io = midplane.io();
        BOOST_FOREACH( auto j, io ) {
            const Uci& location = j.first;
            const unsigned computes = j.second;
            const IoRatio::iterator iterator = _io.find( location );
            if ( iterator == _io.end() ) {
                _io.insert( IoRatio::value_type(location, computes) );
            } else {
                iterator->second += computes;
            }
        }
    }

    // log I/O ratios
    BOOST_FOREACH( auto i, _io ) {
        LOG_DEBUG_MSG( i.first << ": " << i.second << " compute nodes" );
    }
}

void
Compute::arbitrate(
        const job::Create::Ptr& job
        )
{
    _strand.post(
            boost::bind(
                &Compute::arbitrateImpl,
                shared_from_this(),
                job
                )
            );
}

void
Compute::unavailable(
        const Job::Ptr& job
        )
{
    _strand.post(
            boost::bind(
                &Compute::unavailableJobImpl,
                shared_from_this(),
                job
                )
            );
}

void
Compute::arbitrateImpl(
        const job::Create::Ptr& job
        )
{
    LOGGING_DECLARE_BLOCK_MDC( _name );
    LOGGING_DECLARE_JOB_MDC( job->_client_id );

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    // time how long this takes. Note the timer is dismissed until
    // arbitration has completed successfully
    const performance::Counters::JobContainer::Timer::Ptr timer(
            server->getPerformanceCounters()->getJobs()->create()
            ->function( "server" )
            ->subFunction( "arbitrate" )
            ->mode( bgq::utility::performance::Mode::Value::Basic )
            );
    timer->dismiss();
    job->arbitration( timer );

    if ( !this->validateStdinNp(job) ) return;
    if ( !this->validateStraceNp(job) ) return;

    const runjob::JobInfo& info = job->_job_info;
    const runjob::SubBlock& subBlock = info.getSubBlock();
    if ( subBlock.isValid() ) {
        this->arbitrateSubBlock( job );
    } else {
        this->arbitrateFullBlock( job );
    }
}

void
Compute::unavailableJobImpl(
        const Job::Ptr& job
        )
{
    LOGGING_DECLARE_JOB_MDC( job->id() );
    LOGGING_DECLARE_BLOCK_MDC( _name );
    if ( !job->info().getSubBlock().isValid() ) {
        LOG_WARN_MSG( _size << " nodes unavailable" );
    } else {
        // Midplane will log which nodes are unavailable
    }

    // iterate through each midplane
    BOOST_FOREACH( Midplanes::value_type& i, _midplanes ) {
        Midplane& midplane = i.second;
        midplane.unavailable( job );
    }
}

bool
Compute::validateStdinNp(
        const job::Create::Ptr& job
        )
{
    // validate stdin rank fits into np
    const JobInfo& info = job->_job_info;
    const size_t np = info.getNp();

    if ( !np ) return true;

    const size_t stdinRank = info.getStdinRank();
    if ( stdinRank >= np ) {
        job->error(
                "stdin rank of " + boost::lexical_cast<std::string>(stdinRank) +
                " does not fit into np size of " + boost::lexical_cast<std::string>(np),
                runjob::error_code::job_np_invalid
                );

        return false;
    }

    return true;
}

bool
Compute::validateStraceNp(
        const job::Create::Ptr& job
        )
{
    // validate strace rank fits into np
    const JobInfo& info = job->_job_info;

    const size_t np = info.getNp();
    if ( !np ) return true;

    const Strace& strace = info.getStrace();
    if ( strace.getScope() == Strace::None ) return true;

    if ( strace.getRank() >= np ) {
        job->error(
                "strace rank of " + boost::lexical_cast<std::string>(strace.getRank()) +
                " does not fit into np size of " + boost::lexical_cast<std::string>(np),
                runjob::error_code::job_np_invalid
                );

        return false;
    }

    return true;
}

void
Compute::arbitrateSubBlock(
        const job::Create::Ptr& job
        )
{
    const runjob::JobInfo& info = job->_job_info;
    const runjob::SubBlock& subBlock = info.getSubBlock();
    const runjob::Corner& corner = subBlock.corner();
    const std::string midplane = this->getMidplane( job, corner );
    if ( midplane.empty() ) return;
    
    const Midplanes::iterator i = _midplanes.find( midplane );
    if ( i == _midplanes.end() ) {
        job->error(
                "could not find midplane " + midplane + " in block " + _name,
                error_code::corner_invalid
                );

        return;
    }

    Midplane& mp = i->second;
    if ( !mp.arbitrate(subBlock, job) ) return;

    // update timer with sub-block size
    const runjob::Shape& shape = subBlock.shape();
    job->_arbitration_timer->otherData(
            boost::lexical_cast<std::string>( shape.size() ) + "c"
            );

    // the nodes used for a sub-block job are either the shape size
    // or 0 in the case of single core jobs
    const unsigned nodes(
            corner.isCoreSet() ? 0 : shape.size()
            );
    job->node_count( nodes );
}

std::string
Compute::getMidplane(
        const job::Create::Ptr& job,
        const Corner& corner
        )
{
    const Coordinates& coordinates = corner.getBlockCoordinates();
    if ( coordinates.valid() ) {
        return this->getMidplaneFromCoordinates( job, coordinates );
    } else {
        return corner.getMidplane();
    }
}

std::string
Compute::getMidplaneFromCoordinates(
        const job::Create::Ptr& job,
        const Coordinates& coordinates
        )
{
    // corner coordinates will be within the block, but to find the corner we need
    // to know which midplane it exists within. 
    LOG_TRACE_MSG( "corner coordinates: " << coordinates );
    unsigned midplane_a = coordinates.a();
    unsigned midplane_b = coordinates.b();
    unsigned midplane_c = coordinates.c();
    unsigned midplane_d = coordinates.d();
    midplane_a /= BGQTopology::MAX_A_NODE;
    midplane_b /= BGQTopology::MAX_B_NODE;
    midplane_c /= BGQTopology::MAX_C_NODE;
    midplane_d /= BGQTopology::MAX_D_NODE;

    // validate midplane coordinates fit within the maximum
    if (
            midplane_a >= BGQTopology::MAX_A_MIDPLANES ||
            midplane_b >= BGQTopology::MAX_B_MIDPLANES ||
            midplane_c >= BGQTopology::MAX_C_MIDPLANES ||
            midplane_d >= BGQTopology::MAX_D_MIDPLANES
       )
    {
        job->error(
                "could not find corner with coordinates " +
                boost::lexical_cast<std::string>( coordinates ) +
                " in block " +
                _name,
                runjob::error_code::corner_invalid
                );

        return std::string();
    }

    // get midplane and validate it exists within this block
    const BGQMidplaneNodeConfig* result = _config->midplaneNodeConfig( midplane_a, midplane_b, midplane_c, midplane_d );
    if ( !result ) {
        job->error(
                "could not find corner with coordinates " +
                boost::lexical_cast<std::string>( coordinates ) +
                " in block " +
                _name,
                runjob::error_code::corner_invalid
                );

        return std::string();
    }

    LOG_TRACE_MSG(
            "midplane coordinates: (" <<
            midplane_a << "," <<
            midplane_b << "," <<
            midplane_c << "," <<
            midplane_d <<
            ") " <<
            result->posInMachine()
            );

    return result->posInMachine();
}

void
Compute::arbitrateFullBlock(
        const job::Create::Ptr& job
        )
{
    // validate np and ranks per node fits into block size
    JobInfo& info = job->_job_info;
    const size_t np = info.getNp();
    const size_t ranks = info.getRanksPerNode();
    const size_t jobSize = _size * ranks;
    if ( jobSize < np ) {
        job->error(
                "np value of " + boost::lexical_cast<std::string>(np) +
                " with ranks per node value of " + boost::lexical_cast<std::string>(ranks) +
                " is greater than block size of " + boost::lexical_cast<std::string>(_size),
                runjob::error_code::job_np_invalid
                );

        return;
    }

    // validate stdin rank fits into job size
    const size_t stdinRank = info.getStdinRank();
    if ( stdinRank >= jobSize ) {
        job->error(
                "stdin rank of " + boost::lexical_cast<std::string>(stdinRank) +
                " with ranks per node value of " + boost::lexical_cast<std::string>(ranks) +
                " is greater than block size of " + boost::lexical_cast<std::string>(_size),
                runjob::error_code::job_np_invalid
                );
    }

    // validate strace rank fits into job size
    const Strace& strace = info.getStrace();
    if ( strace.getScope() == Strace::Node && strace.getRank() >= jobSize ) {
        job->error(
                "strace rank of " + boost::lexical_cast<std::string>(strace.getRank()) +
                " with ranks per node value of " + boost::lexical_cast<std::string>(ranks) +
                " is greater than block size of " + boost::lexical_cast<std::string>(_size),
                runjob::error_code::job_np_invalid
                );
    }

    // set --np to job size if it has not been set
    if ( !np ) {
        info.setNp( static_cast<unsigned>(jobSize) );
        LOG_TRACE_MSG( "set np to " << jobSize );
    }

    BOOST_FOREACH( Midplanes::value_type& i, _midplanes ) {
        Midplane& mp = i.second;
        if ( !mp.arbitrate(job) ) return;
    }

    // add I/O ratio
    BOOST_FOREACH( auto i, _io ) {
        const Uci& location = i.first;
        const unsigned computes = i.second;
        job->io( location, computes );
    }
 
    // corner is always 0,0,0,0,0 for full-block jobs
    job->_job_info.getSubBlock().corner().setBlockCoordinates(
            Coordinates( 0, 0, 0, 0, 0 )
            );
    job->_job_info.getSubBlock().corner().setMidplaneCoordinates(
            Coordinates( 0, 0, 0, 0, 0 )
            );

    // remember each midplane is running this job
    BOOST_FOREACH( Midplanes::value_type& i, _midplanes ) {
        Midplane& mp = i.second;
        mp.setJob( job->create() );
    }

    // update timer with block size
    job->_arbitration_timer->otherData( 
            boost::lexical_cast<std::string>(_size) + "c"
            );

    job->node_count( _size );
}

} // block
} // server
} // runjob
