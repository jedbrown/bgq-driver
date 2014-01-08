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

#include <bgsched/InternalException.h>

#include "bgsched/JobFilterImpl.h"
#include "bgsched/utility.h"

#include <db/include/api/filtering/JobFilter.h>

#include <utility/include/Log.h>

using namespace std;

LOG_DECLARE_FILE("bgsched");

namespace bgsched {

JobFilter::Impl::Impl() :
    _jobFilter()
{
    /* Nothing to do */
}

JobFilter::JobType::Value
JobFilter::Impl::getJobType() const
{
   BGQDB::filtering::JobFilter::JobType::Value jobType = _jobFilter.getJobType();

   // Map from db filtering job type to bgsched job type
   switch (jobType) {
       case BGQDB::filtering::JobFilter::JobType::Active:
           return JobFilter::JobType::Active;
       case BGQDB::filtering::JobFilter::JobType::Completed:
           return JobFilter::JobType::Completed;
       case BGQDB::filtering::JobFilter::JobType::All:
           return JobFilter::JobType::All;
       default:
           THROW_EXCEPTION(
                   InternalException,
                   InternalErrors::UnexpectedError,
                   "Unexpected error mapping JobFilter job type"
                   );
   }
}

Job::Id
JobFilter::Impl::getJobId() const
{
    return _jobFilter.getJobId();
}

const string&
JobFilter::Impl::getSchedulerData() const
{
    return _jobFilter.getSchedulerData();
}

const string&
JobFilter::Impl::getComputeBlockName() const
{
    return _jobFilter.getComputeBlockName();
}

const string&
JobFilter::Impl::getUser() const
{
    return _jobFilter.getUser();
}

const string&
JobFilter::Impl::getExecutable() const
{
    return _jobFilter.getExecutable();
}

const JobFilter::Statuses
JobFilter::Impl::getStatuses() const
{
    JobFilter::Statuses schedStatus;
    const BGQDB::filtering::JobFilter::Statuses dbStatus = _jobFilter.getStatuses();

    // Map from bgsched status to db filtering status
    if (dbStatus.size() > 0) {
        if (dbStatus.find(BGQDB::job::status::Setup) != dbStatus.end()) {
            schedStatus.insert(Job::Setup);
        }
        if (dbStatus.find(BGQDB::job::status::Loading) != dbStatus.end()) {
            schedStatus.insert(Job::Loading);
        }
        if (dbStatus.find(BGQDB::job::status::Starting) != dbStatus.end()) {
            schedStatus.insert(Job::Starting);
        }
        if (dbStatus.find(BGQDB::job::status::Running) != dbStatus.end()) {
            schedStatus.insert(Job::Running);
        }
        if (dbStatus.find(BGQDB::job::status::Debug) != dbStatus.end()) {
            schedStatus.insert(Job::Debug);
        }
        if (dbStatus.find(BGQDB::job::status::Cleanup) != dbStatus.end()) {
            schedStatus.insert(Job::Cleanup);
        }
        if (dbStatus.find(BGQDB::job::status::Terminated) != dbStatus.end()) {
            schedStatus.insert(Job::Terminated);
        }
        if (dbStatus.find(BGQDB::job::status::Error) != dbStatus.end()) {
            schedStatus.insert(Job::Error);
        }
    }

    return schedStatus;
}

bool
JobFilter::Impl::isExitStatusSet() const
{
    return _jobFilter.isExitStatusSet();
}

int
JobFilter::Impl::getExitStatus() const
{
    return _jobFilter.getExitStatus();
}

const TimeInterval
JobFilter::Impl::getStartTimeInterval() const
{
    BGQDB::filtering::TimeInterval dbTimeInterval = _jobFilter.getStartTimeInterval();
    TimeInterval timeInterval(dbTimeInterval.getStart(), dbTimeInterval.getEnd());
    return timeInterval;
}

const TimeInterval
JobFilter::Impl::getEndTimeInterval() const
{
    BGQDB::filtering::TimeInterval dbTimeInterval = _jobFilter.getEndTimeInterval();
    TimeInterval timeInterval(dbTimeInterval.getStart(), dbTimeInterval.getEnd());
    return timeInterval;
}

void
JobFilter::Impl::setJobType(
        JobFilter::JobType::Value jobType
        )
{
    // Map from bgsched job type to db filtering job type
    switch (jobType) {
        case JobFilter::JobType::Active:
            return _jobFilter.setJobType(BGQDB::filtering::JobFilter::JobType::Active);
        case JobFilter::JobType::Completed:
            return _jobFilter.setJobType(BGQDB::filtering::JobFilter::JobType::Completed);
        case JobFilter::JobType::All:
            return _jobFilter.setJobType(BGQDB::filtering::JobFilter::JobType::All);
        default:
            THROW_EXCEPTION(
                    InternalException,
                    InternalErrors::UnexpectedError,
                    "Unexpected error mapping JobFilter job type"
                    );
    }
}

void
JobFilter::Impl::setJobId(
        const Job::Id jobId
)
{
    _jobFilter.setJobId(jobId);
}

void
JobFilter::Impl::setSchedulerData(
        const string& schedulerData
)
{
    _jobFilter.setSchedulerData(schedulerData);
}

void
JobFilter::Impl::setComputeBlockName(
        const string& name
)
{
    _jobFilter.setComputeBlockName(name);
}

void
JobFilter::Impl::setUser(
        const string& user
)
{
    _jobFilter.setUser(user);
}

void
JobFilter::Impl::setExecutable(
        const string& exec
)
{
    _jobFilter.setExecutable(exec);
}

void
JobFilter::Impl::setStatuses(
        const JobFilter::Statuses* statusesPtr
)
{
    // Create statuses
    BGQDB::filtering::JobFilter::Statuses statuses;

    // Map from bgsched status to db filtering status
    if (statusesPtr != NULL) {
        if (statusesPtr->find(Job::Setup) != statusesPtr->end()) {
            statuses.insert(BGQDB::job::status::Setup);
        }
        if (statusesPtr->find(Job::Loading) != statusesPtr->end()) {
            statuses.insert(BGQDB::job::status::Loading);
        }
        if (statusesPtr->find(Job::Starting) != statusesPtr->end()) {
            statuses.insert(BGQDB::job::status::Starting);
        }
        if (statusesPtr->find(Job::Running) != statusesPtr->end()) {
            statuses.insert(BGQDB::job::status::Running);
        }
        if (statusesPtr->find(Job::Debug) != statusesPtr->end()) {
            statuses.insert(BGQDB::job::status::Debug);
        }
        if (statusesPtr->find(Job::Cleanup) != statusesPtr->end()) {
            statuses.insert(BGQDB::job::status::Cleanup);
        }
        if (statusesPtr->find(Job::Terminated) != statusesPtr->end()) {
            statuses.insert(BGQDB::job::status::Terminated);
        }
        if (statusesPtr->find(Job::Error) != statusesPtr->end()) {
            statuses.insert(BGQDB::job::status::Error);
        }
    }

    _jobFilter.setStatuses(&statuses);
}

void
JobFilter::Impl::setExitStatus(
        const int exitStatus
        )
{
    _jobFilter.setExitStatus(exitStatus);
}

void
JobFilter::Impl::setStartTimeInterval(
        const TimeInterval& timeInterval
        )
{
    const BGQDB::filtering::TimeInterval dbTimeInterval(timeInterval.getStart(), timeInterval.getEnd());
    _jobFilter.setStartTimeInterval(dbTimeInterval);
}

void
JobFilter::Impl::setEndTimeInterval(
        const TimeInterval& timeInterval
        )
{
    const BGQDB::filtering::TimeInterval dbTimeInterval(timeInterval.getStart(), timeInterval.getEnd());
    _jobFilter.setEndTimeInterval(dbTimeInterval);
}

BGQDB::filtering::JobFilter
JobFilter::Impl::convertToDBJobFilter() const
{
    return _jobFilter;
}

} // namespace bgsched
