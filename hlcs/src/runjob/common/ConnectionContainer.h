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
#ifndef RUNJOB_COMMON_CONNECTION_CONTAINER_H
#define RUNJOB_COMMON_CONNECTION_CONTAINER_H

#include "common/Connection.h"

#include <utility/include/portConfiguration/Acceptor.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

#include <map>

namespace runjob {

/*!
 * \brief Container for connections.
 */
class ConnectionContainer : public boost::enable_shared_from_this<ConnectionContainer>
{
public:
    /*!
     * \brief Container type.
     *
     * File descriptor is the key.
     * Connection::WeakPtr is the value.
     */
    typedef std::map<int, Connection::WeakPtr> Container;

    /*!
     * \brief Callback type for getting the container elements.
     */
    typedef boost::function<
            void (
                    const Container& connections
                 )
            > GetHandler;
public:
    /*!
     * \brief ctor.
     */
    ConnectionContainer(
            boost::asio::io_service& io_service //!< [in]
            );

    /*!
     * \brief Add to the container.
     *
     * \throws std::invalid_argument if the connection is already in the container.
     */
    void add(
            const Connection::Ptr& connection  //!< [in]
            );

    /*!
     * \brief Remove from the container.
     */
    void remove(
            const bgq::utility::portConfig::SocketPtr& socket        //!< Socket object
            );

    /*!
     * \brief Get a copy of the container.
     */
    void get(
            const GetHandler& handler     //!< [in]
            );

private:
    void __attribute__ ((visibility("hidden"))) addImpl(
            const Connection::Ptr& connection
            );

    void __attribute__ ((visibility("hidden"))) removeImpl(
            const bgq::utility::portConfig::SocketPtr& socket
            );

    void __attribute__ ((visibility("hidden"))) getImpl(
            const GetHandler& handler
            );

private:
    boost::asio::strand _strand;    //!<
    Container _connections;         //!<
};

} // runjob

#endif

