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
#include <bgsched/realtime/Client.h>
#include <bgsched/realtime/ClientEventListener.h>
#include <bgsched/realtime/Filter.h>

using namespace std;

int main( int argc, char *argv[] ) {
    bgsched::realtime::Client c;

    bgsched::realtime::ClientEventListener l;

    bgsched::realtime::Filter f;
    f.setJobs( false ); // No jobs!

    c.addListener( l );

    return 0;
}
