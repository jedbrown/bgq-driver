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
#include "ConnectSendGetResponseLine.hpp"
#include "EchoResponder.hpp"
#include "GlobalFixture.hpp"
#include "SimpleResponder.hpp"

#include "../../server/AbstractResponder.hpp"
#include "../../server/Request.hpp"
#include "../../server/Response.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <boost/asio/ssl.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include <assert.h>


using std::cout;
using std::ostringstream;
using std::string;


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE server_Connection
#include <boost/test/unit_test.hpp>


BOOST_GLOBAL_FIXTURE( GFHolder )


// This responder sends back "No Content" response.

class NoContentResponder : public capena::server::AbstractResponder
{
public:

    NoContentResponder(
            capena::server::RequestPtr request_ptr
        ) :
            capena::server::AbstractResponder( request_ptr )
    { /* Nothing to do */ }


    static capena::server::ResponderPtr create(
            capena::server::RequestPtr request_ptr
        )
    {
        return capena::server::ResponderPtr( new NoContentResponder(
                request_ptr
            ) );
    }


protected:

    // override
    void _processRequest()
    {
        _getResponse().setStatus( capena::http::Status::NoContent );
        _getResponse().headersComplete();
    }

};


//---------------------------------------------------------------------
// Connection types
//---------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_UserType_Admin )
{
    // If connect presenting the administrative certificate, createResponder is called with a user type of Administrator

    bgq::utility::portConfig::UserType::Value conn_user_type;

    GlobalFixture::get().setCreateResponderFn( bind( &SimpleResponder::createSaveConnType, _1, _2, &conn_user_type ) );

    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    curl_handle_ptr->setClientCert(
            GlobalFixture::get().properties_ptr->getValue( "security.admin", "certificate" ),
            GlobalFixture::get().properties_ptr->getValue( "security.admin", "key_file" )
        );

    curl_handle_ptr->perform();


    BOOST_CHECK_EQUAL( conn_user_type, bgq::utility::portConfig::UserType::Administrator );
}


BOOST_AUTO_TEST_CASE( test_UserType_Normal )
{
    // If connect presenting the command certificate, createResponder is called with a user type of Normal

    bgq::utility::portConfig::UserType::Value conn_user_type;

    GlobalFixture::get().setCreateResponderFn( bind( &SimpleResponder::createSaveConnType, _1, _2, &conn_user_type ) );

    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    curl_handle_ptr->setClientCert(
            GlobalFixture::get().properties_ptr->getValue( "security.command", "certificate" ),
            GlobalFixture::get().properties_ptr->getValue( "security.command", "key_file" )
        );

    curl_handle_ptr->perform();


    BOOST_CHECK_EQUAL( conn_user_type, bgq::utility::portConfig::UserType::Normal );
}


BOOST_AUTO_TEST_CASE( test_UserType_None )
{
    // If connect presenting no certificate, createResponder is called with a user type of None

    bgq::utility::portConfig::UserType::Value conn_user_type;

    GlobalFixture::get().setCreateResponderFn( bind( &SimpleResponder::createSaveConnType, _1, _2, &conn_user_type ) );

    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    curl_handle_ptr->perform();


    BOOST_CHECK_EQUAL( conn_user_type, bgq::utility::portConfig::UserType::None );
}


//---------------------------------------------------------------------
// Headers
//---------------------------------------------------------------------


class RecordHeadersResponder : public capena::server::AbstractResponder
{
public:

    RecordHeadersResponder(
            capena::server::RequestPtr request_ptr,
            capena::server::Headers* headers_out
        ) :
            capena::server::AbstractResponder( request_ptr ),
            _headers_out(headers_out)
    { /* Nothing to do */ }


    static capena::server::ResponderPtr create(
            capena::server::RequestPtr request_ptr,
            capena::server::Headers* headers_out
        )
    { return capena::server::ResponderPtr( new RecordHeadersResponder( request_ptr, headers_out ) ); }


protected:

    // override
    void _processRequest()
    {
        if ( ! _getRequest().isComplete() ) {
            return;
        }

        *_headers_out = _getRequest().getHeaders();

        _getResponse().setStatus( capena::http::Status::NoContent );
        _getResponse().headersComplete();
    }

private:
    capena::server::Headers *_headers_out;
};


