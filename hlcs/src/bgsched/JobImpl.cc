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

#include <bgsched/DatabaseException.h>
#include <bgsched/InternalException.h>

#include "bgsched/JobImpl.h"
#include "bgsched/utility.h"

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/job/types.h>
#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <boost/date_time/posix_time/posix_time.hpp>

LOG_DECLARE_FILE("bgsched");

using namespace std;

namespace {
// Message strings
const string Unexpected_Job_Status_Error_Str("Unexpected job status.");
const string DB_Access_Error_Str("Error occurred while accessing database.");
const string DB_Connection_Error_Str("Communication error occurred while attempting to connect to database.");
} // anonymous namespace

namespace bgsched {

char
Job::Impl::statusToDbChar(
        const Job::Status status
        )
{
    switch (status) {
        case Job::Setup:
            return 'P';
        case Job::Loading:
            return 'L';
        case Job::Starting:
            return 'S';
        case Job::Debug:
            return 'D';
        case Job::Running:
            return 'R';
        case Job::Cleanup:
            return 'N';
        case Job::Terminated:
            return 'T';
        case Job::Error:
            return 'E';
        default:
            // Throw exception if status is unknown
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::UnexpectedError,
                    Unexpected_Job_Status_Error_Str
                    );
    }
}

Job::Status
Job::Impl::statusDbCharToValue(
        const char c
        )
{
    switch (c) {
        case 'P':
            return Job::Setup;
        case 'L':
            return Job::Loading;
        case 'S':
            return Job::Starting;
        case 'D':
            return Job::Debug;
        case 'R':
            return Job::Running;
        case 'N':
            return Job::Cleanup;
        case 'T':
            return Job::Terminated;
        case 'E':
            return Job::Error;
        default:
            // Throw exception if status is unknown
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::UnexpectedError,
                    Unexpected_Job_Status_Error_Str
            );
    }
}

Job::Status
Job::Impl::convertDBStatusToJobStatus(
        const string& status
        )
{
    if (status.compare(BGQDB::job::status_code::Setup) == 0) {
        return Job::Setup;
    } else if (status.compare(BGQDB::job::status_code::Loading) == 0) {
        return Job::Loading;
    } else if (status.compare(BGQDB::job::status_code::Starting) == 0) {
        return Job::Starting;
    } else if (status.compare(BGQDB::job::status_code::Debug) == 0) {
        return Job::Debug;
    } else if (status.compare(BGQDB::job::status_code::Running) == 0) {
        return Job::Running;
    } else if (status.compare(BGQDB::job::status_code::Cleanup) == 0) {
        return Job::Cleanup;
    } else if (status.compare(BGQDB::job::status_code::Terminated) == 0) {
        return Job::Terminated;
    } else if (status.compare(BGQDB::job::status_code::Error) == 0) {
        return Job::Error;
    }

    // Throw exception if status is unknown
    THROW_EXCEPTION(
            bgsched::InternalException,
            bgsched::InternalErrors::UnexpectedError,
            Unexpected_Job_Status_Error_Str
            );
}

string
Job::Impl::formatShape(
        const uint16_t a,
        const uint16_t b,
        const uint16_t c,
        const uint16_t d,
        const uint16_t e)
{
    ostringstream oss;
    oss << a << "x" << b << "x" << c << "x" << d << "x" << e;
    return oss.str();
}

