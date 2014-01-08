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
/* (C) Copyright IBM Corp.  2007, 2011                              */
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

#include "rtdbview.h"

#include <unistd.h>

#include <iostream>
#include <string>

using namespace std;


static void test_disconnect_problem_serv()
{
    const string bg_realtime_so_filename("./bg_realtime.so");
    rtdbview_t rtdbview( bg_realtime_so_filename );

    const string db_properties_filename("db.properties");
    const string log_filename("logs/log1.log");
    int log_level(5);

    int rc;

    rc = rtdbview.init( &db_properties_filename, &log_filename, &log_level );
    cout << "rtdbview.init rc=" << rc << endl;

    string block_id("testblock");

    while ( true ) {
        for ( int i = 0 ; i < 100 ; ++i ) {
            rtdbview.block_state_change( &block_id, 'I', 'F' );
        }
        sleep( 1 );
    }
} // test_disconnect_problem_serv()


int main( int argc, char* argv[] )
{
    test_disconnect_problem_serv();
} // main()
