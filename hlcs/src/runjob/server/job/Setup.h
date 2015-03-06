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
#ifndef RUNJOB_SERVER_JOB_SETUP_H
#define RUNJOB_SERVER_JOB_SETUP_H

#include "common/Uci.h"

#include "server/cios/fwd.h"

#include "server/job/class_route/Generate.h"

#include "server/job/fwd.h"

#include "server/fwd.h"

#include <db/include/api/cxxdb/fwd.h>
#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <map>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief Setup a job through the control and I/O services (cios) daemons.
 *
 * \see Job
 */
class Setup : public boost::enable_shared_from_this<Setup>
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Setup> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static void create(
            const boost::shared_ptr<Job>& job  //!< [in]
            );

private:
    Setup(
            const boost::shared_ptr<Job>& job
         );

    void populate(
            const Uci& location,
            IoNode& node
            );

    void validateMapping();

    void updateDatabaseMapping(
            const std::string& mapping
            ) const;

    void updateModificationTime(
            const std::string& mapping,
            const cxxdb::ConnectionPtr& connection
            ) const;

private:
    const boost::shared_ptr<Job>             _job;
    boost::scoped_ptr<class_route::Generate> _classRoute;
    std::string                              _copiedMapping;
    bool                                     _retainMappingFiles;

};

} // job
} // server
} // runjob

#endif
