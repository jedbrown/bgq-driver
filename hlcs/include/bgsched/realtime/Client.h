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

#ifndef BGSCHED_REALTIME_CLIENT_H_
#define BGSCHED_REALTIME_CLIENT_H_

/*! \file
 *
 *  \brief Class Client.
 */

#include <bgsched/realtime/ClientConfiguration.h>
#include <bgsched/realtime/Filter.h>

#include <boost/shared_ptr.hpp>

namespace bgsched {
namespace realtime {

class ClientEventListener;

/*!
 *  \brief Real-time client.
 *
 *  Notes on threading:
 *
 *  Most of the methods in this class will acquire a lock on the instance.
 *  Therefore any call to a method will block until previous calls have completed.
 *  This is most interesting for potentially long-running methods
 *  like receiveMessages in blocking mode.
 *  You won't be able to call any other methods on an instance of this class
 *  until receiveMessages has exited.
 *
 *  One exception to this rule is the disconnect() method.
 *  disconnect() can be called while another thread is in a blocking
 *  receiveMessages() or sendMessages().
 *  The thread that's blocking will return from the call.
 *
 */
class Client
{
public:

    /*!
     *  \brief Construct a real-time client.
     *
     *  The Client:
     *  - has a default ClientEventListener
     *  - is blocking
     *  - is disconnected
     *  - has not requested real-time updates
     *  - uses the default Filter
     *
     *  \throws bgsched::realtime::ConfigurationException The configuration is not correct.
     */
    Client(
            const ClientConfiguration& configuration = //!< [in] Configuration options object.
                ClientConfiguration::getDefault()
        );

    /*!
     *  \brief Add a ClientEventListener.
     *
     *  If the Client has a default ClientEventListener, the default ClientEventListener is
     *  replaced with the new ClientEventListener. Otherwise, the ClientEventListener is added to
     *  the list of ClientEventListeners for the client. The ClientEventListener object must exist
     *  until it is removed.
     *
     *  \post The ClientEventListener is in the Client.
     */
    void addListener(
            ClientEventListener& l //!< [in] The new ClientEventListener.
        );

    /*!
     *  \brief Remove a ClientEventListener.
     *
     *  If the ClientEventListener that was removed is the Client's last ClientEventListener, a
     *  default ClientEventListener is added. Otherwise, the listener is removed.
     *
     *  If the ClientEventListener wasn't in the Client, an exception is thrown.
     *
     *  \post The ClientEventListener is not in the Client.
     */
    void removeListener(
            ClientEventListener& l  //!< [in] The ClientEventListener to remove.
        );

    /*!
     *  \brief Set the blocking state.
     *
     *  If the Client was non-blocking and <em>blocking</em> is true and there are any messages
     *  waiting to be sent, the messages are sent.
     */
    void setBlocking(
            bool blocking  //!< [in] New blocking mode (true=blocking, false=non-blocking)
        );

    /*!
     *  \brief Get the blocking state.
     */
    bool isBlocking() const;

    /*!
     *  \brief Connect to the real-time server.
     *
     *  If the Client is already connected, this method does nothing. If the client is in
     *  disconnect_pending state, any old messages are discarded.
     *
     *  After calling this method, the Client is connected.
     *
     *  \post The Client is connected.
     *
     *  \throws bgsched::realtime::ConnectionException Could not connect to the real-time server.
     */
    void connect();

    /*!
     *  \brief Disconnect from the real-time server.
     *
     *  After calling disconnect(), the Client is in disconnect_pending state. The Client will
     *  remain in disconnect_pending state until receiveMessages() returns with <i>end_out</i> set
     *  to true, at which point it will be disconnected.
     *
     *  If the client is already disconnected or disconnect_pending, this method does nothing.
     *
     *  \throws bgsched::realtime::InternalErrorException Failed to disconnect.
     */
    void disconnect();

    /*!
     *  \brief Request real-time updates from the server.
     *
     *  \pre The Client is connected.
     *
     *  If the Client is blocking, the Client attempts to send the request. If this send is
     *  interrupted and <em>interrupted_out</em> is not NULL, then <em>*interrupted_out</em> is set to
     *  true and the application must call sendMessages() to send the request. If
     *  <em>interrupted_out</em> is NULL and the send is interrupted then the Client will
     *  repeatedly attempt to send the request until it's not interrupted.
     *
     *  If the Client is non-blocking, the application must call sendMessages() to send the
     *  request and <em>interrupted_out</em> is ignored.
     *
     *  \throws bgsched::realtime::ClientStateException The client isn't connected.
     *  \throws bgsched::realtime::ConnectionException Lost the connection to the server.
     */
    void requestUpdates(
            bool *interrupted_out //!< [out] Interrupted indicator.
        );

