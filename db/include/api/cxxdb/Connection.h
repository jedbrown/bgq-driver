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

#ifndef CXXDB_CONNECTION_H_
#define CXXDB_CONNECTION_H_


#include "fwd.h"

#include <string>
#include <vector>


namespace cxxdb {


class ConnectionHandle;


/*! \brief Interface for a connection to a database.
 *
 *  \section Invalidation
 *
 *  A Connection becomes invalid
 *  if the Environment that it was created from is destroyed.
 *
 *  If a Connection is invalid then any methods called on it will throw InvalidObjectException.
 *
 *  When a Connection is invalidated or destroyed,
 *  any Statements created from it are invalidated.
 *
 *  \section Transactions
 *
 *  A Connection can have either no transaction
 *  or a single transaction on it.
 *  A Connection starts out with no transaction.
 *  A transaction is started by calling startTransaction()
 *  and ended by calling endTransaction().
 *  For convenience, the Transaction class is provided
 *  to guard starting and ending a transaction on a Connection.
 *
 *  When there is no transaction on the Connection,
 *  any operations are committed automatically.
 *  When a transaction is started on a connection,
 *  operations are not committed unless commit() is called
 *  and can be rolled back using rollback().
 *
 */
class Connection
{
public:

    static const ParameterNames NoParameterNames; //!< Use if don't want to supply parameter names to prepare.


    /*! \brief Create a new connection using the application environment.
     *
     *  \see Environment::initializeApplicationEnvironment()
     *
     *  \param db_name DSN.
     *  \param user_name_p User name, may be NULL for no user.
     *  \param user_pwd_p User's password, may be NULL for no password.
     *  \param extra_connection_parameters Extra parameters put at the end of the connection string, NAME=VALUE pairs separated by ;.
     */
    static ConnectionPtr create(
            const std::string& db_name,
            const std::string* user_name_p = NULL,
            const std::string* user_pwd_p = NULL,
            const std::string& extra_connection_parameters = std::string()
        );

    /*! \brief Create a new connection using the application environment.
     *
     *  \see Environment::initializeApplicationEnvironment()
     *
     *  \param db_name DSN.
     *  \param user_name User name.
     *  \param user_pwd User's password.
     *  \param extra_connection_parameters Extra parameters put at the end of the connection string, NAME=VALUE pairs separated by ;.
     */
    static ConnectionPtr create(
            const std::string& db_name,
            const std::string& user_name,
            const std::string& user_pwd,
            const std::string& extra_connection_parameters = std::string()
        );

    /*! \brief Execute the DDL statement.
     *
     *  Use for DDL-type statements (ALTER, CREATE, DROP, GRANT, etc.).
     */
    virtual void execute(
            const std::string& sql
        ) =0;

    /*! \brief Execute the update-style statement.
     *
     *  Use for update statements (UPDATE, DELETE, INSERT, etc.).
     */
    virtual void executeUpdate(
            const std::string& sql,
            uint64_t* affected_row_count_out = NULL
        ) =0;


    /*! \brief Execute a query.
     *
     *  This is useful for simple queries, but doesn't allow bound parameters.
     */
    virtual ResultSetPtr query(
            const std::string& sql
        ) =0;


    /*! \brief Prepare the DDL statement.
     *
     *  Call to prepare a statement to be used for executing DDL-type statements (ALTER, CREATE, DROP, GRANT, etc.).
     *
     *  Assign names to any parameters in the sql
     *  by passing in parameter_names.
     */
    virtual StatementPtr prepare(
            const std::string& sql,
            const ParameterNames& parameter_names = NoParameterNames
        ) =0;

    /*! \brief Prepare an update-type statement.
     *
     *  Call to prepare a statement to be used for executing update-type statements (INSERT, UPDATE, DELETE, etc.).
     *
     *  Assign names to any parameters in the sql
     *  by passing in parameter_names.
     */
    virtual UpdateStatementPtr prepareUpdate(
            const std::string& sql,
            const ParameterNames& parameter_names = NoParameterNames
        ) =0;

    /*! \brief Prepare a SELECT statement.
     *
     *  Call to prepare a statement to be used for executing SELECT statements.
     *
     *  Assign names to any parameters in the sql
     *  by passing in parameter_names.
     */
    virtual QueryStatementPtr prepareQuery(
            const std::string& sql,
            const ParameterNames& parameter_names = NoParameterNames
        ) =0;

    /*! \brief Create a statement for queries, you can prepare it later. */
    virtual QueryStatementPtr createQuery() =0;

    /*! \brief Start a transaction.
     *
     *  \pre A transaction isn't already started on the Connection.
     *
     *  \post A transaction is started on the Connection.
     *
     *  \see Transaction
     *
     *  \throws logic_error If a transaction is already started on the Connection.
     */
    virtual void startTransaction() =0;

    /*! \brief Commit the current transaction. */
    virtual void commit() =0;

    /*! \brief Roll-back the current transaction. */
    virtual void rollback() =0;

    /*! \brief End the transaction.
     *
     *  Rolls back any uncommitted operations on the connection.
     *
     *  \post A transaction is not started on the Connection.
     */
    virtual void endTransaction() =0;

    /*! \brief Get the Environment for this Connection. */
    virtual Environment& environment() =0;

    /*! \brief Don't call this. */
    virtual ConnectionHandle& handle() =0;


    /*! \brief Check if the Connection is valid.
     *
     *  If is valid, may become invalid just after calling, so not terribly useful other than for testing.
     */
    virtual bool isValid() const =0;


    virtual ~Connection();

};

} // namespace cxxdb


#endif
