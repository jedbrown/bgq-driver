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

#include "lite/MMCSCommand_waitjob.h"

#include "lite/Job.h"

#include "MMCSCommand_lite.h"

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "mmcs" );

MMCSCommand_waitjob::MMCSCommand_waitjob(
        const char* name,
        const char* description,
        const MMCSCommandAttributes& attributes
        ) :
    MMCSCommand(name, description, attributes)
{
    // nothing to do
}

MMCSCommand_waitjob*
MMCSCommand_waitjob::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock( true );
    commandAttributes.requiresConnection( true );
    commandAttributes.requiresTarget( false );
    commandAttributes.mmcsLiteCommand( true );
    return new MMCSCommand_waitjob("waitjob", "waitjob [seconds]", commandAttributes);
}

void
MMCSCommand_waitjob::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{
    // cast console controller to LiteConsoleController
    LiteConsoleController* console = dynamic_cast<LiteConsoleController*>( pController );
    BOOST_ASSERT( console );

    // parse args
    unsigned seconds = 0;
    if ( args.size() == 1 ) {
        try {
            seconds = boost::lexical_cast<unsigned>( args[0] );
            if ( seconds == 0 ) {
                reply << FAIL << "seconds must be > 0" << DONE;
                return;
            }
        } catch ( const boost::bad_lexical_cast& e ) {
            reply << FAIL << "seconds " << args[0] << " is not a number" << DONE;
            return;
        }
    } else if ( args.size() > 1 ) {
        reply << FAIL << _description << DONE;
        return;
    }

    // ensure job is running
    if ( console->getJob().expired() ) {
        reply << FAIL << "Job not running;" << this->description() << DONE;
        return;
    }

    // wait for job to complete
    while ( 1 ) {
        lite::Job::Ptr job = console->getJob().lock();
        if ( !job ) {
            reply << OK << DONE;
            return;
        } else {
            // job still active, sleep a bit
            sleep(1);

            if ( args.size() == 1 && --seconds == 0 ) {
                reply << FAIL << "timed out before job completed" << DONE;
                return;
            }
        }
    }
}

void
MMCSCommand_waitjob::help(
        deque<string> args,
        MMCSCommandReply& reply
        )
{
    reply << OK << description();
    reply << ";waits for the running job to complete.";
    reply << ";If seconds are specified, that is the maximum time it will wait. Otherwise it will wait until the job completes.";
    reply << DONE;
}
