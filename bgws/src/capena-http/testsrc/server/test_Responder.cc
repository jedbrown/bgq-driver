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
#include "RecordingResponder.hpp"

#include "../../server/AbstractResponder.hpp"
#include "../../server/Request.hpp"
#include "../../server/Response.hpp"

#include <boost/bind.hpp>

#include <sstream>
#include <string>


using std::ostringstream;
using std::string;


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE server_Responder
#include <boost/test/unit_test.hpp>


BOOST_GLOBAL_FIXTURE( GFHolder )


BOOST_AUTO_TEST_CASE( test_Responder_Request_NoBody )
{
    // If make a request with no body, then _processRequest is called once with isComplete() true
    // and _processRequest is not called again.

    ostringstream processRequestLog;

    GlobalFixture::get().setCreateResponderFn( bind( &RecordingResponder::create, _1, RecordingResponder::ProcessOption::REQUEST_COMPLETE, &processRequestLog ) );

    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    curl_handle_ptr->perform();

    BOOST_CHECK_EQUAL( "PR C\n", processRequestLog.str() );
}


BOOST_AUTO_TEST_CASE( test_Responder_Request_Body_Response_NotComplete )
{
    // If make a request with a body,
    // _processRequest() is called once with _getRequest().isComplete() false, in this case response not complete
    // so _processRequest() is called again with _getRequest().isComplete() true once body is received.

    ostringstream processRequestLog;

    GlobalFixture::get().setCreateResponderFn( bind( &RecordingResponder::create, _1, RecordingResponder::ProcessOption::REQUEST_COMPLETE, &processRequestLog ) );

    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    curl_handle_ptr->setPostChunked( "Here's some data" );

    curl_handle_ptr->perform();

    BOOST_CHECK_EQUAL( "PR N\nPR C\n", processRequestLog.str() );
}


BOOST_AUTO_TEST_CASE( test_Responder_Request_Body_Response_Complete )
{
    // If make a request with a body,
    // _processRequest() is called with _getRequest().isComplete() false, but in this case the response is complete on first call
    // so _processRequest is NOT called again.

    ostringstream processRequestLog;

    GlobalFixture::get().setCreateResponderFn( bind( &RecordingResponder::create, _1, RecordingResponder::ProcessOption::FIRST_CALL, &processRequestLog ) );

    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    curl_handle_ptr->setPostChunked( "test_Responder_Request_Body_Response_Complete" );

    curl_handle_ptr->perform();

    BOOST_CHECK_EQUAL( "PR N\n", processRequestLog.str() );
}


BOOST_AUTO_TEST_CASE( test_Responder_Request_NoBody_throws )
{
    // Make a request with no body, responder _processRequest() throws exception.

    ostringstream processRequestLog;

    GlobalFixture::get().setCreateResponderFn( bind( &RecordingResponder::create, _1, RecordingResponder::ProcessOption::THROW_COMPLETE, &processRequestLog ) );

    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    curl_handle_ptr->perform();

    BOOST_CHECK_EQUAL( "PR C\n", processRequestLog.str() );
    BOOST_CHECK_EQUAL( 404, curl_handle_ptr->getResponseCode() ); // The responder throws 404 Not Found.
}


BOOST_AUTO_TEST_CASE( test_Responder_Request_Body_throws )
{
    // Make a request with body, responder _processRequest() throws exception.
    // _processRequest is NOT called again when the body is complete.

    ostringstream processRequestLog;

    GlobalFixture::get().setCreateResponderFn( bind( &RecordingResponder::create, _1, RecordingResponder::ProcessOption::THROW_FIRST, &processRequestLog ) );

    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    curl_handle_ptr->setPostChunked( "test_Responder_Request_Body_throws" );

    curl_handle_ptr->perform();

    BOOST_CHECK_EQUAL( "PR N\n", processRequestLog.str() );
    BOOST_CHECK_EQUAL( 404, curl_handle_ptr->getResponseCode() ); // The responder throws 404 Not Found.
}


class AsyncResponder : public capena::server::AbstractResponder
{
public:

    AsyncResponder(
            capena::server::RequestPtr request_ptr
        ) :
            capena::server::AbstractResponder( request_ptr )
    { /* Nothing to do. */ }


    static capena::server::ResponderPtr create(
            capena::server::RequestPtr request_ptr
        )
    { return capena::server::ResponderPtr( new AsyncResponder( request_ptr ) ); }


protected:

    // override
    void _processRequest()
    {
        if ( ! _getRequest().isComplete() )  return;

        _getStrand().post( boost::bind( &AsyncResponder::_waitComplete, this, shared_from_this() ) );
    }

private:


    void _waitComplete( capena::server::ResponderPtr /*responder_ptr*/ )
    {
        capena::server::Response &response(_getResponse());

        response.setContentType( "application/data" );
        response.headersComplete();

        response.out() << *(_getRequest().getBodyOpt());
    }
};


BOOST_AUTO_TEST_CASE( test_Responder_async )
{
    // The response is complete when the responder goes away, the responder can do a post from _processRequest when response complete and generate response later.

    GlobalFixture::get().setCreateResponderFn( bind( &AsyncResponder::create, _1 ) );

    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    const string post_data("test_Responder_async");

    curl_handle_ptr->setPostChunked( post_data );

    string result;
    curl_handle_ptr->setResult( &result );

    curl_handle_ptr->perform();

    BOOST_CHECK_EQUAL( result, post_data );
}


/* More tests:
 *

-- Look for tests to write.

 */
