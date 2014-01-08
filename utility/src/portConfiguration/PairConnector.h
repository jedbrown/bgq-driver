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

#ifndef BGQ_UTILITY_PAIR_CONNECTOR_H
#define BGQ_UTILITY_PAIR_CONNECTOR_H


#include "portConfiguration/Connector.h"
#include "portConfiguration/PortConfiguration.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <sstream>


namespace bgq {
namespace utility {


/*!
 * \brief Asynchronous host resolving and connection for clients.
 */
class PairConnector : public boost::enable_shared_from_this<PairConnector>
{
public:

    typedef boost::shared_ptr<PairConnector> Ptr;


    typedef boost::asio::ip::tcp::socket Socket;

    typedef boost::function<void ( Connector::Error::Type error, const std::string& error_str )> ConnectHandler;


    /*!
     * \brief ctor.
     *
     * invoke the start method after constructing to initiate the operation.
     */
    PairConnector(
            boost::asio::io_service& io_service,                //!< [ref]
            const PortConfiguration::Pairs& pairs,              //!< [copy] name/value pairs
            Socket& socket                                      //!< [ref] socket to connect
        );

    /*!
     * \brief start an asynchronous resolve and connect
     */
    void start(
            ConnectHandler connect_handler                      //!< [copy] callback to invoke when complete
        );


private:

    PortConfiguration::Pairs _pairs;
    boost::asio::ip::tcp::resolver _resolver;
    Socket &_socket;
    ConnectHandler _connect_handler;
    std::ostringstream _error_stream;


    /*!
     * \brief handler invoked from async_resolve
     */
    void _resolveHandler(
            const boost::system::error_code& error,                       //!< [in]
            boost::asio::ip::tcp::resolver::iterator resolver_iterator,  //!< [in]
            PortConfiguration::Pairs::const_iterator pair_iterator  //!< [in]
        );

    /*!
     * \brief handler invoked from async_connect
     */
    void _connectHandler(
            const boost::system::error_code& error,                       //!< [in]
            boost::asio::ip::tcp::resolver::iterator resolver_iterator,  //!< [in]
            PortConfiguration::Pairs::const_iterator pair_iterator  //!< [in]
        );

};

} // utility
} // bgq

#endif
