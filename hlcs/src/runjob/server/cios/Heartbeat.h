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
#ifndef RUNJOB_SERVER_CIOS_HEARTBEAT_H
#define RUNJOB_SERVER_CIOS_HEARTBEAT_H

#include "server/block/Container.h"
#include "server/fwd.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

namespace runjob {
namespace server {
namespace cios {

class Heartbeat : public boost::enable_shared_from_this<Heartbeat>
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Heartbeat> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static Ptr create(
            const boost::shared_ptr<Server>& server        //!< [in]
            );

    /*!
     * \brief Dtor.
     */
    ~Heartbeat();

private:
    Heartbeat(
            const boost::shared_ptr<Server>& server
            );

    unsigned getInterval(
            const boost::shared_ptr<Server>& server
            ) const;

    void start();

    void impl(
            const boost::system::error_code& error,
            const unsigned interval
            );

    void blocksHandler(
            const block::Container::Blocks& blocks
            );

private:
    boost::weak_ptr<Server> _server;
    boost::asio::deadline_timer _timer;
};

} // cios
} // server
} // runjob 

#endif


