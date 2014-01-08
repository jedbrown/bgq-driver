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

#include "BGQDBlib.h"
#include "tableapi/DBConnectionPool.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

LOG_DECLARE_FILE( "database" );

namespace BGQDB {

static bool s_is_initialized(false);
bool bypass_ctlaction(false);

void
init(
        const bgq::utility::Properties::ConstPtr properties,
        const std::string& section_name
)
{
    if ( s_is_initialized ) {
        LOG_DEBUG_MSG( "BGQDB is already initialized. Ignoring." );
        return;
    }

    DBConnectionPool::init( properties, section_name );

    try {
        if ( !properties->getValue(section_name,"control_action_bypass").empty() &&
             properties->getValue(section_name,"control_action_bypass") != "false") {
            bypass_ctlaction = true;
        }
    } catch ( std::exception& e ) {
    }

    s_is_initialized = true;
}

void
init(
        const bgq::utility::Properties::ConstPtr properties,
        unsigned connection_pool_size
    )
{
    if ( s_is_initialized ) {
        LOG_DEBUG_MSG( "BGQDB is already initialized. Ignoring." );
        return;
    }

    DBConnectionPool::init( properties, connection_pool_size );

    s_is_initialized = true;
}

} // BGQDB
