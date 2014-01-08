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
#include "server/job/tool/Container.h"

#include "common/Exception.h"
#include "common/logging.h"

#include "server/job/tool/Status.h"

#include "server/job/Tool.h"

#include "server/Job.h"

#include <boost/make_shared.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {
namespace tool {

Container::Container() :
    _tools(),
    _ranks()
{

}

Container::~Container()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

void
Container::add(
        const Job::Ptr& job,
        const runjob::tool::Daemon& tool,
        runjob::tool::Proctable& proctable,
        unsigned id,
        const Tool::Callback& callback
        )
{
    try {
        _tools.insert(
                Tools::value_type(
                    id,
                    boost::make_shared<Tool>(id, job, tool, proctable, callback)
                    )
                );
    } catch ( const std::exception& e ) {
        LOG_INFO_MSG( e.what() );
    }
}

void
Container::remove(
        unsigned id
        )
{
    const auto result = _tools.find( id );
    if ( result != _tools.end() ) {
        _tools.erase( result );
        LOG_TRACE_MSG( "removed tool " << id );
    } else {
        LOG_WARN_MSG( "could not find tool " << id << " to remove" );
    }
}

void
Container::started(
        unsigned id,
        const Uci& location
        )
{
    const auto tool = _tools.find( id );
    if ( tool == _tools.end() ) {
        LOG_WARN_MSG( "could not find tool " << id );
        return;
    }

    tool->second->update( tool::Status::Running, location );
}

void
Container::exited(
        const boost::shared_ptr<bgcios::jobctl::ExitToolMessage>& etm,
        const Uci& location
        )
{
    const auto tool = _tools.find( etm->toolId );
    if ( tool == _tools.end() ) {
        LOG_WARN_MSG( "could not find tool " << etm->toolId );
        return;
    }

    tool->second->exited( etm, location );
}

void
Container::failure(
        const boost::shared_ptr<bgcios::jobctl::StartToolAckMessage>& ack,
        const Uci& location
        )
{
    const auto tool = _tools.find( ack->toolId );
    if ( tool == _tools.end() ) {
        LOG_WARN_MSG( "could not find tool " << ack->toolId );
        return;
    }

    tool->second->failure( ack, location );
}

} // tool
} // job
} // server
} // runjob