BOOST_AUTO_TEST_CASE( test_headers )
{
    // Can send headers

    capena::server::Headers headers;

    GlobalFixture::get().setCreateResponderFn( bind( &RecordHeadersResponder::create, _1, &headers ) );

    boost::asio::io_service io_service;

    ostringstream request_oss;
    request_oss << "GET / HTTP/1.1\r\n";
    request_oss << "Header1: headerValue1\r\n";
    request_oss << "Header2: headerValue2\r\n";
    request_oss << "\r\n";

    ConnectSendGetResponseLine csgrl(
            io_service,
            GlobalFixture::get().endpoint,
            request_oss.str()
        );

    csgrl.start();

    io_service.run();


    BOOST_REQUIRE_EQUAL( headers.size(), 2 );
    BOOST_CHECK_EQUAL( headers["HEADER1"], "headerValue1" );
    BOOST_CHECK_EQUAL( headers["HEADER2"], "headerValue2" );
}


BOOST_AUTO_TEST_CASE( test_no_headers )
{
    // Can send no headers.

    capena::server::Headers headers;
    headers["test_name"] = "test_value";

    GlobalFixture::get().setCreateResponderFn( bind( &RecordHeadersResponder::create, _1, &headers ) );

    boost::asio::io_service io_service;

    ostringstream request_oss;
    request_oss << "GET / HTTP/1.1\r\n";
    // No headers!
    request_oss << "\r\n";

    ConnectSendGetResponseLine csgrl(
            io_service,
            GlobalFixture::get().endpoint,
            request_oss.str()
        );

    csgrl.start();

    io_service.run();

    BOOST_CHECK( headers.empty() );
}


BOOST_AUTO_TEST_CASE( test_continue_headers )
{
    // HTTP allows continuing header by sending next line with spaces or tabs.

    capena::server::Headers headers;

    GlobalFixture::get().setCreateResponderFn( bind( &RecordHeadersResponder::create, _1, &headers ) );

    boost::asio::io_service io_service;

    ostringstream request_oss;
    request_oss << "GET / HTTP/1.1\r\n";
    request_oss << "Header1: headerLine1\r\n";
    request_oss << " headerLine2\r\n";
    request_oss << "\r\n";

    ConnectSendGetResponseLine csgrl(
            io_service,
            GlobalFixture::get().endpoint,
            request_oss.str()
        );

    csgrl.start();

    io_service.run();


    BOOST_REQUIRE_EQUAL( headers.size(), 1 );
    BOOST_CHECK_EQUAL( headers["HEADER1"], "headerLine1headerLine2" );
}


//---------------------------------------------------------------------
// Body data tests
//---------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_content_length )
{
    // Can send data using Content-Length.

    capena::http::Method method;
    GlobalFixture::get().setCreateResponderFn( bind( &EchoResponder::create, _1, &method ) );

    string result_str;

    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    const std::string message("test_post_content_length");

    curl_handle_ptr->setPostWithContentLength( message );

    curl_handle_ptr->setResult( &result_str );

    curl_handle_ptr->perform();

    BOOST_CHECK_EQUAL( method, capena::http::Method::POST );
    BOOST_CHECK_EQUAL( result_str, message );
}


BOOST_AUTO_TEST_CASE( test_chunked )
{
    // Can send data using chunked tranfer coding.

    capena::http::Method method;
    GlobalFixture::get().setCreateResponderFn( bind( &EchoResponder::create, _1, &method ) );

    string result_str;


    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    const std::string message("test_post_chunked");

    curl_handle_ptr->setPostChunked( message );

    curl_handle_ptr->setResult( &result_str );

    curl_handle_ptr->perform();

    BOOST_CHECK_EQUAL( method, capena::http::Method::POST );
    BOOST_CHECK_EQUAL( result_str, message );
}


