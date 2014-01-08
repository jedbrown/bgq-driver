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
#ifndef RUNJOB_SERVER_JOB_TRANSITION_H
#define RUNJOB_SERVER_JOB_TRANSITION_H

#include "server/job/IoNode.h"
#include "server/job/Status.h"

#include "common/fwd.h"

#include "server/fwd.h"

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief Validate runjob::server::job::Status transitions.
 */
class Transition
{
public:
    /*!
     * \brief ctor.
     */
    Transition(
            const boost::shared_ptr<Job>& job   //!< [in]
            );

    /*!
     * \brief Test for a transition.
     */
    std::size_t next(
            Status::Value current,          //!< [in] our current runjob::server::Job::Status value
            IoNode::Map& io                 //!< [in] all I/O nodes in the job
            ) const;

    /*!
     * \brief send a Result message to the multiplexer.
     */
    void run() const;

    /*!
     * \brief Send end messages to the I/O nodes.
     */
    void end() const;

    /*!
     * \brief
     */
    void loaded() const;

private:
    const boost::shared_ptr<Job> _job;
};

} // job
} // server
} // runjob

#endif
