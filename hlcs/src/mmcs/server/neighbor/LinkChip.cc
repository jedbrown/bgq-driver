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

#include "LinkChip.h"

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/tableapi/gensrc/DBTCable.h>

#include <utility/include/CableBadWires.h>
#include <utility/include/Log.h>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace neighbor {

LinkChip::LinkChip(
        const BlockPtr& block,
        const int msgid,
        const bgq::util::Location& location
        ) :
    Info( block, msgid, location )
{
    LOG_TRACE_MSG( _location );
}

void
LinkChip::impl(
        const char* rawdata
        )
{
    const char* matchMask = strstr(rawdata, "Mask=");
    if ( !matchMask ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    "Missing Mask= in raw data for " + _location.getLocation()
                    )
                );
    }
    const char* matchReg = strstr(rawdata, "Register=");
    if ( !matchReg ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    "Missing Register= in raw data for " + _location.getLocation()
                    )
                );
    }

    char regFromEvent[33] = {0};
    int maskInt = 0;
    sscanf(matchMask, "Mask=%8x", &maskInt);
    sscanf(matchReg, "Register=%3s", regFromEvent);

    try  {
        bgq::utility::CableBadWires badWires(_location, std::string(regFromEvent).substr(0,3), maskInt);
        const std::string portLoc = badWires.getPortLocation();

        // using the location that generated this event, get the port locations and I/O node
        // neighbor from the cniolink view and cable table
        const cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
        const cxxdb::ResultSetPtr result(
                connection->query(
                    "SELECT " +
                    BGQDB::DBTCable::TOLOCATION_COL + ", " +
                    BGQDB::DBTCable::FROMLOCATION_COL + ", " +
                    BGQDB::DBTCable::BADWIREMASK_COL + " FROM " +
                    BGQDB::DBTCable().getTableName() + " WHERE " +
                    BGQDB::DBTCable::TOLOCATION_COL +  "='" + portLoc + "' OR " +
                    BGQDB::DBTCable::FROMLOCATION_COL +  "='" + portLoc + "'"
                    )
                );
        if ( !result->fetch() ) {
            LOG_WARN_MSG( "Could not find neighbor for " << _location );
            return;
        }
        const std::string from( result->columns()[BGQDB::DBTCable::FROMLOCATION_COL].getString() );
        const std::string to( result->columns()[BGQDB::DBTCable::TOLOCATION_COL].getString() );
        const uint64_t mask ( result->columns()[BGQDB::DBTCable::BADWIREMASK_COL].as<int64_t>() );
        if ( from.substr(0,6) == _location.getLocation().substr(0,6) ) {
            badWires.setTxPortAndBadWireMask(to, mask);
        } else {
            badWires.setTxPortAndBadWireMask(from, mask);
        }
        _details << "NEIGHBOR=" << badWires.getTxLinkChipLocation();
        _details << " using cable from " << from << " to " << to;
    } catch ( std::exception& e ) {
        LOG_ERROR_MSG( e.what() );
    }
}

} } } // namespace mmcs::server::neighbor
