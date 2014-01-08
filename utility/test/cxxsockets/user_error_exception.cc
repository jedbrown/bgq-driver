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
#define BOOST_TEST_MODULE user_error_exception
#include <boost/test/unit_test.hpp>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <utility/include/cxxsockets/FileSet.h>
#include <utility/include/cxxsockets/ListenerSet.h>
#include <utility/include/cxxsockets/SockAddrList.h>
#include <utility/include/cxxsockets/TCPSocket.h>

#include <iostream>

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

BOOST_AUTO_TEST_CASE( file_set )
{
    struct Foo : public CxxSockets::FileSet
    {

    };

    Foo f;
    const TCPSocketPtr foo( new TCPSocket );

    // add first should be ok
    f.AddFile( foo );

    // duplicate should throw
    BOOST_CHECK_THROW(
            f.AddFile( foo ),
            UserError
            );
}

BOOST_AUTO_TEST_CASE( sock_addr_list )
{
    SockAddrList* list;
    BOOST_CHECK_THROW(
            // empty service and nodename
            list = new SockAddrList( 0, std::string(), std::string() ),
            UserError
            );
}

BOOST_AUTO_TEST_CASE( tcp_socket )
{
    TCPSocket* t;
    BOOST_CHECK_THROW(
            // descriptor and family both 0
            t = new TCPSocket( 0, 0 ),
            UserError
            );
}

BOOST_AUTO_TEST_CASE( listener_set )
{
    SockAddrList list; // empty list
    ListenerSet* l;
    BOOST_CHECK_THROW(
            l = new ListenerSet(list),
            UserError
            );
}


