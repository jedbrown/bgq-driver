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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#include "IoLink.h"

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/tableapi/gensrc/DBTCable.h>
#include <db/include/api/tableapi/gensrc/DBVCniolink.h>

#include <utility/include/Log.h>

#include <boost/assign/list_of.hpp>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace neighbor {

IoLink::IoLink(
        const BlockPtr& block,
        const int msgid,
        const bgq::util::Location& location
        ) :
    Info( block, msgid, location )
{
    LOG_TRACE_MSG( _location );
}

void
IoLink::impl(
        const char* rawdata
        )
{
    if ( _location.getType() == bgq::util::Location::ComputeCardOnNodeBoard ) {
        this->computeNode();
    } else if ( _location.getType() == bgq::util::Location::ComputeCardOnIoBoard ) {
        this->ioNode();
    } else {
        LOG_WARN_MSG( "Unhandled I/O link location type: " << _location );
    }
}

void
IoLink::computeNode()
{
    // this query only works for compute node locations
    if ( _location.getType() != bgq::util::Location::ComputeCardOnNodeBoard ) return;

    // using the location that generated this event, get the port locations and I/O node
    // neighbor from the cniolink view and cable table
    const cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    if ( !connection ) {
        LOG_WARN_MSG( "Could not get database connection" );
        return;
    }
    const cxxdb::ResultSetPtr result(
            connection->query(
                "SELECT " +
                BGQDB::DBVCniolink::DESTINATION_COL + ", " +
                BGQDB::DBVCniolink::ION_COL + ", " +
                BGQDB::DBTCable::FROMLOCATION_COL + " FROM " +
                BGQDB::DBVCniolink().getTableName() + ", " +
                BGQDB::DBTCable().getTableName() + " WHERE " +
                BGQDB::DBVCniolink::SOURCE_COL + "=" +
                "'" + _location.getLocation() + "' AND " +
                BGQDB::DBVCniolink::DESTINATION_COL + "=" +
                BGQDB::DBTCable::TOLOCATION_COL
                )
            );
    if ( !result->fetch() ) {
        LOG_WARN_MSG( "Could not find neighbor for " << _location );
        return;
    }

    const std::string neighbor( result->columns()[BGQDB::DBVCniolink::ION_COL].getString() );
    const std::string ioport( result->columns()[BGQDB::DBVCniolink::DESTINATION_COL].getString());
    const std::string cnport( result->columns()[BGQDB::DBTCable::FROMLOCATION_COL].getString() );
    const std::map<std::string,std::string> ioLinkChip = boost::assign::map_list_of
        ("T00","U05")
        ("T01","U00")
        ("T02","U05")
        ("T03","U00")
        ("T04","U05")
        ("T05","U00")
        ("T06","U05")
        ("T07","U00")
        ("T08","U04")
        ("T09","U01")
        ("T10","U04")
        ("T11","U01")
        ("T12","U04")
        ("T13","U01")
        ("T14","U04")
        ("T15","U01")
        ("T16","U03")
        ("T17","U02")
        ("T18","U03")
        ("T19","U02")
        ("T20","U03")
        ("T21","U02")
        ("T22","U03")
        ("T23","U02")
        ;
    const std::map<std::string,std::string>::const_iterator linkChip = ioLinkChip.find( ioport.substr(7,3) );

    _details << "NEIGHBOR=" << neighbor << " Connects through link chips ";
    _details << _location.getLocation().substr(0,10) << "-U04 and ";
    _details << neighbor.substr(0,7);
    if ( linkChip != ioLinkChip.end() ) {
        _details << linkChip->second;
    } else {
        _details << "N/A";
    }
    _details << " using cable from port " << cnport << " to " << ioport;
}

void
IoLink::ioNode()
{
    // using the location that generated this event, get the port locations and I/O node
    // neighbor from the cniolink view and cable table
    const cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    const cxxdb::ResultSetPtr result(
            connection->query(
                "SELECT " +
                BGQDB::DBTCable::FROMLOCATION_COL + ", " +
                BGQDB::DBTCable::TOLOCATION_COL + " " +
                "FROM " + BGQDB::DBTCable().getTableName() + " WHERE " +
                BGQDB::DBTCable::TOLOCATION_COL + " in " + "(" +
                "SELECT " +
                BGQDB::DBVCniolink::DESTINATION_COL + " FROM " +
                BGQDB::DBVCniolink().getTableName() + " WHERE " +
                BGQDB::DBVCniolink::ION_COL + "='" + _location.getLocation() + "')"
                )
            );

    // each I/O node will have two cables
    if ( !result->fetch() ) return;
    _details << "using cable from ";
    _details << result->columns()[ BGQDB::DBTCable::FROMLOCATION_COL ].getString();
    _details << " to ";
    _details << result->columns()[ BGQDB::DBTCable::TOLOCATION_COL ].getString();

    if ( !result->fetch() ) return;
    _details << " and cable from ";
    _details << result->columns()[ BGQDB::DBTCable::FROMLOCATION_COL ].getString();
    _details << " to ";
    _details << result->columns()[ BGQDB::DBTCable::TOLOCATION_COL ].getString();
}

} } } // namespace mmcs::server::neighbor
