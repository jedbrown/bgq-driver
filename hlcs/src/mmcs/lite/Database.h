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

#ifndef MMCS_LITE_DATABASE_H
#define MMCS_LITE_DATABASE_H

#include <string>

#include <sqlite3.h>

namespace mmcs {
namespace lite {

/*!
 * \brief RAII wrapper around sqlite database
 */
class Database
{
public:
    /*!
     * \brief opens the database connection
     *
     * Uses the sqlite value from the mmcs_lite section in bg.properties by default
     */
    explicit Database(
            const std::string& name = std::string()     //!< [in]
            );

    /*!
     * \brief get database handle
     */
    sqlite3* getHandle() { return _handle; }

    /*!
     * \brief closes the database connection
     */
    ~Database();

private:
    sqlite3* _handle;
};

} } // namespace mmcs::lite

#endif
