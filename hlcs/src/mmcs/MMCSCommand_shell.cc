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
 * \file MMCSCommand_shell.cc
 */

#include <sys/wait.h>

#include <cstdio>
#include <unistd.h>

#include "MMCSCommand_shell.h"
#include "MMCSCommandReply.h"
#include "ConsoleController.h"
#include "MMCSProperties.h"
#include <boost/lexical_cast.hpp>

MMCSCommand_comment*
MMCSCommand_comment::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);             // does not require a BlockController object
    commandAttributes.requiresConnection(false);        // does not require  mc_server connections
    commandAttributes.requiresTarget(false);            // does not require a BlockControllerTarget object
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(DEFAULT);
    return new MMCSCommand_comment("#", "# [<comment>]", commandAttributes);
}

void
MMCSCommand_comment::execute(deque<string> args,
                 MMCSCommandReply& reply,
                 ConsoleController* pController,
                 BlockControllerTarget* pTarget)
{
    if(MMCSProperties::getProperty(NO_SHELL) == "true") {
        reply << FAIL << DONE;
        return;
    }
    reply << OK << DONE;
}

void
MMCSCommand_comment::help(deque<string> args,
              MMCSCommandReply& reply)
{
    reply << OK << description()
      << ";Indicates a comment line"
      << DONE;
}

MMCSCommand_quit*
MMCSCommand_quit::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);             // does not require a BlockController object
    commandAttributes.requiresConnection(false);        // does not require  mc_server connections
    commandAttributes.requiresTarget(false);            // does not require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(DEFAULT);
    return new MMCSCommand_quit("quit", "quit", commandAttributes);
}

void
MMCSCommand_quit::execute(deque<string> args,
              MMCSCommandReply& reply,
              ConsoleController* pController,
              BlockControllerTarget* pTarget)
{
    pController->quit(EXIT_SUCCESS);
    reply << OK << DONE;
}

void
MMCSCommand_quit::help(deque<string> args,
               MMCSCommandReply& reply)
{
    // Lite version of the command drops connections to mc_server.
    reply << OK << description()
      << ";End mmcs. The block is implicitly destroyed and the mc_server connections are dropped."
      << DONE;
}

MMCSConsoleCommand_quit*
MMCSConsoleCommand_quit::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);             // does not require a BlockController object
    commandAttributes.requiresConnection(false);        // does not require  mc_server connections
    commandAttributes.requiresTarget(false);            // does not require a BlockControllerTarget object
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(DEFAULT);
    return new MMCSConsoleCommand_quit("quit", "quit", commandAttributes);
}

void
MMCSConsoleCommand_quit::execute(deque<string> args,
                 MMCSCommandReply& reply,
                 ConsoleController* pController,
                 BlockControllerTarget* pTarget)
{
    pController->quit(EXIT_SUCCESS);
    reply << OK << DONE;
}

void
MMCSConsoleCommand_quit::help(deque<string> args,
                  MMCSCommandReply& reply)
{
    reply << OK << description()
      << ";end the console."
      << ";This command does not change the status of any allocated blocks"
      << DONE;
}

MMCSCommand_sleep*
MMCSCommand_sleep::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);             // does not require a BlockController object
    commandAttributes.requiresConnection(false);        // does not require  mc_server connections
    commandAttributes.requiresTarget(false);            // does not require a BlockControllerTarget object
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(DEFAULT);
    return new MMCSCommand_sleep("sleep", "sleep <seconds>", commandAttributes);
}

void
MMCSCommand_sleep::execute(deque<string> args,
               MMCSCommandReply& reply,
               ConsoleController* pController,
               BlockControllerTarget* pTarget)
{
    if (args.size() != 1)
    {
        reply << FAIL << "args? " << usage <<  DONE;
        return;
    }

    try {
        const int secs = boost::lexical_cast<int>( args[0] );
        if ( secs <= 0 ) {
            reply << FAIL << "seconds must > 0" << DONE;
            return;
        }

        sleep(secs);
        reply << OK << DONE;
    } catch ( const boost::bad_lexical_cast& e ) {
        reply << FAIL << "invalid number of seconds: " << args[0] << DONE;
    }
}

void
MMCSCommand_sleep::help(deque<string> args,
            MMCSCommandReply& reply)
{
    reply << OK << description()
      << ";Pause for the specified number of seconds"
      << DONE;
}

MMCSCommand_sub_shell*
MMCSCommand_sub_shell::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);             // does not require a BlockController object
    commandAttributes.requiresConnection(false);        // does not require  mc_server connections
    commandAttributes.requiresTarget(false);            // does not require a BlockControllerTarget object
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(DEFAULT);
    return new MMCSCommand_sub_shell("!", "! [<shell-command>]", commandAttributes);
}

void
MMCSCommand_sub_shell::execute(deque<string> args,
                  MMCSCommandReply& reply,
                  ConsoleController* pController,
                  BlockControllerTarget* pTarget)
{
    if(MMCSProperties::getProperty(NO_SHELL) == "true") {
        reply << FAIL << DONE;
        return;
    }

    string escapeCommand;

    if (args.size() > 0) {
        for (unsigned i = 0; i < args.size(); ++i)
        {
            escapeCommand += args[i] + " ";
        }
    } else
        escapeCommand = "/bin/bash -i";
    int rc = system(escapeCommand.c_str());
    if ( WEXITSTATUS(rc) != 0 ) {
        reply << FAIL << WEXITSTATUS(rc);
    } else {
        reply << OK;
    }

    reply << DONE;
}

void
MMCSCommand_sub_shell::help(deque<string> args,
               MMCSCommandReply& reply)
{
    reply << OK << description()
      << ";Executes a sub_shell if no <command> is specified."
      << ";Use 'exit' to return to mmcs$ prompt."
      << ";If optional <command> is specified, executes that command in a sub_shell and returns you to mmcs$ prompt."
      << DONE;
}

MMCSCommand_redirect_input*
MMCSCommand_redirect_input::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);             // does not require a BlockController object
    commandAttributes.requiresConnection(false);        // does not require  mc_server connections
    commandAttributes.requiresTarget(false);            // does not require a BlockControllerTarget object
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(DEFAULT);
    return new MMCSCommand_redirect_input("<", "< <filename>", commandAttributes);
}

void
MMCSCommand_redirect_input::execute(deque<string> args,
                    MMCSCommandReply& reply,
                    ConsoleController* pController,
                    BlockControllerTarget* pTarget)
{
    if(MMCSProperties::getProperty(NO_SHELL) == "true") {
        reply << FAIL << DONE;
        return;
    }

    if (args.size() != 1)
    {
        reply << FAIL << "args? " << usage <<  DONE;
        return;
    }


    FILE *fin = fopen(args[0].c_str(), "r");
    if (!fin)
    {
        reply << FAIL << "can't open \"" << args[0] << "\"" << DONE;
        return;
    }

    if (!pController->pushInput(fin))
    {
        reply << FAIL << "input stream recursion too deep" << DONE;
        return;
    }


    reply << OK << DONE;
}

void
MMCSCommand_redirect_input::help(deque<string> args,
                 MMCSCommandReply& reply)
{
    reply << OK << description()
      << ";Reads subsequent commands from <filename>."
      << ";Returns to reading stdin when EOF is reached on <filename>"
      << DONE;
}
