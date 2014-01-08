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
#ifndef RUNJOB_SERVER_HANDLERS_LOCATE_RANK_ACTIVE_JOB_H
#define RUNJOB_SERVER_HANDLERS_LOCATE_RANK_ACTIVE_JOB_H

#include "server/handlers/locate_rank/Job.h"

#include <db/include/api/job/types.h>

#include <db/include/api/cxxdb/fwd.h>

namespace runjob {
namespace server {
namespace handlers {
namespace locate_rank {

/*!
 * \brief Query a job in the bgqjob table.
 */
class ActiveJob : public Job
{
public:
    /*!
     * \brief ctor.
     */
    ActiveJob(
            const cxxdb::ConnectionPtr& connection, //!< [in]
            BGQDB::job::Id job                      //!< [in]
            );
};

} // locate_rank
} // handlers
} // server
} // runjob

#endif
