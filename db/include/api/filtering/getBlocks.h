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
 * \file api/filtering/getBlocks.h
 * \brief getBlocks definition.
 */

#ifndef BGQDB_FILTERING_GETBLOCKS_H_
#define BGQDB_FILTERING_GETBLOCKS_H_

#include <db/include/api/filtering/BlockFilter.h>
#include <db/include/api/filtering/BlockSort.h>

namespace BGQDB {
namespace filtering {

/*!
 * \brief Get compute blocks using filter.
 *
 * Retrieves the compute blocks matching the block filter criteria.
 *
 * \throws BGQDB::Exception - if error occurs connecting to the database
 * \throws cxxdb::DatabaseException - if error occurs accessing the database
 * \throws std::exception - if any type of unexpected error occurs
 *
 * \return Compute blocks matching the block filter criteria.
 */
cxxdb::ResultSetPtr getBlocks(
        const BGQDB::filtering::BlockFilter& filter, //!< [in]  Block filter to search on
        const BGQDB::filtering::BlockSort& sort,     //!< [in]  Blocks will be returned in this order
        cxxdb::Connection& conn                      //!< [in]  Database connection to use for query
        );

} // filtering
} // BGQDB

#endif


