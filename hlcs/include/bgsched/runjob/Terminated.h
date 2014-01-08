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

/*!
 * \file bgsched/runjob/Terminated.h
 * \brief definition and implementation of bgsched::runjob::Terminated class.
 */

#ifndef BGSCHED_RUNJOB_TERMINATED_H
#define BGSCHED_RUNJOB_TERMINATED_H

#include <bgsched/runjob/Coordinates.h>
#include <bgsched/runjob/Node.h>

#include <bgsched/Job.h>

#include <string>
#include <vector>

#include <sys/types.h>

namespace bgsched {
namespace runjob {

/*!
 * \brief Data used to notify when a job has terminated.
 *
 * \section Overview
 *
 * This object is passed to a job scheduler from a runjob_mux when a job
 * has terminated. It provides the following additional information that
 * may be useful:
 *
 *  - exit status
 *  - kill timeout
 *
 *  If a job terminates due to a kill timeout, every node used by the job is unavailable
 *  for future jobs until the block in use has been rebooted. 
 *
 *  The exit status is encoded and can be queried with the usual WIFEXITED, EXITSTATUS,
 *  WIFSIGNALED, and WTERMSIG macros to gather more information. The message parameter
 *  will offer additional information about why the job terminated.
 */
class Terminated
{
public:
    /*!
     * \brief Node container.
     */
    typedef std::vector<Node> Nodes;

public:
    pid_t pid() const;
    Job::Id job() const;
    int status() const;

    /*!
     * \brief Check if delivering a KILL signal timed out.
     *
     * \ingroup V1R2
     *
     * When delivering a KILL signal to a job times out, the kill_timeout() method
     * will return true. For sub-block jobs, this changes the status of every node
     * participating in the job to a Software Failure status. For regular jobs on
     * blocks a midplane and larger, the midplanes in use by the job will change 
     * to a Software Failure status. For regular jobs on blocks smaller than a 
     * midplane, all of the nodes participating in the job will change to a Software
     * Failure status. In every scenario, freeing the encompassing compute block
     * will change the node or midplane's status back to Available.
     */
    bool kill_timeout() const;

    /*!
     * \deprecated This method has been deprecated since V1R1M0
     * \ingroup V1R2
     */
    const Nodes& software_error_nodes() const;
    const std::string& message() const;

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

    /*!
     * \brief Ctor.
     */
    explicit Terminated(
            const Pimpl& impl      //!< [in] Pointer to implementation
            );

private:
    Pimpl _impl;
};

} // runjob
} // bgsched

#endif
