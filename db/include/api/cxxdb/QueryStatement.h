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

#ifndef CXXDB_QUERY_STATEMENT_H_
#define CXXDB_QUERY_STATEMENT_H_


#include "AbstractStatement.h"
#include "Columns.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>


namespace cxxdb {


/*! \brief A database query statement.
 *
 *  \section Usage
 *
 *  Create a QueryStatement using Connection::prepareQuery().
 *
 *  Set the value of any parameters by calling stmt_ptr->parameters()["col"].cast( 123 );
 *
 *  Call execute() which returns a ResultSetPtr, call methods on the ResultSet.
 *
 *  \section Invalidation
 *
 *  A Statement becomes invalid
 *  if the Connection that it was created from is invalidated or destroyed.
 *
 *  If a Statement is invalid then any methods called on it will throw InvalidObjectException.
 *
 */
class QueryStatement : public AbstractStatement, public boost::enable_shared_from_this<QueryStatement>
{
public:

    /*! \brief Prepare SQL for execution */
    void prepare(
            const std::string& sql,
            const ParameterNames& parameter_names = ParameterNames()
        );


    /*! \brief Execute the query.
     *
     *  Any existing ResultSet is invalidated when this is called.
     */
    ResultSetPtr execute();


private:

    typedef boost::shared_ptr<Columns> _ColumnsPtr;
    typedef boost::weak_ptr<ResultSet> _ResultSetWkPtr;


    _ResultSetWkPtr _result_set_ptr; // Keep a weak pointer to the result set so can invalidate it.
    _ColumnsPtr _columns_ptr; // Result set gets the columns from me.


    // Called by BasicConnection.
    QueryStatement(
            BasicConnectionPtr connection_ptr,
            ConnectionHandle& connection_handle,
            const std::string& sql,
            const ParameterNames& parameter_names
        );

    QueryStatement(
            BasicConnectionPtr connection_ptr,
            ConnectionHandle& connection_handle
        );


    // Called by BasicConnection, creates a ResultSet that retains a reference to me.
    ResultSetPtr _executeRetain();

    // Called by BasicConnection.
    void _invalidate(); // overrides

    bool _fetch(); // Called by ResultSet.
    const Columns& _columns() const  { return *_columns_ptr; } // Called by ResultSet.

    void _notifyResultSetDestroyed(); // Called by ResultSet.


    friend class BasicConnection;
    friend class ResultSet;
};

} // namespace cxxdb

#endif