Job::Impl::Impl(
        const cxxdb::Columns& job_cols
    ) :
    _id(0),
    _computeBlockName(),
    _computeBlockCreationId(0), // Compute block creation id is only valid for jobs in history table
    _status(),
    _user(),
    _executable(),
    _workingDirectory(),
    _errorText(),               // Error text is only valid for jobs in history table
    _args(),
    _envs(),
    _startTime(),
    _endTime(),                 // End time is only valid for jobs in history table
    _runTime(0),                // Run time is only valid for jobs in history table
    _inHistory(false),
    _computeNodesUsed(0),
    _sequenceId(0),
    _exitStatus(0),             // Exit status is only valid for jobs in history table
    _ranks(0),
    _schedulerData(),
    _corner(),
    _shape(),
    _mapping(),
    _clientInfo(),
    _np(0)
{
    bool isCompletedJob = true;
    if (job_cols[BGQDB::DBTJob_history::ENTRYDATE_COL].isNull()) {
        isCompletedJob = false;
    }

    // Set common job fields
    _id = job_cols[BGQDB::DBTJob::ID_COL].as<Job::Id>();
    _computeBlockName = job_cols[BGQDB::DBTJob::BLOCKID_COL].isNull() ? "" : job_cols[BGQDB::DBTJob::BLOCKID_COL].getString();
    _status = Job::Impl::convertDBStatusToJobStatus(job_cols[BGQDB::DBTJob::STATUS_COL].getString());
    _user = job_cols[BGQDB::DBTJob::USERNAME_COL].isNull() ? "" : job_cols[BGQDB::DBTJob::USERNAME_COL].getString();
    _executable = job_cols[BGQDB::DBTJob::EXECUTABLE_COL].isNull() ? "" : job_cols[BGQDB::DBTJob::EXECUTABLE_COL].getString();
    _workingDirectory = job_cols[BGQDB::DBTJob::WORKINGDIR_COL].isNull() ? "" : job_cols[BGQDB::DBTJob::WORKINGDIR_COL].getString();
    _args = job_cols[BGQDB::DBTJob::ARGS_COL].isNull() ? "" : job_cols[BGQDB::DBTJob::ARGS_COL].getString();
    _envs = job_cols[BGQDB::DBTJob::ENVS_COL].isNull() ? "" : job_cols[BGQDB::DBTJob::ENVS_COL].getString();
    _startTime = job_cols[BGQDB::DBTJob::STARTTIME_COL].isNull() ? "" : job_cols[BGQDB::DBTJob::STARTTIME_COL].getString();
    _computeNodesUsed = job_cols[BGQDB::DBTJob::NODESUSED_COL].as<uint32_t>();
    _sequenceId = job_cols[BGQDB::DBTJob::SEQID_COL].as<SequenceId>();
    _ranks = job_cols[BGQDB::DBTJob::PROCESSESPERNODE_COL].as<uint32_t>();
    _schedulerData = job_cols[BGQDB::DBTJob::SCHEDULERDATA_COL].isNull() ? "" : job_cols[BGQDB::DBTJob::SCHEDULERDATA_COL].getString();
    _corner = job_cols[BGQDB::DBTJob::CORNER_COL].isNull() ? "" : job_cols[BGQDB::DBTJob::CORNER_COL].getString();
    _shape = formatShape(job_cols[BGQDB::DBTJob::SHAPEA_COL].as<uint16_t>(),
                         job_cols[BGQDB::DBTJob::SHAPEB_COL].as<uint16_t>(),
                         job_cols[BGQDB::DBTJob::SHAPEC_COL].as<uint16_t>(),
                         job_cols[BGQDB::DBTJob::SHAPED_COL].as<uint16_t>(),
                         job_cols[BGQDB::DBTJob::SHAPEE_COL].as<uint16_t>());
    _mapping = job_cols[BGQDB::DBTJob::MAPPING_COL].isNull() ? "" : job_cols[BGQDB::DBTJob::MAPPING_COL].getString();
    _clientInfo = job_cols[BGQDB::DBTJob::HOSTNAME_COL].isNull() ? "" : job_cols[BGQDB::DBTJob::HOSTNAME_COL].getString();
    // Check if we need to convert the second part of the client info
    if (! job_cols[BGQDB::DBTJob::HOSTNAME_COL].isNull()) {
        int pid = job_cols[BGQDB::DBTJob::PID_COL].as<int>();
        ostringstream pid_string;
        pid_string << _clientInfo << ":" << pid;
        _clientInfo = pid_string.str();
    }
    _np = job_cols[BGQDB::DBTJob::NP_COL].as<uint32_t>();

    // Check if active or completed job
    if (isCompletedJob) {
        _computeBlockCreationId = job_cols[BGQDB::DBTJob_history::CREATIONID_COL].as<BlockCreationId>();
        _errorText = job_cols[BGQDB::DBTJob_history::ERRTEXT_COL].isNull() ? "" : job_cols[BGQDB::DBTJob_history::ERRTEXT_COL].getString();
        _endTime = job_cols[BGQDB::DBTJob_history::ENTRYDATE_COL].isNull() ? "" : job_cols[BGQDB::DBTJob_history::ENTRYDATE_COL].getString();
        _inHistory = true;
        _exitStatus = (job_cols[BGQDB::DBTJob_history::EXITSTATUS_COL] ? job_cols[BGQDB::DBTJob_history::EXITSTATUS_COL].as<int>() : -1);

        // Calculate run time value based on start time and end time
        if ((_startTime.size() == 26) && (_endTime.size() == 26)) {
            boost::posix_time::ptime startTime = job_cols[BGQDB::DBTJob::STARTTIME_COL].getTimestamp();
            boost::posix_time::ptime endTime = job_cols[BGQDB::DBTJob_history::ENTRYDATE_COL].getTimestamp();
            boost::posix_time::time_duration td = endTime - startTime;
            _runTime = td.total_seconds();
        }
    } else {
        // Active job
        _inHistory = false;
    }
}

Job::Pimpl
Job::Impl::createFromDatabase(
        const cxxdb::Columns& job_cols
    )
{
    Job::Pimpl ret(new Job::Impl(job_cols));
    return ret;
}

Job::Id
Job::Impl::getId() const
{
    return _id;
}

const string&
Job::Impl::getComputeBlockName() const
{
    return _computeBlockName;
}

BlockCreationId
Job::Impl::getComputeBlockCreationId() const
{
    return _computeBlockCreationId;
}

EnumWrapper<Job::Status>
Job::Impl::getStatus() const
{
    return _status;
}

const string&
Job::Impl::getUser() const
{
    return _user;
}

const string&
Job::Impl::getExecutable() const
{
    return _executable;
}

const string&
Job::Impl::getWorkingDirectory() const
{
    return _workingDirectory;
}

const string&
Job::Impl::getErrorText() const
{
    return _errorText;
}

const string&
Job::Impl::getArgs() const
{
    return _args;
}

const string&
Job::Impl::getEnvs() const
{
    return _envs;
}

const string&
Job::Impl::getStartTime() const
{
    return _startTime;
}

const string&
Job::Impl::getEndTime() const
{
    return _endTime;
}

uint32_t
Job::Impl::getRunTime() const
{
    return _runTime;
}

bool
Job::Impl::isInHistory() const
{
    return _inHistory;
}

uint32_t
Job::Impl::getComputeNodesUsed() const
{
    return _computeNodesUsed;
}

SequenceId
Job::Impl::getSequenceId() const
{
    return _sequenceId;
}

int
Job::Impl::getExitStatus() const
{
    return _exitStatus;
}

uint32_t
Job::Impl::getRanksPerNode() const
{
    return _ranks;
}

uint32_t
Job::Impl::getNP() const
{
    return _np;
}

const string&
Job::Impl::getSchedulerData() const
{
    return _schedulerData;
}

const string&
Job::Impl::getCorner() const
{
    return _corner;
}

const string&
Job::Impl::getShape() const
{
    return _shape;
}

const string&
Job::Impl::getMapping() const
{
    return _mapping;
}

const string&
Job::Impl::getClientInfo() const
{
    return _clientInfo;
}

} // namespace bgsched
