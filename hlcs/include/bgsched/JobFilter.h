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
 * \file bgsched/JobFilter.h
 * \brief Definition of JobFilter class.
 */

#ifndef BGSCHED_JOB_FILTER_H_
#define BGSCHED_JOB_FILTER_H_

#include <bgsched/Job.h>
#include <bgsched/TimeInterval.h>

#include <boost/shared_ptr.hpp>

#include <set>
#include <string>

namespace bgsched {

/*!
 * \brief Represents a job filter.
 */
class JobFilter
{
public:
    /*!
     * \brief Container of Job::Status values.
     */
    typedef std::set<Job::Status> Statuses;

    /*!
     * \brief Job filter type.
     */
    struct JobType {
        enum Value {
            Active = 0, //!< Active jobs
            Completed,  //!< Completed jobs
            All         //!< All active and completed jobs
        };
    };

    /*!
     * \brief
     */
    JobFilter();

    /*!
     * \brief Set job type filter.
     */
    void setJobType(
            JobType::Value jobType  //!< [in] Job type to filter on
    );

    /*!
     * \brief Get job type filter.
     *
     * \return Job type to filter on.
     */
    JobType::Value getJobType() const;

    /*!
     * \brief Set job Id filter.
     */
    void setJobId(
            const Job::Id jobId      //!< [in] Job id to filter on
            );

    /*!
     * \brief Get job Id filter.
     *
     * \return Job Id to filter on.
     */
    Job::Id getJobId() const;

    /*!
     * \brief Set scheduler data filter.
     */
    void setSchedulerData(
            const std::string& schedulerData    //!< [in] Scheduler data to filter on
            );

    /*!
     * \brief Get scheduler data filter.
     *
     * \return Scheduler data to filter on.
     */
    const std::string& getSchedulerData() const;

    /*!
     * \brief Set compute block name filter.
     */
    void setComputeBlockName(
            const std::string& name  //!< [in] Compute block name to filter on
            );

    /*!
     * \brief Get compute block name filter.
     *
     * \return Compute block name to filter on.
     */
    const std::string& getComputeBlockName() const;

    /*!
     * \brief Set user name filter.
     */
    void setUser(
            const std::string& user  //!< [in] User name to filter on
            );

    /*!
     * \brief Get user name filter.
     *
     * \return User name to filter on.
     */
    const std::string& getUser() const;

    /*!
     * \brief Set executable filter.
     */
    void setExecutable(
            const std::string& exec     //!< [in] Executable to filter on
            );

    /*!
     * \brief Get executable filter.
     *
     * \return Executable to filter on.
     */
    const std::string& getExecutable() const;

    /*!
     * \brief Set job status filter.
     *
     * Note: Depending on the job type (Active, Completed or All) some status filter settings may be ignored when using
     * the core::getjobs() API. For example, when searching for "Active" jobs any of the statuses for completed jobs are
     * ignored. When searching for "Completed" jobs any of the statuses for active jobs are ignored. When searching for
     * "All" jobs both the active job table and the job history table are queried separately and the results combined.
     * In this situation the statuses that are valid for the table type are applied and the other non-applicable statuses
     * are ignored.
     *
     * Valid status values for active jobs are: Setup, Loading, Starting, Running and Cleanup.
     * Completed jobs in history table will have status of Terminated or Error.
     */
    void setStatuses(
            const Statuses* statusesPtr     //!< [in] Pointer to Statuses set to filter on
            );

    /*!
     * \brief Get job status filter.
     *
     * \return Job status filter.
     */
    const Statuses getStatuses() const;

    /*!
     * \brief Set job exit status filter.
     *
     * Note: This filter only applies for completed jobs and is ignored when searching the active job table. When searching
     * for "All" jobs both the active job table and the job history table are queried and the results combined. In this
     * situation the filter is applied when querying the job history table but ignored for the active job table query.
     */
    void setExitStatus(
            const int exitStatus  //!< [in] Exit status to filter on
    );

    /*!
     * \brief Returns true if the job exit status filter has been set using setExitStatus().
     *
     * \return True if the job exit status filter has been set using setExitStatus().
     */
    bool isExitStatusSet() const;

    /*!
     * \brief Get job exit status filter.
     *
     * Note: This filter value is ignored unless isExitStatusSet() is also true.
     *
     * \return Job exit status to filter on.
     */
    int getExitStatus() const;

    /*!
     * \brief Set job start time interval filter.
     */
    void setStartTimeInterval(
            const bgsched::TimeInterval& timeInterval  //!< [in] Start time interval to filter on
    );

    /*!
     * \brief Get job start time interval filter.
     *
     * \return Job start time interval to filter on.
     */
    const bgsched::TimeInterval getStartTimeInterval() const;

    /*!
     * \brief Set job end time interval filter.
     *
     * Note: This filter only applies for completed jobs and is ignored when searching the active job table. When searching
     * for "All" jobs both the active job table and the job history table are queried and the results combined. In this
     * situation the filter is applied when querying the job history table but ignored for the active job table query.
     */
    void setEndTimeInterval(
            const bgsched::TimeInterval& timeInterval  //!< [in] End time interval to filter on
    );

    /*!
     * \brief Get job end time interval filter.
     *
     * \return Job end time interval to filter on.
     */
    const bgsched::TimeInterval getEndTimeInterval() const;

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

    /*!
     * \brief Get pointer to implementation.
     *
     * \return Pointer to implementation.
     */
    Pimpl getPimpl() const;

protected:

    Pimpl _impl;

};

} // namespace bgsched

#endif
