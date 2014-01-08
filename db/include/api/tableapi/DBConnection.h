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

#ifndef DB_CONNECTION_H_
#define DB_CONNECTION_H_

#include "utilcli.h"

#include "../cxxdb/fwd.h"

#include <utility/include/Properties.h>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace BGQDB {

/*!
 * \brief
 */
class DBConnection : boost::noncopyable
{
public:
    /*!
     * \brief
     */
    typedef boost::shared_ptr<DBConnection> Ptr;


    static cxxdb::ConnectionPtr createConnection(
            const bgq::utility::Properties& properties,
            const std::string& extra_connection_parameters
        );


    /*!
     * \brief ctor.
     */
    DBConnection(
            bgq::utility::Properties::ConstPtr properties,
            const std::string& extra_connection_parameters
            );

    DBConnection(
            bgq::utility::Properties::ConstPtr properties,
            const std::string& extra_connection_parameters,
            cxxdb::ConnectionPtr conn_ptr
        );

    ~DBConnection();

    SQLRETURN commit();
    SQLRETURN connect();
    SQLRETURN disconnect();
    SQLRETURN rollback();

    SQLHANDLE getConnHandle();
    SQLHANDLE getEnvHandle();
    SQLRETURN getStatus(); // return the connection's status
    bool isAutoCommit();
    SQLRETURN setAutoCommit(bool enableAutoCommit);

protected:
    bgq::utility::Properties::ConstPtr _properties;
    std::string _extra_connection_parameters;
    cxxdb::TransactionPtr _transaction_ptr;
    cxxdb::ConnectionPtr _connection_ptr;

protected:
    SQLRETURN                     setSchema(const char* schema);
};

} // BGQDB

#endif
