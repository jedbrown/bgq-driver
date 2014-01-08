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
#ifndef RUNJOB_MUX_COMMAND_LISTENER_H_
#define RUNJOB_MUX_COMMAND_LISTENER_H_

#include "common/fwd.h"

#include "mux/fwd.h"

#include <utility/include/portConfiguration/Acceptor.h>

#include <boost/asio/io_service.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <boost/weak_ptr.hpp>

namespace runjob {
namespace mux {

/*!
 * \brief Listens for command connections.
 */
class CommandListener : public boost::enable_shared_from_this<CommandListener>
{
public:
    /*!
     * \brief ctor.
     */
    explicit CommandListener(
            boost::shared_ptr<Multiplexer> mux  //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~CommandListener();

    /*!
     * \brief start listening for connections.
     */
    void start();

private:
    /*!
     * \brief Handle a new connection.
     */
    void acceptHandler(
            const bgq::utility::Acceptor::AcceptArguments& args //!< [in]
            );

private:
    boost::asio::io_service& _io_service;       //!<
    const Options& _options;                    //!<
    bgq::utility::Acceptor _acceptor;           //!<
    boost::weak_ptr<Multiplexer> _mux;          //!<
};

} // mux
} // runjob

#endif

