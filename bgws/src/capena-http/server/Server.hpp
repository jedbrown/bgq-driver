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

#ifndef CAPENA_SERVER_SERVER_HPP_
#define CAPENA_SERVER_SERVER_HPP_


#include "fwd.hpp"

#include <utility/include/portConfiguration/Acceptor.h>
#include <utility/include/portConfiguration/ServerPortConfiguration.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <set>

#include <stdint.h>


namespace capena {
namespace server {


/** The HTTP server, the application creates an instance of this.
 *
 * Create using the create() function, then call start().
 *
 * Listens for connections on ports as specified in the port config.
 * When gets a connection,
 * creates a Connection object to process requests.
 *
 */
class Server : public boost::enable_shared_from_this<Server>, boost::noncopyable
{
public:

    typedef boost::shared_ptr<Server> Ptr;


    /*! Create a Server. */
    static Ptr create(
            boost::asio::io_service& io_service, //!< [ref]
            const bgq::utility::ServerPortConfiguration& port_config, //!< [copy]
            const ResponderCreatorFn& responder_creator_fn //!< [copy]
        );


    typedef std::vector<boost::asio::ip::tcp::endpoint> Endpoints;

    typedef boost::function<void (const Endpoints& endpoints)> ListeningCallbackFn;

    /*! The callback_fn will be called when the server is listening and can accept connections. */
    void setListeningCallback( ListeningCallbackFn callback_fn );


    /*! Call to start running as a server. */
    void start();

    /*! Call to stop running the server. */
    void stop();


private:

    boost::asio::io_service &_io_service;

    ResponderCreatorFn _responder_creator_fn;

    boost::shared_ptr<bgq::utility::Acceptor> _acceptor_ptr;

    ListeningCallbackFn _listening_fn;


    Server(
            boost::asio::io_service& io_service,
            const bgq::utility::ServerPortConfiguration& port_config,
            const ResponderCreatorFn& responder_creator_fn
        );

    void _acceptorCallback(
            const bgq::utility::Acceptor::AcceptArguments& args
        );

};


} } // namespace capena::server

#endif
