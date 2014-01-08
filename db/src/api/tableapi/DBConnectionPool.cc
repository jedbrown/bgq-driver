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

#include "Exception.h"

#include "tableapi/DBConnectionPool.h"
#include "tableapi/DBConnection.h"

#include "cxxdb/Connection.h"
#include "cxxdb/Environment.h"
#include "cxxdb/exceptions.h"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <memory>
#include <stdexcept>
#include <string>

using boost::shared_ptr;

using std::exception;
using std::string;

LOG_DECLARE_FILE( "database" );

namespace BGQDB {

const unsigned DBConnectionPool::DefaultSize = 10;


void
DBConnectionPool::init(
        const bgq::utility::Properties::ConstPtr properties,
        const std::string& section
    )
{
    cxxdb::Environment::initializeApplicationEnvironment();

    // create connection pool
    std::auto_ptr<DBConnectionPool> pool(
            new DBConnectionPool(properties, section)
            );

    // set as the instance
    try {
        DBConnectionPool::setInstance( pool.get() );

        // release auto_ptr control 
        pool.release();
    } catch ( const std::invalid_argument& e ) {
        LOG_WARN_MSG( "could not set instance. Has library already been initialized?" );
        throw std::logic_error( "init" );
    }
}

void
DBConnectionPool::init(
        const bgq::utility::Properties::ConstPtr properties,
        unsigned size
    )
{
    cxxdb::Environment::initializeApplicationEnvironment();

    // create connection pool
    std::auto_ptr<DBConnectionPool> pool(
            new DBConnectionPool(properties, size)
            );

    // set as the instance
    try {
        DBConnectionPool::setInstance( pool.get() );

        // release auto_ptr control 
        pool.release();
    } catch ( const std::invalid_argument& e ) {
        LOG_WARN_MSG( "could not set instance. Has library already been initialized?" );
        throw std::logic_error( "init" );
    }
}

DBConnectionPool::DBConnectionPool(
        const bgq::utility::Properties::ConstPtr properties,
        const std::string& section_name
        ) :
    _properties(properties),
    _size(DefaultSize)
{
    if ( ! _properties ) {
        LOG_ERROR_MSG( "DBConnection pool initialized incorrectly with null properties!" );
        throw std::invalid_argument( "properties" );
    }


    // get value from properties

    try {
        uint32_t connection_pool_size(boost::lexical_cast<uint32_t>(_properties->getValue( section_name, "connection_pool_size")));
            // Not checking for lexical_cast("-1") since will check size isn't too big next.

        _size = connection_pool_size;
    } catch ( std::exception& e ) {
        LOG_WARN_MSG( "Didn't find connection_pool_size in " << section_name << " section of properties file, will use default" );
        _size = DefaultSize;
    }

    try {
        _extraConnectionParameters = _properties->getValue( section_name, "extra_connection_parameters");
    } catch ( const std::exception& e ) {
        LOG_DEBUG_MSG( "no extra_connection_parameters key in " << section_name << " section of properties file, no extra connection parameters" );
    }


    // initialize cxxdb pool
    this->initCxxdbPool();
}

DBConnectionPool::DBConnectionPool(
        const bgq::utility::Properties::ConstPtr properties,
        unsigned size
    ) :
    _properties(properties),
    _size(size)
{
    if ( ! _properties ) {
        LOG_ERROR_MSG( "DBConnection pool initialized incorrectly with null properties!" );
        throw std::invalid_argument( "properties" );
    }

    // init pool
    this->initCxxdbPool();
}


void
DBConnectionPool::initCxxdbPool()
{
    LOG_DEBUG_MSG( "using " << _size << " for pool size" );
    try {
        _connection_pool_ptr.reset( 
                new cxxdb::ConnectionPool(
                    boost::bind(
                        &DBConnection::createConnection, 
                        boost::ref( *_properties ),
                        boost::ref( _extraConnectionParameters )
                        ),
                    _size
                    )
                );
    } catch ( std::exception& e ) {
        LOG_ERROR_MSG( "Failed to initialize the DB connection pool, " << e.what() );
    }
}

DBConnectionPool::~DBConnectionPool()
{
    LOG_DEBUG_MSG( "Destroying connection pool." );
}

DBConnection::Ptr
DBConnectionPool::checkout()
{
    try {
        cxxdb::ConnectionPtr conn_ptr(getConnection());
        DBConnection::Ptr ret( new DBConnection( _properties, _extraConnectionParameters, conn_ptr ) );
        return ret;
    } catch ( cxxdb::DatabaseException& dbe ) {
        LOG_ERROR_MSG( "Error checking out connection from connection pool, " << dbe.what() );
    } catch ( std::exception& e ) {
        LOG_ERROR_MSG( "Error checking out connection from connection pool, " << e.what() );
    } catch ( ... ) {
        LOG_ERROR_MSG( "Unexpected exception checking out connection from connection pool." );
    }

    return DBConnection::Ptr();
}

cxxdb::ConnectionPtr
DBConnectionPool::getConnection()
{
    if ( ! _connection_pool_ptr ) {
        BOOST_THROW_EXCEPTION( BGQDB::Exception( BGQDB::CONNECTION_ERROR, "connection pool not initialized" ) );
        return cxxdb::ConnectionPtr(); // here because compiler complains that throw might not happen
    }

    try {
        return _connection_pool_ptr->getConnection();
    } catch ( std::exception& e ) {
        BOOST_THROW_EXCEPTION( BGQDB::Exception(
                BGQDB::CONNECTION_ERROR,
                string() + "failed to get a connection from the connection pool, " + e.what()
            ) );
        return cxxdb::ConnectionPtr(); // here because compiler complains that throw might not happen
    }
}

} // BGQDB
