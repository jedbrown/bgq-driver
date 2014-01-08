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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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

#include "DBStatics.h"

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/tableapi/gensrc/DBTIonode.h>
#include <db/include/api/tableapi/gensrc/DBTIodrawer.h>
#include <db/include/api/tableapi/gensrc/DBTNode.h>
#include <db/include/api/tableapi/gensrc/DBTNodecard.h>
#include <db/include/api/tableapi/gensrc/DBTServicecard.h>
#include <db/include/api/cxxdb/cxxdb.h>

#include <utility/include/Log.h>

#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace DBStatics {

std::string
hardwareStatusToString(
        const Status status
        )
{
    typedef std::map<Status, std::string> hardwareStatusMap;
    static const hardwareStatusMap hardwareStatusStrings = boost::assign::map_list_of
        (AVAILABLE, "AVAILABLE")
        (ERROR, "ERROR")
        (MISSING, "MISSING")
        (SOFTWARE_FAILURE, "SOFTWARE FAILURE")
        ;
    return hardwareStatusStrings.at(status);
}

bool
setLocationStatus(
         const std::vector<std::string>& locations,
         const Status status,
         const bgq::util::Location::Type type
         )
 {
    if (locations.empty()) {
        LOG_WARN_MSG("No locations to update.");
        return false;
    }

    const cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    if ( !connection ) {
        LOG_ERROR_MSG("Unable to connect to database.");
        return false;
    }

    std::ostringstream sqlstrm;
    sqlstrm << "UPDATE ";
    switch ( type ) {
        case bgq::util::Location::ComputeCardOnIoBoard:
            sqlstrm << BGQDB::DBTIonode().getTableName();
            break;
        case bgq::util::Location::ComputeCardOnNodeBoard:
            sqlstrm << BGQDB::DBTNode().getTableName();
            break;
        case bgq::util::Location::IoBoardOnComputeRack:
        case bgq::util::Location::IoBoardOnIoRack:
            sqlstrm << BGQDB::DBTIodrawer().getTableName();
            break;
        case bgq::util::Location::NodeBoard:
            sqlstrm << BGQDB::DBTNodecard().getTableName();
            break;
        case bgq::util::Location::ServiceCard:
            sqlstrm << BGQDB::DBTServicecard().getTableName();
            break;
        default:
            throw std::invalid_argument(
                    boost::lexical_cast<std::string>(type) + " is not a supported location type"
                    );
    }

    if (status == AVAILABLE) {
        sqlstrm << " set status='" << BGQDB::HARDWARE_AVAILABLE << "'";
    } else if(status == ERROR) {
        sqlstrm << " set status='" << BGQDB::HARDWARE_ERROR << "'";
    } else if(status == MISSING) {
        sqlstrm << " set status='" << BGQDB::HARDWARE_MISSING << "'";
    } else if(status == SOFTWARE_FAILURE) {
        sqlstrm << " set status='" << BGQDB::SOFTWARE_FAILURE << "'";
    }
    sqlstrm << " WHERE LOCATION=?";

    LOG_TRACE_MSG( "Preparing " << sqlstrm.str() );
    const cxxdb::UpdateStatementPtr update(
            connection->prepareUpdate(
                sqlstrm.str(),
                boost::assign::list_of("LOCATION")
                )
            );
    if ( !update ) {
        LOG_ERROR_MSG( "Could not prepare update." );
        return false;
    }

    cxxdb::Transaction tx( *connection );

    unsigned txcount(0);
    BOOST_FOREACH( const std::string& i, locations ) {
        update->parameters()[ "LOCATION" ].set( i );
        try {
            update->execute();
            LOG_TRACE_MSG( "Updated " << i << " to status " << hardwareStatusToString(status) );
        } catch ( const std::exception& e ) {
            LOG_ERROR_MSG( e.what() );
            return false;
        }
        if ( txcount++ == 25 ) {
            // 25 rows per transaction is an arbitrary choice
            connection->commit();
            txcount = 0;
        }
    }
    if ( txcount ) {
        connection->commit();
    }

    LOG_INFO_MSG(
            "Updated " << locations.size() << " node" << (locations.size() == 1 ? "" : "s") <<
            " to status " << hardwareStatusToString(status)
            );

    return true;
}

} } } // namespace mmcs::server::DBStatics
