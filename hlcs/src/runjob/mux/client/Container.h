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
#ifndef RUNJOB_MUX_CLIENT_CONTAINER_H
#define RUNJOB_MUX_CLIENT_CONTAINER_H

#include "mux/client/fwd.h"
#include "mux/client/Id.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

namespace runjob {
namespace mux {
namespace client {

/*!
 * \brief contains multiple %runjob %clients.
 */
class Container : public boost::enable_shared_from_this<Container>
{
public:
    /*!
     * \brief Container type.
     */
    typedef boost::unordered_map< Id::value_type, boost::weak_ptr<Runjob> > Map;

    /*!
     * \brief Callback type for finding a specific client in the container.
     */
    typedef boost::function< void(const boost::shared_ptr<Runjob>&)> FindHandler;

    /*!
     * \brief Callback type for adding a client to the container.
     */
    typedef boost::function<void()> AddHandler;

    /*!
     * \brief Callback type for getting the container elements.
     */
    typedef boost::function<void(const Map& connections)> GetHandler;

public:
    /*!
     * \brief ctor.
     */
    Container(
            boost::asio::io_service& io_service //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Container();

    /*!
     * \brief Add a client to the container.
     */
    void add(
            const boost::shared_ptr<Runjob>& client,    //!< [in]
            const AddHandler& handler                   //!< [in]
            );

    /*!
     * \brief Remove a client from the container.
     */
    void remove(
            const Id& id            //!< [in]
            );

    /*!
     * \brief Find a client in the container.
     */
    void find(
            const Id& id,               //!< [in]
            const FindHandler& handler  //!< [in]
            );

    /*!
     * \brief Get a reference to the container.
     */
    void get(
            const GetHandler& handler  //!< [in]
            );

private:
    void addImpl(
            const boost::shared_ptr<Runjob>& client,
            const AddHandler& handler
            );

    void removeImpl(
            const Id& id
            );

    void findImpl(
            const Id& id,
            const FindHandler& handler
            );

    void getImpl(
            const GetHandler& handler
            );

private:
    boost::asio::strand _strand;    //!<
    Map _clients;                   //!<
};

} // client
} // mux
} // runjob

#endif
