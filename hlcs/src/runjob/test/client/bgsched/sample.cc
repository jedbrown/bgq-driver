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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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
#include <bgsched/runjob/Client.h>

#include <iostream>

#include <sys/wait.h>
#include <unistd.h>

int
main( int argc, char** argv )
{
    try {
        using namespace bgsched::runjob;

        Client runjob( argc, argv );
        const int status = runjob.start(
                STDIN_FILENO,
                STDOUT_FILENO,
                STDERR_FILENO
                );
        if ( WIFEXITED(status) ) {
            std::cout << "exited with status: " << WEXITSTATUS(status) << std::endl;
            exit( WEXITSTATUS(status) );
        } else if ( WIFSIGNALED(status) ) {
            std::cout << "terminated by signal " << WTERMSIG(status) << std::endl;
            exit( 128 + WTERMSIG(status) );
        }
    } catch ( const std::exception& e ) {
        std::cerr << "could not runjob: " << e.what() << std::endl;
        exit( EXIT_FAILURE );
    }
}
