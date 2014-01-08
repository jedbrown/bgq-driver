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

#ifndef CXXDB_STATEMENT_H_
#define CXXDB_STATEMENT_H_


#include "AbstractStatement.h"


namespace cxxdb {


/*! \brief A database statement for DDL-type statements (ALTER, CREATE, DROP, GRANT, REVOKE, SET, etc.).
 *
 *  \section Usage
 *
 *  Create a Statement using Connection::prepare().
 *
 *  Call execute().
 *
 *  \section Invalidation
 *
 *  A Statement becomes invalid
 *  if the Connection that it was created from is invalidated or destroyed.
 *
 *  If a Statement is invalid then any methods called on it will throw InvalidObjectException.
 *
 */
class Statement : public AbstractStatement
{
public:

    /*! \brief Execute the Statement.
     */
    void execute();


private:

    /*! \brief Called by BasicConnection::prepare(). */
    Statement(
            BasicConnectionPtr connection_ptr,
            ConnectionHandle& connection_handle,
            const std::string& sql,
            const ParameterNames& parameter_names
        );


    friend class BasicConnection;
};

} // namespace cxxdb

#endif
