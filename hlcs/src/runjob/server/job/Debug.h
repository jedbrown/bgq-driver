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
#ifndef RUNJOB_SERVER_JOB_DEBUG_H
#define RUNJOB_SERVER_JOB_DEBUG_H

#include "common/message/Proctable.h"
#include "common/message/StartTool.h"

#include "server/mux/fwd.h"
#include "server/fwd.h"

#include <hlcs/include/runjob/commands/error.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <string>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief
 *
 * \see Job
 */
class Debug : public boost::enable_shared_from_this<Debug>
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Debug> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static void create(
            const boost::shared_ptr<Job>& job,                      //!< [in]
            const runjob::message::StartTool::Ptr& request,         //!< [in]
            const boost::shared_ptr<mux::Connection>& mux           //!< [in]
            );

private:
    Debug(
            const boost::shared_ptr<Job>& job,
            const runjob::message::StartTool::Ptr& request,
            const boost::shared_ptr<mux::Connection>& mux
         );

    void impl();

    void callback(
            unsigned id,
            runjob::commands::error::rc error,
            const std::string& message
            );

    void addIo(
            const boost::shared_ptr<Job>& job
            );

private:
    const boost::weak_ptr<Job> _job;
    const runjob::message::StartTool::Ptr _request;
    const runjob::message::Proctable::Ptr _response;
    const boost::weak_ptr<mux::Connection> _mux;
};

} // job
} // server
} // runjob

#endif
