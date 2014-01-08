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
#include "server/mux/StartJob.h"

#include "server/job/Start.h"
#include "server/job/Container.h"

#include "common/logging.h"
#include "common/Message.h"

#include "server/Job.h"
#include "server/Server.h"

#include <ramdisk/include/services/MessageHeader.h>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace mux {

void
StartJob::create(
        const Server::Ptr& server, 
        const runjob::Message::Ptr& message,
        const std::string& hostname
        )
{
    const Ptr result(
            new StartJob( message, hostname )
            );

    server->getJobs()->find(
            message->getJobId(),
            boost::bind(
                &StartJob::findJobHandler,
                result,
                _1
                )
            );

}

StartJob::StartJob(
        const runjob::Message::Ptr& message,
        const std::string& hostname
        ) :
    _message( message ),
    _hostname( hostname )
{

}

void
StartJob::findJobHandler(
        const Job::Ptr& job
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _hostname );
    LOGGING_DECLARE_JOB_MDC( _message->getJobId() );
    if ( !job ) {
        LOG_WARN_MSG( "could not find job" );
        return;
    }

    job->strand().post(
            boost::bind(
                &job::Start::create,
                job,
                bgcios::StdioService
                )
            );
}

} // mux
} // server
} // runjob

