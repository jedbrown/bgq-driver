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
 * \file api/filtering/getJobs.h
 * \brief getJobs definition.
 */

#ifndef BGQDB_FILTERING_GETJOBS_H_
#define BGQDB_FILTERING_GETJOBS_H_


#include "JobFilter.h"
#include "JobSort.h"

#include "../cxxdb/fwd.h"

#include <string>

#include <stdint.h>


namespace BGQDB {
namespace filtering {

/*!
 * \brief Get jobs using job filter.
 *
 * Retrieves jobs matching the job filter criteria.
 *
 * \throws BGQDB::Exception - if error occurs connecting to the database
 * \throws cxxdb::DatabaseException - if error occurs accessing the database
 * \throws std::invalid_argument - if starting or ending row argument is out of range
 * \throws std::exception - if any type of unexpected error occurs
 *
 * \return Jobs matching the job filter criteria in a result set and optionally the number of rows.
 */
void getJobs(
        const JobFilter& filter,                        //!< [in] Job filter to search on
        const JobSort& sort,                            //!< [in] Jobs will be returned in this order
        cxxdb::Connection& conn,                        //!< [in] Database connection to use for query
        const std::string& user,                        //!< [in] User for security validation
        const std::string& securityWhereClauseAddendum, //!< [in] Security Where clause addendum
        int64_t rowNumStart,                            //!< [in] Starting row to return in result set (0 returns all)
        int64_t rowNumEnd,                              //!< [in] Ending row to return in result set
        uint64_t* row_count_out,                        //!< [in/out] If not NULL, will return row count from query
        cxxdb::ResultSetPtr* rs_ptr_out                 //!< [out] Job query result set
        );

} // filtering
} // BGQDB

#endif
