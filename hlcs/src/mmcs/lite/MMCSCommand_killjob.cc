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

#include "lite/MMCSCommand_killjob.h"

#include "lite/Job.h"

#include "MMCSCommand_lite.h"

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "mmcs" );

MMCSCommand_killjob::MMCSCommand_killjob(
        const char* name,
        const char* description,
        const MMCSCommandAttributes& attributes
        ) :
    MMCSCommand(name, description, attributes)
{
    // nothing to do
}

MMCSCommand_killjob*
MMCSCommand_killjob::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock( true );
    commandAttributes.requiresConnection( true );
    commandAttributes.requiresTarget( false );
    commandAttributes.mmcsLiteCommand( true );
    return new MMCSCommand_killjob("killjob", "killjob [signal]", commandAttributes);
}

void
MMCSCommand_killjob::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{
    // cast console controller to LiteConsoleController
    LiteConsoleController* console = dynamic_cast<LiteConsoleController*>( pController );
    BOOST_ASSERT( console );

    // validate args
    if ( args.size() > 1 ) {
        reply << FAIL << _description << DONE;
        return;
    }

    // ensure a previous job is still running
    lite::Job::Ptr job = console->getJob().lock();
    if ( !job ) {
        reply << FAIL << "Job not running" << DONE;
        return;
    }

    int signo = SIGKILL;
    if ( args.size() == 1 ) {
        try {
            signo = boost::lexical_cast<int>( args[0] );
        } catch ( const boost::bad_lexical_cast& e ) {
            reply << FAIL << "signal " << args[0] << " is not a number" << DONE;
            return;
        }
    }

    // kill job
    job->kill( signo );
    reply << OK << "sent signal " << signo << " to job" << DONE;
}

void
MMCSCommand_killjob::help(
        deque<string> args,
        MMCSCommandReply& reply
        )
{
    reply << OK << description();
    reply << ";sends the specified signal to the currently active job.";
    reply << ";If no signal is specified, SIGKILL is sent.";
    reply << DONE;
}
