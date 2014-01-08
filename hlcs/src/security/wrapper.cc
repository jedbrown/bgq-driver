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
#include <hlcs/include/security/wrapper.h>
#include <hlcs/include/security/exception.h>

#include <bgq_util/include/string_tokenizer.h>

#include <db/include/api/Exception.h>
#include <db/include/api/genblock.h>
#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/filtering/getJobs.h>
#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <boost/foreach.hpp>
#include <boost/throw_exception.hpp>

#include <sstream>

using namespace std;

namespace hlcs {
namespace security {

LOG_DECLARE_FILE("security");


static bool
hasJobAuthorityType(
        const bgq::utility::Properties::ConstPtr& properties,
        const string authorityType,
        const bgq::utility::UserId& user
        )
{
    const string section("security.jobs");
    LOG_DEBUG_MSG("Looking for " << authorityType << " in [" << section << "]");

    string result;
    // Get any users with specified authority type (e.g. all, read) to jobs
    try {
        result = properties->getValue(section, authorityType);
    } catch (const std::invalid_argument& e) {
        // Missing section isn't an error, just log and continue
        LOG_DEBUG_MSG(e.what());
        return false;
    }

    StringTokenizer tokenizer;
    tokenizer.tokenize(result);

    // Match user or one of their secondary groups in one of the tokens
    BOOST_FOREACH(const string& token, tokenizer) {
        if (user.getUser() == token) {
            LOG_DEBUG_MSG(user.getUser() << " has " << authorityType << " authority" );
            return true;
        }

        if (user.isMember(token)) {
            LOG_DEBUG_MSG(user.getUser() << " in secondary group " << token << " has " << authorityType << " authority" );
            return true;
        }
    }
    return false;
}


void
getJobs(
        const BGQDB::filtering::JobFilter& filter,
        const BGQDB::filtering::JobSort& sort,
        const cxxdb::ConnectionPtr& conn_ptr,
        bgq::utility::Properties::ConstPtr properties,
        int64_t rowNumStart,
        int64_t rowNumEnd,
        uint64_t* row_count_out,
        cxxdb::ResultSetPtr* rs_ptr_out,
        bgq::utility::UserId::ConstPtr user_id_ptr
    )
{
    cxxdb::ParameterNames parameter_names;
    cxxdb::ResultSetPtr local_rs_ptr;
    ostringstream sql, whereClauseAddendum, jobIdList;
    string emptyUser, emptyWhereClauseAddendum;

    // Rules:
    // 1) If user is empty then no further security checking is necessary.
    // 2) If user is specified then check  the [security.jobs] section in the properties file to see if user has authority for jobs.
    //    A user with "read" or "all" authority can view all jobs.
    // 3) If user is specified and they don't have "read" or "all" authority in the properties file then the job security table needs
    //    to be checked. If no entries exist in the security table a fastpath step is followed where the username column in the job
    //    table is checked against the user.
    // 4) If user is specified and they don't have "read" or "all" authority in the properties file and the job security table has
    //    entries then the following addendum is added to the Where clause when searching for jobs:
    //    " id IN(<list of job ids from job security table>)"
    try {
        // An null UserId means no security filtering will be performed and all results will be returned.
        if (! user_id_ptr) {
            return BGQDB::filtering::getJobs(filter, sort, *conn_ptr, emptyUser, emptyWhereClauseAddendum,
                                             rowNumStart, rowNumEnd, row_count_out, rs_ptr_out);
        } else {
            LOG_DEBUG_MSG("Performing security filtering on jobs for user " << user_id_ptr->getUser());
            // First check if user has Read or All authority in [security.jobs] section of properties file
            if (hasJobAuthorityType(properties, "all", *user_id_ptr) || hasJobAuthorityType(properties, "read", *user_id_ptr)) {
                LOG_DEBUG_MSG("Property file settings allow user " << user_id_ptr->getUser() << " read authority to all jobs");
                return BGQDB::filtering::getJobs(filter, sort, *conn_ptr, emptyUser, emptyWhereClauseAddendum,
                                                 rowNumStart, rowNumEnd, row_count_out, rs_ptr_out);
            } else {
                // At this point user was specified but does not have read authority from properties file, so check job security table
                LOG_DEBUG_MSG("Property file settings do not allow user " << user_id_ptr->getUser() << " read authority to all jobs");
                // Check if filtering on specific job id
                BGQDB::filtering::JobFilter::Id jobId = filter.getJobId();
                // Id of 0 means no job id filtering so need to obtain a list of all jobs user is authorized to
                if (jobId == 0) {
                    // Perform a job security table lookup for user looking for entries with (R)ead authority
                    sql << "SELECT " << BGQDB::DBTJobsecurity::JOBID_COL << " FROM TBGQJobSecurity WHERE " <<
                           BGQDB::DBTJobsecurity::AUTHID_COL << "='" << user_id_ptr->getUser() << "' AND " <<
                           BGQDB::DBTJobsecurity::AUTHORITY_COL << "='R'";
                    cxxdb::QueryStatementPtr statement(conn_ptr->prepareQuery(
                            sql.str(),
                            parameter_names
                        ));
                    LOG_DEBUG_MSG("Checking job security table to find entries where user " << user_id_ptr->getUser() << " has read authority");
                    local_rs_ptr = statement->execute();
                    int fetchCount = 0;
                    while (local_rs_ptr->fetch()) {
                        if (fetchCount == 0) {
                            fetchCount++;
                            jobIdList << "'" << local_rs_ptr->columns()[BGQDB::DBTJobsecurity::JOBID_COL].as<long long int>() << "'";
                        } else {
                            jobIdList << "," << "'" << local_rs_ptr->columns()[BGQDB::DBTJobsecurity::JOBID_COL].as<long long int>() << "'";
                        }
                    }
                    // Check if we got any entries back
                    if (fetchCount == 0) {
                        // No entries for user in the job security table, just pass along the user (fastpath algorithm)
                        LOG_DEBUG_MSG("No entries in security table found so using fastpath algorithm");
                        return BGQDB::filtering::getJobs(filter, sort, *conn_ptr, user_id_ptr->getUser(), emptyWhereClauseAddendum,
                                                         rowNumStart, rowNumEnd, row_count_out, rs_ptr_out);
                    } else {
                        // Got matches for user in the security table so need to provide search addendum
                          LOG_DEBUG_MSG("Entries in security table found so building security addendum");
                          whereClauseAddendum << " id IN (" << jobIdList.str() << ")";
                          return BGQDB::filtering::getJobs(filter, sort, *conn_ptr, user_id_ptr->getUser(), whereClauseAddendum.str(),
                                                           rowNumStart, rowNumEnd, row_count_out, rs_ptr_out);
                    }
                } else {
                    // Just perform a job security table lookup for the single job Id to see if user has (R)ead authority
                    sql << "SELECT " << BGQDB::DBTJobsecurity::JOBID_COL << " FROM TBGQJobSecurity WHERE " <<
                          BGQDB::DBTJobsecurity::AUTHID_COL << "='" << user_id_ptr->getUser() << "' AND " <<
                          BGQDB::DBTJobsecurity::AUTHORITY_COL << "='R'" << " AND " <<
                          BGQDB::DBTJobsecurity::JOBID_COL << "=" << jobId;
                    cxxdb::QueryStatementPtr statement(conn_ptr->prepareQuery(
                            sql.str(),
                            parameter_names
                        ));
                    LOG_DEBUG_MSG("Checking job security table to see if user " << user_id_ptr->getUser() << " has read authority to job " << jobId);
                    local_rs_ptr = statement->execute();
                    if (local_rs_ptr->fetch()) {
                        // Got a match in the security table so need to provide search addendum
                        LOG_DEBUG_MSG("Entry in security table found so building security addendum");
                        whereClauseAddendum << " id IN ('" << jobId << "')";
                        return BGQDB::filtering::getJobs(filter, sort, *conn_ptr, user_id_ptr->getUser(), whereClauseAddendum.str(),
                                                         rowNumStart, rowNumEnd, row_count_out, rs_ptr_out);
                    } else {
                        // No read authority in security table, just pass along the user (fastpath algorithm)
                        LOG_DEBUG_MSG("No entry in security table found so using fastpath algorithm");
                        return BGQDB::filtering::getJobs(filter, sort, *conn_ptr, user_id_ptr->getUser(), emptyWhereClauseAddendum,
                                                         rowNumStart, rowNumEnd, row_count_out, rs_ptr_out);
                    }
                }
            }
        }
    } catch (const BGQDB::Exception& e) {
        // just re-throw it
        throw;
    } catch (const cxxdb::DatabaseException& e) {
        // just re-throw it
        throw;
    } catch (const std::invalid_argument& e) {
        // just re-throw it
        throw;
    } catch (const std::exception& e) {
        // just re-throw it
        throw;
    }
}


void
getJobs(
        const BGQDB::filtering::JobFilter& filter,
        const BGQDB::filtering::JobSort& sort,
        const cxxdb::ConnectionPtr& conn_ptr,
        bgq::utility::Properties::ConstPtr properties,
        int64_t rowNumStart,
        int64_t rowNumEnd,
        uint64_t* row_count_out,
        cxxdb::ResultSetPtr* rs_ptr_out,
        const string& user
    )
{
    bgq::utility::UserId::ConstPtr user_id_ptr;

    if ( user != string() ) {
        user_id_ptr.reset(new bgq::utility::UserId(user, true));
    }

    getJobs(
            filter,
            sort,
            conn_ptr,
            properties,
            rowNumStart,
            rowNumEnd,
            row_count_out,
            rs_ptr_out,
            user_id_ptr
        );

}

} } // namespace hlcs::security
