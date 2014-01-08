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

#ifndef CXXDB_CONNECTION_HANDLE_H_
#define CXXDB_CONNECTION_HANDLE_H_

#include <boost/utility.hpp>

#include <sql.h>

#include <string>

namespace cxxdb {

class EnvironmentHandle;

/*! \brief Wrapper around an ODBC connection handle. */
class ConnectionHandle : boost::noncopyable
{
public:

    ConnectionHandle( EnvironmentHandle& env_handle );

    /*! \brief Calls SQLSetConnectAttr */
    void setAttribute(
            SQLINTEGER attr,
            SQLPOINTER value_ptr,
            SQLINTEGER string_len
        );

    /*! \brief Calls SQLConnect. */
    void connect(
            const std::string& db_name,
            const std::string* user_name_p = NULL,
            const std::string* pwd_p = NULL
        );

    /*! \brief A handy way to connect. */
    void connect(
            const std::string& db_name,
            const std::string& user_name,
            const std::string& pwd
        )
    { connect( db_name, &user_name, &pwd ); }

    /*! \brief Calls SQLDriverconnect.
     *  \return the output connection string, to be used on subsequent connection requests. */
    std::string driverConnect(
            const std::string& connection_string
        );

    /*! \brief Sets autocommit ON. */
    void setCommitAutomatically();

    /*! \brief Sets autocommit OFF. */
    void setCommitRequired();

    /*! \brief Calls SQLEndTran. */
    void endTransaction(
            SQLSMALLINT completion_type
        );

    /*! \brief Calls SQLDisconnect. */
    void disconnect();


    /*! \brief Don't call this, it's dangerous. */
    SQLHANDLE& getSqlHandle()  { return _hdbc; }

    /*! \brief Calls SQLFreeHandle. */
    ~ConnectionHandle();


private:
    SQLHANDLE _hdbc;

};


} // namespace cxxdb

#endif
