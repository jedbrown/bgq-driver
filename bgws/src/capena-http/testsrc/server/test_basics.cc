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


#include "CurlHandle.hpp"
#include "GlobalFixture.hpp"
#include "SimpleResponder.hpp"

#include "../../server/AbstractResponder.hpp"
#include "../../server/Request.hpp"
#include "../../server/Response.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <stdint.h>


using std::cout;
using std::string;
using std::vector;


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE server_basics
#include <boost/test/unit_test.hpp>


static std::string random_bytes( uint64_t size )
{
#if 1
    vector<char> buf( size );

    // read a bunch of bytes from /dev/urandom.

    std::ifstream ifs( "/dev/urandom" );

    // cout << "Reading " << message_size << " bytes from /dev/urandom\n";

    ifs.read( buf.data(), buf.size() );

    return string( buf.begin(), buf.end() );
#else

    string ret;
    for ( uint64_t i = 0 ; i < size ; ++i ) {
        ret += 'A' + (i % 26);
    }

    return ret;

#endif
}


BOOST_GLOBAL_FIXTURE( GFHolder )


BOOST_AUTO_TEST_CASE( test_SimpleRequest )
{
    // Can make HTTP request, get back the response.

    const string message( "test_SimpleRequest" );
    GlobalFixture::get().setCreateResponderFn( boost::bind( &SimpleResponder::create, _1, message ) );


    // Perform the HTTP request and get the result string.
    string result_str;


    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    curl_handle_ptr->setResult( &result_str );

    curl_handle_ptr->perform();


    BOOST_CHECK_EQUAL( result_str, message );
}


BOOST_AUTO_TEST_CASE( test_LargeResponse )
{
    // Response can be huge. Only do 1MB since it takes a long time for larger, should be able to make larger if can be more efficient.

    uint64_t message_size(1*1024*1024);
    string message_str(random_bytes( message_size ));

    GlobalFixture::get().setCreateResponderFn( boost::bind( &SimpleResponder::create, _1, boost::ref( message_str ) ) );


    // Perform the HTTP request and get the result string.
    string result_str;


    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    curl_handle_ptr->setResult( &result_str );

    cout << "Waiting for data from responder...\n";

    curl_handle_ptr->perform();

    cout << "Got " << result_str.size() << " bytes\n";

    BOOST_REQUIRE_EQUAL( result_str.size(), message_str.size() );
    BOOST_CHECK( result_str == message_str );
}


class LargeRequestResponder : public capena::server::AbstractResponder
{
public:

    LargeRequestResponder(
            capena::server::RequestPtr request_ptr,
            std::string *data_received_out
        ) :
            capena::server::AbstractResponder( request_ptr ),
            _data_received_out(data_received_out)
    { /* Nothing to do */ }


    static capena::server::ResponderPtr create(
            capena::server::RequestPtr request_ptr,
            std::string *data_received_out
        )
    {
        return capena::server::ResponderPtr( new LargeRequestResponder( request_ptr, data_received_out ) );
    }


private:

    std::string *_data_received_out;


    // override
    void _processRequest()
    {
        if ( ! _getRequest().isComplete() )  return;

        *_data_received_out = *(_getRequest().getBodyOpt());

        capena::server::Response &response(_getResponse());

        response.setStatus( capena::http::Status::NoContent );
        response.headersComplete();
    }

};


BOOST_AUTO_TEST_CASE( test_LargeRequest )
{
    // Request data can be huge. Try 10MB, takes about 3sec on my pc.

    string message_received;

    GlobalFixture::get().setCreateResponderFn( boost::bind( &LargeRequestResponder::create, _1, &message_received ) );

    // had a problem with my test program where didn't send second chunk correctly. uint64_t message_size(16373);
    uint64_t message_size(10 * 1024 * 1024);
    const string data_str(random_bytes( message_size ));

    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    curl_handle_ptr->setPostChunked( data_str );

    curl_handle_ptr->perform();

    cout << "Message received size=" << message_received.size() << " data sent size=" << data_str.size() << "\n";

    BOOST_REQUIRE_EQUAL( message_received.size(), data_str.size() );
    BOOST_CHECK( message_received == data_str );

#if 0
    cout << "Message received=\n" << message_received << "\n"
            "data sent=\n" << data_str << "\n";
#endif

}
