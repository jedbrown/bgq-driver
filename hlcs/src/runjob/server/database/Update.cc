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
#include "server/database/Update.h"

#include "common/error.h"
#include "common/Exception.h"
#include "common/logging.h"

#include <db/include/api/tableapi/gensrc/DBTJob.h>

#include <db/include/api/job/types.h>

#include <db/include/api/BGQDBlib.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace database {

Update::Update(
        const boost::shared_ptr<BGQDB::job::Operations>& operations
        ) :
    _operations( operations )
{

}

void
Update::execute(
        const BGQDB::job::Id id,
        const job::Status::Value status
        )
{
    BGQDB::job::status::Value value;
    switch ( status ) {
        case job::Status::Cleanup:          value = BGQDB::job::status::Cleanup; break;
        case job::Status::Debug  :          value = BGQDB::job::status::Debug; break;
        case job::Status::Loading:          value = BGQDB::job::status::Loading; break;
        case job::Status::Running:          value = BGQDB::job::status::Running; break;
        case job::Status::Setup:            value = BGQDB::job::status::Setup; break;
        case job::Status::OutputStarting:   value = BGQDB::job::status::Starting; break;
        default:
            BOOST_ASSERT( !"unhandled status" );
    }

    try {
        _operations->update( id, value );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }
}

} // database
} // server
} // runjob
