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
ListUsers::execute(deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget)
{
    PthreadMutexHolder mutex;
    mutex.Lock(&pController->_midplaneControllerListMutex);
    reply << mmcs_client::OK;
    if (pController->_midplaneControllerList.size() != 0)
    {
        for(MidplaneControllerList::iterator it = pController->_midplaneControllerList.begin(); it != pController->_midplaneControllerList.end(); ++it)
        {
            // print thread id
            const common::Thread* mmcsThread = (*it)->getThread();
            reply << "Thread: ";
            if (mmcsThread)
            {
                reply << mmcsThread->getThreadId() << " (" << mmcsThread->getThreadName() << ")\t";
            }
            else
            {
                reply << "n/a\t";
            }

            // print user name
            reply << " User: " << setw(8) << left << (*it)->getUser().getUser();

            // print block info
            reply << " Block: " << setw(16) << left;
            //        DBBlockController* dbBlockController = (DBBlockController*)((*it)->_blockController.get());
            DBBlockPtr dbBlockController = boost::dynamic_pointer_cast<DBBlockController>((*it)->_blockController);
            if (dbBlockController != NULL)
            {
                reply << dbBlockController->getBase()->getBlockName();
                if (dbBlockController->getBase()->peekDisconnecting())
                {
                    reply << "\tdisconnecting - " << dbBlockController->getBase()->disconnectReason();
                }
            }
            else
            {
                reply << "n/a";
            }

            // is this DBConsoleController used for redirection?
            if ((*it)->getRedirecting())
            {
                reply << "\tredirecting";
            }

            reply << "\n";
        }
    }
    reply << mmcs_client::DONE;
    mutex.Unlock();
    return;
}

void
ListUsers::help(deque<string> args,
        mmcs_client::CommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << mmcs_client::OK << description()
        << ";List mmcs users."
        << ";Output includes thread number, block ID and if output is redirected to console."
        << mmcs_client::DONE;
}


} } } // namespace mmcs::server::command
