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

#ifndef CXXDB_ENVIRONMENT_H_
#define CXXDB_ENVIRONMENT_H_

#include "EnvironmentHandle.h"
#include "fwd.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/weak_ptr.hpp>

#include <boost/thread/mutex.hpp>

#include <sql.h>

#include <map>

namespace cxxdb {

/*! \brief A database environment, Connection factory.
 *
 *  An application typically only needs one Environment.
 *  This class provides an <i>application environment</i> to hold the
 *  Environment for the application.
 *
 *  Environment sets the handle's ODBC version to 3.
 *
 *  When the Environment is destroyed,
 *  any Connections created from it are invalidated.
 *
 */
class Environment : public boost::enable_shared_from_this<Environment>
{
public:


    /*! \brief Create an Environment */
    static EnvironmentPtr create();

    /*! \brief Initialize the application environment.
     *
     *  This function is not thread-safe
     *  so should be called early
     *  before the application is multi-threaded.
     *
     *  \post The application environment is initialized
     */
    static void initializeApplicationEnvironment();

    /*! \brief Get the application environment.
     *
     *  \pre The application environment must be initialized.
     *
     *  \throws logic_error if the application environment is not initialized.
     */
    static Environment& getApplicationEnvironment();

    /*! \brief Destroy the application environment.
     *
     *  This function is not thread-safe.
     *
     *  \pre The application environment is not initialized.
     */
    static void resetApplicationEnvironment();


    /*! \brief Create a new connection.
     *
     *  \param db_name DSN.
     *  \param user_name_p User name, may be NULL for no user.
     *  \param user_pwd_p User's password, may be NULL for no password.
     *  \param extra_connection_parameters Extra parameters put at the end of the connection string, NAME=VALUE pairs separated by ;.
     */
    ConnectionPtr newConnection(
            const std::string& db_name,
            const std::string* user_name_p = NULL,
            const std::string* user_pwd_p = NULL,
            const std::string& extra_connection_parameters = std::string()
        );

    /*! \brief Create a new connection.
     *
     *  \param db_name DSN.
     *  \param user_name User name.
     *  \param user_pwd User's password.
     *  \param extra_connection_parameters Extra parameters put at the end of the connection string, NAME=VALUE pairs separated by ;.
     */
    ConnectionPtr newConnection(
            const std::string& db_name,
            const std::string& user_name,
            const std::string& user_pwd,
            const std::string& extra_connection_parameters = std::string()
        );

    /*! \brief This is dangerous, don't call it. */
    EnvironmentHandle& handle()  { return _handle; }

    /*! \brief Applications must not call this function directly. */
    void notifyConnectionDestroyed( BasicConnection* conn_p );

    ~Environment();


private:

    typedef boost::weak_ptr<BasicConnection> _ConnWkPtr;

    typedef std::map<BasicConnection*,_ConnWkPtr> _ConnPtrs;

    typedef boost::mutex _Mutex;
    typedef boost::lock_guard<boost::mutex> _LockGuard;


    static EnvironmentPtr *_application_environment_ptr_p;


    EnvironmentHandle _handle;
    _Mutex _mtx;
    _ConnPtrs _conn_ptrs;


    Environment();
};


} // namespace cxxdb

#endif
