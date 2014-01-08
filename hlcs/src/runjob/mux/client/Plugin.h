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
#ifndef RUNJOB_MUX_CLIENT_PLUGIN_H_
#define RUNJOB_MUX_CLIENT_PLUGIN_H_

#include "common/message/ExitJob.h"

#include "common/fwd.h"

#include "mux/client/fwd.h"

#include "mux/Plugin.h"

#include <db/include/api/job/types.h>

#include <hlcs/include/bgsched/runjob/Verify.h>

namespace runjob {
namespace message {

class Result;

}
namespace mux {
namespace client {

/*!
 * \brief Interact with a scheduler plugin.
 */
class Plugin
{
public:
    /*!
     * \brief ctor.
     */
    Plugin(
            const Id& id,                                       //!< [in]
            Timers& timers,                                     //!< [in]
            const runjob::mux::Plugin::WeakPtr& plugin          //!< [in]
          );

    /*!
     * \brief Verify that this job can start.
     */
    void verify(
            pid_t pid,                                          //!< [in]
            JobInfo& info,                                      //!< [in]
            const boost::shared_ptr<message::Result>& result    //!< [in]
            );

    /*!
     * \brief Notify that this job started.
     */
    void started(
            pid_t pid,      //!< [in]
            BGQDB::job::Id  //!< [in]
            );

    /*!
     * \brief Notify that this job failed to start.
     */
    void startError(
            pid_t pid,                  //!< [in]
            error_code::rc error,       //!< [in]
            const std::string& message  //!< [in]
            );

    /*!
     * \brief Notify that this job terminated.
     */
    void terminated(
            pid_t pid,                              //!< [in]
            BGQDB::job::Id id,                      //!< [in]
            const message::ExitJob::ConstPtr& msg   //!< [in]
            );

private:
    void updateResourceInfo(
            JobInfo& job,
            const bgsched::runjob::Verify& data
            );

private:
    const Id& _id;                                          //!<
    Timers& _timers;                                        //!<
    const runjob::mux::Plugin::WeakPtr _plugin;             //!<
};

} // client
} // mux
} // runjob

#endif
