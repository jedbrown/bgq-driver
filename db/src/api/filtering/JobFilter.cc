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
#include <db/include/api/filtering/JobFilter.h>

using namespace std;

namespace BGQDB {
namespace filtering {

JobFilter::JobFilter():
    _jobId(0),
    _schedulerData(),
    _computeBlockName(),
    _user(),
    _executable(),
    _status(),
    _jobType(BGQDB::filtering::JobFilter::JobType::Active),
    _isExitStatusSet(false),
    _exitStatus(0),
    _endTimeInterval(TimeInterval::ALL),
    _startTimeInterval(TimeInterval::ALL),
    _pid(-1)
{
    // Nothing to do.
}

BGQDB::filtering::JobFilter::JobType::Value
JobFilter::getJobType() const
{
    return _jobType;
}

BGQDB::filtering::JobFilter::Id
JobFilter::getJobId() const
{
    return _jobId;
}

const string&
JobFilter::getSchedulerData() const
{
    return _schedulerData;
}

const string&
JobFilter::getComputeBlockName() const
{
    return _computeBlockName;
}

const string&
JobFilter::getUser() const
{
    return _user;
}

const string&
JobFilter::getExecutable() const
{
    return _executable;
}

const BGQDB::filtering::JobFilter::Statuses
JobFilter::getStatuses() const
{
    return _status;
}

bool
JobFilter::isExitStatusSet() const
{
    return _isExitStatusSet;
}

int
JobFilter::getExitStatus() const
{
    return _exitStatus;
}

const BGQDB::filtering::TimeInterval
JobFilter::getEndTimeInterval() const
{
    return _endTimeInterval;
}

const BGQDB::filtering::TimeInterval
JobFilter::getStartTimeInterval() const
{
    return _startTimeInterval;
}

void
JobFilter::setJobType(
        BGQDB::filtering::JobFilter::JobType::Value jobType
        )
{
    _jobType = jobType;
}

void
JobFilter::setJobId(
        const BGQDB::filtering::JobFilter::Id jobId
)
{
    _jobId = jobId;
}

void
JobFilter::setSchedulerData(
        const string& schedulerData
)
{
    _schedulerData = schedulerData;
}

void
JobFilter::setComputeBlockName(
        const string& name
)
{
    _computeBlockName = name;
}

void
JobFilter::setUser(
        const string& user
)
{
    _user = user;
}

void
JobFilter::setExecutable(
        const string& exec
)
{
    _executable = exec;
}

void
JobFilter::setStatuses(
        const BGQDB::filtering::JobFilter::Statuses* statusesPtr
)
{
    _status.clear();

    if (statusesPtr != NULL) {
        _status = *statusesPtr;
    }
}

void
JobFilter::setExitStatus(
        const int exitStatus
        )
{
    _isExitStatusSet = true;
    _exitStatus = exitStatus;
}

void
JobFilter::setStartTimeInterval(
        const BGQDB::filtering::TimeInterval timeInterval
        )
{
    _startTimeInterval = timeInterval;
}

void
JobFilter::setEndTimeInterval(
        const BGQDB::filtering::TimeInterval timeInterval
        )
{
    _endTimeInterval = timeInterval;
}

} // filtering
} // BGQDB

