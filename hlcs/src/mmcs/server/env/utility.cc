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

#include "utility.h"

#include <db/include/api/cxxdb/cxxdb.h>

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/utility.hpp>

#include <algorithm>
#include <string>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace env {

Racks
getRacks(
        const cxxdb::ConnectionPtr& connection
        )
{
    BOOST_ASSERT( connection );

    // Get list of midplanes from tbgqmidplane, we only care about the M0 midplane
    const cxxdb::ResultSetPtr midplanes(
            connection->query(
                "select LOCATION from BGQMidplane where location like 'R%-M0'"
                )
            );

    Racks result;
    while ( midplanes->fetch() ) {
        result.push_back( midplanes->columns()[ "LOCATION" ].getString().substr(0,3) );
    }

    return result;
}

IoDrawers
getIoDrawers(
        const cxxdb::ConnectionPtr& connection
        )
{
    BOOST_ASSERT( connection );

    const cxxdb::ResultSetPtr drawers(
            connection->query(
                "select LOCATION from BGQIODrawer where status <> 'M' "
                )
            );

    IoDrawers result;
    while ( drawers->fetch() ) {
        result.push_back( drawers->columns()[ "LOCATION" ].getString() );
    }

    return result;
}

IoDrawers
getIoRacks(
        const cxxdb::ConnectionPtr& connection
        )
{
    BOOST_ASSERT( connection );

    const cxxdb::ResultSetPtr drawers(
            connection->query(
                "select LOCATION from BGQIODrawer where status <> 'M' and location like 'Q%'"
                )
            );

    IoDrawers result;
    while ( drawers->fetch() ) {
        result.push_back( drawers->columns()[ "LOCATION" ].getString() );
    }

    return result;
}

unsigned
calculateConnectionSize(
        const Racks& racks
        )
{
    // count all racks with the location Rx0
    const unsigned rows = std::count_if(
            racks.begin(),
            racks.end(),
            boost::bind(
                &std::string::substr,
                _1,
                2,1
                ) == "0"
            );
    // LOG_TRACE_MSG( rows << " rows" );

    // count all racks with the location R0x
    const unsigned columns = std::count_if(
            racks.begin(),
            racks.end(),
            boost::bind(
                &std::string::substr,
                _1,
                1,1
                ) == "0"
            );
    // LOG_TRACE_MSG( columns << " columns" );

    // idea here is we'll create a connection per number of rows or columns,
    // whichever is greater
    return rows > columns ? rows : columns;
}


} } } // namespace mmcs::server::env