BOOST_AUTO_TEST_CASE( test_put_operation )
{
    // Can do a PUT operation.

    capena::http::Method method;
    GlobalFixture::get().setCreateResponderFn( bind( &EchoResponder::create, _1, &method ) );

    string result_str;

    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    const string message("test_put_operation");

    curl_handle_ptr->setPut( message );

    curl_handle_ptr->setResult( &result_str );

    curl_handle_ptr->perform();

    BOOST_CHECK_EQUAL( method, capena::http::Method::PUT );

    BOOST_CHECK_EQUAL( result_str, message );
}


BOOST_AUTO_TEST_CASE( test_DELETE_operation )
{
    // Can do a DELETE operation.

    capena::http::Method method;
    GlobalFixture::get().setCreateResponderFn( bind( &EchoResponder::create, _1, &method ) );

    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    curl_handle_ptr->setOperation( "DELETE" );

    curl_handle_ptr->perform();

    BOOST_CHECK_EQUAL( method, capena::http::Method::DELETE );
}


BOOST_AUTO_TEST_CASE( test_no_response_body )
{
    // Operation can have no body.

    GlobalFixture::get().setCreateResponderFn( bind( &NoContentResponder::create, _1 ) );

    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    curl_handle_ptr->perform();

    BOOST_CHECK_EQUAL( curl_handle_ptr->getResponseCode(), int(capena::http::Status::NoContent) );
}


BOOST_AUTO_TEST_CASE( test_multiple )
{
    // Can make multiple requests on a single connection.

    /* The curl library will send multiple requests if call peform() with the same handle.
     * I verified that the curl library was making 2 requests on the same connection by looking at trace output on the server:
     * ibm.capena-http.server.Connection: Receiving request #2 for this connection.
     */


    const string message( "test_multiple" );

    GlobalFixture::get().setCreateResponderFn( bind( &SimpleResponder::create, _1, message ) );

    string result1, result2;

    CurlHandlePtr curl_handle_ptr(GlobalFixture::createCurlHandle());

    cout << "Performing request #1\n";
    curl_handle_ptr->setResult( &result1 );
    curl_handle_ptr->perform();

    cout << "Performing request #2\n";
    curl_handle_ptr->setResult( &result2 );
    curl_handle_ptr->perform();

    BOOST_CHECK_EQUAL( result1, message );
    BOOST_CHECK_EQUAL( result2, message );
}


class TestConnectionExpectsContinue
{
public:

    TestConnectionExpectsContinue(
            boost::asio::io_service& io_service,
            const boost::asio::ip::tcp::endpoint& endpoint
        ) :
            _io_service(io_service),
            _endpoint(endpoint),
            _ssl_context( _io_service, boost::asio::ssl::context::tlsv1_client ),
            _socket( io_service, _ssl_context ),
            _got_continue(false),
            _request_body_str( "Here's the rest of the request." )
    {
        std::ostream request_ostr( &_request_sb );
        request_ostr << "POST / HTTP/1.1\r\n";
        request_ostr << "Host: " << GlobalFixture::get().endpoint << "\r\n";
        request_ostr << "Accept: */*\r\n";
        request_ostr << "Content-Length: " << _request_body_str.size() << "\r\n";
        request_ostr << "Expect: 100-continue\r\n";
        request_ostr << "\r\n";
    }


    void start()
    {
        _socket.lowest_layer().async_connect(
                _endpoint,
                boost::bind( &TestConnectionExpectsContinue::_connected, this, boost::asio::placeholders::error )
            );
    }


    bool gotContinue()
    {
        return _got_continue;
    }


private:

    boost::asio::io_service &_io_service;
    const boost::asio::ip::tcp::endpoint &_endpoint;

    boost::asio::ssl::context _ssl_context;

    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> _socket;

    boost::asio::streambuf _request_sb;
    boost::asio::streambuf _response_sb;

    std::string _status_line;
    bool _got_continue;

    const std::string _request_body_str;


    void _connected( const boost::system::error_code& err )
    {
        assert( ! err );

        _socket.async_handshake(
                boost::asio::ssl::stream_base::client,
                boost::bind( &TestConnectionExpectsContinue::_handshakeComplete, this, boost::asio::placeholders::error )
            );
    }


