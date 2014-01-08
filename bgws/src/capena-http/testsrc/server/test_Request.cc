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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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


#include "ConnectSendGetResponseLine.hpp"
#include "GlobalFixture.hpp"
#include "SimpleResponder.hpp"

#include <sstream>


using std::ostringstream;


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE server_Request
#include <boost/test/unit_test.hpp>


BOOST_GLOBAL_FIXTURE( GFHolder )


BOOST_AUTO_TEST_CASE( test_Request_http_request_line_only_method_error )
{
    // If send a request line with only a method then server should reject and close the connection. Result should be "400 Bad Request".

    GlobalFixture::get().setCreateResponderFn( bind( &SimpleResponder::createDefault, _1 ) );


    boost::asio::io_service io_service;

    ostringstream request_oss;
    request_oss << "GET\r\n"; // Note that there's no Request-URI or HTTP-Version!
    request_oss << "Host: " << GlobalFixture::get().endpoint << "\r\n";
    request_oss << "Accept: */*\r\n";
    request_oss << "Connection: close\r\n\r\n";

    ConnectSendGetResponseLine tr(
            io_service,
            GlobalFixture::get().endpoint,
            request_oss.str()
        );


    tr.start();

    io_service.run();

    BOOST_CHECK_EQUAL( tr.getStatusLine(), "HTTP/1.1 400 Bad Request\r" );
}


/*

Request:
 69 - only 2 parts in the request line
 80 - HTTP protocol version is wrong
 89 - invalid URL

-- Test invalid Expect: header (only accepts 100-continue, otherwise Bad Request).

-- test exception thrown after headers complete.

 */
