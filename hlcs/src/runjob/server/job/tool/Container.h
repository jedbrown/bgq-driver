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
#ifndef RUNJOB_SERVER_JOB_TOOL_CONTAINER_H
#define RUNJOB_SERVER_JOB_TOOL_CONTAINER_H

#include "common/tool/Daemon.h"
#include "common/tool/Proctable.h"

#include "common/Uci.h"

#include "server/job/fwd.h"
#include "server/job/Tool.h"

#include "server/fwd.h"

#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/shared_ptr.hpp>

#include <map>

namespace runjob {
namespace server {
namespace job {
namespace tool {

/*!
 * \brief
 */
class Container
{
public:
    /*!
     * \brief
     */
    typedef std::map<int, Tool::Ptr> Tools;

public:
    /*!
     * \brief
     */
    void add(
            const boost::shared_ptr<Job>& job,          //!< [in]
            const runjob::tool::Daemon& tool,           //!< [in]
            runjob::tool::Proctable& proctable,         //!< [in]
            unsigned id,                                //!< [in]
            const Tool::Callback& callback              //!< [in]
            );

    /*!
     * \brief
     */
    void remove(
            unsigned id
            );

    /*!
     * \brief
     */
    void started(
            unsigned id,
            const Uci& location
            );
    
    /*!
     * \brief
     */
    void exited(
            const boost::shared_ptr<bgcios::jobctl::ExitToolMessage>& etm,  //!< [in]
            const Uci& location                                             //!< [in]
            );

    /*!
     * \brief
     */
    void failure(
            const boost::shared_ptr<bgcios::jobctl::StartToolAckMessage>& ack,  //!< [in]
            const Uci& location                                                 //!< [in]
            );

    /*!
     * \brief
     */
    const Tools& get() const { return _tools; }

    /*!
     * \brief
     */
    const boost::shared_ptr<RankMapping> ranks() const { return _ranks; }

public:
    /*!
     * \brief ctor.
     */
    Container();

    /*!
     * \brief dtor.
     */
    ~Container();

private:
    Tools _tools;
    boost::shared_ptr<RankMapping> _ranks;
};

} // tool
} // job
} // server
} // runjob

#endif
