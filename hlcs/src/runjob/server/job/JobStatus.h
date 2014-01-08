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
#ifndef RUNJOB_SERVER_JOB_JOB_STATUS_H
#define RUNJOB_SERVER_JOB_JOB_STATUS_H

#include "server/fwd.h"

#include <hlcs/include/runjob/commands/error.h>

#include <boost/shared_ptr.hpp>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief Query information about an active job.
 */
class JobStatus
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<JobStatus> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static void create(
            const boost::shared_ptr<Job>& job,                         //!< [in]
            const boost::shared_ptr<CommandConnection>& connection     //!< [in]
            );

private:
    JobStatus(
            const boost::shared_ptr<Job>& job,
            const boost::shared_ptr<CommandConnection>& connection
          );

    void impl();

private:
    boost::shared_ptr<Job> _job;
    boost::shared_ptr<CommandConnection> _connection;
};

} // job
} // server
} // runjob

#endif
