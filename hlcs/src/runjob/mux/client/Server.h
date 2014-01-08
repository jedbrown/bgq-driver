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
#ifndef RUNJOB_MUX_CLIENT_SERVER_H_
#define RUNJOB_MUX_CLIENT_SERVER_H_

#include "mux/client/fwd.h"
#include "mux/server/fwd.h"

#include <db/include/api/job/types.h>

#include <boost/system/error_code.hpp>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <stdint.h>

namespace runjob {

class Message;

namespace mux {
namespace client {

/*!
 * \brief Send a message to the runjob_server.
 */
class Server
{
public:
    /*!
     * \brief Callback type for sending a message.
     */
    typedef boost::function<
            void (
                    const boost::system::error_code&
                 )
            > SendHandler;

public:
    /*!
     * \brief ctor.
     */
    Server(
            const boost::shared_ptr<server::Connection>&,   //!< [in]
            const Id& id                                    //!< [in]
          );

    /*!
     * \brief Send a message to the runjob_server.
     */
    void send(
            const boost::shared_ptr<runjob::Message>& message,  //!< [in]
            BGQDB::job::Id job,                                 //!< [in]
            const SendHandler& handler = SendHandler()          //!< [in]
            );

private:
    const boost::shared_ptr<server::Connection> _server;
    const Id& _id;
};

} // client
} // mux
} // runjob

#endif