    /*!
     *  \brief Set the server-side event filter.
     *
     *  The application can set the event filter on the Client to tell the server the types of
     *  events it wants to receive.
     *
     *  The Client assigns a filter ID which is returned in <em>filter_id_out</em>.
     *  When the real-time server has received the filter and applied it to the client,
     *  it sends back an acknowledgement with this filter ID.
     *  Filter IDs start at 0.
     *
     *  The Client will only send the filter if the application has
     *  not called requestUpdates.
     *  One way to tell if the client sent the filter is to check if
     *  the assigned filter ID increased after the call to setFilter().
     *  If the client sends the filter and the Client is <em>blocking</em>,
     *  the Client sends the request right away.
     *  If this send is interrupted and <em>interrupted_out</em> is not NULL,
     *  then <em>*interrupted_out</em> is set to true
     *  and the application must call sendMessages() to send the request.
     *  If <em>interrupted_out</em> is NULL and the filter is sent,
     *  setFilter will not return until the request has been sent.
     *
     *  If the Client is non-blocking and setFilter sent the filter,
     *  the application must call sendMessages() to send the
     *  request.
     *
     *  If the application has not called requestUpdates, then the
     *  Client doesn't send the filter.
     *  The filter will be sent when the application calls requestUpdates().
     *
     *  \throws bgsched::realtime::ConnectionException Lost the connection to the server.
     *
     */
    void setFilter(
            const Filter& filter, //!< [in] The Filter.
            Filter::Id* filter_id_out, //!< [out] The assigned filter ID, optional.
            bool* interrupted_out //!< [out] Interrupted indicator, optional.
        );

    /*!
     *  \brief Get a file descriptor that the application can poll on.
     *
     *  When the client is in non-blocking mode, the application can use this file descriptor with
     *  select() or poll() to wait for read- and/or write-readiness.
     *
     *  Note that this file desciptor must not be used for anything other than in a select()-type
     *  API.
     *
     *  \pre The client is connected.
     *
     *  \throws bgsched::realtime::ClientStateException The client isn't connected.
     */
    int getPollDescriptor();

    /*!
     *  \brief Send messages to the server.
     *
     *  If the Client is non-blocking and one of the sending methods (requestUpdates() or
     *  setFilter()) has been called or if one of the sending methods was interrupted, the
     *  application must call this method to send the request to the server.
     *
     *  If there are no messages queued to be sent then this method does nothing.
     *
     *  If the Client is blocking and <em>interrupted_out</em> is NULL then this method will block
     *  until the messages have been written. If <em>interrupted_out</em> is not NULL then this
     *  method will return with <em>*interrupted_out</em> set to true if the write is interrupted
     *  by a signal. The <em>again_out</em> parameter is ignored.
     *
     *  If the Client is non-blocking, then if this method returns with <em>*again_out</em> true,
     *  the application should wait until the poll descriptor is write ready and then call this
     *  method again. If <em>interrupted_out</em> is not NULL and the write call is interrupted by
     *  a signal, this method will return with <em>*interrupted_out</em> set to true. Otherwise if
     *  <em>interrupted_out</em> is NULL then the write call will be tried again.
     *
     *  If this method returns with <em>*interrupted_out</em> true then the messages have not been
     *  fully sent and the method should be called again.
     *
     *  \throws bgsched::realtime::ClientStateException The client isn't connected.
     *  \throws bgsched::realtime::ConnectionException Lost the connection to the server.
     *
     */
    void sendMessages(
            bool* interrupted_out, //!< [out] Interrupted indicator.
            bool* again_out //!< [out] true iff the Client is non-blocking and the I/O operation would block.
        );

    /*!
     *  \brief Receive messages from the server.
     *
     *  If the Client is blocking and <em>interrupted_out</em> is NULL then this method will block
     *  until a message is processed and one of the listeners'
     *  ClientEventListener::getRealtimeContinue() method return false or the server closes the
     *  client's connection. If <em>interrupted_out</em> is not NULL then this method will also
     *  return if the read call is interrupted by a signal.
     *
     *  If the Client is non-blocking, then this method will also return if the read() call
     *  indicates that there are no messages waiting to be processed. In this case,
     *  <em>*again_out</em> is set to true and the application should wait until the poll
     *  descriptor indicates the descriptor is read ready.
     *
     *  If the server closes the client's connection then <em>*end_out</em> is set to
     *  true and the client is disconnected.
     *
     *  If the read call is interrupted by a signal then <em>interrupted_out</em> is set to true.
     *
     *  \throws bgsched::realtime::ClientStateException The client isn't connected.
     *  \throws bgsched::realtime::ConnectionException Lost the connection to the server.
     *  \throws bgsched::realtime::ProtocolException Received an invalid message from the server.
     *
     */
    void receiveMessages(
            bool* interrupted_out, //!< [out] Interrupted indicator.
            bool* again_out, //!< [out] true iff the Client is non-blocking and the I/O operation would block.
            bool* end_out    //!< [out] true iff the server has disconnected the client.
        );

    /*!
     *  \brief Destructor.
     */
    ~Client();


    class Impl; //!< Internal implementation class.
    typedef boost::shared_ptr<Impl> ImplPtr; //!< Shared pointer to implementation class.


private:

    ImplPtr _impl_ptr; //!< Internal implementation.

};


} // namespace bgsched::realtime
} // namespace bgsched

#endif
