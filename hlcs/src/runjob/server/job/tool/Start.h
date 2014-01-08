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
#ifndef RUNJOB_SERVER_JOB_START_TOOL_H
#define RUNJOB_SERVER_JOB_START_TOOL_H

#include "common/tool/Daemon.h"
#include "common/tool/Proctable.h"

#include "server/job/Tool.h"

#include "server/fwd.h"

#include <db/include/api/cxxdb/fwd.h>

#include <hlcs/include/runjob/commands/error.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <sstream>

namespace runjob {
namespace server {
namespace job {
namespace tool {

/*!
 * \brief
 */
class Start: public boost::enable_shared_from_this<Start>
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Start> Ptr;

    /*!
     * \brief
     */
    static const unsigned MaximumToolsPerJob;

public:
    /*!
     * \brief Factory.
     */
    static void create(
            const boost::shared_ptr<Job>& job,  //!< [in]
            const runjob::tool::Daemon& tool,   //!< [in]
            runjob::tool::Proctable& proctable, //!< [in]
            const Tool::Callback& callback      //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Start();

private:
    Start(
            const boost::shared_ptr<Job>& job,
            const runjob::tool::Daemon& tool,
            runjob::tool::Proctable& proctable,
            const Tool::Callback& callback
         );

    void query(
            const boost::shared_ptr<Job>& job,
            const cxxdb::ConnectionPtr& connection,
            const boost::shared_ptr<cxxdb::Transaction>& transaction
            );

    void calculateId(
            const boost::shared_ptr<Job>& job,
            const cxxdb::ConnectionPtr& connection,
            const boost::shared_ptr<cxxdb::Transaction>& transaction
            );
    
    void insert(
            const boost::shared_ptr<Job>& job,
            const cxxdb::ConnectionPtr& connection,
            const boost::shared_ptr<cxxdb::Transaction>& transaction
            );

    void validate(
            const cxxdb::ConnectionPtr& connection,
            const boost::shared_ptr<cxxdb::Transaction>& transaction
            );

    void impl();

private:
    boost::weak_ptr<Job> _job;
    runjob::tool::Daemon _tool;
    runjob::tool::Proctable& _proctable;
    unsigned _id;
    runjob::commands::error::rc _status;
    std::ostringstream _message;
    Tool::Callback _callback;
};

} // tool
} // job
} // server 
} // runjob

#endif
