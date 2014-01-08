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
#ifndef RUNJOB_SERVER_DATABASE_HANDLE_H
#define RUNJOB_SERVER_DATABASE_HANDLE_H

#include <db/include/api/cxxdb/cxxdb.h>

namespace runjob {
namespace server {
namespace database {

/*!
 * \brief Generic database handle containing connection and statement objects.
 */
class Handle
{
public:
    /*!
     * \brief ctor.
     */
    Handle() :
        _connection(),
        _statement()
    {

    }

protected:
    cxxdb::ConnectionPtr _connection;       //!< database connection
    cxxdb::QueryStatementPtr _statement;    //!< database prepared statement
};

} // database
} // server
} // runjob
 
#endif
