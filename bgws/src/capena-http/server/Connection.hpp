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

#ifndef CAPENA_SERVER_CONNECTION_HPP_
#define CAPENA_SERVER_CONNECTION_HPP_


#include "fwd.hpp"
#include "Response.hpp"

#include "../http/http.hpp"

#include <utility/include/portConfiguration/types.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <iosfwd>
#include <string>
#include <vector>

#include <stdint.h>


namespace capena {
namespace server {


/*! \brief An HTTP connection.
 *
 *  Handles the HTTP protocol stuff.
 *
 *  The client writes a request on the connection,
 *  it's composed of a header + body.
 *  The body is optional.
 *
 *  Multiple requests can be sent on a single connection.
 *  The reponses for requests must be sent back in the order
 *  of the requests.
 *
 *  The request is read and passed off to the application's Responder.
 *  The application calculates a response and this class writes the
 *  response back to the client.
 *  Then it reads the next request and so on.
 */
class Connection : public boost::enable_shared_from_this<Connection>, boost::noncopyable
{
public:


    static ConnectionPtr create(
            SocketPtr socket_ptr,
            bgq::utility::portConfig::UserType::Value user_type,
            ResponderCreatorFn responder_creator_fn
        );


    void start();


    boost::asio::io_service& ioService()  { return _socket_ptr->get_io_service(); }


    ~Connection();


private:

    typedef std::vector<std::string> OutMsgs;

    typedef boost::function<void ( const boost::system::error_code& error, const std::string& line )> ReadLineCallback;


    static std::size_t _checkStreambufContains(
            const boost::system::error_code& error,
            const boost::asio::streambuf& streambuf,
            uint64_t bytes_required
        );


    SocketPtr _socket_ptr;
    bgq::utility::portConfig::UserType::Value _user_type;
    ResponderCreatorFn _responder_creator_fn;

    boost::asio::strand _strand;

    uint64_t _request_count;

    bool _error; //!< Will be set to true if a nasty error occurred and should not continue.

    boost::asio::streambuf _streambuf;
    std::istream _istream;

    std::string _header_line;
    std::string _header_name;
    std::string _header_value;

    std::string _chunk_data;

    OutMsgs _responses; //!< Internal messages waiting to be output to the socket.

    OutMsgs _out_msgs; //!< Messages currently being output to the socket.

    RequestPtr _request_ptr;
    ResponderPtr _responder_ptr;
    boost::weak_ptr<AbstractResponder> _responder_wk_ptr;


    Connection(
            SocketPtr socket_ptr,
            bgq::utility::portConfig::UserType::Value user_type,
            ResponderCreatorFn responder_creator_fn
        );


    void _startReadingRequest();

    void _handleReadRequestLine(
            const boost::system::error_code& error,
            const std::string& request_line
        );

    void _startReadHeaderLine();

    void _handleReadHeaderLine(
            const boost::system::error_code& error,
            const std::string& header_line
        );

    void _startingNewHeader();

    void _headersComplete();


    void _initializeResponder();


    void _startReadingBody();


    void _readBodyChunked();

    void _startReadChunkHeader();

    void _handleReadChunkHeader(
            const boost::system::error_code& error,
            const std::string& chunk_header_line
        );

    void _handleReadChunkData(
            const boost::system::error_code& error,
            size_t bytes_transferred,
            uint64_t chunk_size
        );

    void _handleReadChunkDataLastLine(
            const boost::system::error_code& error,
            const std::string& chunk_last_line
        );

    void _startReadChunkTrailer();

    void _handleReadChunkTrailerLine(
            const boost::system::error_code& error,
            const std::string& chunk_trailer_line
        );

    void _doneReadingBodyChunked();


    void _readBodyContentLength();

    void _handleReadBodyContentLength(
            const boost::system::error_code& error,
            size_t bytes_transferred
        );


    void _readBodyIdent();

    void _handleReadBodyIdent(
            const boost::system::error_code& error,
            size_t bytes_transferred
        );


    void _readLine(
            ReadLineCallback callback
        );

    void _handleReadLine(
            ReadLineCallback callback,
            const boost::system::error_code& error,
            std::size_t bytes_transferred
        );


    void _notifyResponseStatusHeaders(
            http::Status status,
            const Headers& headers,
            BodyPresense expect_body
        );
    
    void _notifyResponseStatusHeadersImpl(
            http::Status status,
            const Headers& headers,
            BodyPresense expect_body
        );

    void _notifyResponseBodyData(
            const std::string& data,
            DataContinuesIndicator data_continues
        );
    
    void _notifyResponseBodyDataImpl(
            const std::string& data,
            DataContinuesIndicator data_continues
        );


    void _newResponse(
            const std::string& message
        );

    void _checkWrite();

    void _handleWrite(
            const boost::system::error_code& error,
            size_t bytes_transferred
        );

};


} } // namespace capena::server

#endif
