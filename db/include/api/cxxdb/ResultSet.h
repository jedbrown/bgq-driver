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

#ifndef CXXDB_RESULT_SET_H_
#define CXXDB_RESULT_SET_H_

#include "fwd.h"

#include <boost/weak_ptr.hpp>

namespace cxxdb {

/*! \brief Query results.
 *
 *  \section Usage
 *
 *  Create a ResultSet using Connection::query() or QueryStatement::execute().
 *
 *  Call fetch() in a loop, checking if it returns true.
 *
 *  Get the value of the result columns by calling rs_ptr->columns()["col"].as<uint32_t>().
 *
 *  \section Invalidation
 *
 *  The ResultSet is invalidated if the QueryStatement it was created from is destroyed or invalidated.
 *  The ResultSet is also invalidated if another execute() is done on the QueryStatement that it was created from.
 *
 */
class ResultSet
{
public:

    /*! \brief Fetch the next row in the result.
     *
     *  \return true if there is a next row and false if there is no next row.
     */
    bool fetch();

    /*! \brief Access the Columns. */
    const Columns& columns() const;


    /*! \brief Internalize the statement pointer.
     *
     *  This ResultSet stores a ref to the statement pointer so that this ResultSet won't be invalidated.
     *  This is useful if returning a ResultSet from a function.
     */
    void internalize( QueryStatementPtr stmt_ptr ) { _qstmt_retained_ptr = stmt_ptr; }


    /*! \brief Test validity.
     *
     *  If is valid, may become invalid just after calling, so not terribly useful other than for testing.
     */
    bool isValid()  { return bool(_qstmt_ptr.lock()); }


    ~ResultSet();


private:

    typedef boost::weak_ptr<QueryStatement> _QstmtWkPtr;


    _QstmtWkPtr _qstmt_ptr; // Keep a weak pointer to the statement so can notify it when ResultSet destroyed.
    QueryStatementPtr _qstmt_retained_ptr; // Only set if constructed with retain=true, or if called internalize.


    ResultSet(
            QueryStatementPtr qstmt_ptr,
            bool retain
        );


    void _invalidate(); // Called by QueryStatement.


    friend class QueryStatement;
};


}

#endif
