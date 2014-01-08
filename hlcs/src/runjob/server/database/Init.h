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
#ifndef RUNJOB_SERVER_DATABASE_INIT_H
#define RUNJOB_SERVER_DATABASE_INIT_H

#include "server/database/fwd.h"

#include "common/error.h"

#include <db/include/api/job/Operations.h>

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>

namespace runjob {
namespace server {

class Options;
class Server;

namespace database {

/*!
 * \brief Abstraction around SQL to insert, update, and remove entries from the bgqjob and bgqjob_history tables.
 *
 * This class uses three prepared statements to store and maintain job information in the bgqjob table
 *
 * - insert
 * - update
 * - delete
 *
 * Each statement uses a single connection from the database connection pool. The insert statement is not
 * proteced by a strand because its only invoked from inside another strand in the job container. The update
 * and delete statemnts have their own strands because they can be invoked from multiple Boost.Asio handlers 
 * from multiple jobs concurrently.
 *
 * \see Delete
 * \see Insert
 * \see NodeStatus
 * \see Update
 * \see BGQDB::init
 */
class Init
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Init> Ptr;

    /*!
     * \brief Factory.
     *
     * Initializes the BGQDB api.
     */
    static Ptr create(
            const boost::shared_ptr<Server>& server    //!< [in]
            );
public:
    /*!
     * \brief dtor.
     */
    ~Init();

    Insert& getInsert() { return *_insert; } //!< Get the Insert object.
    Update& getUpdate() { return *_update; } //!< Get the Update object.
    Delete& getDelete() { return *_delete; } //!< Get the Delete object.

private:
    Init(
            const boost::shared_ptr<Server>& server
        );

private:
    boost::shared_ptr<BGQDB::job::Operations> _operations;
    boost::scoped_ptr<Insert> _insert;
    boost::scoped_ptr<Update> _update;
    boost::scoped_ptr<Delete> _delete;
};

} // database
} // server
} // runjob

#endif
