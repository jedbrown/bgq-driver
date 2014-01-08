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
#include "server/database/Delete.h"

#include "common/error.h"
#include "common/Exception.h"
#include "common/logging.h"

#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <db/include/api/tableapi/gensrc/DBTJob.h>
#include <db/include/api/tableapi/gensrc/DBTJob_history.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <db/include/api/job/types.h>

#include <db/include/api/BGQDBlib.h>

#include <boost/assign/list_of.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace database {

Delete::Delete(
        boost::asio::io_service& io_service,
        const boost::shared_ptr<BGQDB::job::Operations>& operations
        ) :
    _operations( operations ),
    _strand( io_service )
{

}

void
Delete::execute(
        const BGQDB::job::Id id,
        const job::ExitStatus& exit,
        const Callback& callback
        )
{
    _strand.post(
            boost::bind(
                &Delete::executeImpl,
                this,
                id,
                exit,
                callback
                )
            );
}

void
Delete::executeImpl(
        const BGQDB::job::Id id,
        const job::ExitStatus& exit,
        const Callback& callback
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );

    boost::scoped_ptr<BGQDB::job::RemoveInfo> info;
    if ( exit.getError() ) {
        info.reset(
                new BGQDB::job::RemoveInfo( exit.getMessage() )
                );
    } else if ( exit.getMessage().empty() ) {
        info.reset(
                new BGQDB::job::RemoveInfo( exit.getStatus().get() )
                );
    } else {
        info.reset(
                new BGQDB::job::RemoveInfo(
                    exit.getStatus().get(),
                    exit.getMessage()
                    )
                );
    }

    try {
        _operations->remove( id, *info );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    if ( callback ) callback();
}

} // database
} // server
} // runjob
