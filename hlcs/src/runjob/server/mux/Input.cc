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
#include "server/mux/Input.h"

#include "common/message/StdIo.h"

#include "common/logging.h"

#include "server/job/Container.h"
#include "server/job/Input.h"

#include "common/Message.h"

#include "server/Job.h"
#include "server/Server.h"

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace mux {

void
Input::create(
        const Server::Ptr& server, 
        const runjob::Message::Ptr& message,
        const std::string& hostname
        )
{
    const message::StdIo::Ptr msg = boost::static_pointer_cast<message::StdIo>( message );

    const Ptr result(
            new Input( msg, hostname )
            );

    server->getJobs()->find(
            msg->getJobId(),
            boost::bind(
                &Input::findJobHandler,
                result,
                _1
                )
            );
}

Input::Input(
        const message::StdIo::Ptr& message,
        const std::string& hostname
        ) :
    _message( message ),
    _hostname( hostname )
{

}

void
Input::findJobHandler(
        const Job::Ptr& job
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _hostname );
    LOGGING_DECLARE_JOB_MDC( _message->getJobId() );
    if ( !job ) {
        LOG_WARN_MSG( "could not find job" );
        return;
    }

    job::Input::create( job, _message );
}

} // mux
} // server
} // runjob
