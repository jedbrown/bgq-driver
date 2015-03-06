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
#include "server/Ras.h"

#include "common/logging.h"

#include <bgq_util/include/Location.h>

#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <db/include/api/tableapi/gensrc/DBTEventlog.h>
#include <db/include/api/tableapi/gensrc/DBTIonode.h>
#include <db/include/api/tableapi/gensrc/DBVIonode.h>
#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/BGQDBlib.h>

#include <ras/include/RasEventHandlerChain.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>
#include <boost/throw_exception.hpp>

#include <string>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {

Ras
Ras::create(
        const Value type
        )
{
    Ras result( type );
    return result;
}

Ras::Ras(
        const Value type
        ) :
    _impl( type ),
    _job( 0 ),
    _block()
{

}

cxxdb::QueryStatementPtr
Ras::prepare( 
        const cxxdb::ConnectionPtr& connection
        )
{
    if ( !connection ) BOOST_THROW_EXCEPTION( std::invalid_argument("database connection") );
    const cxxdb::QueryStatementPtr result(
            connection->prepareQuery(
                std::string("SELECT ") + BGQDB::DBTEventlog::RECID_COL + " FROM NEW TABLE (" +
                "INSERT into " + BGQDB::DBTEventlog().getTableName() + " (" +
                BGQDB::DBTEventlog::MSG_ID_COL + "," +
                BGQDB::DBTEventlog::CATEGORY_COL + "," +
                BGQDB::DBTEventlog::COMPONENT_COL + "," +
                BGQDB::DBTEventlog::SEVERITY_COL + "," +
                BGQDB::DBTEventlog::MESSAGE_COL + "," +
                BGQDB::DBTEventlog::BLOCK_COL + "," +
                BGQDB::DBTEventlog::QUALIFIER_COL + "," +
                BGQDB::DBTEventlog::JOBID_COL + "," +
                BGQDB::DBTEventlog::LOCATION_COL + "," +
                BGQDB::DBTEventlog::ECID_COL + "," +
                BGQDB::DBTEventlog::SERIALNUMBER_COL + "," +
                BGQDB::DBTEventlog::CTLACTION_COL + 
                ") VALUES (?,?,?,?,?,?,?,?,?," +
                "(select " + BGQDB::DBTIonode::ECID_COL + " from TBGQIONODE " +
                "where " + BGQDB::DBTIonode::IOPOS_COL + " || '-' || " + BGQDB::DBTIonode::POSITION_COL + " = ?)," +
                "(select " +  BGQDB::DBTIonode::SERIALNUMBER_COL + " from TBGQIONODE " +
                "where " + BGQDB::DBTIonode::IOPOS_COL + " || '-' || " + BGQDB::DBTIonode::POSITION_COL + " = ?)," +
                "?))",
                // parameter marker names, this is the c++0x initializer syntax
                {
                BGQDB::DBTEventlog::MSG_ID_COL,
                BGQDB::DBTEventlog::CATEGORY_COL,
                BGQDB::DBTEventlog::COMPONENT_COL,
                BGQDB::DBTEventlog::SEVERITY_COL,
                BGQDB::DBTEventlog::MESSAGE_COL,
                BGQDB::DBTEventlog::BLOCK_COL,
                BGQDB::DBTEventlog::QUALIFIER_COL,
                BGQDB::DBTEventlog::JOBID_COL,
                BGQDB::DBTEventlog::LOCATION_COL,
                BGQDB::DBTEventlog::ECID_COL,
                BGQDB::DBTEventlog::SERIALNUMBER_COL,
                BGQDB::DBTEventlog::CTLACTION_COL
                }
                )
            );

    return result;
}

