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
 * \file bgsched/Job.h
 * \brief Job class definition.
 */

#ifndef BGSCHED_JOB_H_
#define BGSCHED_JOB_H_

#include <bgsched/EnumWrapper.h>
#include <bgsched/types.h>

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

namespace bgsched {

/*!
 * \brief Represents a job.
 *
 * \image html job_state.png
 */
class Job
{
public:

    typedef boost::shared_ptr<Job> Ptr;                  //!< Pointer type.
    typedef boost::shared_ptr<const Job> ConstPtr;       //!< Const pointer type.

    typedef std::vector<Ptr> Ptrs;                        //!< Collection of pointers.
    typedef std::vector<ConstPtr> ConstPtrs;              //!< Collection of const pointers.

    /*!
     * \brief Job status.
     */
    enum Status {
        Setup = 0,   //!< Job is setting up.
        Loading,     //!< Job is loading.
        Starting,    //!< Job is starting.
        Running,     //!< Job is running.
        Debug,       //!< Job is being debugged.
        Cleanup,     //!< Job is cleaning up.
        Terminated,  //!< Job is terminated.
        Error        //!< Job is in error status.
    };

    /*!
     * \brief Id type.
     */
    typedef uint64_t Id;

    /*!
     * \brief Get job Id.
     *
     * \return Job Id.
     */
    Id getId() const;

    /*!
     * \brief Get job's compute block name (Id).
     *
     * \return Job's compute block name (Id).
     */
    const std::string& getComputeBlockName() const;

    /*!
     * \brief Get job's compute block creation Id (for jobs in history table only).
     *
     * \return Job's compute block creation Id.
     */
    BlockCreationId getComputeBlockCreationId() const;

    /*!
     * \brief Get job status.
     *
     * \return Job status.
     */
    EnumWrapper<Status> getStatus() const;

    /*!
     * \brief Get job user name.
     *
     * \return Job user name.
     */
    const std::string& getUser() const;

    /*!
     * \brief Get job executable.
     *
     * \return Job executable.
     */
    const std::string& getExecutable() const;

    /*!
     * \brief Get job working directory.
     *
     * \return Job working directory.
     */
    const std::string& getWorkingDirectory() const;

     /*!
     * \brief Get job error text.
     *
     * \return Job error text.
     */
    const std::string& getErrorText() const;

    /*!
     * \brief Get job arguments.
     *
     * \return Job arguments.
     */
    const std::string& getArgs() const;

    /*!
     * \brief Get job environment variables.
     *
     * \return Job environmental variables.
     */
    const std::string& getEnvs() const;

    /*!
     * \brief Get job start time.
     *
     * \return Job start time.
     */
    const std::string& getStartTime() const;

    /*!
     * \brief Get job end time.
     *
     * \return Job end time or empty string for jobs that have not ended.
     */
    const std::string& getEndTime() const;

    /*!
     * \brief Get job run time (in seconds).
     *
     * \return Job run time (in seconds).
     */
    uint64_t getRunTime() const;

    /*!
     * \brief Indication if job info came from history table (in history means job ended).
     *
     * \return true if job was in history table.
     */
    bool isInHistory() const;

    /*!
     * \brief Get job compute nodes used.
     *
     * \return Job compute nodes used.
     */
    uint32_t getComputeNodesUsed() const;

    /*!
     * \brief Get job sequence ID.
     *
     * \return Job sequence ID.
     */
    SequenceId getSequenceId() const;

    /*!
     * \brief Get job exit status.
     *
     * \return Job exit status.
     */
    int getExitStatus() const;

    /*!
     * \brief Get corner location for sub-block job.
     *
     * \return Corner location for sub-block job.
     */
    const std::string& getCorner() const;

    /*!
     * \brief Get shape for sub-block job.
     *
     * \return Shape for sub-block job.
     */
    const std::string& getShape() const;

    /*!
     * \brief Get job ranks per compute node (1, 2, 4, 8, 16, 32, or 64)
     *
     * \return Job ranks per compute node.
     */
    uint32_t getRanksPerNode() const;

    /*!
     * \brief Get number of processes for job.
     *
     * \return Number of processes for job.
     */
    uint32_t getNP() const;

    /*!
     * \brief Get job scheduler data.
     *
     * \return Job scheduler data.
     */
    const std::string& getSchedulerData() const;

    /*!
     * \brief Get job mapping.
     *
     * \return Job mapping.
     */
    const std::string& getMapping() const;

    /*!
     * \brief Get job client info (host and process id).
     *
     * \return Job client info (host and process id).
     */
    const std::string& getClientInfo() const;

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

    /*!
     * \brief
     */
    explicit Job(
            Pimpl impl  //!< [in] Pointer to implementation
    );

protected:

    Pimpl _impl;

};

} // namespace bgsched

#endif
