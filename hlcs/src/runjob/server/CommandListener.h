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
#ifndef RUNJOB_SERVER_COMMAND_LISTENER_H_
#define RUNJOB_SERVER_COMMAND_LISTENER_H_

#include "server/fwd.h"

#include <utility/include/portConfiguration/Acceptor.h>

#include <boost/asio.hpp>

namespace runjob {
namespace server {

/*!
 * \brief Listens for command connections.
 */
class CommandListener
{
public:
    /*!
     * \brief ctor.
     */
    CommandListener(
            const boost::shared_ptr<Server>& server               //!< [in]
            );

private:
    void acceptHandler(
            const bgq::utility::Acceptor::AcceptArguments& args //!< [in]
            );

private:
    boost::asio::io_service& _io_service;                           //!<
    const Options& _options;                                        //!<
    boost::weak_ptr<Server> _server;                                //!<
    bgq::utility::Acceptor _acceptor;                               //!<
};

} // server
} // runjob

#endif

