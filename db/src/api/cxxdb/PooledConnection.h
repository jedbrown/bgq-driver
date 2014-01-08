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

#ifndef CXXDB_POOLED_CONNECTION_H_
#define CXXDB_POOLED_CONNECTION_H_

#include "cxxdb/Connection.h"

#include <boost/shared_ptr.hpp>

namespace cxxdb {

class ConnectionPool;

/*! \brief Delegates calls to Connection, destructor returns the connection to the ConnectionPool. */
class PooledConnection : public Connection
{
public:

    typedef boost::shared_ptr<PooledConnection> Ptr;


    PooledConnection(
            ConnectionPtr conn_ptr,
            ConnectionPool& pool
        );

    void execute(
            const std::string& sql
        )
    { _conn_ptr->execute( sql ); }

    void executeUpdate(
            const std::string& sql,
            uint64_t* affected_row_count_out
        )
    { _conn_ptr->executeUpdate( sql, affected_row_count_out ); }

    ResultSetPtr query(
            const std::string& sql
        )
    { return _conn_ptr->query( sql ); }

    StatementPtr prepare(
            const std::string& sql,
            const ParameterNames& parameter_names
        )
    { return _conn_ptr->prepare( sql, parameter_names ); }

    UpdateStatementPtr prepareUpdate(
            const std::string& sql,
            const ParameterNames& parameter_names
        )
    {  return _conn_ptr->prepareUpdate( sql, parameter_names ); }

    QueryStatementPtr prepareQuery(
            const std::string& sql,
            const ParameterNames& parameter_names
        )
    {  return _conn_ptr->prepareQuery( sql, parameter_names ); }

    QueryStatementPtr createQuery()
    { return _conn_ptr->createQuery(); }

    void startTransaction()  { _conn_ptr->startTransaction(); }
    void commit()  { _conn_ptr->commit(); }
    void rollback()  { _conn_ptr->rollback(); }
    void endTransaction()  { _conn_ptr->endTransaction(); }

    ConnectionHandle& handle()  { return _conn_ptr->handle(); }
    Environment& environment() { return _conn_ptr->environment(); }

    bool isValid() const  { return _conn_ptr->isValid(); }


    ~PooledConnection();


private:

    ConnectionPtr _conn_ptr;
    ConnectionPool &_pool;
};

} // namespace cxxdb

#endif
