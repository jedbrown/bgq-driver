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

#ifndef CXXDB_ENVIRONMENT_HANDLE_H_
#define CXXDB_ENVIRONMENT_HANDLE_H_

#include <boost/utility.hpp>

#include <sql.h>

namespace cxxdb {

/*! \brief Wrapper around an ODBC enironment handle. */
class EnvironmentHandle : boost::noncopyable
{
public:

    /*! \brief Calls SQLAllocHandle. */
    EnvironmentHandle();


    /*! \brief Calls SQLSetEnvAttr. */
    void setAttribute(
            SQLINTEGER attr,
            SQLPOINTER value_ptr,
            SQLINTEGER string_length
        );

    /*! \brief Calls setAttribute with SQL_ATTR_ODBC_VERSION. */
    void setOdbcVersion( unsigned long odbc_version );

    /*! \brief Don't call this, it's dangerous. */
    SQLHANDLE& getSqlHandle()  { return _henv; }

    /*! \brief Calls SQLFreeHandle. */
    ~EnvironmentHandle();


private:

    SQLHANDLE _henv;

};

} // namespace cxxdb

#endif
