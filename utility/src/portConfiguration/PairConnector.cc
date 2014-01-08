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

#include "PairConnector.h"

#include <Log.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>


using std::string;

using boost::asio::ip::tcp;


LOG_DECLARE_FILE("utility");


namespace bgq {
namespace utility {


PairConnector::PairConnector(
        boost::asio::io_service& io_service,                //!< [ref]
        const PortConfiguration::Pairs& pairs,              //!< [copy] name/value pairs
        Socket& socket                                      //!< [ref] socket to connect
    ) :
        _pairs(pairs),
        _resolver( io_service ),
        _socket(socket)
{
    // nothing to do
}


void
PairConnector::start(
        ConnectHandler connect_handler                      //!< [copy] callback to invoke when complete
    )
{
    _connect_handler = connect_handler;

    // get first host:port pair
    PortConfiguration::Pairs::const_iterator pair(_pairs.begin());

    // start resolving endpoint
    LOG_DEBUG_MSG("resolving " << pair->first << ":" << pair->second);
    _resolver.async_resolve(
            tcp::resolver::query( pair->first, pair->second ),
            boost::bind(
                &PairConnector::_resolveHandler,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::iterator,
                pair
                )
            );
}

void
PairConnector::_resolveHandler(
        const boost::system::error_code& error,                       //!< [in]
        boost::asio::ip::tcp::resolver::iterator resolver_iterator,  //!< [in]
        PortConfiguration::Pairs::const_iterator pair_iterator  //!< [in]
    )
{
    LOG_TRACE_MSG("resolve handler");
    if ( !error ) {
        // resolve successful, start connecting
        tcp::endpoint ep = *resolver_iterator;
        LOG_DEBUG_MSG("resolved " << ep);
        _socket.async_connect(
                ep,
                boost::bind(
                    &PairConnector::_connectHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    ++resolver_iterator,
                    pair_iterator
                    )
                );
    } else {
        // couldn't resolve
        LOG_DEBUG_MSG( "could not resolve: " << error.message() );

        // add to error strings
        if ( ! _error_stream.str().empty() ) {
            _error_stream << ",";
        }
        _error_stream << "[" << pair_iterator->first << "]:" << pair_iterator->second << ":resolve[" << error.message() << "]";

        if ( resolver_iterator != tcp::resolver::iterator() ) {
            // try next endpoint in list
            tcp::endpoint ep = *resolver_iterator;
            _socket.async_connect(
                    ep,
                    boost::bind(
                        &PairConnector::_connectHandler,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        ++resolver_iterator,
                        pair_iterator
                        )
                    );
        } else if ( ++pair_iterator != _pairs.end() ) {
            // try next pair LOG_DEBUG_MSG("resolving " << pair_iterator->first << ":" << pair_iterator->second);
            _resolver.async_resolve(
                    tcp::resolver::query( pair_iterator->first, pair_iterator->second ),
                    boost::bind(
                        &PairConnector::_resolveHandler,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::iterator,
                        pair_iterator
                        )
                    );
        } else {
            // done, invoke callback

            Connector::Error::Type error(Connector::Error::ResolveError);
            string error_str(_error_stream.str());

            _connect_handler( error, error_str );
        }
    }
}


void
PairConnector::_connectHandler(
        const boost::system::error_code& error,                       //!< [in]
        boost::asio::ip::tcp::resolver::iterator resolver_iterator,  //!< [in]
        PortConfiguration::Pairs::const_iterator pair_iterator  //!< [in]
    )
{
    LOG_TRACE_MSG("connect handler");
    if ( !error ) {
        // Connected. Invoke the callback with success.

        Connector::Error::Type error(Connector::Error::Success);
        string error_str(_error_stream.str());

        _connect_handler( error, error_str );
        return;
    }

    // connection failed, close socket
    LOG_DEBUG_MSG( "could not connect: " << error.message() );
    _socket.close();

    // add to error strings
    if ( ! _error_stream.str().empty() ) {
        _error_stream << ",";
    }
    _error_stream << "[" << pair_iterator->first << "]:" << pair_iterator->second << ":[" << error << "]";

    if ( resolver_iterator != tcp::resolver::iterator() ) {
        // try next endpoint in list
        tcp::endpoint ep = *resolver_iterator;
        _socket.async_connect(
                ep,
                boost::bind(
                    &PairConnector::_connectHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    ++resolver_iterator,
                    pair_iterator
                    )
                );
    } else if ( ++pair_iterator != _pairs.end() ) {
        // resolve next pair
        LOG_DEBUG_MSG("resolving " << pair_iterator->first << ":" << pair_iterator->second);
        _resolver.async_resolve(
                tcp::resolver::query( pair_iterator->first, pair_iterator->second ),
                boost::bind(
                    &PairConnector::_resolveHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::iterator,
                    pair_iterator
                    )
                );
    } else {
        // nothing else to resolve, invoke callback

        Connector::Error::Type error(Connector::Error::ConnectError);
        string error_str(_error_stream.str());

        _connect_handler( error, error_str );
    }
}


} // utility
} // bgq


