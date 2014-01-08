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
#ifndef RUNJOB_MUX_MULTIPLEXER_H
#define RUNJOB_MUX_MULTIPLEXER_H

#include "common/fwd.h"

#include "mux/client/fwd.h"

#include "mux/performance/fwd.h"

#include "mux/server/fwd.h"

#include "mux/fwd.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

namespace runjob {
namespace mux {

/*!
 * \brief Objects used throughout runjob_mux.
 */
class Multiplexer : public boost::enable_shared_from_this<Multiplexer>
{
public:
    /*!
     * \brief Pointer type
     */
    typedef boost::shared_ptr<Multiplexer> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static Ptr create(
            const Options& options                 //!< [in]
            );

    /*!
     * \brief Start the Multiplexer.
     */
    int start();

    /*!
     * \brief dtor.
     */
    ~Multiplexer();

public:
    const Options& getOptions() const { return _options; }    //!< Get program options.
    boost::asio::io_service& getIoService() { return _io_service; } //!< Get I/O service.
    const boost::shared_ptr<Plugin>& getPlugin() const { return _plugin; }  //!< Get plugin handle.
    const boost::shared_ptr<client::Container>& getClientContainer() const { return _clientContainer; } //!< Get client container.
    const boost::shared_ptr<ConnectionContainer>& getConnections() const { return _connections; } //!< Get connection container.
    const boost::shared_ptr<server::Connection>& getServer() const { return _server; } //!< Get server connection.
    const boost::shared_ptr<performance::Counters>& getCounters() const { return _counters; } //!< Get performance counters.
    const boost::shared_ptr<CommandListener>& getCommandListener() const { return _commandListener; } //!< Get command listener.
    const boost::shared_ptr<client::Listener>& getRunjobListener() const { return _runjobListener; } //!< Get runjob listener.
    const boost::shared_ptr<TerminateHandler>& getTerminateHandler() const { return _signalHandler; } //!< Get terminate handler.

private:
    Multiplexer(
            const Options& options
            );

private:
    const Options& _options;
    boost::asio::io_service _io_service;
    boost::shared_ptr<Plugin> _plugin;
    const boost::shared_ptr<client::Container> _clientContainer;
    const boost::shared_ptr<ConnectionContainer> _connections;
    const boost::shared_ptr<server::Connection> _server;
    const boost::shared_ptr<performance::Counters> _counters;
    boost::shared_ptr<CommandListener> _commandListener;
    boost::shared_ptr<client::Listener> _runjobListener;
    const boost::shared_ptr<TerminateHandler> _signalHandler;
};

} // mux
} // runjob

#endif
