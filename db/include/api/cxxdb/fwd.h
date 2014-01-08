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

#ifndef CXXDB_FWD_H_
#define CXXDB_FWD_H_

/*! \file
 *  \brief Forward declarations for cxxdb types and functions for use in header files using cxxdb.
 */

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

#include <stdint.h>

namespace cxxdb {

typedef std::vector<std::string> ParameterNames; //!< Type for parameter names, see Parameters. */
typedef std::vector<uint8_t> Bytes; //!< \brief Type for a bunch of bytes, as in FOR BIT DATA.


class AbstractStatement;

typedef boost::shared_ptr<AbstractStatement> AbstractStatementPtr;


class BasicConnection;

typedef boost::shared_ptr<BasicConnection> BasicConnectionPtr;


class Columns;


class Connection;

typedef boost::shared_ptr<Connection> ConnectionPtr;


class Environment;

typedef boost::shared_ptr<Environment> EnvironmentPtr;


class Parameters;


class QueryStatement;

typedef boost::shared_ptr<QueryStatement> QueryStatementPtr;


class ResultSet;

typedef boost::shared_ptr<ResultSet> ResultSetPtr;


class Statement;

typedef boost::shared_ptr<Statement> StatementPtr;


class Transaction;

typedef boost::shared_ptr<Transaction> TransactionPtr;


class UpdateStatement;

typedef boost::shared_ptr<UpdateStatement> UpdateStatementPtr;


} // namespace cxxdb

/*! \namespace cxxdb
 *
 *  \brief C++ wrapper around unixODBC.
 *
 *  To use this library, you should know something about ODBC:
 *  - DB2 has good documentation: http://publib.boulder.ibm.com/infocenter/db2luw/v9r7/index.jsp?topic=/com.ibm.db2.luw.apdv.cli.doc/doc/c0007944.html
 *  - unixODBC: http://www.unixodbc.org/doc/ProgrammerManual/Tutorial/
 *
 *  \section Usage
 *
 *  You'll need an Environment.
 *  Use Environment::initializeApplicationEnvironment().
 *
 *  Next, you'll need a Connection.
 *  Connections should not be shared between threads.
 *  Get connections from a ConnectionPool.
 *
 *  With a Connection,
 *  you can execute simple SQL statements,
 *  perform simple SQL queries,
 *  and prepare SQL statements.
 *
 *
 *  \section Exceptions
 *
 *  Most operations use SQL APIs,
 *  which return SQLRETURN codes and provide extra error information in diagnostic records.
 *  cxxdb wrappers these functions and if a failure occurs
 *  gathers diagnostic information
 *  and throws a DatabaseException.
 *
 *  Parameters and Columns provide setters and getters that are valid only for
 *  certain field types.
 *  If the setter or getter isn't valid for the field type then WrongType is thrown.
 *
 *  A runtime check is made when assigning a string to a Parameter
 *  to ensure the string isn't bigger than the field.
 *  There are other times this check is made.
 *  If the string is too long or value is too big then ValueTooBig is thrown.
 *
 *  \sa \ref Invalidation
 *
 *  \section Invalidation
 *
 *  A Connection is created from an Environment and a Statement is created from a Connection.
 *  If the Environment goes away before the Connection the Connection is invalidated, and
 *  if a Connection goes away or is invalidated before a Statement the Statement is invalidated.
 *  If a method is called on an invalid object then InvalidObjectException is thrown.
 *
 */

#endif
