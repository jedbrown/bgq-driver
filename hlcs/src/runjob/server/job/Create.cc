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
#include "server/job/Create.h"

#include "server/block/Compute.h"
#include "server/block/IoNode.h"

#include "server/job/Container.h"

#include "server/mux/ClientContainer.h"
#include "server/mux/Connection.h"

#include "common/JobInfo.h"
#include "common/logging.h"

#include "server/Job.h"
#include "server/Server.h"

#include <control/include/bgqconfig/BGQTopology.h>

#include <boost/make_shared.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

Create::Create(
        const JobInfo& info
        ) :
    _job(),
    _server(),
    _client_id( 0 ),
    _io(),
    _node_count( 0 ),
    _job_info( info ),
    _mux(),
    _block(),
    _pacing(),
    _message(),
    _error( error_code::success ),
    _arbitration_timer(),
    _security_timer(),
    _reconnect( 0 ),
    _callback()
{

}

Create::~Create()
{
    if ( !_job ) {
        this->create();
    }

    if ( _error ) {
        if ( _callback ) _callback( _error, _job );
        return;
    }

    _server->getJobs()->add(
            _job,
            _callback,
            _node_count,
            _arbitration_timer,
            _security_timer
            );
}

Create*
Create::io(
        const boost::shared_ptr<block::IoNode>& io,
        const unsigned computes
        )
{
    const IoNode::Map::const_iterator result = _io.find( io->getLocation() );
    if ( result != _io.end() ) {
        LOG_TRACE_MSG( "already using I/O node " << io->getLocation() );
        return this;
    }

    (void)_io.insert(
            IoNode::Map::value_type(
                io->getLocation(),
                IoNode( io, computes )
                )
            );
    if ( computes != 0 ) {
        LOG_DEBUG_MSG( "added I/O node " << io->getLocation() << " managing " << computes << " compute nodes" );
    }

    return this;
}

Create*
Create::io(
        const Uci& node,
        const unsigned computes
        )
{
    const IoNode::Map::iterator result = _io.find( node );
    if ( result == _io.end() ) {
        LOG_WARN_MSG( "could not find I/O node " << node );
        return this;
    }
    result->second.setComputes( computes );
    LOG_DEBUG_MSG( "I/O node " << node << " manages " << computes << " compute nodes" );

    return this;
}

boost::weak_ptr<Job>
Create::create()
{
    if ( !_job ) {
        _job.reset( new Job(*this) );
    }

    return _job;
}

} // job
} // server
} // runjob
