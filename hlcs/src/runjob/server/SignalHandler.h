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
#ifndef RUNJOB_SERVER_SIGNAL_HANDLER_H
#define RUNJOB_SERVER_SIGNAL_HANDLER_H

#include "server/fwd.h"

#include "server/block/Container.h"

#include <utility/include/SignalHandler.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace runjob {
namespace server {

/*!
 * \brief Handle SIGINT and SIGTERM signals.
 */
class SignalHandler : public boost::enable_shared_from_this<SignalHandler>
{
public:
    /*!
     * \brief Type of signals to watch.
     */
    typedef bgq::utility::SignalHandler<SIGINT,SIGTERM> Signals;

    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<SignalHandler> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static Ptr create(
            const boost::shared_ptr<Server>& server //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~SignalHandler();

private:
    SignalHandler(
            const boost::shared_ptr<Server>& server
            );

    void handle(
            const boost::system::error_code& error,
            const siginfo_t& siginfo
            );

    void getIoHandler(
            const block::Container::Blocks& blocks
            );
private:
    Signals _signals;
    boost::weak_ptr<Server> _server;
};

} // server
} // runjob

#endif
