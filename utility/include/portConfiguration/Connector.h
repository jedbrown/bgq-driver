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


#ifndef BGQ_UTILITY_CONNECTOR_H_
#define BGQ_UTILITY_CONNECTOR_H_

/*! \file
 *
 *  \brief Class for connecting to configured ports.
 */


#include "ClientPortConfiguration.h"
#include "SslConfiguration.h"
#include "types.h"

#include <boost/asio.hpp>
#include <boost/function.hpp>

#include <stdexcept>
#include <string>


namespace bgq {
namespace utility {


/*! \brief
 *

This class provides methods to attempt to connect
to the configured ports, given a ClientPortConfiguration.

Example pseudo-code:

<pre>

ClientPortConfiguration port_config( MyDefaultPort );

...

port_config.notifyComplete(); // throws exceptions.

boost::asio::io_service io_service;

Connector connector(
    io_service,
    port_config
  );

portConfig::SocketPtr socket_ptr(connector.connect()); // throws.

// read to, write from *socket_ptr.

</pre>

 *
 */
class Connector
{
public:


    /*! \brief %Error thrown when connect fails.
     */
    class ConnectError : public std::runtime_error {
    public:
        /*! \brief Constructor.
         */
        ConnectError( const std::string& what_str )  : std::runtime_error( what_str ) { /* Nothing to do */ }
    };


    /*! \brief %Error thrown when connect fails and couldn't resolve any of the hosts.
     */
    class ResolveError : public ConnectError
    {
    public:
        /*! \brief Constructor.
         */
        ResolveError( const std::string& what_str ) : ConnectError( what_str ) { /* Nothing to do */ }
    };


    class HandshakeError : public ConnectError
    {
    public:
        HandshakeError( const std::string& what_str ) : ConnectError( what_str )  { /* Nothing to do */ }
    };


    /*!
     * \brief
     */
    struct Error
    {
        enum Type {
            Success=0,     //!< Successfully connected.
            ResolveError,  //!< Failed to resolve any hostname/service name pairs.
            ConnectError,  //!< Failed to connect to any resolved hostname/service name pairs.
            HandshakeError //!< Failed to handshake.
        };
    };


    struct ConnectResult
    {
        portConfig::SocketPtr socket_ptr;
        Error::Type error;
        std::string error_str;
        std::string server_cn;

        ConnectResult();

        ConnectResult(
                portConfig::SocketPtr& socket_ptr,
                Error::Type error,
                const std::string& error_str,
                std::string server_cn
            );
    };


    /*!
     * \brief Callback type for asynchronous connect operations.
     */
    typedef boost::function<
        void (
                const ConnectResult& result
                )
        > ConnectHandler;


    /*! \brief constructor.
     *
     */
    Connector(
            boost::asio::io_service& io_service, //!< [held]
            const ClientPortConfiguration& port_configuration //!< [copied]
        );


    /*! \brief Connect to the server.
     *
     *  The client goes through all of the configured host-port pairs in order and attempts to connect to them.
     *
     *  The first one that successfully connects is returned as the socket.
     *
     *  \throws ResolveError Couldn't resolve any of the host-port pairs.
     *  \throws ConnectError Couldn't connect to any of the host-port pairs.
     *
     *  \note This method invokes io_service::run and io_service::reset on the io_service passed in to the
     *  constructor.
     */
    portConfig::SocketPtr connect();

    /*! \brief Connect to the server.
     *
     *  The client goes through all of the configured host-port pairs in order and attempts to connect to them.
     *
     *  The first one that successfully connects is returned as the socket.
     */
    portConfig::SocketPtr connect(
            Error::Type& error_out
            );

    /*!
     * \brief Connect to the server asynchronously.
     */
    void async_connect(
            ConnectHandler connect_handler  //!< [in] The function to invoke when complete.
            );


private:

    boost::asio::io_service &_io_service;
    ClientPortConfiguration _port_config;
    SslConfiguration::ContextPtr _context_ptr;

    void _connectHandler(
            portConfig::SocketPtr socket_ptr,
            ConnectHandler connect_handler,
            Error::Type err,
            const std::string& error_str
            );

    void _handleHandshake(
            portConfig::SocketPtr socket_ptr,
            ConnectHandler connect_handler,
            const std::string& message,
            const std::string& server_cn
            );
};


} // namespace bgq::utility
} // namespace bgq


#endif
