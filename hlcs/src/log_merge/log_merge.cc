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


#include "LogMerge.hpp"


#include <iostream>
#include <stdexcept>

#include <stdlib.h>


using namespace log_merge;

using std::cerr;
using std::cout;


int main( int argc, char *argv[] )
{
    Configuration configuration;

    try {

        configuration.configure(
                Configuration::Args( &(argv[0]), &(argv[argc]) )
            );

        if ( configuration.isHelpRequested() ) {
            return EXIT_SUCCESS;
        }

        LogMerge log_merge;

        log_merge.setConfiguration( configuration );

        log_merge.run();

    } catch ( std::exception& e ) {

        cerr << argv[0] << ": " << e.what() << "\n";
        return EXIT_FAILURE;

    }


	return EXIT_SUCCESS;
}
