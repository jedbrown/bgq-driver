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
#ifndef RUNJOB_COMMON_CONNECTION_H
#define RUNJOB_COMMON_CONNECTION_H

#include <utility/include/portConfiguration/Acceptor.h>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace runjob {

/*!
 * \brief Base class for connections.
 *
 * The _outgoingMessage stream buffer member is used to provide the buffer lifetime
 * gurantee required by boost::asio::async_write. 
 */
class Connection
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Connection> Ptr;

    /*!
     * \brief Weak pointer type.
     */
    typedef boost::weak_ptr<Connection> WeakPtr;

public:
    /*!
     * \brief ctor.
     *
     * \pre socket != NULL
     */
    Connection(
            const bgq::utility::portConfig::SocketPtr& socket   //!< [in]
            );

    /*!
     * \brief Get the file descriptor.
     */
    int getFileDescriptor() const { return _socket->lowest_layer().native(); }

    /*!
     * \brief Get the remote endpoint.
     * 
     * \throws boost::system::system_error
     */
    boost::asio::ip::tcp::endpoint getEndpoint() const { return _socket->next_layer().remote_endpoint(); }

    /*!
     * \brief Start handling requests on this connection.
     */
    void start();

    /*!
     * \brief dtor.
     */
    virtual ~Connection() { }

    /*!
     * \brief Get remote hostname.
     */
    const std::string& hostname() const { return _hostname; }
    
    /*!
     * \brief Get remote hostname (short).
     */
    const std::string& shortHostname() const { return _shortHostname; }

protected:
    /*!
     * \brief Get a shared pointer to this.
     */
    virtual Ptr getShared() = 0;

    /*!
     * \brief start reading header on this connection.
     */
    virtual void readHeader() = 0;

private:
    /*!
     * \brief
     */
    void __attribute__ ((visibility("hidden"))) resolveHandler(
            const boost::system::error_code& error,                           //!< [in]
            const boost::asio::ip::tcp::resolver::iterator& endpoint_iterator //!< [in]
            );

    /*!
     * \brief
     */
    void __attribute__ ((visibility("hidden"))) timerHandler(
            const boost::system::error_code& error                      //!< [in]
            );

    /*!
     * \brief invoked when async_resolve is complete.
     */
    virtual __attribute__ ((visibility("hidden"))) void resolveComplete();

protected:
    const bgq::utility::portConfig::SocketPtr _socket;  //!< Socket object
    boost::asio::ip::tcp::resolver _resolver;           //!< resolver
    std::string _hostname;                              //!< hostname
    std::string _shortHostname;                         //!< short hostname
    boost::asio::streambuf _incomingMessage;            //!< incoming message buffer
    boost::asio::streambuf _outgoingMessage;            //!< outgoing message buffer

private:
    boost::asio::deadline_timer _timer;                 //!< timer to cancel async_resolve
    boost::asio::strand _strand;                        //!< strand for protecting resolving hostname
};

} // runjob

#endif
