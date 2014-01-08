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
 * \file bgsched/JobFilterImpl.h
 * \brief JobFilter::Impl class definition.
 */

#ifndef BGSCHED_JOB_FILTER_IMPL_H_
#define BGSCHED_JOB_FILTER_IMPL_H_

#include <bgsched/JobFilter.h>

#include <db/include/api/filtering/JobFilter.h>

namespace bgsched {

/*!
 * \brief Represents a job filter.
 */
class JobFilter::Impl
{
public:
    /*!
     * \brief
     */
    Impl();

    /*!
     * \brief Get job type filter.
     *
     * \return Job type to filter on.
     */
    bgsched::JobFilter::JobType::Value getJobType() const;

    /*!
     * \brief Get job Id filter.
     *
     * \return Job Id to filter on.
     */
    bgsched::Job::Id getJobId() const;

    /*!
     * \brief Get scheduler data filter.
     *
     * \return Scheduler data to filter on.
     */
    const std::string& getSchedulerData() const;

    /*!
     * \brief Get compute block name filter.
     *
     * \return Compute block name to filter on.
     */
    const std::string& getComputeBlockName() const;

    /*!
     * \brief Get user name filter.
     *
     * \return User name to filter on.
     */
    const std::string& getUser() const;

    /*!
     * \brief Get executable filter.
     *
     * \return Executable to filter on.
     */
    const std::string& getExecutable() const;

    /*!
     * \brief Get job status filter.
     *
     * \return Job status filter.
     */
    const bgsched::JobFilter::Statuses getStatuses() const;

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
     * \brief Get job start time interval filter.
     *
     * \return Job start time interval to filter on.
     */
    const bgsched::TimeInterval getStartTimeInterval() const;

    /*!
     * \brief Get job end time interval filter.
     *
     * \return Job end time interval to filter on.
     */
    const bgsched::TimeInterval getEndTimeInterval() const;

    /*!
     * \brief Set job type filter.
     */
    void setJobType(
            bgsched::JobFilter::JobType::Value jobType  //!< [in] Job type to filter on
    );

    /*!
     * \brief Set job Id filter.
     */
    void setJobId(
            const bgsched::Job::Id jobId      //!< [in] Job id to filter on
    );

    /*!
     * \brief Set scheduler data filter.
     */
    void setSchedulerData(
            const std::string& schedulerData    //!< [in] Scheduler data to filter on
    );

    /*!
     * \brief Set compute block name filter.
     */
    void setComputeBlockName(
            const std::string& name  //!< [in] Compute block name to filter on
    );

    /*!
     * \brief Set user name filter.
     */
    void setUser(
            const std::string& user  //!< [in] User name to filter on
    );

    /*!
     * \brief Set executable filter.
     */
    void setExecutable(
            const std::string& exec     //!< [in] Executable to filter on
    );

    /*!
     * \brief Set job status filter.
     */
    void setStatuses(
            const bgsched::JobFilter::Statuses* statusesPtr   //!< [in] Pointer to Status set to filter on
    );

    /*!
     * \brief Set job exit status filter.
     */
    void setExitStatus(
            const int exitStatus  //!< [in] Exit status to filter on
    );

    /*!
     * \brief Set job start time interval filter.
     */
    void setStartTimeInterval(
            const bgsched::TimeInterval& timeInterval  //!< [in] Start time interval to filter on
    );

    /*!
     * \brief Set job end time interval filter.
     */
    void setEndTimeInterval(
            const bgsched::TimeInterval& timeInterval  //!< [in] End time interval to filter on
    );

    /*!
     * \brief Convert bgsched::JobFilter to BGQDB::filtering::JobFilter object.
     *
     * \return BGQDB::filtering::JobFilter object.
     */
    BGQDB::filtering::JobFilter convertToDBJobFilter() const;

protected:

    BGQDB::filtering::JobFilter         _jobFilter; //!< Job filter

};

} // namespace bgsched

#endif
