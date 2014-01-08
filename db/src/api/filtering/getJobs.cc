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
#include "cxxdb/cxxdb.h"
#include "Exception.h"
#include "filtering/getJobs.h"
#include "job/types.h"
#include "tableapi/gensrc/bgqtableapi.h"

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/algorithm/string.hpp>

#include <sstream>
#include <string>
#include <vector>


using std::ostringstream;
using std::string;

using namespace boost::assign;

using boost::lexical_cast;


LOG_DECLARE_FILE("database");

namespace BGQDB {
namespace filtering {


static std::string
calcSql( const Duration& d )
{
    std::vector<string> parts;

    if ( d.getDate().years() != 0 )  parts += lexical_cast<string>( d.getDate().years() ) + " YEAR";
    if ( d.getDate().months() != 0 )  parts += lexical_cast<string>( d.getDate().months() ) + " MONTH";
    if ( d.getDate().days() != 0 )  parts += lexical_cast<string>( d.getDate().days() ) + " DAY";

    if ( d.getTime().hours() != 0 )  parts += lexical_cast<string>( d.getTime().hours() ) + " HOUR";
    if ( d.getTime().minutes() != 0 )  parts += lexical_cast<string>( d.getTime().minutes() ) + " MINUTE";
    if ( d.getTime().seconds() != 0 )  parts += lexical_cast<string>( d.getTime().seconds() ) + " SECOND";

    return boost::algorithm::join( parts, " - " );
}


static bool containsWildcard(
        const std::string& s
    )
{
    if ( s.find_first_of( "*?" ) == string::npos )  return false;
    return true;
}


static std::string translateWildcard(
        std::string s
    )
{
    // Convert the glob wildcard character to its SQL equivalent, * becomes % and ? becomes _
    boost::algorithm::replace_all( s, "*", "%" ); // * glob becomes % SQL.
    boost::algorithm::replace_all( s, "?", "_" ); // ? glob becomes _ SQL.
    return s;
}


void
getJobs(
        const JobFilter& filter,
        const JobSort& sort,
        cxxdb::Connection& conn,
        const string& user,
        const string& securityWhereClauseAddendum,
        int64_t rowNumStart,
        int64_t rowNumEnd,
        uint64_t* row_count_out,
        cxxdb::ResultSetPtr* rs_ptr_out
        )
{
    cxxdb::ParameterNames parameter_names;
    string filterValue, conjunction, history_conjunction;
    ostringstream job_where_clause_sql, job_history_where_clause_sql, sort_clause_sql;

    // Validity check row start/end arguments
    if (rowNumStart != 0) {
        // Row start is being honored so make sure it is valid
        if ((rowNumStart < 1) || (rowNumStart > rowNumEnd))  {
            BOOST_THROW_EXCEPTION(
                    std::invalid_argument("Invalid starting row number")
                    );
        }
    }

    // Calculate sort order
    if (sort.isSorted()) {
        sort_clause_sql <<
        (sort.getField() == JobSort::Field::User ? DBTJob::USERNAME_COL :
         sort.getField() == JobSort::Field::Block ? DBTJob::BLOCKID_COL :
         sort.getField() == JobSort::Field::Executable ? DBTJob::EXECUTABLE_COL :
         sort.getField() == JobSort::Field::StartTime ? DBTJob::STARTTIME_COL :
         sort.getField() == JobSort::Field::Status ? DBTJob::STATUS_COL :
         sort.getField() == JobSort::Field::RanksPerNode ? DBTJob::PROCESSESPERNODE_COL :
         sort.getField() == JobSort::Field::ComputeNodesUsed ? DBTJob::NODESUSED_COL :
         sort.getField() == JobSort::Field::EndTime ? DBTJob_history::ENTRYDATE_COL :
         sort.getField() == JobSort::Field::ExitStatus ? DBTJob_history::EXITSTATUS_COL :
         DBTJob::ID_COL) << " "  << (sort.getSortOrder() == SortOrder::Ascending ? "ASC" : "DESC");

        if (sort.getField() != JobSort::Field::Id) {
            sort_clause_sql << ", " << DBTJob::ID_COL << " " << (sort.getSortOrder() == SortOrder::Ascending ? "ASC" : "DESC");
        }
    } else {
        sort_clause_sql << DBTJob::ID_COL << " ASC";
    }

    const JobFilter::Statuses jobStatuses = filter.getStatuses();
    // Build job "Where" clause from filter provided
    conjunction = " WHERE ";
    // Valid status values for active jobs are: Setup, Loading, Starting, Running and Cleanup
    // Jobs in history table will have status of Terminated or Error
    // Search all statuses if filter is not set
    if (jobStatuses.size() > 0) {
        job_where_clause_sql << conjunction;
        conjunction = " ( STATUS = ";
        if (jobStatuses.find(job::status::Setup) != jobStatuses.end()) {
            job_where_clause_sql << conjunction << "'" << job::status_code::Setup << "'";
            conjunction = " OR STATUS = ";
        }
        if (jobStatuses.find(job::status::Loading) != jobStatuses.end()) {
            job_where_clause_sql << conjunction << "'" << job::status_code::Loading << "'";
            conjunction = " OR STATUS = ";
        }
        if (jobStatuses.find(job::status::Starting) != jobStatuses.end()) {
            job_where_clause_sql << conjunction << "'" << job::status_code::Starting << "'";
            conjunction = " OR STATUS = ";
        }
        if (jobStatuses.find(job::status::Running) != jobStatuses.end()) {
            job_where_clause_sql << conjunction << "'" << job::status_code::Running << "'";
            conjunction = " OR STATUS = ";
        }
        if (jobStatuses.find(job::status::Debug) != jobStatuses.end()) {
            job_where_clause_sql << conjunction << "'" << job::status_code::Debug << "'";
            conjunction = " OR STATUS = ";
        }
        if (jobStatuses.find(job::status::Cleanup) != jobStatuses.end()) {
            job_where_clause_sql << conjunction << "'" << job::status_code::Cleanup << "'";
        }
        if (jobStatuses.find(job::status::Terminated) != jobStatuses.end()) {
            job_where_clause_sql << conjunction << "'" << job::status_code::Terminated << "'";
            conjunction = " OR STATUS = ";
        }
        if (jobStatuses.find(job::status::Error) != jobStatuses.end()) {
            job_where_clause_sql << conjunction << "'" << job::status_code::Error << "'";
        }

        job_where_clause_sql << " )";
        conjunction = " AND ";
    }

    filterValue = filter.getSchedulerData();
    if (!filterValue.empty()) {
        job_where_clause_sql << conjunction << "SCHEDULERDATA = '" << filter.getSchedulerData()  << "'";
        conjunction = " AND ";
    }

    filterValue = filter.getComputeBlockName();
    if (!filterValue.empty()) {
        job_where_clause_sql << conjunction << "BLOCKID = '" << filter.getComputeBlockName() << "'";
        conjunction = " AND ";
    }

    filterValue = filter.getExecutable();
    if (!filterValue.empty()) {

        if ( containsWildcard( filterValue ) ) {
            job_where_clause_sql << conjunction << "EXECUTABLE LIKE '" << translateWildcard( filter.getExecutable() ) << "'";
        } else {
            job_where_clause_sql << conjunction << "EXECUTABLE = '" << filter.getExecutable() << "'";
        }

        conjunction = " AND ";
    }

    if ( ! filter.getHostname().empty() ) {
        job_where_clause_sql << conjunction << "hostname = '" << filter.getHostname() << "'";
        conjunction = " AND ";
    }

    if ( filter.getPid() != -1 ) {
        job_where_clause_sql << conjunction << "pid = " << filter.getPid();
        conjunction = " AND ";
    }

    // All previous Where clause are common between job and job history
    job_history_where_clause_sql << job_where_clause_sql.str();
    history_conjunction = conjunction;

    // Add-on any additional Where clause filtering for active jobs
    bool isJobStartTimeIntervalSet = false;
    TimeInterval startTimeInterval(filter.getStartTimeInterval());
    if ((filter.getJobType() == JobFilter::JobType::Active) ||
        (filter.getJobType() == JobFilter::JobType::All))
    {
        bool skipJobId = false;

        // When retrieving jobs where user security is being enforced, the user is only entitled to view their own jobs
        // or other active jobs where they have been granted (R)ead authority in the job security table.
        filterValue = filter.getUser();
        // Check if user security is being enforced
        if (user.empty()) {
            // User security is NOT being enforced so allow filtering on any user name
            if (!filterValue.empty()) {
                job_where_clause_sql << conjunction << "USERNAME = '" << filterValue << "'";
                conjunction = " AND ";
            }
        } else {
            // User security is being enforced, only allow viewing own jobs or those user has read authority to
            if (filterValue.empty()) {
                // No user filter value specified, restrict search to own user or jobs with read authority
                if (securityWhereClauseAddendum.empty()) {
                    // No other job ids to search on
                    job_where_clause_sql << conjunction << "USERNAME = '" << user << "'";
                } else {
                    // Include job ids for other users
                    job_where_clause_sql << conjunction << "(USERNAME = '" << user << "' OR " << securityWhereClauseAddendum << ")";
                }
                conjunction = " AND ";
            } else {
                // User filter was specified, check if security user and filter user match
                if (user.compare(filterValue) == 0) {
                    // Security user and filter user are the same, ignore the security addendum and restrict search to security user
                    job_where_clause_sql << conjunction << "USERNAME = '" << user << "'";
                } else {
                    // Security user and filter user are different, only allow specific job ids for the other user
                    if (securityWhereClauseAddendum.empty()) {
                        // The security user does not have authority to any other jobs, setup query so nothing is returned
                        skipJobId = true;
                        job_where_clause_sql << conjunction << "(USERNAME = '" << filterValue << "' AND " << DBTJob::ID_COL << " = 0)";
                    } else {
                        job_where_clause_sql << conjunction << "(USERNAME = '" << filterValue << "' AND " << securityWhereClauseAddendum << ")";
                    }
                }
                conjunction = " AND ";
            }
        }

        // Check if job id was already handled as part of user security
        if (!skipJobId) {
            if (filter.getJobId() != 0) {
                job_where_clause_sql << conjunction << DBTJob::ID_COL << " = " << filter.getJobId();
                conjunction = " AND ";
            }
        }

        if ( ! (startTimeInterval == TimeInterval() || startTimeInterval == TimeInterval::ALL) ) {
            isJobStartTimeIntervalSet = true;

            if ( startTimeInterval.getStart() != TimeInterval::Timestamp() || startTimeInterval.getEnd() != TimeInterval::Timestamp() ) {
                job_where_clause_sql << conjunction << "STARTTIME BETWEEN ? AND ?";

                parameter_names.push_back("jobStartTimeStart");
                parameter_names.push_back("jobStartTimeEnd");
            } else {
                job_where_clause_sql << conjunction << "STARTTIME >= (CURRENT_TIMESTAMP - " << calcSql( startTimeInterval.getDuration() ) << ")";
            }

            conjunction = " AND ";
        }
    }

    // Add-on any additional Where clause filtering for history jobs
    bool isJobHistoryStartTimeIntervalSet = false;
    bool isJobHistoryEndTimeIntervalSet = false;
    TimeInterval endTimeInterval(filter.getEndTimeInterval());
    if ((filter.getJobType() == JobFilter::JobType::Completed) ||
        (filter.getJobType() == JobFilter::JobType::All))
    {
        bool skipJobId = false;

        // When retrieving jobs where user security is being enforced, the user is only entitled to view their own jobs in history table
        filterValue = filter.getUser();
        // Check if user security is being enforced
        if (user.empty()) {
            // User security is NOT being enforced so allow filtering on any user name
            if (!filterValue.empty()) {
                job_history_where_clause_sql << history_conjunction << "USERNAME = '" << filterValue << "'";
                history_conjunction = " AND ";
            }
        } else {
            // User security is being enforced, only allow viewing own jobs
            if (filterValue.empty()) {
                // No user filter value specified, restrict search to own user
                job_history_where_clause_sql << history_conjunction << "USERNAME = '" << user << "'";
                history_conjunction = " AND ";
            } else {
                // User filter was specified, check if security user and filter user match
                if (user.compare(filterValue) == 0) {
                    // Security user and filter user are the same
                    job_history_where_clause_sql << history_conjunction << "USERNAME = '" << user << "'";
                } else {
                    // Security user and filter user are different, the security user does not have authority
                    // to any other jobs, setup query so nothing is returned
                    skipJobId = true;
                    job_history_where_clause_sql << history_conjunction << "(USERNAME = '" << filterValue << "' AND " << DBTJob::ID_COL << " = 0)";
                }
            }
            history_conjunction = " AND ";
        }

        // Check if job id was already handled as part of user security
        if (!skipJobId) {
            if (filter.getJobId() != 0) {
                job_history_where_clause_sql << history_conjunction << DBTJob::ID_COL << " = " << filter.getJobId();
                history_conjunction = " AND ";
            }
        }

        if (filter.isExitStatusSet()) {
            job_history_where_clause_sql << history_conjunction << "EXITSTATUS = " << filter.getExitStatus();
            history_conjunction = " AND ";
        }

        if ( ! (startTimeInterval == TimeInterval() || startTimeInterval == TimeInterval::ALL) ) {
            isJobHistoryStartTimeIntervalSet = true;

            if ( startTimeInterval.getStart() != TimeInterval::Timestamp() || startTimeInterval.getEnd() != TimeInterval::Timestamp() ) {
                job_history_where_clause_sql << history_conjunction << "STARTTIME BETWEEN ? AND ?";

                parameter_names.push_back("jobHistoryStartTimeStart");
                parameter_names.push_back("jobHistoryStartTimeEnd");
            } else {
                job_history_where_clause_sql << history_conjunction << "STARTTIME >= (CURRENT_TIMESTAMP - " << calcSql( startTimeInterval.getDuration() ) << ")";
            }

            history_conjunction = " AND ";
        }

        if ( ! (endTimeInterval == TimeInterval() || endTimeInterval == TimeInterval::ALL) ) {
            isJobHistoryEndTimeIntervalSet = true;

            if ( endTimeInterval.getStart() != TimeInterval::Timestamp() || endTimeInterval.getEnd() != TimeInterval::Timestamp() ) {
                job_history_where_clause_sql << history_conjunction << "ENTRYDATE BETWEEN ? AND ?";

                parameter_names.push_back("jobHistoryEndTimeStart");
                parameter_names.push_back("jobHistoryEndTimeEnd");
            } else {
                job_history_where_clause_sql << history_conjunction << "ENTRYDATE >= (CURRENT_TIMESTAMP - " << calcSql( endTimeInterval.getDuration() ) << ")";
            }

            history_conjunction = " AND ";
        }
    }

    // Check if row count should be returned
    if (row_count_out != NULL) {
        string sql_row_count = string() + "WITH all AS (";

        if ((filter.getJobType() == JobFilter::JobType::Active) ||
                (filter.getJobType() == JobFilter::JobType::All)) {
            sql_row_count += string() + " SELECT id FROM bgqJob" + job_where_clause_sql.str();
        }

        if (filter.getJobType() == JobFilter::JobType::All) {
            sql_row_count += string() + " UNION ALL";
        }

        if ((filter.getJobType() == JobFilter::JobType::Completed) ||
                (filter.getJobType() == JobFilter::JobType::All)) {
            sql_row_count += string() + " SELECT id FROM bgqJob_history" + job_history_where_clause_sql.str();
        }

        sql_row_count += " ) SELECT COUNT(*) AS c FROM all";

        LOG_DEBUG_MSG("Preparing: " << sql_row_count);

        // Query job count
        try {
            // Prepare query
            cxxdb::QueryStatementPtr stmt_ptr(conn.prepareQuery(sql_row_count, parameter_names));

            // Set any parameters
            if (isJobStartTimeIntervalSet) {
                if ( startTimeInterval.getStart() != TimeInterval::Timestamp() || startTimeInterval.getEnd() != TimeInterval::Timestamp() ) {
                    TimeInterval::Timestamp start(startTimeInterval.getStart());
                    TimeInterval::Timestamp end(startTimeInterval.getEnd());

                    if ( start == TimeInterval::Timestamp() ) {
                        start = end - startTimeInterval.getDuration();
                    } else if ( end == TimeInterval::Timestamp() ) {
                        end = start + startTimeInterval.getDuration();
                    }

                    stmt_ptr->parameters()["jobStartTimeStart"].set(start);
                    stmt_ptr->parameters()["jobStartTimeEnd"].set(end);
                }
            }

            if (isJobHistoryStartTimeIntervalSet) {
                if ( startTimeInterval.getStart() != TimeInterval::Timestamp() || startTimeInterval.getEnd() != TimeInterval::Timestamp() ) {
                    TimeInterval::Timestamp start(startTimeInterval.getStart());
                    TimeInterval::Timestamp end(startTimeInterval.getEnd());

                    if ( start == TimeInterval::Timestamp() ) {
                        start = end - startTimeInterval.getDuration();
                    } else if ( end == TimeInterval::Timestamp() ) {
                        end = start + startTimeInterval.getDuration();
                    }

                    stmt_ptr->parameters()["jobHistoryStartTimeStart"].set(start);
                    stmt_ptr->parameters()["jobHistoryStartTimeEnd"].set(end);
                }
            }

            if (isJobHistoryEndTimeIntervalSet) {
                if ( endTimeInterval.getStart() != TimeInterval::Timestamp() || endTimeInterval.getEnd() != TimeInterval::Timestamp() ) {
                    TimeInterval::Timestamp start(endTimeInterval.getStart());
                    TimeInterval::Timestamp end(endTimeInterval.getEnd());

                    if ( start == TimeInterval::Timestamp() ) {
                        start = end - endTimeInterval.getDuration();
                    } else if ( end == TimeInterval::Timestamp() ) {
                        end = start + endTimeInterval.getDuration();
                    }

                    stmt_ptr->parameters()["jobHistoryEndTimeStart"].set(start);
                    stmt_ptr->parameters()["jobHistoryEndTimeEnd"].set(end);
                }
            }

            // Run the query
            cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

            // Check if any rows returned
            if (rs_ptr->fetch()) {
                *row_count_out = rs_ptr->columns()["c"].as<uint64_t>();
            } else {
                // No matches on job query so just return
                *row_count_out = 0;
                return;
            }
        } catch (const Exception& e) {
            throw; // just re-throw it
        } catch (const cxxdb::DatabaseException& e) {
            throw; // just re-throw it
        } catch (const std::exception& e) {
            throw; // just re-throw it
        }
    }

    // Build query for jobs

    string sql = string() + "WITH all AS (";

    if ((filter.getJobType() == JobFilter::JobType::Active) ||
        (filter.getJobType() == JobFilter::JobType::All)) {
        sql += string() +
" SELECT id, username, blockId, executable, workingdir, startTime, status, seqId, args, envs, mapping,"
       " nodesUsed, shapeA, shapeB, shapeC, shapeD, shapeE, processesPerNode, schedulerData, corner, hostname, pid, np,"
       " CAST( NULL AS INTEGER ) AS creationId,"
       " CAST( NULL AS TIMESTAMP ) AS entryDate,"
       " CAST( NULL AS INTEGER ) AS exitStatus,"
       " CAST( NULL AS CHAR(128) ) AS errText"
  " FROM bgqJob" + job_where_clause_sql.str();
    }

    if (filter.getJobType() == JobFilter::JobType::All) {
        sql += string() + " UNION ALL";
    }

    if ((filter.getJobType() == JobFilter::JobType::Completed) ||
        (filter.getJobType() == JobFilter::JobType::All)) {
        sql += string() +
" SELECT id, username, blockId, executable, workingdir, startTime, status, seqId, args, envs, mapping,"
       " nodesUsed, shapeA, shapeB, shapeC, shapeD, shapeE, processesPerNode, schedulerData, corner, hostname, pid, np,"
       " creationId, entryDate, exitStatus, errText"
  " FROM bgqJob_history" + job_history_where_clause_sql.str();
    }

    sql += string() +
" ), all_ordered AS ("

" SELECT id, username, blockId, executable, workingdir, startTime, status, seqId, args, envs, mapping,"
       " nodesUsed, shapeA, shapeB, shapeC, shapeD, shapeE, processesPerNode, schedulerData, corner, hostname, pid, np,"
       " creationId, entryDate, exitStatus, errText,"
       " ROW_NUMBER() OVER ( ORDER BY " + sort_clause_sql.str() + " ) AS row_num"
  " FROM all"

" )"

" SELECT id, username, blockId, executable, workingdir, startTime, status, seqId, args, envs, mapping,"
       " nodesUsed, shapeA, shapeB, shapeC, shapeD, shapeE, processesPerNode, schedulerData, corner, hostname, pid, np,"
       " creationId, entryDate, exitStatus, errText"
  " FROM all_ordered"
        ;

    // Return all rows unless row range is requested
    bool isRowRangeSet = false;
    if (rowNumStart != 0) {
        isRowRangeSet = true;
        sql += string() + " WHERE row_num BETWEEN ? AND ?";  // rowNumStart rowNumEnd
        parameter_names.push_back("rowNumStart");
        parameter_names.push_back("rowNumEnd");
    }

    sql += string() + " ORDER BY row_num";

    LOG_DEBUG_MSG("Querying for jobs: " << sql);

    // Query jobs
    try {
        cxxdb::QueryStatementPtr stmt_ptr(conn.prepareQuery(sql, parameter_names));

        if (isJobStartTimeIntervalSet) {
            if ( startTimeInterval.getStart() != TimeInterval::Timestamp() || startTimeInterval.getEnd() != TimeInterval::Timestamp() ) {
                TimeInterval::Timestamp start(startTimeInterval.getStart());
                TimeInterval::Timestamp end(startTimeInterval.getEnd());

                if ( start == TimeInterval::Timestamp() ) {
                    start = end - startTimeInterval.getDuration();
                } else if ( end == TimeInterval::Timestamp() ) {
                    end = start + startTimeInterval.getDuration();
                }

                stmt_ptr->parameters()["jobStartTimeStart"].set(start);
                stmt_ptr->parameters()["jobStartTimeEnd"].set(end);
            }
        }

        if (isJobHistoryStartTimeIntervalSet) {
            if ( startTimeInterval.getStart() != TimeInterval::Timestamp() || startTimeInterval.getEnd() != TimeInterval::Timestamp() ) {
                TimeInterval::Timestamp start(startTimeInterval.getStart());
                TimeInterval::Timestamp end(startTimeInterval.getEnd());

                if ( start == TimeInterval::Timestamp() ) {
                    start = end - startTimeInterval.getDuration();
                } else if ( end == TimeInterval::Timestamp() ) {
                    end = start + startTimeInterval.getDuration();
                }

                stmt_ptr->parameters()["jobHistoryStartTimeStart"].set(start);
                stmt_ptr->parameters()["jobHistoryStartTimeEnd"].set(end);
            }
        }

        if (isJobHistoryEndTimeIntervalSet) {
            if ( endTimeInterval.getStart() != TimeInterval::Timestamp() || endTimeInterval.getEnd() != TimeInterval::Timestamp() ) {
                TimeInterval::Timestamp start(endTimeInterval.getStart());
                TimeInterval::Timestamp end(endTimeInterval.getEnd());

                if ( start == TimeInterval::Timestamp() ) {
                    start = end - endTimeInterval.getDuration();
                } else if ( end == TimeInterval::Timestamp() ) {
                    end = start + endTimeInterval.getDuration();
                }

                stmt_ptr->parameters()["jobHistoryEndTimeStart"].set(start);
                stmt_ptr->parameters()["jobHistoryEndTimeEnd"].set(end);
            }
        }

        if (isRowRangeSet) {
            stmt_ptr->parameters()["rowNumStart"].set(rowNumStart);
            stmt_ptr->parameters()["rowNumEnd"].set(rowNumEnd);
        }

        *rs_ptr_out = stmt_ptr->execute();

        (*rs_ptr_out)->internalize(stmt_ptr);

        return;

    } catch (const Exception& e) {
        throw; // just re-throw it
    } catch (const cxxdb::DatabaseException& e) {
        throw; // just re-throw it
    } catch (const std::exception& e) {
        throw; // just re-throw it
    }
}

} // filtering
} // BGQDB
