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
#include "server/database/Init.h"

#include "server/database/Delete.h"
#include "server/database/Insert.h"
#include "server/database/Update.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include "server/Options.h"
#include "server/Server.h"

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <db/include/api/BGQDBlib.h>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace database {

Init::Ptr
Init::create(
        const Server::Ptr& server
        )
{
    const Ptr result(
            new Init( server )
            );

    return result;
}

Init::Init(
        const Server::Ptr& server
        ) :
    _operations(),
    _insert(),
    _update(),
    _delete()
{
    // initialize API
    const bgq::utility::Properties::ConstPtr properties( server->getOptions().getProperties() );

    LOG_DEBUG_MSG( "initializing database" );
    
    BGQDB::init(
            server->getOptions().getProperties(),
            PropertiesSection
            );

    // initialize prepared statements
    try {
        _operations.reset( new BGQDB::job::Operations );
        _insert.reset( new Insert(_operations) );
        _update.reset( new Update(_operations) );
        _delete.reset( new Delete(server->getIoService(), _operations) );
    } catch ( const std::exception& e ) {
        LOG_FATAL_MSG( e.what() );
        server->getIoService().stop();
    }
}

Init::~Init()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

} // database
} // server
} // runjob
