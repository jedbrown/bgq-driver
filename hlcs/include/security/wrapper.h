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

#ifndef BGQ_HLCS_SECURITY_WRAPPER_H_
#define BGQ_HLCS_SECURITY_WRAPPER_H_
/*!
 * \file hlcs/include/security/wrapper.h
 * \brief wrapper methods used to enforce consistent privileges are used for validation.
 */

#include "Enforcer.h"

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/GenBlockParams.h>

#include <db/include/api/filtering/JobFilter.h>
#include <db/include/api/filtering/JobSort.h>

#include <utility/include/Properties.h>
#include <utility/include/UserId.h>

#include <string>


namespace hlcs {
namespace security {


/*!
 *
 * \brief Wrapper for BGQDB::filtering::getJobs(). Retrieves jobs matching the filter and security criteria.
 *
 * \throws BGQDB::Exception - if error occurs connecting to the database
 * \throws cxxdb::DatabaseException - if error occurs accessing the database
 * \throws std::invalid_argument - if starting or ending row argument is out of range
 * \throws std::exception - if any type of unexpected error occurs
 *
 * \return Jobs matching the job filter criteria in a result set and optionally the number of rows.
 */
void
getJobs(
        const BGQDB::filtering::JobFilter& filter,      //!< [in] Job filter to search on
        const BGQDB::filtering::JobSort& sort,          //!< [in] Jobs will be returned in this order
        const cxxdb::ConnectionPtr& conn_ptr,           //!< [in] Database connection pointer
        bgq::utility::Properties::ConstPtr properties,  //!< [in] bg.properties pointer
        int64_t rowNumStart,                            //!< [in] Starting row to return in result set (0 returns all)
        int64_t rowNumEnd,                              //!< [in] Ending row to return in result set
        uint64_t* row_count_out,                        //!< [in/out] If not NULL, will return row count from query
        cxxdb::ResultSetPtr* rs_ptr_out,                //!< [out] Job query result set
        bgq::utility::UserId::ConstPtr user_id_ptr      //!< [in] Check read authority to jobs for user, null indicates don't check authority.
            = bgq::utility::UserId::ConstPtr()
);


/*!
 *  See the other getJobs, this one just calls that after converting user string to UserId::ConstPtr.
 */
void
getJobs(
        const BGQDB::filtering::JobFilter& filter,      //!< [in] Job filter to search on
        const BGQDB::filtering::JobSort& sort,          //!< [in] Jobs will be returned in this order
        const cxxdb::ConnectionPtr& conn_ptr,           //!< [in] Database connection pointer
        bgq::utility::Properties::ConstPtr properties,  //!< [in] bg.properties pointer
        int64_t rowNumStart,                            //!< [in] Starting row to return in result set (0 returns all)
        int64_t rowNumEnd,                              //!< [in] Ending row to return in result set
        uint64_t* row_count_out,                        //!< [in/out] If not NULL, will return row count from query
        cxxdb::ResultSetPtr* rs_ptr_out,                //!< [out] Job query result set
        const std::string& user                         //!< [in] Check read authority to jobs for user
);


} } // namespace hlcs::security

#endif
