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

#include "Connection.hpp"

#include "AbstractResponder.hpp"
#include "ErrorResponder.hpp"
#include "exception.hpp"
#include "Request.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/system/error_code.hpp>

#include <iostream>
#include <string>
#include <vector>


using boost::bind;

using std::string;
using std::vector;


LOG_DECLARE_FILE( "capena-http" );


namespace capena {
namespace server {


ConnectionPtr Connection::create(
        SocketPtr socket_ptr,
        bgq::utility::portConfig::UserType::Value user_type,
        ResponderCreatorFn responder_creator_fn
    )
{
    return ConnectionPtr( new Connection(
            socket_ptr,
            user_type,
            responder_creator_fn
        ) );
}


Connection::Connection(
        SocketPtr socket_ptr,
        bgq::utility::portConfig::UserType::Value user_type,
        ResponderCreatorFn responder_creator_fn
    ) :
        _socket_ptr(socket_ptr),
        _user_type(user_type),
        _responder_creator_fn(responder_creator_fn),
        _strand( _socket_ptr->get_io_service() ),
        _request_count(0),
        _error(false),
        _streambuf(),
        _istream( &_streambuf ),
        _chunk_data(),
        _responses(),
        _out_msgs(),
        _request_ptr(),
        _responder_ptr()
{
    // Nothing to do.
}


void Connection::start()
{
    LOG_DEBUG_MSG( "Starting new connection." );

    // This class works by reading requests in a loop,
    // start by reading the first request and when that one's complete it'll eventually start reading the next request by calling this _startReadingRequest.

    _startReadingRequest();
}


Connection::~Connection()
{
    LOG_DEBUG_MSG( "Discarding Connection" );
}


std::size_t Connection::_checkStreambufContains(
        const boost::system::error_code& error,
        const boost::asio::streambuf& streambuf,
        uint64_t bytes_required
    )
{
    // This helper function gets called during async_read_until
    // to figure out if the streambuf contains at least bytes_required bytes,
    // so that that many bytes can be read off the buffer and processed.

    if ( error )  return 0; // There was an error, don't need any more
    if ( streambuf.size() >= bytes_required )  return 0;  // Streambuf contains the bytes.
    return (bytes_required - streambuf.size()); // Return the number of bytes I want
}


void Connection::_startReadingRequest()
{
    if ( _error ) {
        // In an error condition, probably due to write error, don't read another request.
        // Returning from here should cause this Connection to be destroyed.
        return;
    }

    LOG_TRACE_MSG( "Waiting for request #" << (_request_count+1) );

    // The first thing on the request is the "request line", like "HTTP/1.1 GET /bg/blocks"

    _readLine(
            bind(
                    &Connection::_handleReadRequestLine,
                    shared_from_this(),
                    _1, _2
                )
        );
}


void Connection::_handleReadRequestLine(
        const boost::system::error_code& error,
        const std::string& request_line
    )
{
    // Could have been an error reading, if so just drop the connection.

    // If there wasn't actually a request line (just empty string from client)
    // restart the process of reading the request.

    // Create the current Request object to hold request info (headers, etc.) given the current request line.
    // After the request line comes the header lines (like "Accept: application/json"), terminated by an empty line, so we
    // wait for the header line.


    if ( error ) {
        if ( error == boost::asio::error::eof ) {
            LOG_INFO_MSG( "Connection from " << _socket_ptr->lowest_layer().remote_endpoint() << " closed (got eof)" );

            ResponderPtr rptr(_responder_wk_ptr.lock());
            if ( rptr )  rptr->notifyDisconnect();

            return;
        }

        if ( error == boost::asio::error::shut_down ) {
            LOG_INFO_MSG( "Connection from " << _socket_ptr->lowest_layer().remote_endpoint() << " closed (got shutdown)" );

            ResponderPtr rptr(_responder_wk_ptr.lock());
            if ( rptr )  rptr->notifyDisconnect();

            return;
        }

        LOG_WARN_MSG( "Unexpected error reading request line, error code = " << error << " message=" << error.message() );

        ResponderPtr rptr(_responder_wk_ptr.lock());
        if ( rptr )  rptr->notifyDisconnect();

        return;
    }

    try {

        if ( request_line.empty() ) { // ignore empty status line.
            _startReadingRequest();
            return;
        }

        ++_request_count;
        LOG_DEBUG_MSG( "Receiving request #" << _request_count << " for this connection." );

        try {

            _request_ptr.reset( new Request(
                    request_line
                ) );

        } catch ( std::invalid_argument& e ) {

            LOG_WARN_MSG( string() + "The request line was not valid, error is " + e.what() + ". The request line is '" + request_line + "'" );

            BOOST_THROW_EXCEPTION( exception::BadRequest( "the request line was not valid" ) );

        }

        _startReadHeaderLine();

    } catch ( std::exception& e ) {

        Response r(
                _strand.wrap( boost::bind(
                        &Connection::_notifyResponseStatusHeaders,
                        shared_from_this(),
                        _1, _2, _3
                    ) ),
                _strand.wrap( boost::bind(
                        &Connection::_notifyResponseBodyData,
                        shared_from_this(),
                        _1, _2
                    ) )
            );
        r.setException( e );

    }
}


void Connection::_startReadHeaderLine()
{
    _readLine(
            bind(
                    &Connection::_handleReadHeaderLine,
                    shared_from_this(),
                    _1, _2
                )
        );
}


void Connection::_handleReadHeaderLine(
        const boost::system::error_code& error,
        const std::string& header_line
    )
{
    if ( error ) {
        LOG_WARN_MSG( "Error reading header line, error code = " << error << " message=" << error.message() );
        return;
    }

    if ( header_line.empty() ) {

        _startingNewHeader();

        _headersComplete();

        return;
    }

    bool is_continuation(false);
    string header_name_norm;
    string header_value;

    try {
        http::parseHeaderLine(
                header_line,
                &is_continuation,
                &header_name_norm,
                &header_value
            );

        if ( is_continuation ) {
            // This is a continuation line, append new line after trimming SP and HT to value.

            _header_line += "...";

            _header_value += header_value;

        } else {
            // This is a new header.

            _startingNewHeader();

            _header_line = header_line;

            _header_name = header_name_norm;
            _header_value = header_value;
        }

        _startReadHeaderLine();
    } catch ( std::exception& e )
    {
        LOG_WARN_MSG( "Didn't get a valid header line from HTTP client. Discarding connection." );
    }
}


void Connection::_startingNewHeader()
{
    if ( _header_line.empty() ) {
        return;
    }

    LOG_DEBUG_MSG( "Header line: '" << _header_line << "' -> '" << _header_name << "' : '" << _header_value << "'" );

    boost::algorithm::trim_right( _header_value ); // section 4.2

    _request_ptr->postHeader( _header_name, _header_value );

    _header_line.clear();
}


void Connection::_headersComplete()
{
    LOG_DEBUG_MSG( "End of headers" );

    // This function is called inside of an asio ssl service strand.
    // If a lot of work is done in this frame (like database calls), then ssl operations (like new connections) will be blocked.
    // To prevent blocking, I'll post 'initializing the responder' to a new stack to let the ssl strand continue.

    _strand.post(
            bind( &Connection::_initializeResponder, shared_from_this() )
        );
}


void Connection::_initializeResponder()
{

    ResponderPtr responder_ptr;

    bool dont_read_body_and_close(false);

    try {

        {
            // RFC 2616, Section 3.6, says if don't understand Transfer-Coding then should response with 501 Unimplemented and close connection.

            // If Transfer-Coding is anything other than not present or chunked then respond with 501 and close.
            if ( ! (_request_ptr->getTransferCoding().empty() || _request_ptr->getTransferCoding() == http::TRANSFER_CODING_CHUNKED) ) {
                LOG_WARN_MSG( "Unexpected transfer coding " << _request_ptr->getTransferCoding() );

                dont_read_body_and_close = true;

                BOOST_THROW_EXCEPTION(
                        exception::Error(
                                http::Status::NotImplemented,
                                string() + "Unexpected transfer coding " + _request_ptr->getTransferCoding()
                            )
                    );
            }
        }

        responder_ptr = _responder_creator_fn(
                _request_ptr,
                _user_type
            );

        if ( ! responder_ptr ) {
            BOOST_THROW_EXCEPTION( std::runtime_error(
                    "NULL pointer returned by responder creator function."
                ) );
        }

    } catch ( std::exception& e )
    {
        LOG_WARN_MSG( "Exception creating responder, request URL='" << _request_ptr->getUrlStr() << "', user_type=" << _user_type << ", error=" << e.what() );

        // Will generate a response with the error.
        responder_ptr = ErrorResponder::create( _request_ptr, std::current_exception() );
    }


    _responder_wk_ptr = responder_ptr;

    AbstractResponder::ResponseComplete response_complete(AbstractResponder::ResponseComplete::CONTINUE);

    responder_ptr->initialize(
            _strand.get_io_service(),
            boost::bind(
                &Connection::_notifyResponseStatusHeaders,
                shared_from_this(),
                _1, _2, _3
                ),
            boost::bind(
                &Connection::_notifyResponseBodyData,
                shared_from_this(),
                _1, _2
                ),
            &response_complete
        );

    if ( dont_read_body_and_close ) {
        // Don't read body or next request, just go away once the response is written.
        return;
    }

    if ( response_complete == AbstractResponder::ResponseComplete::COMPLETE ) {

        // Responder already generated response from headers,
        // figure out if need to read off the rest of the request.

        LOG_DEBUG_MSG( "Responder complete after calling initialize" );

        if ( _request_ptr->isComplete() || // no body in the request.
             _request_ptr->getExpectsContinue() ) // client didn't send body.
        {
            // Read the next request.
            _startReadingRequest();
            return;
        }

        // Need to read the body.
        _startReadingBody();

        return;
    }

    if ( _request_ptr->isComplete() ) {
        // There's no more data coming so done with reading request, waiting for response complete from Responder.
        LOG_DEBUG_MSG( "Request is complete, waiting for response from Responder." );

        _startReadingRequest();

        return;
    }

    // Responder didn't generate response based on headers, so need to read the request data.

    _responder_ptr = responder_ptr;
    LOG_DEBUG_MSG( "Saving Responder until got request data." );

    if ( _request_ptr->getExpectsContinue() ) {
        // Send back a 100 Continue response...

        _newResponse( http::CONTINUE_RESPONSE_ENTITY );
    }

    _startReadingBody();
}


void Connection::_startReadingBody()
{
    if ( _request_ptr->getTransferCoding() == http::TRANSFER_CODING_CHUNKED ) {
        _readBodyChunked();
        return;
    }

    if ( _request_ptr->getContentLength() != -1 ) {
        _readBodyContentLength();
        return;
    }

    _readBodyIdent();
}


void Connection::_readBodyChunked()
{
    _startReadChunkHeader();
}


void Connection::_startReadChunkHeader()
{
    _readLine(
            bind(
                    &Connection::_handleReadChunkHeader,
                    shared_from_this(),
                    _1, _2
                )
        );
}


void Connection::_handleReadChunkHeader(
        const boost::system::error_code& error,
        const std::string& chunk_header_line
    )
{
    if ( error ) {
        LOG_WARN_MSG( "Error reading chunk header, error code = " << error << " message=" << error.message() );
        return;
    }

    std::istringstream iss( chunk_header_line );
    uint64_t chunk_size;
    iss >> std::hex >> chunk_size;

    LOG_DEBUG_MSG( "chunk size=" << chunk_header_line << "->" << chunk_size );

    if ( chunk_size == 0 ) { // indicates last chunk.

        _startReadChunkTrailer();

        return;
    }

    boost::asio::async_read(
            *_socket_ptr,
            _streambuf,
            bind( &Connection::_checkStreambufContains, // Note: tried using boost::asio::transfer_at_least but didn't work as expected.
                  _1,
                  boost::ref( _streambuf ),
                  chunk_size
                ),
            _strand.wrap( bind(
                    &Connection::_handleReadChunkData,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred,
                    chunk_size
                ) )
        );
}


void Connection::_handleReadChunkData(
        const boost::system::error_code& error,
        size_t ,//bytes_transferred,
        uint64_t chunk_size
    )
{
    if ( error ) {
        LOG_WARN_MSG( "Error reading chunk data, error code = " << error << " message=" << error.message() );
        return;
    }

    // Read off _chunk_size bytes...

    vector<char> bytes( chunk_size );

    _istream.read( bytes.data(), bytes.size() );

    _chunk_data = string( bytes.begin(), bytes.end() );

    _readLine(
            bind(
                    &Connection::_handleReadChunkDataLastLine,
                    shared_from_this(),
                    _1, _2
                )
        );
}


void Connection::_handleReadChunkDataLastLine(
        const boost::system::error_code& error,
        const std::string& chunk_last_line
    )
{
    if ( error ) {
        LOG_WARN_MSG( "Error reading chunk data last line (empty line after chunk data and before next chunk header), error code = " << error << " message=" << error.message() );
        return;
    }

    // According to RFC2616, Section 3.6.1, chunk ends with CRLF.
    if ( ! chunk_last_line.empty() ) {
        LOG_WARN_MSG( "Error reading chunk data, the last line is not empty as expected. line=" << chunk_last_line );
        return;
    }

    if ( _responder_ptr ) {
        _responder_ptr->postRequestData( _chunk_data, DataContinuesIndicator::EXPECT_MORE_DATA );
    }
    _chunk_data.clear();

    // Go on to the next chunk.
    _startReadChunkHeader();
}


void Connection::_startReadChunkTrailer()
{
    _readLine(
            bind(
                    &Connection::_handleReadChunkTrailerLine,
                    shared_from_this(),
                    _1, _2
                )
        );
}


void Connection::_handleReadChunkTrailerLine(
        const boost::system::error_code& error,
        const std::string& chunk_trailer_line
    )
{
    if ( error ) {
        LOG_WARN_MSG( "Error reading chunk trailer line, error code = " << error << " message=" << error.message() );
        return;
    }

    if ( chunk_trailer_line.empty() ) {
        // We're done with this request...

        _doneReadingBodyChunked();

        return;
    }

    LOG_WARN_MSG( "Ignoring unexpected chunk trailer: " << chunk_trailer_line );

    _startReadChunkTrailer();
}


void Connection::_doneReadingBodyChunked()
{
    if ( _responder_ptr ) {
        _responder_ptr->postRequestData( string(), DataContinuesIndicator::END_OF_DATA );
    }

    LOG_DEBUG_MSG( "Discarding Responder." );

    _responder_ptr.reset();

    _startReadingRequest(); // Read the next request.
}


void Connection::_readBodyContentLength()
{
    LOG_TRACE_MSG( "Reading non-chunked content, length=" << _request_ptr->getContentLength() );

    boost::asio::async_read(
            *_socket_ptr,
            _streambuf,
            bind( &Connection::_checkStreambufContains, // Note: tried using boost::asio::transfer_at_least but didn't work as expected.
                  _1,
                  boost::ref( _streambuf ),
                  _request_ptr->getContentLength()
                ),
            _strand.wrap( bind(
                    &Connection::_handleReadBodyContentLength,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                ) )
        );
}


void Connection::_handleReadBodyContentLength(
        const boost::system::error_code& error,
        size_t /*bytes_transferred*/
    )
{
    if ( error ) {
        LOG_WARN_MSG( "Error reading body (content-length), error code = " << error << " message=" << error.message() );
        return;
    }

    vector<char> bytes( _request_ptr->getContentLength() );

    _istream.read( bytes.data(), bytes.size() );

    string data( bytes.begin(), bytes.end() );

    LOG_DEBUG_MSG( "Finished reading body data, size=" << data.size() );
    LOG_TRACE_MSG( "data=" << data );

    if ( _responder_ptr ) {
        _responder_ptr->postRequestData( data, DataContinuesIndicator::END_OF_DATA );
    }

    LOG_DEBUG_MSG( "Discarding Responder." );

    _responder_ptr.reset();

    _startReadingRequest(); // Read the next request.
}


void Connection::_readBodyIdent()
{
    // Read and continue until eof.

    boost::asio::async_read(
            *_socket_ptr,
            _streambuf,
            bind( &Connection::_checkStreambufContains, // Note: tried using boost::asio::transfer_at_least but didn't work as expected.
                  _1,
                  boost::ref( _streambuf ),
                  1
                ),
            _strand.wrap( bind(
                    &Connection::_handleReadBodyIdent,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                ) )
        );
}


void Connection::_handleReadBodyIdent(
        const boost::system::error_code& error,
        size_t bytes_transferred
    )
{
    // if error indicates end of file, then notify responder and return so connection is discarded/closed.
    // otherwise read the data, notify the responder with the data, and call again.


    if ( error ) {
        if ( error == boost::asio::error::eof ) {
            LOG_DEBUG_MSG( "eof reading body ident." );

            if ( _responder_ptr ) {
                _responder_ptr->postRequestData( string(), DataContinuesIndicator::END_OF_DATA );
            }

            return;
        }

        LOG_WARN_MSG( "Unexpected error in " << __FUNCTION__ << " error=" << error );
        return;
    }

    vector<char> bytes( bytes_transferred );

    _istream.read( bytes.data(), bytes.size() );

    if ( _responder_ptr ) {
        _responder_ptr->postRequestData(
                string( bytes.begin(), bytes.end() ),
                DataContinuesIndicator::EXPECT_MORE_DATA
            );
    }

    _readBodyIdent();
}


void Connection::_readLine(
        ReadLineCallback callback
    )
{
    // simply waits until get LF char (or error), then calls the callback with the line (or error).
    // Note that wraps callback in the strand so caller doesn't need to.

    boost::asio::async_read_until(
            *_socket_ptr,
            _streambuf,
            http::LF,
            _strand.wrap( bind(
                    &Connection::_handleReadLine,
                    shared_from_this(),
                    callback,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                ) )
        );
}


void Connection::_handleReadLine(
        ReadLineCallback callback,
        const boost::system::error_code& error,
        std::size_t /*bytes_transferred*/
    )
{
    // if error, calls the callback fn with the error
    // otherwise, reads the line from the stream and calls the cb fn with that line.

    if ( error ) {
        callback( error, string() );
        return;
    }

    string line;
    std::getline( _istream, line, http::LF );

    if ( ! _istream ) {
        LOG_WARN_MSG( "std::getline failed." );
        callback( boost::system::errc::make_error_code( boost::system::errc::io_error ), string() );
        return;
    }

    // Strip of the CR if ended in CRLF.
    if ( (! line.empty()) && line[line.size()-1] == http::CR ) {
        line = line.substr( 0, line.size() - 1 );
    }

    callback( boost::system::error_code(), line );
}


void Connection::_notifyResponseStatusHeaders(
        http::Status status,
        const Headers& headers,
        BodyPresense expect_body
    )
{
    _strand.post(
            boost::bind(
                &Connection::_notifyResponseStatusHeadersImpl,
                shared_from_this(),
                status, headers, expect_body
                )
            );
}

void Connection::_notifyResponseStatusHeadersImpl(
        http::Status status,
        const Headers& headers,
        BodyPresense expect_body
    )
{
    _newResponse( string() +
            http::PROTOCOL_VERSION + " " + http::getText( status ) + http::CRLF
        );

    BOOST_FOREACH( const Headers::value_type &h_pair, headers ) {
        _newResponse( http::formatHeaderLine( h_pair.first, h_pair.second ) );
    }

    if ( expect_body == BodyPresense::EXPECT_BODY ) {
        _newResponse( http::formatHeaderLine( http::header::TRANSFER_ENCODING, http::TRANSFER_CODING_CHUNKED ) );
    } else {
        _newResponse( http::formatHeaderLine( http::header::CONTENT_LENGTH, "0" ) );
    }

    static const string NO_CACHE_HEADERS(
            http::formatHeaderLine( http::header::PRAGMA, "no-cache" ) +
            http::formatHeaderLine( http::header::CACHE_CONTROL, "no-cache" ) +
            http::formatHeaderLine( http::header::EXPIRES, "Wed, 31 Dec 1969 18:00:00 CST" )
        );

    // Tell client and caches not to cache the dynamic response.
    _newResponse( NO_CACHE_HEADERS );

    _newResponse( http::CRLF );
}


void Connection::_notifyResponseBodyData(
        const std::string& data,
        DataContinuesIndicator data_continues
        )
{
    _strand.post(
            boost::bind(
                &Connection::_notifyResponseBodyDataImpl,
                shared_from_this(),
                data,
                data_continues
                )
            );
}

void Connection::_notifyResponseBodyDataImpl(
        const std::string& data,
        DataContinuesIndicator data_continues
    )
{
    if ( ! data.empty() ) {
        std::ostringstream chunk_header_oss;
        chunk_header_oss << std::hex << data.size();

        LOG_TRACE_MSG( "Chunk header=" << chunk_header_oss.str() );

        _newResponse( string() + chunk_header_oss.str() + http::CRLF );
        _newResponse( data );
        _newResponse( http::CRLF );
    }

    if ( data_continues == DataContinuesIndicator::END_OF_DATA ) {
        _newResponse( string() +
                "0" + http::CRLF + // Last-chunk
                http::CRLF // end of trailers
            );
    }
}


void Connection::_newResponse(
        const std::string& message
    )
{
    _responses.push_back( message );
    _checkWrite();
}


void Connection::_checkWrite()
{
    if ( _error ) {
        // In an error condition so don't try to write anything.
        return;
    }

    if ( ! _out_msgs.empty() ) {
        // Still waiting for previous output messages to be sent
        return;
    }

    if ( _responses.empty() ) {
        // Nothing to send.
        return;
    }

    _out_msgs.swap( _responses ); // since out_msgs was empty now responses is empty.

    // Stick references to out messages data into buffers and start writing them.
    vector<boost::asio::const_buffer> buffers;
    for ( OutMsgs::const_iterator i(_out_msgs.begin()) ; i != _out_msgs.end() ; ++i ) {
        buffers.push_back( boost::asio::const_buffer( i->data(), i->size() ) );
    }

    boost::asio::async_write(
            *_socket_ptr,
            buffers,
            _strand.wrap( bind(
                    &Connection::_handleWrite,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                ) )
        );
}


void Connection::_handleWrite(
        const boost::system::error_code& error,
        size_t /*bytes_transferred*/
    )
{
    if ( error ) {
        LOG_WARN_MSG( "Error writing, error code = " << error << " message=" << error.message() );
        _error = true;
        _out_msgs.clear();
        _responses.clear();
        return;
    }

    // Finished writing _out_msgs, so can discard them and look for new messages to send.

    LOG_TRACE_MSG( "Finished writing " << _out_msgs.size() << " messages, now have " << _responses.size() );

    _out_msgs.clear();

    _checkWrite();
}


} } // namespace capena::server
