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

#include <bgsched/Job.h>

#include "bgsched/JobImpl.h"

using namespace std;

namespace bgsched {

Job::Job(
        Pimpl impl
        ) :
    _impl(impl)
{
}

Job::Id
Job::getId() const
{
    return _impl->getId();
}

const string&
Job::getComputeBlockName() const
{
    return _impl->getComputeBlockName();
}

BlockCreationId
Job::getComputeBlockCreationId() const
{
    return _impl->getComputeBlockCreationId();
}

EnumWrapper<Job::Status>
Job::getStatus() const
{
    return _impl->getStatus();
}

const string&
Job::getUser() const
{
    return _impl->getUser();
}

const string&
Job::getExecutable() const
{
    return _impl->getExecutable();
}

const string&
Job::getWorkingDirectory() const
{
    return _impl->getWorkingDirectory();
}

const string&
Job::getErrorText() const
{
    return _impl->getErrorText();
}

const string&
Job::getArgs() const
{
    return _impl->getArgs();
}

const string&
Job::getEnvs() const
{
    return _impl->getEnvs();
}

const string&
Job::getStartTime() const
{
    return _impl->getStartTime();
}

const string&
Job::getEndTime() const
{
    return _impl->getEndTime();
}

uint64_t
Job::getRunTime() const
{
    return _impl->getRunTime();
}

bool
Job::isInHistory() const
{
    return _impl->isInHistory();
}

uint32_t
Job::getComputeNodesUsed() const
{
    return _impl->getComputeNodesUsed();
}

SequenceId
Job::getSequenceId() const
{
    return _impl->getSequenceId();
}

int
Job::getExitStatus() const
{
    return _impl->getExitStatus();
}

const string&
Job::getCorner() const
{
    return _impl->getCorner();
}

const string&
Job::getShape() const
{
    return _impl->getShape();
}

uint32_t
Job::getRanksPerNode() const
{
    return _impl->getRanksPerNode();
}

uint32_t
Job::getNP() const
{
    return _impl->getNP();
}

const string&
Job::getSchedulerData() const
{
    return _impl->getSchedulerData();
}

const string&
Job::getMapping() const
{
    return _impl->getMapping();
}

const string&
Job::getClientInfo() const
{
    return _impl->getClientInfo();
}

} // namespace bgsched
