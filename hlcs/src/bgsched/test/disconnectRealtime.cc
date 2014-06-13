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

// This tests that when a thread is doing receiveMessages(), another thread can call disconnect(), and the call to receiveMessages does something sane.

#include <bgsched/bgsched.h>

#include <bgsched/realtime/Client.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <stdexcept>
#include <string>


void doReceiveMessages(
        bgsched::realtime::Client& client
    )
{
    try {
        bgsched::realtime::Client client2;
        client2.connect();
        client2.requestUpdates( NULL );

        bool rt_ended;

        std::cout << "Calling client.receiveMessages().\n";
        client.receiveMessages( NULL, NULL, &rt_ended );
    } catch ( std::exception& e ) {
        std::cout << "Caught exception in doReceiveMessages thread, " << e.what() << "\n";
    }
}


void disconnectRealtimeClient(
        bgsched::realtime::Client& client
    )
{
    try {
        std::cout << "Waiting 10 sec to disconnect real-time client.\n";
        boost::thread::sleep( boost::get_system_time() + boost::posix_time::seconds( 10 ) ); // wait 10 seconds.

        std::cout << "Disconnecting real-time client.\n";
        client.disconnect();
    } catch ( std::exception& e ) {
        std::cout << "Caught exception in disconnectRealtimeClient thread, " << e.what() << "\n";
    }
}


int main( int /*argc*/, char*/*argv*/[] )
{
    std::cout << "Working...\n";

    bgsched::init( std::string() );

    bgsched::realtime::Client client;

    std::cout << "Calling connect...\n";
    client.connect();

    std::cout << "Calling requestUpdates...\n";
    client.requestUpdates( NULL );

    std::cout << "Creating threads\n";

    boost::thread receive_messages_thd( boost::bind( &doReceiveMessages, boost::ref( client ) ) );
    boost::thread disconnect_realtime_client_thd( boost::bind( &disconnectRealtimeClient, boost::ref( client ) ) );

    std::cout << "Waiting for threads to complete...";

    receive_messages_thd.join();
    disconnect_realtime_client_thd.join();

    client.connect();
    client.requestUpdates( NULL );
    client.disconnect();

    std::cout << "Done!\n";
}
