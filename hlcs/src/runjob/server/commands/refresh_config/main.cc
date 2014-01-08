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
#include "common/commands/Shell.h"

#include "common/logging.h"

#include "server/commands/refresh_config/Options.h"

LOG_DECLARE_FILE( runjob::server::commands::log );

int
main(
    int argc,
    char** argv
    )
{
    int status = EXIT_FAILURE;
    try {
        using namespace runjob::server::commands;
        status = runjob::commands::Shell<refresh_config::Options>(argc, argv).run();
    } catch ( const std::exception& e ) {
        LOG_FATAL_MSG( e.what() );
    }
        
    exit( status );
}

