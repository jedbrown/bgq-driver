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


#ifndef CONNECTSENDGETRESPONSELINE_HPP_
#define CONNECTSENDGETRESPONSELINE_HPP_


#include <boost/asio.hpp>

#include <boost/asio/ssl.hpp>

#include <string>


class ConnectSendGetResponseLine
{
public:

    ConnectSendGetResponseLine(
            boost::asio::io_service& io_service,
            const boost::asio::ip::tcp::endpoint& endpoint,
            const std::string& send_str
        );

    void start();

    bool gotReadError() const  { return _got_read_error; }
    const std::string& getStatusLine() const  { return _status_line; }


private:

    boost::asio::io_service &_io_service;
    boost::asio::ssl::context _ssl_context;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> _socket;
    boost::asio::ip::tcp::endpoint _endpoint;
    boost::asio::streambuf _request_sb;
    boost::asio::streambuf _response_sb;

    bool _got_read_error;
    std::string _status_line;


    void _connected( const boost::system::error_code& err );

    void _handshakeComplete( const boost::system::error_code& err );

    void _writeComplete( const boost::system::error_code& err );

    void _readHeadersComplete( const boost::system::error_code& err );

};

#endif
