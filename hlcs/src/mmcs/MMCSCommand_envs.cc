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
/* (C) Copyright IBM Corp.  2005, 2011                              */
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

/*!
 * \file MMCSCommand_envs.cc
 */

#include "BlockControllerTarget.h"
#include "ConsoleController.h"
#include "MMCSCommandReply.h"
#include "MMCSCommand_envs.h"
#include "MMCSEnvMonitor.h"

#include <memory>

extern EnvMonitorThread*        envMonitor;             // background thread for handling environmentals
extern LocationThreadList*      locThreads;

MMCSCommand_start_hw_polling*
MMCSCommand_start_hw_polling::build() {
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);           // does not require a BlockController object
    commandAttributes.requiresConnection(false);      // does not require connected target set
    commandAttributes.requiresTarget(false);          // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);          // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(false);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    commandAttributes.bgadminAuth(true);
    return new MMCSCommand_start_hw_polling("start_hw_polling", "start_hw_polling  <type> <location> <seconds>", commandAttributes);
}

void
MMCSCommand_start_hw_polling::execute(std::deque<std::string> args,
                              MMCSCommandReply& reply,
                              ConsoleController* pController,
                              BlockControllerTarget* pTarget)
{
    // Create a thread for a specific location
    std::auto_ptr<LocationThread> locThread( new LocationThread );

    try {
        const int seconds = boost::lexical_cast<unsigned>(args[2]);
        if ( seconds <= 0 ) {
            reply << FAIL << ";invalid value for seconds: " << args[2] << DONE;
            return;
        }
        locThread->seconds = seconds;
    } catch (const boost::bad_lexical_cast& e) {
        reply << FAIL << ";invalid value for seconds" << DONE;
        return;
    }

    locThread->location = args[1];
    locThread->type = args[0];

    if ((args[0]  != "service") && (args[0]  != "node")  && (args[0]  != "io")  && (args[0]  != "bulk")) {
        reply << FAIL << ";polling could not be started, " << args[0] << " is an invalid type" << DONE;
        return;
    }

    if (locThreads->add(locThread.get())) {
        locThread.release();
        reply << OK << DONE;
    } else
        reply << FAIL << ";polling could not be started, or is already in effect for location " << args[1] << DONE;
    return;
}

void
MMCSCommand_start_hw_polling::help(deque<string> args,
                 MMCSCommandReply& reply)
{
    reply << OK << description()
          << ";start polling a specific hardware location for environmental readings"
          << ";supported values for type are service, node, io, and bulk"
          << ";location supports regular expressions"
          << DONE;
}

MMCSCommand_stop_hw_polling*
MMCSCommand_stop_hw_polling::build() {
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);           // does not require a BlockController object
    commandAttributes.requiresConnection(false);      // does not require connected target set
    commandAttributes.requiresTarget(false);          // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);          // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(false);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    commandAttributes.bgadminAuth(true);
    return new MMCSCommand_stop_hw_polling("stop_hw_polling", "stop_hw_polling <location>", commandAttributes);
}

void
MMCSCommand_stop_hw_polling::execute(std::deque<std::string> args,
                              MMCSCommandReply& reply,
                              ConsoleController* pController,
                              BlockControllerTarget* pTarget)
{
    // look up the location in the map
    // kill the thread
    // remove it from the map
    if (locThreads->remove(args[0])) {
        reply << OK << DONE;
    } else
        reply << FAIL << ";no polling in effect for location " << args[0] << DONE;

    return;
}

void
MMCSCommand_stop_hw_polling::help(deque<string> args,
                 MMCSCommandReply& reply)
{
    reply << OK << description()
        << ";stop polling the specific hardware location for environmental readings"
        << DONE;
}



MMCSCommand_list_hw_polling*
MMCSCommand_list_hw_polling::build() {
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);           // does not require a BlockController object
    commandAttributes.requiresConnection(false);      // does not require connected target set
    commandAttributes.requiresTarget(false);          // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);          // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(false);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    commandAttributes.bgadminAuth(true);
    return new MMCSCommand_list_hw_polling("list_hw_polling", "list_hw_polling", commandAttributes);
}

void
MMCSCommand_list_hw_polling::execute(std::deque<std::string> args,
                              MMCSCommandReply& reply,
                              ConsoleController* pController,
                              BlockControllerTarget* pTarget)
{
    if (args.size() != 0)
    {
        reply << FAIL << "no arguments " << usage <<  DONE;
        return;
    }

    locThreads->list(reply);
    reply << DONE;

    return;
}

void
MMCSCommand_list_hw_polling::help(deque<string> args,
                 MMCSCommandReply& reply)
{
    reply << OK << description()
        << ";list the specific hardware locations that have polling in effect"
        << DONE;
}

