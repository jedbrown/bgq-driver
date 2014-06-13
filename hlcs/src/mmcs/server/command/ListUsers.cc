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

#include "ListUsers.h"

#include "../BlockControllerBase.h"
#include "../DBBlockController.h"
#include "../DBConsoleController.h"

#include <utility/include/cxxsockets/SockAddr.h>
#include <utility/include/cxxsockets/TCPSocket.h>
#include <utility/include/Log.h>

LOG_DECLARE_FILE( "mmcs.server" );

using namespace std;

namespace mmcs {
namespace server {
namespace command {

ListUsers*
ListUsers::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.bgadminAuth(true);
    commandAttributes.helpCategory(common::ADMIN);
    return new ListUsers("list_users", "list_users", commandAttributes);
}


void
ListUsers::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    reply << mmcs_client::OK;

    // copy console list info local buffer
    ConsoleControllerList list;
    {
        PthreadMutexHolder mutex;
        mutex.Lock(&pController->_consoleControllerListMutex);
        list = pController->_consoleControllerList;
    }

    for (
            ConsoleControllerList::const_iterator it = list.begin();
            it != list.end();
            ++it
        )
    {
        // print thread id
        const common::Thread* mmcsThread = (*it)->getThread();
        reply << "Thread: ";
        if (mmcsThread) {
            reply << std::hex << mmcsThread->getThreadId() << " (" << mmcsThread->getThreadName() << ")\t";
        } else {
            reply << "n/a\t";
        }

        // print user name
        reply << " User: " << (*it)->getUser().getUser();

        // print host name
        if ( !(*it)->getPeerName().empty() ) {
            std::string hostname( (*it)->getPeerName() );
            const std::string::size_type period = hostname.find_first_of('.');
            if ( period != std::string::npos ) {
                hostname.erase( period );
            }
            reply << "@" << hostname;
        }

        // print block info
        reply << " Block: " << setw(16) << left;
        if ( DBBlockPtr db = boost::dynamic_pointer_cast<DBBlockController>((*it)->_blockController) ) {
            reply << db->getBase()->getBlockName();
            if (db->getBase()->peekDisconnecting()) {
                reply << "\tdisconnecting - " << db->getBase()->disconnectReason();
            }
        } else {
            reply << "n/a";
        }

        // is this DBConsoleController used for redirection?
        if ((*it)->getRedirecting()) {
            reply << "\tredirecting";
        }

        reply << "\n";
    }
    reply << mmcs_client::DONE;
    return;
}

void
ListUsers::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";List mmcs users."
          << ";Output includes thread number, block ID and if output is redirected to console."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
