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
#include "server/job/Start.h"

#include "server/block/IoNode.h"

#include "server/cios/Message.h"

#include "server/job/SubNodePacing.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/JobInfo.h"

#include "server/Job.h"

#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/StdioMessages.h>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

void
Start::create(
        const Job::Ptr& job,
        const uint8_t service
        )
{
    const Start::Ptr result( 
            new Start( job, service )
            );
}

Start::Start(
        const Job::Ptr& job,
        const uint8_t service
        ) :
    _job( job ),
    _message( cios::Message::create(bgcios::jobctl::StartJob, _job->id()) )
{
    if ( service == bgcios::JobctlService ) {
        _job->status().set(
                Status::ControlStarting,
                _job
                );
    } else if ( service == bgcios::StdioService ) {
        _job->status().set(
                Status::OutputStarting,
                _job
                );
    } else {
        LOG_FATAL_MSG( "unhandled service: " << service );
        BOOST_ASSERT( !"service" );
    }
   
    // for simulation, we need to inform each stdio daemon where its ranks start
    uint32_t simulation_starting_rank = 0;
    const unsigned ranks_per_node = _job->info().getRanksPerNode();

    // CNK needs to know current time stamp
    struct timeval now;
    gettimeofday(&now, NULL);
    const uint64_t currentTime = now.tv_sec * bgcios::MicrosecondsPerSecond + now.tv_usec;
    if ( service == bgcios::JobctlService ) {
        LOG_DEBUG_MSG( "current time 0x" << std::hex << currentTime );
    }

    // send message to each node
    BOOST_FOREACH( IoNode::Map::value_type& i, _job->io() ) {
        IoNode& node = i.second;
        LOGGING_DECLARE_LOCATION_MDC( i.first );
        const uint32_t num_ranks = static_cast<uint32_t>(node.getComputes() * ranks_per_node);
        LOG_TRACE_MSG( 
                num_ranks << " ranks with " <<
                ranks_per_node << " rank" << (ranks_per_node == 1 ? "" : "s" ) <<
                " per node"
                );

        cios::Message::Ptr msg;

        if ( service == bgcios::JobctlService ) {
            msg = cios::Message::create( bgcios::jobctl::StartJob, _job->id() );
            msg->as<bgcios::jobctl::StartJobMessage>()->header.rank = simulation_starting_rank;
            msg->as<bgcios::jobctl::StartJobMessage>()->numRanksForIONode = num_ranks;
            msg->as<bgcios::jobctl::StartJobMessage>()->currentTime = currentTime;
            if ( _job->pacing() ) {
                _job->pacing()->add( msg, _job );
            } else {
                node.writeControl( msg );
            }
        } else if ( service == bgcios::StdioService ) {
            msg = cios::Message::create( bgcios::stdio::StartJob, _job->id() );
            msg->as<bgcios::stdio::StartJobMessage>()->header.rank = simulation_starting_rank;
            msg->as<bgcios::stdio::StartJobMessage>()->numRanksForIONode = num_ranks;
            node.writeData( msg );
        }
        
        // update starting rank
        simulation_starting_rank += num_ranks;
    }
}

} // job
} // server
} // runjob