    void _handshakeComplete( const boost::system::error_code& err )
    {
        assert( ! err );

        boost::asio::async_write(
                _socket,
                _request_sb,
                boost::bind( &TestConnectionExpectsContinue::_writeHeadersComplete, this, boost::asio::placeholders::error )
            );

        boost::asio::async_read_until(
                _socket,
                _response_sb,
                "\r\n\r\n",
                boost::bind( &TestConnectionExpectsContinue::_readContinueHeadersComplete, this, boost::asio::placeholders::error )
            );
    }


    void _writeHeadersComplete( const boost::system::error_code& err )
    {
        assert( ! err );

        cout << "TestConnectionExpectsContinue: Sent headers, waiting for 100 Continue response...\n";
    }


    void _readContinueHeadersComplete( const boost::system::error_code& err )
    {
        assert( ! err );

        std::istream response_stream( &_response_sb );
        std::getline( response_stream, _status_line );

        _got_continue = (_status_line == "HTTP/1.1 100 Continue\r");

        cout << "TestConnectionExpectsContinue: read headers, status line='" << _status_line << "'\n";

        std::string end_headers_line;
        std::getline( response_stream, end_headers_line );

        assert( end_headers_line == "\r" );

        std::ostream request_ostr( &_request_sb );
        request_ostr << "Here's the rest of the request.";


        boost::asio::async_write(
                _socket,
                _request_sb,
                boost::bind( &TestConnectionExpectsContinue::_writeBodyComplete, this, boost::asio::placeholders::error )
            );

        boost::asio::async_read_until(
                _socket,
                _response_sb,
                "\r\n\r\n",
                boost::bind( &TestConnectionExpectsContinue::_readHeadersComplete, this, boost::asio::placeholders::error )
            );
    }


    void _writeBodyComplete( const boost::system::error_code& err )
    {
        assert( ! err );

        cout << "TestConnectionExpectsContinue: Wrote body.\n";

        _socket.lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_send );
    }


    void _readHeadersComplete( const boost::system::error_code& err )
    {
        assert( ! err );

        std::istream response_stream( &_response_sb );
        std::getline( response_stream, _status_line );

        cout << "TestConnectionExpectsContinue: Read response headers. status line=" << _status_line << "\n";
    }
};


BOOST_AUTO_TEST_CASE( test_expects_continue )
{
    // Server correctly handles client that sends Expect: 100-continue header by sending "100 Continue" response when get headers.

    GlobalFixture::get().setCreateResponderFn( bind( &SimpleResponder::createDefault, _1 ) );

    boost::asio::io_service io_service;

    TestConnectionExpectsContinue tr(
            io_service,
            GlobalFixture::get().endpoint
        );


    tr.start();

    io_service.run();

    BOOST_CHECK( tr.gotContinue() );
}


//---------------------------------------------------------------------
// Odd behavior.
//---------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_abort_sending_headers )
{
    // If send a request that doesn't have the empty line at the end of headers, the server just closes the connection.

    GlobalFixture::get().setCreateResponderFn( bind( &SimpleResponder::createDefault, _1 ) );

    boost::asio::io_service io_service;

    ostringstream request_oss;
    request_oss << "GET / HTTP/1.1\r\n";
    request_oss << "Host: " << GlobalFixture::get().endpoint << "\r\n";
    request_oss << "Accept: */*\r\n";
    request_oss << "Connection: close\r\n";
    // Notice I don't send the \r\n to indicate end of headers.

    ConnectSendGetResponseLine tr(
            io_service,
            GlobalFixture::get().endpoint,
            request_oss.str()
        );


    tr.start();

    io_service.run();

    BOOST_CHECK( tr.gotReadError() );
}


class DisconnectClient
{
public:
    DisconnectClient(
            boost::asio::io_service& io_service,
            const boost::asio::ip::tcp::endpoint& endpoint,
            boost::condition_variable* progress_cond_p,
            boost::mutex* progress_mtx_p,
            int* progress_p
        ) :
            _io_service(io_service),
            _ssl_context( _io_service, boost::asio::ssl::context::tlsv1_client ),
            _socket( io_service, _ssl_context ),
            _endpoint(endpoint),
            _progress_cond_p(progress_cond_p),
            _progress_mtx_p(progress_mtx_p),
            _progress_p(progress_p)
    {
        std::ostream request_stream( &_request_sb );

        request_stream << "GET / HTTP/1.1\r\n";
        request_stream << "Host: " << endpoint << "\r\n";
        request_stream << "\r\n";
    }


