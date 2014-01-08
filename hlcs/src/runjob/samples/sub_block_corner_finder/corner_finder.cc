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
#include "Block.h"
#include "Options.h"

#include "common/logging.h"

#include <iostream>


LOG_DECLARE_FILE( runjob::log );

using namespace runjob::samples;

int
main(int argc, char** argv)
{
    try {
        Options options( argc, argv );
        if ( options.getHelp() ) {
            options.help( std::cout );
            exit(EXIT_SUCCESS);
        } else if ( options.getVersion() ) {
            options.version( std::cout );
            exit(EXIT_SUCCESS);
        }

        // create block object
        Block steve( options );
    } catch ( const std::exception& e ) {
        LOG_FATAL_MSG( e.what() );
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
