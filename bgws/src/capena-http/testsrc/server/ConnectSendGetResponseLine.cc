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

#include <boost/bind.hpp>

#include <iostream>

#include <assert.h>


using std::cout;


ConnectSendGetResponseLine::ConnectSendGetResponseLine(
        boost::asio::io_service& io_service,
        const boost::asio::ip::tcp::endpoint& endpoint,
        const std::string& send_str
    ) :
        _io_service(io_service),
        _ssl_context( _io_service, boost::asio::ssl::context::tlsv1_client ),
        _socket( io_service, _ssl_context ),
        _endpoint(endpoint),
        _got_read_error(false)
{
    std::ostream request_stream( &_request_sb );
    request_stream << send_str;
}


void ConnectSendGetResponseLine::start()
{
    _socket.lowest_layer().async_connect(
            _endpoint,
            boost::bind( &ConnectSendGetResponseLine::_connected, this, boost::asio::placeholders::error )
        );
}


void ConnectSendGetResponseLine::_connected( const boost::system::error_code& err )
{
    assert( ! err );

    cout << "ConnectSendGetResponseLine: Connected!\n";

    _socket.async_handshake(
            boost::asio::ssl::stream_base::client,
            boost::bind( &ConnectSendGetResponseLine::_handshakeComplete, this, boost::asio::placeholders::error )
        );
}


void ConnectSendGetResponseLine::_handshakeComplete( const boost::system::error_code& err )
{
    assert( ! err );

    cout << "ConnectSendGetResponseLine: handshake worked!\n";

    boost::asio::async_write(
            _socket,
            _request_sb,
            boost::bind( &ConnectSendGetResponseLine::_writeComplete, this, boost::asio::placeholders::error )
        );

    boost::asio::async_read_until(
            _socket,
            _response_sb,
            "\r\n\r\n",
            boost::bind( &ConnectSendGetResponseLine::_readHeadersComplete, this, boost::asio::placeholders::error )
        );
}


void ConnectSendGetResponseLine::_writeComplete( const boost::system::error_code& err )
{
    assert( ! err );

    cout << "ConnectSendGetResponseLine: write complete!\n";

    _socket.lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_send );
}


void ConnectSendGetResponseLine::_readHeadersComplete( const boost::system::error_code& err )
{
    if ( err ) {
        cout << "ConnectSendGetResponseLine: read error err=" << err << "\n";
        _got_read_error = true;
        return;
    }

    std::istream response_stream( &_response_sb );
    std::getline( response_stream, _status_line );

    cout << "ConnectSendGetResponseLine: read headers, status line='" << _status_line << "'\n";
}
