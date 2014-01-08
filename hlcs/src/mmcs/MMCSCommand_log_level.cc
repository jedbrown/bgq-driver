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

#include "MMCSCommand_log_level.h"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>

#include <boost/foreach.hpp>

LOG_DECLARE_FILE( "mmcs" );

MMCSCommand_log_level::MMCSCommand_log_level(
        const char* name,
        const char* description,
        const MMCSCommandAttributes& attributes
        ) :
    MMCSCommand(name, description, attributes)
{
    // nothing to do
}

MMCSCommand_log_level*
MMCSCommand_log_level::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock( false );
    commandAttributes.requiresConnection( false );
    commandAttributes.requiresTarget( false );
    commandAttributes.mmcsLiteCommand( true );
    commandAttributes.mmcsServerCommand( true );
    commandAttributes.bgadminAuth(true);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    return new MMCSCommand_log_level("log_level", "log_level [logger=level ... logger=level]", commandAttributes);
}

void
MMCSCommand_log_level::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{
    using namespace log4cxx;

    if ( args.empty() ) {
        // output all current loggers and their levels
        LoggerPtr root = Logger::getRootLogger();
        reply << OK;
        BOOST_FOREACH( LoggerPtr i, root->getLoggerRepository()->getCurrentLoggers() ) {
            if ( i && i->getLevel() ) {
                std::ostringstream os;
                os <<
                    std::setw(40) << std::left <<
                    i->getName() << std::right <<
                    i->getLevel()->toString()
                    ;
                reply << os.str() << ";";
            }
        }
        reply << DONE;
    } else if ( args.size() > 0 ) {
        // collect arguments
        bgq::utility::LoggingProgramOptions::Strings strings;
        while ( !args.empty() ) {
            strings.push_back( args[0] );
            (void)args.pop_front();
        }

        // set logging level
        try {
            bgq::utility::LoggingProgramOptions lpo( "ibm.mmcs" );
            lpo.notifier( strings );
            lpo.apply();
            reply << OK << DONE;
        } catch ( const std::invalid_argument& e ) {
            reply << FAIL << e.what() << DONE;
        }
    } else {
        reply << FAIL << this->description() << DONE;
    }
}

void
MMCSCommand_log_level::help(
        deque<string> args,
        MMCSCommandReply& reply
        )
{
    reply << OK << description();
    reply << ";sets the specified loggers to the requested levels.  If no logger is given,";
    reply << ";all loggers and their current levels are displayed. Each logger parameter can";
    reply << ";have one of three formats:";
    reply << ";   1. logger=level : set the logger to the level.";
    reply << ";   2. level : set the ibm.mmcs logger to the level.";
    reply << ";   3. logger : set the logger's level to Debug. The logger must contain a '.' character.";
    reply << ";";
    reply << ";Valid levels are:";
    reply << ";OFF / O / 0";
    reply << ";FATAL / F / 1";
    reply << ";ERROR / E / 2";
    reply << ";WARN / W / 3";
    reply << ";INFO / I / 4";
    reply << ";DEBUG / D / 5";
    reply << ";TRACE / T / 6";
    reply << ";ALL / A / 7";
    reply << ";";
    reply << ";Example:";
    reply << ";log_level ibm.mmcs=T";

    reply << DONE;
}
