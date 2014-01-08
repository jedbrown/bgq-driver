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

#ifndef MMCS_LITE_CONNECTION_H
#define MMCS_LITE_CONNECTION_H

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/MessageHeader.h>
#include <ramdisk/include/services/StdioMessages.h>

namespace mmcs {
namespace lite {

/*!
 * \brief Connection to an I/O node daemon.
 */
class Connection
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Connection> Ptr;

public:
    /*!
     * \brief ctor.
     */
    explicit Connection(
            boost::asio::io_service& io_service         //!< [in] asio service
            ) :
        _socket( io_service ),
        _endpoint(),
        _header(),
        _buffer(),
        _connected(false)
    {
        // zero header
        memset( &_header, 0, sizeof(_header) );
    }

    /*!
     * \brief start connecting.
     */
    template <typename Callback>
    void start(
            const boost::asio::ip::tcp::endpoint& ep,   //!< [in] endpoint to connect to
            Callback callback                           //!< [in] callback handler
            )
    {
        // remember endpoint
        _endpoint = ep;

        // start connect
        _socket.async_connect(
                _endpoint,
                callback
                );
    }

    /*!
     * \brief Stop all asynchronous operations.
     */
    void stop() { _socket.cancel(); }

    /*!
     * \brief write a message.
     */
    template <typename Callback>
    void write(
            const void* buf,    //!< [in] buffer to write
            size_t size,        //!< [in] size of the buffer
            Callback callback   //!< [in] callback handler
            )
    {
        boost::asio::async_write(
                _socket,
                boost::asio::buffer(
                    buf,
                    size
                    ),
                callback
                );
    }

    /*!
     * \brief read a header.
     *
     * use getHeader to obtain the header after the callback is invoked.
     */
    template <typename Callback>
    void readHeader(
            Callback callback   //!< [in] callback handler
            )
    {
        boost::asio::async_read(
                _socket,
                boost::asio::buffer(
                    &_header,
                    sizeof(_header)
                    ),
                callback
                );
    }

    /*!
     * \brief read a message.
     *
     * use getBuffer to get the message after the callback is invoked.
     */
    template <typename Callback>
    void readMessage(
            Callback callback   //!< [in] callback handler
            )
    {
        // create buffer based on header
        if ( _header.service == bgcios::JobctlService ) {
            switch ( _header.type ) {
                case bgcios::jobctl::ErrorAck:
                    _buffer.reset( new bgcios::jobctl::ErrorAckMessage );
                    break;
                case bgcios::jobctl::SetupJobAck:
                    _buffer.reset( new bgcios::jobctl::SetupJobAckMessage );
                    break;
                case bgcios::jobctl::ExitProcess:
                    _buffer.reset( new bgcios::jobctl::ExitProcessMessage );
                    break;
                case bgcios::jobctl::LoadJobAck:
                    _buffer.reset( new bgcios::jobctl::LoadJobAckMessage );
                    break;
                case bgcios::jobctl::SignalJobAck:
                    _buffer.reset( new bgcios::jobctl::SignalJobAckMessage );
                    break;
                case bgcios::jobctl::StartJobAck:
                    _buffer.reset( new bgcios::jobctl::StartJobAckMessage );
                    break;
                case bgcios::jobctl::ExitJob:
                    _buffer.reset( new bgcios::jobctl::ExitJobMessage );
                    break;
                case bgcios::jobctl::CleanupJobAck:
                    _buffer.reset( new bgcios::jobctl::CleanupJobAckMessage );
                    break;
                case bgcios::jobctl::AuthenticateAck:
                    _buffer.reset( new bgcios::jobctl::AuthenticateAckMessage );
                    break;
                default:
                    BOOST_ASSERT(!"unhandled message type");
            }
        } else if ( _header.service == bgcios::StdioService ) {
            switch ( _header.type ) {
                case bgcios::stdio::WriteStdout:
                    _buffer.reset( new bgcios::stdio::WriteStdioMessage );
                    break;
                case bgcios::stdio::WriteStderr:
                    _buffer.reset( new bgcios::stdio::WriteStdioMessage );
                    break;
                case bgcios::stdio::StartJobAck:
                    _buffer.reset( new bgcios::stdio::StartJobAckMessage );
                    break;
                case bgcios::stdio::CloseStdio:
                    _buffer.reset( new bgcios::stdio::CloseStdioMessage );
                    break;
                case bgcios::stdio::CloseStdioAck:
                    _buffer.reset( new bgcios::stdio::CloseStdioAckMessage );
                    break;
                case bgcios::stdio::ReadStdin:
                    _buffer.reset( new bgcios::stdio::ReadStdinMessage );
                    break;
                case bgcios::stdio::AuthenticateAck:
                    _buffer.reset( new bgcios::stdio::AuthenticateAckMessage );
                    break;
                default:
                    BOOST_ASSERT(!"unhandled message type");
            }
        } else {
            BOOST_ASSERT( !"unhandled service type" );
        }

        // copy header into buffer
        memcpy( _buffer.get(), &_header, sizeof(_header) );

        // create mutable buffer for asio that is offset
        // since we've already read the header
        boost::asio::mutable_buffer buffer =
            boost::asio::mutable_buffer(
                    _buffer.get(),
                    _header.length
                    ) + sizeof(_header);

        // start the read
        boost::asio::async_read(
                _socket,
                boost::asio::buffer( buffer ),
                callback
                );
    }

    /*!
     * \brief Get header.
     */
    const bgcios::MessageHeader& getHeader() const { return _header; }

    /*!
     * \brief Get connected flag.
     */
    bool getConnected() const { return _connected; }

    /*!
     * \brief Set connected flag.
     */
    void setConnected( bool c = true ) { _connected = c; }

    /*!
     * \brief Get message.
     */
    template <typename T>
    boost::shared_ptr<const T> getBuffer() const { return boost::static_pointer_cast<T>(_buffer); }

    /*!
     * \brief Get remote endpoint.
     */
    const boost::asio::ip::tcp::endpoint& remote_endpoint() const { return _endpoint; }

private:
    boost::asio::ip::tcp::socket _socket;           //!< socket
    boost::asio::ip::tcp::endpoint _endpoint;       //!< remote endpoint
    bgcios::MessageHeader _header;                  //!< incoming header
    boost::shared_ptr<void> _buffer;                //!< incoming message buffer
    bool _connected;                                //!< flag if we have connected
};

} } // namespace mmcs::lite

#endif
