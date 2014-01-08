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
#include "server/mux/Kill.h"

#include "common/message/KillJob.h"

#include "common/logging.h"

#include "server/job/Container.h"

#include "server/mux/ClientContainer.h"
#include "server/mux/Connection.h"

#include "common/Message.h"

#include "server/Job.h"

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace mux {

Kill::Ptr
Kill::create()
{
    const Ptr result( new Kill() );
    return result;
}

void
Kill::start()
{
    BOOST_ASSERT( _clients );
    BOOST_ASSERT( _message );

    _clients->kill(
            _message,
            boost::bind(
                &Kill::clientHandler,
                    shared_from_this(),
                    _1
                )
            );
}

Kill::Kill() :
    _message(),
    _clients(),
    _hostname()
{

}

void
Kill::clientHandler(
        bool result
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _hostname );
    if ( !result ) {
        LOGGING_DECLARE_JOB_MDC( _message->getClientId() );
        LOG_WARN_MSG( "could not find client" );
    }

    return;
}

} // mux
} // server
} // runjob
