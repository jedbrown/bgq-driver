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
#ifndef RUNJOB_SERVER_MUX_LISTENER_H_
#define RUNJOB_SERVER_MUX_LISTENER_H_

#include "server/fwd.h"

#include <utility/include/portConfiguration/Acceptor.h>

#include <boost/asio/io_service.hpp>

#include <boost/weak_ptr.hpp>

namespace runjob {
namespace server {
namespace mux {

/*!
 * \brief Listens for connections from a runjob_mux.
 */
class Listener
{
public:
    /*!
     * \brief ctor.
     */
    Listener(
            const boost::shared_ptr<Server>& server                                  //!< [in]
            );

    void start();
private:
    /*!
     * \brief Handle a new connection.
     */
    void acceptHandler(
            const bgq::utility::Acceptor::AcceptArguments& args //!< [in]
            );

private:
    boost::asio::io_service& _io_service;                           //!<
    const Options& _options;                                        //!<
    const boost::weak_ptr<Server> _server;                          //!<
    bgq::utility::Acceptor _acceptor;                               //!<
};

} // mux
} // server
} // runjob

#endif
