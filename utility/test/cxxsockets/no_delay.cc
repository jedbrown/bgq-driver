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
#define BOOST_TEST_MODULE no_delay
#include <boost/test/unit_test.hpp>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>
#include <utility/include/cxxsockets/ListeningSocket.h>
#include <utility/include/cxxsockets/SockAddr.h>
#include <utility/include/cxxsockets/TCPSocket.h>

#include <iostream>

#include <netinet/tcp.h>

using namespace bgq::utility;
using namespace CxxSockets;

class InitializeLoggingFixture
{
public:
    InitializeLoggingFixture()
    {
        using namespace bgq::utility;
        bgq::utility::initializeLogging( *Properties::create() );
    }
};

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

BOOST_AUTO_TEST_CASE( default_ctor )
{
    // open a listening socket
    SockAddr addr( AF_INET );
    ListeningSocket listen( addr );
    listen.getSockName( addr );
    BOOST_CHECK( addr.getServicePort() != 0 );

    // connect to it
    TCPSocket client;
    client.Connect( addr );

    int flag;
    socklen_t length = sizeof(flag);
    const int ret = getsockopt( client.getFileDescriptor(), IPPROTO_TCP, TCP_NODELAY, &flag, &length );
    BOOST_CHECK_EQUAL( ret, 0 );
    BOOST_CHECK_EQUAL( flag, 1u );
}
