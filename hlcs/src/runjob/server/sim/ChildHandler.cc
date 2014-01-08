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
#include "server/sim/ChildHandler.h"

#include "common/logging.h"

#include <cerrno>
#include <csignal>
#include <cstring>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace sim {

ChildHandler::ChildHandler()
{
    struct sigaction sa;
    memset( &sa, 0, sizeof(sa) );
    sa.sa_flags = SA_NOCLDWAIT;
    if ( sigaction( SIGCHLD, &sa, NULL ) != 0 ) {
        LOG_WARN_MSG( "sigaction: " << strerror(errno) );
    }
}

} // sim
} // server
} // runjob
