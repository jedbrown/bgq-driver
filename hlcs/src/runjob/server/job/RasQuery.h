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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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
#ifndef RUNJOB_SERVER_JOB_RAS_QUERY_H
#define RUNJOB_SERVER_JOB_RAS_QUERY_H

#include "common/message/ExitJob.h"

#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/job/types.h>

#include "server/job/ExitStatus.h"
#include "server/fwd.h"

#include <map>
#include <string>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief
 */
class RasQuery
{
public:
    typedef std::map<std::string, unsigned> SeverityCount;

public:
    /*!
     * \brief ctor.
     */
    RasQuery(
            const BGQDB::job::Id job,   //!< [in]
            const ExitStatus& status    //!< [in]
            );

    /*!
     * \brief
     */
    void add(
            const message::ExitJob::Ptr& message //!< [in]
            ) const;

private:
    void count(
            const cxxdb::ConnectionPtr& connection,
            const BGQDB::job::Id job
            );

    void message(
            const cxxdb::ConnectionPtr& connection,
            const BGQDB::job::Id job,
            const ExitStatus& status
            );
private:
    SeverityCount _count;
    std::string _message;
    std::string _severity;
};

} // job
} // server
} // runjob

#endif
