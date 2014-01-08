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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <csignal>

void
signal(
        bgsched::runjob::Client& client
        )
{
    sleep( 10 );
    client.kill( SIGUSR1 );
}

int
main( int argc, char** argv )
{
    bgsched::runjob::Client client( argc, argv );
    boost::thread t(
            boost::bind(
                &signal,
                boost::ref(client)
                )
            );
    client.start( STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO );
}
