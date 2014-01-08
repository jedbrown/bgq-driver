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

#include <bgq_util/include/Location.h>

#include <db/include/api/job/types.h>

#include <vector>

#ifndef DB_API_JOB_SUB_BLOCK_RAS_H
#define DB_API_JOB_SUB_BLOCK_RAS_H

namespace BGQDB {
namespace job {

/*!
 * \brief Find a job ID based on a block ID and node location.
 *
 * If no job is found using the location, 0 is returned.
 */
std::vector<Id>
subBlockRas(
        const std::string& block,               //!< [in]
        const bgq::util::Location& node         //!< [in]
    );

} // job
} // BGQDB


#endif
