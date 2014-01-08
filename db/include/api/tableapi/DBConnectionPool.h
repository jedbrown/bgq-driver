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

#ifndef DBCONNECTIONPOOL
#define DBCONNECTIONPOOL

#include "DBConnection.h"

#include "../cxxdb/ConnectionPool.h"
#include "../cxxdb/fwd.h"

#include <utility/include/Singleton.h>
#include <utility/include/Properties.h>

#include <boost/shared_ptr.hpp>

#include <boost/thread/mutex.hpp>

#include <vector>
#include <string>

namespace BGQDB {

/*!
 * \brief Database connection pool.
 */
class DBConnectionPool : public bgq::utility::Singleton<DBConnectionPool, bgq::utility::CreateStatic>
{
public:
    static const unsigned DefaultSize; //!< Default number of connections in the pool if not provided explicitly or from a properties file.

public:
    /*!
     * \brief Initialize the connection pool.
     *
     * \copydetails DBConnectionPool(bgq::utility::Properties::ConstPtr,const std::string&)
     */
    static void init(
            const bgq::utility::Properties::ConstPtr properties,    //!< [in]
            const std::string& section_name                         //!< [in]
            );

    /*!
     * \brief Initialize the connection pool.
     *
     * \copydetails DBConnectionPool(bgq::utility::Properties::ConstPtr,const std::string&)
     */
    static void init(
            const bgq::utility::Properties::ConstPtr properties,    //!< [in]
            unsigned size                                           //!< [in]
            );

    DBConnection::Ptr checkout();

    /*! \brief Get a pooled connection.
     *
     *  \throw BGQDB::Exception with CONNECTION_ERROR if an error occurred.
     */
    cxxdb::ConnectionPtr getConnection();

    unsigned availableCount() const { return _connection_pool_ptr->getStatus().connsAvailable; }
    unsigned usedCount() const      { return _connection_pool_ptr->getStatus().connsInUse; }
    unsigned maxCount() const      { return _connection_pool_ptr->getStatus().connsMax; }
    unsigned size() const  { return _size;}
    bgq::utility::Properties::ConstPtr getProperties() const { return _properties; }

    /*!
     * \brief dtor.
     */
    ~DBConnectionPool();

protected:
    bgq::utility::Properties::ConstPtr    _properties;
    boost::shared_ptr<cxxdb::ConnectionPool> _connection_pool_ptr;
    unsigned                              _size;
    std::string                           _extraConnectionParameters;

    /*!
     * \brief ctor.
     *
     * Obtains the connection_pool_size value from the requested section of the
     * properties file. If not found, or outside the acceptable range, the default value is used.
     *
     * The minimum connections will be 0 and the maximum connections will be the connection_pool_size.
     *
     * \throws std::invalid_argument if properties == NULL
     */
    DBConnectionPool(
            const bgq::utility::Properties::ConstPtr properties,    //!< [in]
            const std::string& section_name                         //!< [in]
            );

    /*!
     * \brief ctor.
     *
     * \throws std::invalid_argument if properties == NULL
     */
    DBConnectionPool(
            const bgq::utility::Properties::ConstPtr properties,
            unsigned size
            );

private:
    void initCxxdbPool();
};

} // BGQDB

#endif
