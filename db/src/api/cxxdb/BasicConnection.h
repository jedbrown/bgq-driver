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

#ifndef CXXDB_BASIC_CONNECTION_H_
#define CXXDB_BASIC_CONNECTION_H_

#include "cxxdb/Connection.h"
#include "cxxdb/fwd.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <map>
#include <string>

namespace cxxdb {

class ConnectionHandle;
class EnvironmentHandle;

/*! \brief A real connection to the database. */
class BasicConnection : public Connection, public boost::enable_shared_from_this<BasicConnection>
{
public:
    BasicConnection(
            EnvironmentPtr env_ptr,
            EnvironmentHandle& env_handle,
            const std::string& db_name,
            const std::string* user_name_p = NULL,
            const std::string* user_pwd_p = NULL,
            const std::string& extra_connection_parameters = std::string()
        );

    void execute(
            const std::string& sql
        );

    void executeUpdate(
            const std::string& sql,
            uint64_t* affected_row_count_out
        );

    ResultSetPtr query(
            const std::string& sql
        );

    StatementPtr prepare(
            const std::string& sql,
            const ParameterNames& parameter_names
        );

    UpdateStatementPtr prepareUpdate(
            const std::string& sql,
            const ParameterNames& parameter_names
        );

    QueryStatementPtr prepareQuery(
            const std::string& sql,
            const ParameterNames& parameter_names
        );

    QueryStatementPtr createQuery();

    void startTransaction();
    void commit();
    void rollback();
    void endTransaction();

    ConnectionHandle& handle()  { return *_handle_ptr; }
    Environment& environment()  { return *_env_ptr.lock(); }

    // If is valid, may become invalid just after calling, so not terribly useful other than for testing.
    bool isValid() const  { return bool(_handle_ptr); }

    void invalidate();

    void notifyStatementDestroyed( AbstractStatement* stmt_p );

    ~BasicConnection();


private:

    typedef boost::shared_ptr<ConnectionHandle> HandlePtr;
    typedef boost::weak_ptr<Environment> _EnvWkPtr;
    typedef boost::weak_ptr<AbstractStatement> _StmtWkPtr;
    typedef std::map<AbstractStatement*,_StmtWkPtr> _StmtPtrs;


    _EnvWkPtr _env_ptr;
    HandlePtr _handle_ptr;
    _StmtPtrs _stmt_ptrs;

    bool _transaction;


    void _invalidate();

};


} // namespace cxxdb


#endif