    void start()
    {
        _socket.lowest_layer().async_connect(
                _endpoint,
                boost::bind( &DisconnectClient::_connected, this, boost::asio::placeholders::error )
            );
    }


private:

    boost::asio::io_service &_io_service;
    boost::asio::ssl::context _ssl_context;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> _socket;
    boost::asio::ip::tcp::endpoint _endpoint;
    boost::asio::streambuf _request_sb;

    boost::condition_variable *_progress_cond_p;
    boost::mutex *_progress_mtx_p;
    int *_progress_p;


    void _connected( const boost::system::error_code& err )
    {
        assert( ! err );

        _socket.async_handshake(
                boost::asio::ssl::stream_base::client,
                boost::bind( &DisconnectClient::_handshakeComplete, this, boost::asio::placeholders::error )
            );
    }


    void _handshakeComplete( const boost::system::error_code& err )
    {
        assert( ! err );

        cout << "DisconnectClient: handshake worked, writing request.\n";

        boost::asio::async_write(
                _socket,
                _request_sb,
                boost::bind( &DisconnectClient::_writeComplete, this, boost::asio::placeholders::error )
            );
    }


    void _writeComplete( const boost::system::error_code& err )
    {
        assert( ! err );

        cout << "DisconnectClient: wrote request, waiting for responder be waiting for disconnect...\n";

        {
            boost::unique_lock<boost::mutex> lock( *_progress_mtx_p );
            while ( *_progress_p < 1 ) {
                _progress_cond_p->wait( lock );
            }
        }

        cout << "DisconnectClient: Responder is ready, closing connection\n";

        _socket.lowest_layer().close();
    }

};


class DisconnectResponder : public capena::server::AbstractResponder
{
public:

    DisconnectResponder(
            capena::server::RequestPtr request_ptr,
            bool* disconnect_out,
            boost::condition_variable* progress_cond_p,
            boost::mutex* progress_mtx_p,
            int* progress_p
        ) :
            capena::server::AbstractResponder( request_ptr ),
            _disconnect_out(disconnect_out),
            _progress_cond_p(progress_cond_p),
            _progress_mtx_p(progress_mtx_p),
            _progress_p(progress_p)
    { /* Nothing to do. */ }


    static capena::server::ResponderPtr create(
            capena::server::RequestPtr request_ptr,
            bool *disconnect_out,
            boost::condition_variable* progress_cond_p, boost::mutex* progress_mtx_p, int* progress_p
        )
    { return capena::server::ResponderPtr( new DisconnectResponder( request_ptr, disconnect_out, progress_cond_p, progress_mtx_p, progress_p ) ); }


    // override
    void notifyDisconnect()
    {
        _getStrand().post( boost::bind( &DisconnectResponder::_notifyDisconnect, this, shared_from_this() ) );
    }


    ~DisconnectResponder()
    {
        cout << "~DisconnectResponder.\n";

        boost::unique_lock<boost::mutex> lock( *_progress_mtx_p );
        *_progress_p = 2;
        _progress_cond_p->notify_all();
    }


protected:


    // override
    void _processRequest()
    {
        if ( ! _getRequest().isComplete() )  return;

        // This acceptor is just here as something that the responder can wait on, and can be cancelled.

        _acceptor_ptr.reset( new boost::asio::ip::tcp::acceptor( _getStrand().get_io_service() ) );

        boost::asio::ip::tcp::endpoint endpoint( boost::asio::ip::tcp::v4(), 0 );
        _acceptor_ptr->open( endpoint.protocol() );
        _acceptor_ptr->bind( endpoint );
        _acceptor_ptr->listen();

        _new_socket_ptr.reset( new boost::asio::ip::tcp::socket( _getStrand().get_io_service() ) );

        _acceptor_ptr->async_accept(
                *_new_socket_ptr,
                _getStrand().wrap(
                        boost::bind( &DisconnectResponder::_acceptCb, this, boost::asio::placeholders::error, shared_from_this() )
                    )
            );

        cout << "DisconnectResponder: waiting on acceptor.\n";

        boost::unique_lock<boost::mutex> lock( *_progress_mtx_p );
        *_progress_p = 1;
        _progress_cond_p->notify_all();
    }


private:

