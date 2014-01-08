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
#ifndef RUNJOB_SERVER_BLOCK_RECONNECT_H
#define RUNJOB_SERVER_BLOCK_RECONNECT_H

#include "common/error.h"

#include "server/block/fwd.h"

#include "server/fwd.h"

#include <db/include/api/cxxdb/cxxdb.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>
#include <string>

namespace runjob {
namespace server {
namespace block {

/*!
 * \brief Reconnect to initialized blocks by querying the database.
 */
class Reconnect : public boost::enable_shared_from_this<Reconnect>
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Reconnect> Ptr;

    /*!
     * \brief Factory.
     */
    static void create(
            const boost::shared_ptr<Server>& server
            );

    /*!
     * \brief dtor.
     */
    ~Reconnect();

private:
    Reconnect(
            const boost::shared_ptr<Server>& server
            );

    void nextBlock();

    void createCallback(
            error_code::rc error,
            const std::string& message
            );

    void findCallback(
            const boost::shared_ptr<block::Compute>& block
            );
    
    void initializedCallback(
            error_code::rc error,
            const std::string& message
            );
    
    void softwareFailures(
            const boost::shared_ptr<block::Compute>& block,
            const cxxdb::ResultSetPtr& results
            );

private:
    boost::weak_ptr<Server> _server;
    const cxxdb::ConnectionPtr _connection;
    cxxdb::ResultSetPtr _results;
    int64_t _sequence;
};

} // block
} // server
} // runjob

#endif
