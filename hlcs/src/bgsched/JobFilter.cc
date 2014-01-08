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

#include <bgsched/JobFilter.h>

#include "bgsched/JobFilterImpl.h"

using namespace std;

namespace bgsched {

JobFilter::JobFilter() :
    _impl(new Impl())
{
    // Nothing to do
}

JobFilter::Pimpl
JobFilter::getPimpl() const
{
    return _impl;
}

bgsched::JobFilter::JobType::Value
JobFilter::getJobType() const
{
    return _impl->getJobType();
}

void
JobFilter::setJobType(
        bgsched::JobFilter::JobType::Value jobType
        )
{
    _impl->setJobType(jobType);
}

Job::Id
JobFilter::getJobId() const
{
    return _impl->getJobId();
}

void
JobFilter::setJobId(
        const Job::Id jobId
        )
{
    _impl->setJobId(jobId);
}

const string&
JobFilter::getSchedulerData() const
{
    return _impl->getSchedulerData();
}

void
JobFilter::setSchedulerData(
        const string& schedulerData
        )
{
    _impl->setSchedulerData(schedulerData);
}

const string&
JobFilter::getComputeBlockName() const
{
    return _impl->getComputeBlockName();
}

void
JobFilter::setComputeBlockName(
        const string& name
        )
{
    _impl->setComputeBlockName(name);
}

const string&
JobFilter::getUser() const
{
    return _impl->getUser();
}

void
JobFilter::setUser(
        const string& user
        )
{
    _impl->setUser(user);
}

const string&
JobFilter::getExecutable() const
{
    return _impl->getExecutable();
}

void
JobFilter::setExecutable(
        const string& exec
        )
{
    _impl->setExecutable(exec);
}

const JobFilter::Statuses
JobFilter::getStatuses() const
{
    return _impl->getStatuses();
}

void
JobFilter::setStatuses(
        const Statuses* statusesPtr
        )
{
    _impl->setStatuses(statusesPtr);
}

bool
JobFilter::isExitStatusSet() const
{
    return _impl->isExitStatusSet();
}

int
JobFilter::getExitStatus() const
{
    return _impl->getExitStatus();
}

void
JobFilter::setExitStatus(
        const int exitStatus
        )
{
    _impl->setExitStatus(exitStatus);
}

const bgsched::TimeInterval
JobFilter::getStartTimeInterval() const
{
    return _impl->getStartTimeInterval();
}

void
JobFilter::setStartTimeInterval(
        const bgsched::TimeInterval& timeInterval
        )
{
    _impl->setStartTimeInterval(timeInterval);
}

const bgsched::TimeInterval
JobFilter::getEndTimeInterval() const
{
    return _impl->getEndTimeInterval();
}

void
JobFilter::setEndTimeInterval(
        const bgsched::TimeInterval& timeInterval
        )
{
    _impl->setEndTimeInterval(timeInterval);
}

} // namespace bgsched
