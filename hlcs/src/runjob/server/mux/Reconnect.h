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
#ifndef RUNJOB_SERVER_MUX_RECONNECT_H
#define RUNJOB_SERVER_MUX_RECONNECT_H

#include "server/mux/fwd.h"

#include "server/fwd.h"

#include <db/include/api/cxxdb/cxxdb.h>

#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

namespace runjob {
namespace server {
namespace mux {

/*!
 * \brief
 */
class Reconnect : public boost::enable_shared_from_this<Reconnect>
{
public:
    /*!
     * \brief
     */
    typedef boost::shared_ptr<Reconnect> Ptr;

    /*!
     * \brief
     */
    static void create(
            const boost::shared_ptr<Server>& server,
            const boost::shared_ptr<Connection>& mux
            );

    /*!
     * \brief dtor.
     */
    ~Reconnect();

private:
    Reconnect(
            const boost::shared_ptr<Server>& server,
            const boost::shared_ptr<Connection>& mux
            );

    void nextJob();

    void findJob(
            const boost::shared_ptr<Job>& job
            );

    void setConnection(
            const boost::shared_ptr<Job>& job
            );

    void addClient(
            const boost::shared_ptr<Job>& job
            );

    void updateClient(
            const boost::shared_ptr<Job>& job
            );

private:
    boost::weak_ptr<Server> _server;
    boost::weak_ptr<Connection> _mux;
    const cxxdb::ConnectionPtr _connection;
    cxxdb::QueryStatementPtr _query;
    cxxdb::ResultSetPtr _results;
};

} // mux
} // server
} // runjob

#endif
