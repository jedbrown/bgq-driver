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
#ifndef RUNJOB_SERVER_JOB_STDIO_QUEUE_H
#define RUNJOB_SERVER_JOB_STDIO_QUEUE_H

#include "common/message/StdIo.h"

#include "server/mux/fwd.h"

#include <boost/function.hpp>
#include <boost/weak_ptr.hpp>

#include <vector>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief Queue of stdio messages.
 *
 * This object is used when stdio arrives from some nodes before all nodes have indicated the job can start.
 */
class StdioQueue
{
public:
    /*!
     * \brief Callback type
     */
    typedef boost::function<void()> Callback;

public:
    /*!
     * \brief ctor.
     */
    StdioQueue(
            const boost::weak_ptr<mux::Connection>& mux    //!< [in]
            );

    /*!
     * \brief add a message to the queue
     */
    void add(
            const message::StdIo::Ptr& message, //!< [in]
            const Callback& callback            //!< [in]
            );

    /*!
     * \brief drain the queue.
     */
    void drain();

    /*!
     * \brief
     */
    void clientDisconnected();

    /*!
     * \brief
     */
    bool isClientDisconnected() const { return _clientDisconnected; }

private:
    friend class mux::Reconnect;
    bool _drained;                              //!<
    bool _clientDisconnected;                   //!<
    boost::weak_ptr<mux::Connection> _mux;      //!< connection to multiplexer.
    std::vector<message::StdIo::Ptr> _queue;    //!< queue of stdio
};

} // job
} // server
} // runjob

#endif
