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

#include "RedirectMonitorThread.h"

#include "command/MmcsServerConnect.h"

#include "libmmcs_client/ConsolePort.h"

#include <utility/include/Log.h>

#include <auto_ptr.h>
#include <errno.h>

using mmcs::common::ConsoleController;

LOG_DECLARE_FILE( "mmcs.console" );

namespace mmcs {
namespace console {

// Condition variable for synchronization between main thread and redirect thread
bool redirectInitComplete;

// Cleanup handler for the Redirect monitor thread
// arg is mmcs_client::CommandReply, but is only valid while redirectInitComplete is false
void redirectCleanupHandler(void* arg)
{
    if (!redirectInitComplete) {
        mmcs_client::CommandReply* const reply = reinterpret_cast<mmcs_client::CommandReply*>( arg );
        if (reply && reply->getStatus() == mmcs_client::CommandReply::STATUS_NOT_SET)
            *reply << mmcs_client::FAIL << "Internal error: redirect thread initialization failed" << mmcs_client::DONE;
        redirectInitComplete = true;
        pthread_cond_signal(&RedirectMonitorThread::redirectInitCond);
        RedirectMonitorThread::redirectInitMutex.Unlock();

    }
    RedirectMonitorThread::redirectMonitorThread = NULL;
}

RedirectMonitorThread *RedirectMonitorThread::redirectMonitorThread = NULL;
PthreadMutex RedirectMonitorThread::redirectInitMutex(PTHREAD_MUTEX_ERRORCHECK_NP);
pthread_cond_t RedirectMonitorThread::redirectInitCond = PTHREAD_COND_INITIALIZER;

void*
RedirectMonitorThread::threadStart()
{
    std::string reply_str;
    boost::shared_ptr<Parms> parms;
    this->getArg( parms );
    mmcs_client::CommandReply& reply = *(parms->reply);
    reply.reset();

    // coordinate the initialization with the main thread
    redirectInitMutex.Lock();
    redirectInitComplete = false;
    pthread_cleanup_push(redirectCleanupHandler, parms->reply);

    // Create a command processor
    MMCSCommandProcessor mmcsCommandProcessor(parms->mmcsCommands); // MMCS command processor
    mmcsCommandProcessor.logFailures(false); // don't clutter the console with log messages

    // Create a ConsoleController
    const std::auto_ptr<ConsoleController> pController(new ConsoleController(&mmcsCommandProcessor, parms->user));
    pController->setPortConfiguration( parms->controller->getPortConfiguration() );

    std::deque<std::string> mmcs_redirect = MMCSCommandProcessor::parseCommand("mmcs_server_cmd redirect " + parms->blockName + " on");

    // connect to the mmcs server
    const boost::scoped_ptr<console::command::MmcsServerConnect> conncmd( console::command::MmcsServerConnect::build() );
    conncmd->execute(std::deque<std::string>(), reply, pController.get());

    if (reply.getStatus() != 0)
        goto done;

    // send the redirect command
    mmcsCommandProcessor.execute(mmcs_redirect, reply, pController.get());
    if (reply.getStatus() != 0)
        goto done;

    // notify main thread that we have completed setup
    reply << mmcs_client::OK << mmcs_client::DONE;
    redirectMonitorThread = this;
    redirectInitComplete = true;
    pthread_cond_signal(&redirectInitCond);
    redirectInitMutex.Unlock();

    // loop receiving and printing mailbox output
    while (isThreadStopping() == false) {
        try  {
            pController->getConsolePort()->pollReceiveMessage(reply_str, 2);
            if (reply_str.length() != 0) {
                if (reply_str[reply_str.length()-1] == '\n')
                    reply_str.resize(reply_str.length()-1);
                if (stdout)
                    std::cout << reply_str << std::endl;
                else
                    std::cerr << reply_str << std::endl;
            }
        } catch (const mmcs_client::ConsolePort::Error& e) {
            switch (e.errcode)
            {
                case EAGAIN:
                case EINTR:
                    continue;
                case ECONNRESET:
                case ESPIPE:
                case EPIPE:
                case EBADF:
                    LOG_ERROR_MSG("Redirection terminating");
                    goto done;
                default:
                    LOG_ERROR_MSG("Redirection terminating: " << e.what());
                    goto done;
            }
            LOG_ERROR_MSG("Redirection terminating: " << e.what());
            goto done;
        }
    }

    // return when done
done:
    // Turn off redirect
    if (force_end) {
        mmcs_client::CommandReply redirectOffReply;
        mmcs_redirect = MMCSCommandProcessor::parseCommand("mmcs_server_cmd redirect " + parms->blockName + " off");
        mmcsCommandProcessor.execute(mmcs_redirect, redirectOffReply, pController.get());
    }
    pthread_cleanup_pop(1);
    return NULL;
}

} } // namespace mmcs::console