cxxdb::ResultSetPtr
Ras::insert(
        const cxxdb::ConnectionPtr& connection,
        const cxxdb::QueryStatementPtr& query
        )
{
    if ( !query ) BOOST_THROW_EXCEPTION( std::invalid_argument("query statement") );

    query->parameters()[ BGQDB::DBTEventlog::MSG_ID_COL ].set( _impl.getDetail(RasEvent::MSG_ID) );
    query->parameters()[ BGQDB::DBTEventlog::CATEGORY_COL ].set( _impl.getDetail(RasEvent::CATEGORY) );
    query->parameters()[ BGQDB::DBTEventlog::COMPONENT_COL ].set( _impl.getDetail(RasEvent::COMPONENT) );
    query->parameters()[ BGQDB::DBTEventlog::SEVERITY_COL ].set( _impl.getDetail(RasEvent::SEVERITY) );
    query->parameters()[ BGQDB::DBTEventlog::MESSAGE_COL ].set( _impl.getDetail(RasEvent::MESSAGE) );
    if ( !_block.empty() ) {
        query->parameters()[ BGQDB::DBTEventlog::BLOCK_COL ].set( _block );
        query->parameters()[ BGQDB::DBTEventlog::QUALIFIER_COL ].set( this->getQualifier(connection) );
    } else {
        query->parameters()[ BGQDB::DBTEventlog::BLOCK_COL ].setNull();
        query->parameters()[ BGQDB::DBTEventlog::QUALIFIER_COL ].setNull();
    }
    if ( _job ) {
        query->parameters()[ BGQDB::DBTEventlog::JOBID_COL ].cast( _job );
    } else {
        query->parameters()[ BGQDB::DBTEventlog::JOBID_COL ].setNull();
    }
    if ( !_impl.getDetail(RasEvent::LOCATION).empty() ) {
        query->parameters()[ BGQDB::DBTEventlog::LOCATION_COL ].set( _impl.getDetail(RasEvent::LOCATION) );
        query->parameters()[ BGQDB::DBTEventlog::ECID_COL ].set( _impl.getDetail(RasEvent::LOCATION) );
        query->parameters()[ BGQDB::DBTEventlog::SERIALNUMBER_COL ].set( _impl.getDetail(RasEvent::LOCATION) );
    } else {
        query->parameters()[ BGQDB::DBTEventlog::LOCATION_COL ].setNull();
        query->parameters()[ BGQDB::DBTEventlog::ECID_COL ].setNull();
        query->parameters()[ BGQDB::DBTEventlog::SERIALNUMBER_COL ].setNull();
    }
    if ( !_impl.getDetail(RasEvent::CONTROL_ACTION).empty() ) {
        query->parameters()[ BGQDB::DBTEventlog::CTLACTION_COL ].set( _impl.getDetail(RasEvent::CONTROL_ACTION) );
    } else {
        query->parameters()[ BGQDB::DBTEventlog::CTLACTION_COL ].setNull();
    }

    const cxxdb::ResultSetPtr result( query->execute() );

    return result;
}

