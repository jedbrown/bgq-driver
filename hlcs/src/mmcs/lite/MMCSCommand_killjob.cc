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

#include "MMCSCommand_killjob.h"

#include "ConsoleController.h"
#include "Job.h"
#include "MMCSCommand_lite.h"

#include <utility/include/Log.h>


LOG_DECLARE_FILE( "mmcs.lite" );


using namespace std;


namespace mmcs {
namespace lite {


MMCSCommand_killjob::MMCSCommand_killjob(
        const char* name,
        const char* description,
        const Attributes& attributes
        ) :
    AbstractCommand(name, description, attributes)
{
    // nothing to do
}

MMCSCommand_killjob*
MMCSCommand_killjob::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock( true );
    commandAttributes.requiresConnection( true );
    commandAttributes.requiresTarget( false );
    commandAttributes.mmcsLiteCommand( true );
    return new MMCSCommand_killjob("killjob", "killjob [signal]", commandAttributes);
}

void
MMCSCommand_killjob::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        server::BlockControllerTarget* pTarget
        )
{
    // cast console controller to LiteConsoleController
    lite::ConsoleController* console = dynamic_cast<lite::ConsoleController*>( pController );
    BOOST_ASSERT( console );

    // validate args
    if ( args.size() > 1 ) {
        reply << mmcs_client::FAIL << _description << mmcs_client::DONE;
        return;
    }

    // ensure a previous job is still running
    lite::Job::Ptr job = console->getJob().lock();
    if ( !job ) {
        reply << mmcs_client::FAIL << "Job not running" << mmcs_client::DONE;
        return;
    }

    int signo = SIGKILL;
    if ( args.size() == 1 ) {
        try {
            signo = boost::lexical_cast<int>( args[0] );
        } catch ( const boost::bad_lexical_cast& e ) {
            reply << mmcs_client::FAIL << "signal " << args[0] << " is not a number" << mmcs_client::DONE;
            return;
        }
    }

    // kill job
    job->kill( signo );
    reply << mmcs_client::OK << "sent signal " << signo << " to job" << mmcs_client::DONE;
}

void
MMCSCommand_killjob::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description();
    reply << ";sends the specified signal to the currently active job.";
    reply << ";If no signal is specified, SIGKILL is sent.";
    reply << mmcs_client::DONE;
}

} } // namespace mmcs::lite
