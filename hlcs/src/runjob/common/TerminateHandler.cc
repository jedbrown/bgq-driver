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
#include "common/TerminateHandler.h"

#include "common/logging.h"
#include "common/LogSignalInfo.h"

LOG_DECLARE_FILE( runjob::log );

namespace runjob {

void
TerminateHandler::handler(
        const boost::system::error_code& error,
        const siginfo_t& siginfo
        )
{
    // log information about the signal
    LogSignalInfo info( siginfo );

    // stop I/O service
    if ( !error ) {
        _io_service.stop();
    }
}

} // runjob
