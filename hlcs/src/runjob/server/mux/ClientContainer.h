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
#ifndef RUNJOB_SERVER_MUX_CLIENT_CONTAINER_H
#define RUNJOB_SERVER_MUX_CLIENT_CONTAINER_H

#include "common/message/KillJob.h"

#include "server/fwd.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/weak_ptr.hpp>

#include <map>

namespace runjob {
namespace server {
namespace mux {

/*!
 * \brief A container of clients associated with a single multiplexer.
 *
 * This container holds weak pointers to Job objects since the strong pointers are
 * held in the job::Container object. It is used to deliver signals to jobs that
 * have not started yet (ex if they are still in arbitration) and to kill all jobs
 * associated with the multiplexer when it closes its connection.
 *
 * A single strand protects access to the container.
 */
class ClientContainer : public boost::enable_shared_from_this<ClientContainer>
{
public:
    /*!
     * \brief Callback type.
     */
    typedef boost::function<void(bool)> Callback;

    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<ClientContainer> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static Ptr create(
            boost::asio::io_service& io_service //!< [in]
            );

    /*!
     * \brief
     */
    void add(
            uint64_t client,
            const Callback& callback
            );

    /*!
     * \brief
     */
    void update(
            uint64_t client,
            const boost::shared_ptr<Job>& job,
            const Callback& callback
            );

    /*!
     * \brief
     */
    void kill(
            const boost::shared_ptr<message::KillJob>& message,
            const Callback& callback
            );

    /*!
     * \brief
     */
    void remove(
            uint64_t client
            );

    /*!
     * \brief
     */
    void eof();

private:
    ClientContainer(
            boost::asio::io_service& io_service
            );

    void addImpl(
            uint64_t client,
            const Callback& callback
            );

    void updateImpl(
            uint64_t client,
            const boost::shared_ptr<Job>& job,
            const Callback& callback
            );

    void killImpl(
            const boost::shared_ptr<message::KillJob>& message,
            const Callback& callback
            );

    void removeImpl(
            uint64_t client
            );

    void eofImpl();

    void signalImpl(
            const boost::shared_ptr<Job>& job,
            const boost::shared_ptr<message::KillJob>& message
            );

private:
    typedef std::map< uint64_t, boost::weak_ptr<Job> > Container;

private:
    boost::asio::strand _strand;
    Container _clients;
};

} // mux
} // server
} // runjob

#endif
