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

#ifndef MMCS_CONSOLE_REDIRECT_MONITOR_THREAD_H_
#define MMCS_CONSOLE_REDIRECT_MONITOR_THREAD_H_

#include "common/ConsoleController.h"
#include "common/Thread.h"

#include "../MMCSCommandProcessor.h"

#include "libmmcs_client/CommandReply.h"

#include <bgq_util/include/pthreadmutex.h>

#include <utility/include/UserId.h>

#include <pthread.h>

namespace mmcs {
namespace console {

class RedirectMonitorThread : public common::Thread
{
public:

    struct Parms
    {
        Parms(
                common::ConsoleController* controllerarg,
                const std::string& block,
                mmcs_client::CommandReply* replyarg
                ) :
            controller(controllerarg),
            user(controller->getUser()),
            blockName(block),
            mmcsCommands(controller->getCommandProcessor()->getCommandMap()),
            reply(replyarg)
        {

        }
        common::ConsoleController* const controller;
        const bgq::utility::UserId user;
        const std::string blockName;
        MMCSCommandMap* const mmcsCommands;
        mmcs_client::CommandReply* const reply;
    };

    // Each mmcs console is allowed to redirect a single block
    static RedirectMonitorThread *redirectMonitorThread;

    static PthreadMutex redirectInitMutex;
    static pthread_cond_t redirectInitCond;

    RedirectMonitorThread() : Thread() { stdout = true; force_end = false; };
    void* threadStart();
    bool stdout;
    bool force_end;
};

} } // namespace mmcs::console

#endif
