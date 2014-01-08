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
#ifndef BGSCHED_REALTIME_CLIENT_IMPL_H_
#define BGSCHED_REALTIME_CLIENT_IMPL_H_

#include <bgsched/realtime/Client.h>

#include <bgsched/realtime/ClientEventListener.h>

#include "AbstractMessage.h"
#include "FilterImpl.h"
#include "FinalConfiguration.h"

#include <utility/include/portConfiguration/types.h>

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>

#include <deque>
#include <set>
#include <string>

class ClientConfiguration;

namespace bgsched {
namespace realtime {

class Client::Impl
{
public:

    Impl( const ClientConfiguration& client_configuration );

    void addListener( ClientEventListener& l );
    void removeListener( ClientEventListener& l );

    void setBlocking( bool blocking );
    bool isBlocking() const;

    void connect();
    void disconnect();

    void requestUpdates( bool* interrupted_out );

    void setFilter(
        const Filter::Impl& filter,
        Filter::Id* filter_id_out,
        bool* interrupted_out
      );

    int getPollDescriptor();

    void sendMessages(
            bool* interrupted_out,
            bool* again_out
        );

    void receiveMessages(
            bool* interrupted_out,
            bool* again_out,
            bool* end_out
        );

    ~Impl();

private:

    typedef std::set<ClientEventListener*> _Listeners;
    typedef boost::lock_guard<boost::mutex> _LockGuard;
    typedef std::deque<AbstractMessage::ConstPtr> _OutMsgs;
    typedef bgq::utility::portConfig::SocketPtr _SocketPtr;


    ClientEventListener _default_client_event_listener;

    FinalConfiguration _config;

    mutable boost::mutex _mtx;

    _Listeners _listener_ps;

    boost::asio::io_service _io_service;

    bool _blocking;

    bool _receiving_events;

    Filter::Impl _filter; // !< The client filter.
    Filter::Id _filter_id; //!< The current filter ID to be assigned.

    _SocketPtr _socket_ptr;
    bool _connected;
    boost::system::error_code _error;

    _OutMsgs _out_msgs; //!< Messages waiting to be sent.

    std::string _write_buf;
    bool _async_write_in_progress; //!< Will be true if there's an async write already in progress.

    boost::asio::streambuf _in_sb; //!< Input stream.
    bool _async_read_in_progress; //!< Will be true if there's an async read already in progress.


    /*!
     *  \brief Write to the socket.
     *
     *  Writes to the socket from _write_buf starting at _write_buf_i.
     *  If _write_buf is completely written, clears _write_buf and resets _write_buf_i.
     *  Otherwise, _write_buf_i is set to point at the next stuff to write.
     *  If didn't write complete buffer because EINTR, sets *interrupted_out to true.
     *  If didn't write complete buffer because EAGAIN, sets *interrupted_out to true.
     */
    void _write(
            bool *interrupted_out, //!< [out] Set to true if got EINTR
            bool *again_out, //!< [out] Set to true if got EAGAIN
            bool *end_out //!< [out] Set to true if the server disconnected or connection closed.
        );

    void _handleMessage(
            const std::string& msg_str,
            bool *continue_reading
        );

    void _putOutMsgsToWriteBuf();

    void _writeComplete( const boost::system::error_code &error );

    void _readComplete( const boost::system::error_code &error );

    void _disconnect();

    void _callOnEachListener(
            boost::function<void (ClientEventListener&)> fn,
            bool* continue_reading_out
        );
};


} // namespace bgsched::realtime
} // namespace bgsched

#endif