    bool *_disconnect_out;

    boost::condition_variable *_progress_cond_p;
    boost::mutex *_progress_mtx_p;
    int *_progress_p;

    boost::shared_ptr<boost::asio::ip::tcp::acceptor> _acceptor_ptr;
    boost::shared_ptr<boost::asio::ip::tcp::socket> _new_socket_ptr;


    void _acceptCb( const boost::system::error_code& err, capena::server::ResponderPtr /*responder_ptr*/ )
    {
        cout << "DisconnectResponder:_acceptCb. Expected error since closed. err=" << err << "\n";
    }


    void _notifyDisconnect( capena::server::ResponderPtr /*responder_ptr*/ )
    {
        cout << "DisconnectResponder: notified of disconnect, closing acceptor.\n";
        *_disconnect_out = true;

        _acceptor_ptr->close();
    }
};



BOOST_AUTO_TEST_CASE( test_disconnect )
{
    // Server must handle case where client disconnects. Application should get notified.


    bool disconnect(false);

    boost::condition_variable progress_cond;
    boost::mutex progress_mtx;
    int progress(0);
    // 0 = initial, client waiting for responder to notify and then will close connection.
    // 1 = responder is ready for responder to shutdown, app waiting for responder shutdown complete.
    // 2 = responder got shutdown / is complete.

    GlobalFixture::get().setCreateResponderFn( bind( &DisconnectResponder::create, _1,
            &disconnect,
            &progress_cond, &progress_mtx, &progress
        ) );

    {
        boost::asio::io_service io_service;

        boost::shared_ptr<DisconnectClient> _client_ptr( new DisconnectClient(
                io_service,
                GlobalFixture::get().endpoint,
                &progress_cond, &progress_mtx, &progress
            ) );

        _client_ptr->start();

        cout << "test_disconnect: Calling run()\n";

        io_service.run();

        cout << "test_disconnect: run() completed.\n";
    }

    {
        cout << "test_disconnect: Waiting for responder complete...\n";
        boost::unique_lock<boost::mutex> lock( progress_mtx );
        while ( progress < 2 ) {
            progress_cond.wait( lock );
        }
    }

    BOOST_CHECK( disconnect );
}


/* More tests:

 - abort request
   - disconnect in middle of sending body (content-length)
   - disconnect in middle of sending body (chunked)

 - Headers
   - Continuation line
   - Invalid header line (no :)
   - trim spaces before and after
   - extra spaces at end
   - multiple occurrences of a header (Request, not Connection)

 353 - unexpected transfer coding (valid are not present or chunked)
 402 - response complete with headers
 410 - expects continue
 418 - No body when expect body
 449 - Read body chunked
 454 - read body given content-length
 459 - read body ident
 493 - invalid chunk size (not a hex string) -- this is broken!
     - send more than chunk size or less
 564 - no empty line after chunk size
 600 - chunk trailers or no trailer
 630 - content-length doesn't match length
 811 - response with body, check transfer coding chunked header
 813 - response no body
 834 - response with body, get content

-- Test case where client sends "Expect: 100-continue", but responder generates response from headers; server should NOT send 100 Continue & just send response.
   -- Test with Responder throws exception

-- Test different ways that application indicates no data in response and _processRequest doesn't get called again.

-- test case where responder does headersComplete() and then throws exception.

--
This seems to be a problem, when send
        Connection: close -- is this a valid way to indicate the request has a message?
        Expect: 100-continue

I think the Request is saying there's no body.

--

BOOST_AUTO_TEST_CASE( test_http_request_valid_methods )
{
    // Can make request with different methods: OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT, or other extension method (a valid token)
    // See [HTTP 5.1.1]


}

 */