void
Ras::handleControlActions(
        const cxxdb::ConnectionPtr& connection
        )
{
    bool bgas = false;
    try {
        std::string bgasValue = BGQDB::DBConnectionPool::Instance().getProperties()->getValue(  "mmcs", "bgas" );
        if ( bgasValue.compare("true") == 0 || bgasValue.compare("TRUE") == 0 ) {
            bgas = true;
        }
    } catch ( const std::exception& e ) {
        LOG_TRACE_MSG( "Could not find key bgas in [mmcs] section. Using default value of false" );
    }

    if ( _impl.getDetail(RasEvent::CONTROL_ACTION).empty() ) return;

    //LOG_DEBUG_MSG("RB: Ras::handleControlActions " << "bgas = " << (int)bgas << " location = " << _impl.getDetail(RasEvent::LOCATION)
    //        << "CONTROL_ACTION = " << _impl.getDetail(RasEvent::CONTROL_ACTION) );

    typedef std::vector<std::string> Tokens;
    Tokens tokens;
    const std::string actions( _impl.getDetail(RasEvent::CONTROL_ACTION) );
    boost::split( tokens, actions, boost::is_any_of(",") );
    BOOST_FOREACH( const Tokens::value_type& i, tokens ) {
        if ( i == "SOFTWARE_IN_ERROR" ) continue;
        if ( i == "FREE_COMPUTE_BLOCK" ) continue;
        if ( i == "END_JOB" ) continue;
        if ( i == "NONE" ) continue;
        if ( i.empty() ) continue;
        LOG_WARN_MSG( "unsupported control action: " << i );
        return;
    }

    const bgq::util::Location location( _impl.getDetail(RasEvent::LOCATION) );
    if (location.getType() != bgq::util::Location::ComputeCardOnIoBoard) {
        LOG_WARN_MSG( "unsupported location type for control action: " << location );
        return;
    }

    const Tokens::const_iterator softwareFailure = std::find(tokens.begin(), tokens.end(), "SOFTWARE_IN_ERROR");
    if ( softwareFailure != tokens.end() ) {
        LOG_DEBUG_MSG( "handling " << *softwareFailure << " control action" );
        uint64_t rows = 0;
        connection->executeUpdate(
                std::string("UPDATE ") + BGQDB::DBVIonode().getTableName() + " SET " +
                BGQDB::DBVIonode::STATUS_COL + " = '" + BGQDB::SOFTWARE_FAILURE + "' WHERE " +
                BGQDB::DBVIonode::STATUS_COL + " in ('" + BGQDB::HARDWARE_AVAILABLE + "') AND " +
                BGQDB::DBVIonode::LOCATION_COL + " = '" + location.getLocation() + "'",
                &rows
                );
        LOG_TRACE_MSG( "updated " << rows << " rows" );
    }
    
    const Tokens::const_iterator freeComputeBlock = std::find(tokens.begin(), tokens.end(), "FREE_COMPUTE_BLOCK");
    if ( freeComputeBlock != tokens.end() ) {
        uint64_t rows = 0;
        if ( bgas ) {
            LOG_DEBUG_MSG( "BGAS handling " << *freeComputeBlock << " control action" );
            connection->executeUpdate(
                    std::string("UPDATE ") + BGQDB::DBTBlock().getTableName() + " SET " +
                    BGQDB::DBTBlock::ACTION_COL + " = '" + BGQDB::BLOCK_DEALLOCATING + "' WHERE " +
                    BGQDB::DBTBlock::BLOCKID_COL + " in (" +
                    "SELECT a.blockid from bgqblock a, bgqiousage b " +
                    "WHERE b.blockid = a.blockid " +
                    "AND b.ionode = '" + _impl.getDetail(RasEvent::LOCATION) + "' " +
                    "AND a.status <> 'F'" +
                    ")",
                    &rows
                    );
        } else {
            LOG_DEBUG_MSG( "handling " << *freeComputeBlock << " control action" );
            connection->executeUpdate(
                    std::string("UPDATE ") + BGQDB::DBTBlock().getTableName() + " SET " +
                    BGQDB::DBTBlock::ACTION_COL + " = '" + BGQDB::BLOCK_DEALLOCATING + "' WHERE " +
                    BGQDB::DBTBlock::BLOCKID_COL + " in (" +
                    "SELECT a.blockid from bgqblock a, bgqcnioblockmap b " +
                    "WHERE b.cnblock = a.blockid " +
                    "AND b.ion = '" + _impl.getDetail(RasEvent::LOCATION) + "' " +
                    "AND a.status <> 'F'" +
                    ")",
                    &rows
                    );
        }
        LOG_TRACE_MSG( "updated " << rows << " rows" );
    }

    // END_JOB control action is handled elsewhere
}

Ras::~Ras()
{
    _impl.setDetail( RasEvent::BLOCKID, _block );
    _impl.setDetail( RasEvent::JOBID, boost::lexical_cast<std::string>(_job) );
    try {
        RasEventHandlerChain::handle( _impl );
        const cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
        const cxxdb::QueryStatementPtr query = this->prepare( connection );
        const cxxdb::ResultSetPtr results = this->insert( connection, query );
        if ( !results->fetch() ) {
            LOG_WARN_MSG( "could not insert RAS" );
            return;
        }

        const int recid = results->columns()[ BGQDB::DBTEventlog::RECID_COL ].as<int32_t>();
        LOG_TRACE_MSG( "inserted RAS with recid " << recid );

        this->handleControlActions( connection );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    } catch ( ... ) {
        // can't throw from a dtor
        LOG_WARN_MSG( "caught some other exception" );
    }
}

std::string
Ras::getQualifier(
        const cxxdb::ConnectionPtr& connection
        )
{
    if ( _block.empty() ) return std::string();

    const cxxdb::QueryStatementPtr query(
            connection->prepareQuery(
                "SELECT " + BGQDB::DBTBlock::QUALIFIER_COL + " " +
                "FROM " + BGQDB::DBTBlock().getTableName() + " " +
                "WHERE " + BGQDB::DBTBlock::BLOCKID_COL + "=?",
                { BGQDB::DBTBlock::BLOCKID_COL }
                )
            );
    query->parameters()[ BGQDB::DBTBlock::BLOCKID_COL ].set( _block );
    const cxxdb::ResultSetPtr results( query->execute() );
    if ( !results->fetch() ) {
        LOG_WARN_MSG( "could not get qualifier for block '" << _block << "'" );
        return std::string();
    }

    return 
        results->columns()[ BGQDB::DBTBlock::QUALIFIER_COL ].isNull() ?
        std::string() :
        results->columns()[ BGQDB::DBTBlock::QUALIFIER_COL ].getString()
        ;
}

} // server
} // runjob

