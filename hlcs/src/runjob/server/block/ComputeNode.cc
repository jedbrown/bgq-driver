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
#include "server/block/ComputeNode.h"

#include "server/block/ComputeCore.h"
#include "server/block/IoLink.h"
#include "server/block/IoNode.h"

#include "common/error.h"
#include "common/Exception.h"
#include "common/logging.h"
#include "common/SubBlock.h"

#include "server/job/Create.h"
#include "server/job/SubNodePacing.h"

#include "server/Job.h"

#include <control/include/bgqconfig/BGQNodePos.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

#include <algorithm>
#include <iostream>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace block {

ComputeNode::ComputeNode(
        const Uci& midplane,
        const BGQNodePos& compute,
        const Uci& connectedIo,
        const Io::Links& links
        ) :
    _location( ),
    _io(),
    _connectedIo(),
    _job(),
    _status( Status::Available ),
    _cores(),
    _pacing( boost::make_shared<job::SubNodePacing>() )
{
    BOOST_ASSERT( 
            BG_UCI_GET_COMPONENT( midplane.get() ) == BG_UCI_Component_Midplane
            );

    // create location based on midplane
    BG_UniversalComponentIdentifier uci( 0 );
    uci |= BG_UCI_SET_ROW( BG_UCI_GET_ROW(midplane.get()) );
    uci |= BG_UCI_SET_COLUMN( BG_UCI_GET_COLUMN(midplane.get()) );
    uci |= BG_UCI_SET_MIDPLANE( BG_UCI_GET_MIDPLANE(midplane.get() ) );
    uci |= BG_UCI_SET_NODE_BOARD( compute.nodeCard() );
    uci |= BG_UCI_SET_COMPUTE_CARD( compute.jtagPort() );
    uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_ComputeCardOnNodeBoard );
    _location = Uci( uci );

    // populate core array
    // we subtract 1 from cores on a card because jobs cannot run on the 17th core
    for ( unsigned i = 0; i < bgq::util::Location::ComputeCardCoresOnBoard - 1; ++i ) {
        _cores[i] = ComputeCore::Ptr( new ComputeCore(i) );
    }

    // find I/O link and connected I/O node
    BOOST_FOREACH( const IoLink& link, links ) {
        if ( link.getCompute() == _location ) {
            _io = link.getIo();
            LOG_TRACE_MSG( "added link " << _io->getLocation() );
        }
        
        if ( link.getIo()->getLocation() == connectedIo ) {
            _connectedIo = link.getIo();
        }
    }
    
    // each compute is required to have a connected I/O node
    if ( !_connectedIo ) {
        LOG_RUNJOB_EXCEPTION( error_code::compute_node_invalid, _location << " missing connected I/O node" );
    }
}

void
ComputeNode::unavailable()
{
    _status = Status::SoftwareFailure;
}

void
ComputeNode::unavailable(
        const Job::Ptr& job
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );

    if ( _job.lock() != job ) return;

    this->unavailable();
    LOG_DEBUG_MSG( _location << " status: " << _status );
}

bool
ComputeNode::arbitrate(
        const SubBlock& resource,
        const job::Create::Ptr& job
        )
{
    // sanity check to ensure we're looking at a core corner
    const runjob::Corner& corner = resource.corner();
    BOOST_ASSERT( corner.isCoreSet() );

    // core 17 is a valid location string, but not a valid core for running jobs
    if ( corner.getCore() >= _cores.size() ) {
        job->error(
                boost::lexical_cast<std::string>(_location) + " core " +
                boost::lexical_cast<std::string>( static_cast<unsigned>(corner.getCore()) ) +
                " is not a valid core location",
                runjob::error_code::corner_invalid
                );

        return false;
    }

    // ensure no job is running
    if ( const Job::Ptr& temp = _job.lock() ) {
        LOG_WARN_MSG( _location << " is running job " << temp->id() );
        job->error(
                boost::lexical_cast<std::string>(_location) + " has a sub-block job running",
                runjob::error_code::block_busy
                );

        return false;
    }

    // ensure core is available
    if ( const Job::Ptr& temp = _cores[ corner.getCore()]->getJob().lock() ) {
        LOG_WARN_MSG(
                boost::lexical_cast<std::string>(_location) << " core " <<
                boost::lexical_cast<std::string>( static_cast<unsigned>(corner.getCore()) ) <<
                " is running job " << temp->id() 
                );
        job->error(
                boost::lexical_cast<std::string>(_location) + " core " + 
                boost::lexical_cast<std::string>( static_cast<unsigned>(corner.getCore()) ) + 
                " has a job running",
                runjob::error_code::block_busy
                );

        return false;
    }

    // core is available
    job->io( _connectedIo, 1 );
    job->pacing( _pacing );
    _cores[ corner.getCore() ]->setJob( job->create() );

    return true;
}

bool
ComputeNode::available(
        const job::Create::Ptr& job
        ) const
{
    // check node status
    if ( _status != Status::Available ) {
        job->error(
                "node " + boost::lexical_cast<std::string>(_location) + " is not available: " + boost::lexical_cast<std::string>( _status ),
                error_code::block_unavailable
                );

        return false;
    }

    // check active job by converting weak_ptr into a shared_ptr
    if ( const Job::Ptr& temp = _job.lock() ) {
        LOG_WARN_MSG( _location << " is running job " << temp->id() );
        job->error(
                "node " + boost::lexical_cast<std::string>(_location) + " has an active job",
                error_code::block_busy
                );

        return false;
    }

    // otherwise check each compute core for active jobs
    BOOST_FOREACH( const ComputeCore::Ptr& core, _cores ) {
        if ( const Job::Ptr& temp = core->getJob().lock() ) {
            LOG_WARN_MSG( "core " << core->getCore() << " is running job " << temp->id() );
            job->error(
                    "node " + boost::lexical_cast<std::string>(_location) + " core " + 
                    boost::lexical_cast<std::string>(core->getCore()) + 
                    " has an active job",
                    error_code::block_busy
                    );
            return false;
        }
    }

    // no job running
    return true;
}

std::ostream&
operator<<(
        std::ostream& os,
        const ComputeNode::Status value
        )
{
    switch ( value ) {
        case ComputeNode::Status::Available:         os << "Available"; break;
        case ComputeNode::Status::SoftwareFailure:   os << "Software Failure"; break;
        default: BOOST_ASSERT( !"unhandled value" );
    }

    return os;
}

} // block
} // server
} // runjob

