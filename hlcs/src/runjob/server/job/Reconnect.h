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
#ifndef RUNJOB_SERVER_JOB_RECONNECT_H
#define RUNJOB_SERVER_JOB_RECONNECT_H

#include "common/error.h"
#include "common/fwd.h"

#include "server/block/fwd.h"

#include "server/job/fwd.h"

#include "server/fwd.h"

#include <db/include/api/cxxdb/cxxdb.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/weak_ptr.hpp>

namespace runjob {
namespace server {
namespace job {

/*!
 *
 * \brief 
 * \note noncopyable because destructor does actual work
 */
class Reconnect : public boost::enable_shared_from_this<Reconnect>, private boost::noncopyable
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

    JobInfo getJobInfo();

    void nextJob();

    void removeJob(
            error_code::rc error,
            const std::string& message
            );

    void findBlockCallback(
            const boost::shared_ptr<block::Compute>& block
            );

    void arbitrateCallback(
            const error_code::rc error,
            const boost::shared_ptr<Job>& job
            );
private:
    boost::weak_ptr<Server> _server;
    const cxxdb::ConnectionPtr _connection;
    cxxdb::ResultSetPtr _results;
};

} // job
} // server 
} // runjob

#endif
