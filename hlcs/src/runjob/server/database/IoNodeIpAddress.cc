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
#include "server/database/IoNodeIpAddress.h"

#include "common/error.h"
#include "common/Exception.h"
#include "common/logging.h"

#include <control/include/bgqconfig/BGQBlockNodeConfig.h>
#include <control/include/bgqconfig/BGQIOBoardNodeConfig.h>

#include <db/include/api/tableapi/gensrc/DBTIoblockmap.h>
#include <db/include/api/tableapi/gensrc/DBTNetconfig.h>

#include <db/include/api/cxxdb/Connection.h>
#include <db/include/api/cxxdb/QueryStatement.h>
#include <db/include/api/cxxdb/ResultSet.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace database {

IoNodeIpAddress::IoNodeIpAddress(
        const BGQBlockNodeConfig& block
        ) :
    _block( block ),
    _connection( BGQDB::DBConnectionPool::instance().getConnection() ),
    _statement(),
    _sql(),
    _result()
{
    // must be an I/O block
    BOOST_ASSERT( block.ioboardBegin() != block.ioboardEnd() );

    if ( !_connection ) {
        LOG_RUNJOB_EXCEPTION( error_code::database_error, "could not get database connection" );
        return;
    }

    _sql  <<
        "SELECT " << BGQDB::DBTNetconfig::ITEMNAME_COL << "," <<
        BGQDB::DBTNetconfig::ITEMVALUE_COL << "," <<
        BGQDB::DBTNetconfig::INTERFACE_COL << "," <<
        BGQDB::DBTNetconfig::LOCATION_COL <<
        " FROM " << BGQDB::DBTNetconfig().getTableName() << 
        " WHERE " <<
        BGQDB::DBTNetconfig::ITEMNAME_COL << " in ('ipv4address','ipv6address')" <<
        " AND ";

    if ( block.ioboardBegin()->fullIOBoardUsed() ) {
        // only look at I/O drawer location, not entire node location
        _sql << "substr(" << BGQDB::DBTNetconfig::LOCATION_COL << ",1,6)";
    } else {
        _sql << BGQDB::DBTNetconfig::LOCATION_COL;
    }

    _sql <<
        " in (SELECT " << BGQDB::DBTIoblockmap::LOCATION_COL << " FROM " <<
        BGQDB::DBTIoblockmap().getTableName() << " WHERE " <<
        BGQDB::DBTIoblockmap::BLOCKID_COL << "=?)"
    ;
   
    try {
        _statement = _connection->prepareQuery( 
                _sql.str(),
                boost::assign::list_of(BGQDB::DBTIoblockmap::BLOCKID_COL)
                );
        _statement->parameters()[ BGQDB::DBTIoblockmap::BLOCKID_COL ].set( _block.blockName() );
        this->execute();
    } catch ( const std::exception& e ) {
        LOG_ERROR_MSG( e.what() );
    }
}

const boost::asio::ip::address&
IoNodeIpAddress::get(
        const std::string& location
        ) const
{
    const IpMap::const_iterator i = _result.find( location );
    if ( i == _result.end() ) {
        LOG_RUNJOB_EXCEPTION( error_code::database_error, "ip address for '" << location << "' not found" );
    }

    return i->second;
}

void
IoNodeIpAddress::execute()
{
    if ( !_statement ) return;

    try {
        const cxxdb::ResultSetPtr results = _statement->execute();

        while ( results->fetch() ) {
            const cxxdb::Columns& columns = results->columns();
            const std::string& location = columns[ BGQDB::DBTNetconfig::LOCATION_COL ].getString();
            const std::string& interface = columns[ BGQDB::DBTNetconfig::INTERFACE_COL ].getString();
            const std::string& item = columns[ BGQDB::DBTNetconfig::ITEMNAME_COL ].getString();
            const std::string& value = columns[ BGQDB::DBTNetconfig::ITEMVALUE_COL ].getString();

            LOG_DEBUG_MSG( 
                    location << " " << interface << ": " << item << "=" << value
                    );

            boost::system::error_code error;
            const boost::asio::ip::address address(
                    boost::asio::ip::address::from_string( value, error )
                    );
            if ( error ) {
                LOG_ERROR_MSG( "could not convert address '" << value << "' into ip address" );
            } else {
                const std::pair<IpMap::const_iterator,bool> result = _result.insert(
                        IpMap::value_type(
                            columns[ BGQDB::DBTNetconfig::LOCATION_COL ].getString(),
                            address
                            )
                    );
                if ( !result.second ) {
                    LOG_DEBUG_MSG( location << " already has IP address " << result.first->second << " defined, ignoring " << value );
                }
            }
        }
    } catch ( const std::exception& e ) {
        LOG_ERROR_MSG( e.what() );
    }
}


} // database
} // server
} // runjob
