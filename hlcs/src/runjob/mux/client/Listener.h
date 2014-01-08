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
#ifndef RUNJOB_MUX_CLIENT_LISTENER_H
#define RUNJOB_MUX_CLIENT_LISTENER_H

#include "mux/client/fwd.h"
#include "mux/client/Id.h"

#include "mux/server/fwd.h"

#include "mux/fwd.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/weak_ptr.hpp>

namespace runjob {
namespace mux {
namespace client {

/*!
 * \brief Listens for connections from %runjob clients.
 */
class Listener : public boost::enable_shared_from_this<Listener>
{
public:
    /*!
     * \brief ctor.
     */
    Listener(
            const boost::shared_ptr<Multiplexer>& mux  //!< [in]
            );

    /*!
     * \brief dtor
     */
    ~Listener();

    /*!
     * \brief Start listening.
     */
    void start();

private:
    /*!
     * \brief
     */
    void accept();

    /*!
     * \brief
     */
    void acceptHandler(
            const boost::system::error_code& error,            //!< [in]
            const boost::shared_ptr<Runjob>& client            //!< [in]
            );

    /*!
     * \brief
     */
    int getBacklog() const;

private:
    const Options& _options;                                            //!<
    boost::weak_ptr<Multiplexer> _mux;                                  //!<
    boost::asio::local::stream_protocol::acceptor _acceptor;            //!<
    client::Id::value_type _id_counter;                                 //!<
};

} // client
} // mux
} // runjob

#endif

