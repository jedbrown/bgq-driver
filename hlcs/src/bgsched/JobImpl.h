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
 * \file bgsched/JobImpl.h
 * \brief Job::Impl class definition.
 */

#ifndef BGSCHED_JOB_IMPL_H_
#define BGSCHED_JOB_IMPL_H_

#include <bgsched/Job.h>
#include <bgsched/types.h>

#include <db/include/api/cxxdb/fwd.h>

#include <string>

namespace BGQDB {

class DBTJob;
class DBTJob_history;

} // namespace BGQDB

namespace bgsched {

/*!
 * \brief Represents a job.
 *
 */
class Job::Impl
{
public:

    /*!
     * \brief Convert Scheduler API job status to database job status.
     *
     * \return Database job status in single character format converted from Scheduler API job status.
     */
    static char statusToDbChar(
            const Job::Status status  //!< [in] Scheduler API job status
    );

    /*!
     * \brief Convert database job status to Scheduler API job status.
     *
     * \return Scheduler API job status converted from single character database job status.
     */
    static Job::Status statusDbCharToValue(
            const char status_char  //!< [in] Database job status
    );

    /*!
     * \brief Convert database job status to Scheduler API job status.
     *
     * \return Scheduler API job status converted from string database job status.
     */
    static Job::Status convertDBStatusToJobStatus(
            const std::string& status  //!< [in] Database job status
    );

    /*!
     * \brief Convert job shape to string format.
     *
     * \return Job shape as a string.
     */
    static std::string formatShape(
            const uint16_t a,  //!< [in] Size of A dimension
            const uint16_t b,  //!< [in] Size of B dimension
            const uint16_t c,  //!< [in] Size of C dimension
            const uint16_t d,  //!< [in] Size of D dimension
            const uint16_t e   //!< [in] Size of E dimension
    );

    /*!
     * \brief Create a job from the database.
     *
     * \return Pointer to job implementation object.
     */
    static Job::Pimpl createFromDatabase(
            const cxxdb::Columns& job_cols  //!< [in] Database job columns
    );

    /*!
     * \brief Get job Id.
     *
     * \return Job Id.
     */
    Job::Id getId() const;

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
    EnumWrapper<Job::Status> getStatus() const;

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
    uint32_t getRunTime() const;

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

protected:

    /*!
     * \brief Construct a job from the database.
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::OperationFailed - if a database operation failed (i.e query, fetch, etc.)
     * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
     */
    Impl(
            const cxxdb::Columns& job_cols   //!< [in] Database job columns
    );

protected:

    Job::Id           _id;                      //!< Job Id
    std::string       _computeBlockName;        //!< Job compute block
    BlockCreationId   _computeBlockCreationId;  //!< Job compute block creation Id
    Job::Status       _status;                  //!< Job status
    std::string       _user;                    //!< Job user
    std::string       _executable;              //!< Job executable
    std::string       _workingDirectory;        //!< Job working directory
    std::string       _errorText;               //!< Job error text
    std::string       _args;                    //!< Job arguments
    std::string       _envs;                    //!< Job environmentals
    std::string       _startTime;               //!< Job start time
    std::string       _endTime;                 //!< Job end time
    uint32_t          _runTime;                 //!< Job run time
    bool              _inHistory;               //!< Indicator if job info was from history table
    uint32_t          _computeNodesUsed;        //!< Job compute nodes used
    SequenceId        _sequenceId;              //!< Job sequence Id
    int               _exitStatus;              //!< Job exit status
    uint32_t          _ranks;                   //!< Job ranks
    std::string       _schedulerData;           //!< Job scheduler data
    std::string       _corner;                  //!< Job corner (for sub-block job)
    std::string       _shape;                   //!< Job shape (for sub-block job)
    std::string       _mapping;                 //!< Job mapping
    std::string       _clientInfo;              //!< Job client info
    uint32_t          _np;                      //!< Number of job processes

};

} // namespace bgsched

#endif
