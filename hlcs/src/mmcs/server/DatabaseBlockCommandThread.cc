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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#include "DatabaseBlockCommandThread.h"

#include "CommandProcessor.h"
#include "DBConsoleController.h"

#include "libmmcs_client/CommandReply.h"

#include <utility/include/Log.h>

#include <boost/scoped_ptr.hpp>

using namespace std;

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {

void*
DatabaseBlockCommandThread::threadStart()
{
    std::deque<string> args;         // arguments to MMCS command
    mmcs_client::CommandReply reply;             // holds reply from MMCS command
    CommandProcessor mmcsCommandProcessor(_commands); // MMCS command processor

    if ( _userName.empty() ) {
        _userName = "nobody";
    }

    // Create a midplane controller object
    const bool allowRemote( true );
    const bgq::utility::UserId uid( _userName, allowRemote );
    DBConsoleController midplaneController(&mmcsCommandProcessor, uid, CxxSockets::Administrator);
    this->setThreadName("DBBlockCmd");
    midplaneController.setMMCSThread(this);

    const log4cxx::MDC _block_user_mdc_( "user", std::string("[") + _userName + "] " );
    LOG_INFO_MSG("Started: block " << _name << ", action=" << BGQDB::blockActionToString(_action));

    switch(_action)
    {
        case BGQDB::NO_BLOCK_ACTION:
            break;
        case BGQDB::CONFIGURE_BLOCK_NO_CHECK:  // Allocate and boot I/O block with nodes in error
        case BGQDB::CONFIGURE_BLOCK:           // Allocate and boot the block (I/O or compute)
            BGQDB::clearBlockAction(_name);
            args.clear();
            args.push_back(_name);
            // Set the "no_check" argument when calling allocate to boot I/O blocks with nodes in error
            if (_action == BGQDB::CONFIGURE_BLOCK_NO_CHECK) {
                args.push_back("no_check");
            }
            mmcsCommandProcessor.logCommand("db_allocate", args);
            mmcsCommandProcessor.execute("allocate", args, reply, &midplaneController);
            midplaneController.deselectBlock();
            break;
        case BGQDB::DEALLOCATE_BLOCK:  // Deallocate the block (I/O or compute)
            args.clear();
            args.push_back(_name);
            mmcsCommandProcessor.logCommand("db_free", args);
            mmcsCommandProcessor.execute("free", args, reply, &midplaneController);
            midplaneController.deselectBlock();
            break;
        default:
            LOG_ERROR_MSG( "Unexpected blockAction " << _action);
    }

    // End the transaction
    _transactions->endTransaction(_name, _action);

    // Return when done
    // LOG_TRACE_MSG("Stopped");
    return NULL;
}

} } // namespace mmcs::server
