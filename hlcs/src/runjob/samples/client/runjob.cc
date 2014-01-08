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
#include <bgsched/runjob/Client.h>

#include <boost/thread.hpp>

#include <cstdio>
#include <csignal>
#include <iostream>

void
signal( 
        bgsched::runjob::Client& client
      )
{
    sleep( 10 );

    std::cout << "sending SIGINT twice" << std::endl;
    client.kill( SIGINT );
    client.kill( SIGINT );
}

int
main(int argc, char** argv)
{
    bgsched::runjob::Client client( argc, argv );

    boost::thread t(
            boost::bind(
                &signal,
                boost::ref(client)
                )
            );
    
    const int rc = client.start( STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO );

    std::cout << "done with job, exit status=" << rc << std::endl;
}
