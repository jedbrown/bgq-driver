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
#ifndef RUNJOB_MUX_CLIENT_MESSAGE_QUEUE_H
#define RUNJOB_MUX_CLIENT_MESSAGE_QUEUE_H

#include "mux/client/fwd.h"
#include "mux/client/Id.h"

#include "common/message/Header.h"

#include "mux/fwd.h"

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <deque>

namespace runjob {

class Message;

namespace mux {
namespace client {

/*!
 * \brief Queue of outgoing messages to a %runjob client.
 */
class MessageQueue
{
public:
    /*!
     * \brief Message pointer type.
     */
    typedef boost::shared_ptr<runjob::mux::client::Message> MessagePtr;

public:
    /*!
     * \brief ctor.
     */
    MessageQueue(
            const Options& options, //!< [in]
            const Id& id            //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~MessageQueue();

    /*!
     * \brief Get the maximum queue size.
     */
    size_t getMaximumSize() const { return _highWaterMark; }

    /*!
     * \brief Get the current queue size.
     */
    size_t getCurrentSize() const { return _size; }

    /*!
     * \brief Get the dropped message count.
     */
    size_t getDropped() const { return _dropped; }

    /*!
     * \brief Add a message.
     *
     * \returns true if the message was added to the queue
     */
    bool push_back(
            const boost::shared_ptr<runjob::Message>& message  //!< [in]
            );

    /*!
     * \brief Remove the first message.
     */
    void pop_front();

    /*!
     * \brief Get the first message.
     */
    const MessagePtr& front() const {
        return _container[0];
    }

    /*!
     * \brief Check if the queue is empty.
     */
    bool empty() const { return _container.empty(); }

private:
    typedef std::deque<MessagePtr> Container;

private:
    const Id _id;               //!<
    size_t _size;               //!<
    size_t _highWaterMark;      //!<
    size_t _dropped;            //!<
    Container _container;       //!<
};

} // client
} // mux
} // runjob

#endif
