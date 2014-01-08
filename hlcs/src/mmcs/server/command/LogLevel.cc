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

#include "LogLevel.h"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>

#include <boost/foreach.hpp>

LOG_DECLARE_FILE( "mmcs.server" );

using namespace std;

namespace mmcs {
namespace server {
namespace command {

LogLevel::LogLevel(
        const char* name,
        const char* description,
        const Attributes& attributes
        ) :
    AbstractCommand(name, description, attributes)
{
    // nothing to do
}

LogLevel*
LogLevel::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock( false );
    commandAttributes.requiresConnection( false );
    commandAttributes.requiresTarget( false );
    commandAttributes.mmcsLiteCommand( true );
    commandAttributes.mmcsServerCommand( true );
    commandAttributes.bgadminAuth(true);
    commandAttributes.helpCategory(common::ADMIN);             // 'help admin'  will include this command's summary
    return new LogLevel("log_level", "log_level [logger=level ... logger=level]", commandAttributes);
}

void
LogLevel::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    using namespace log4cxx;

    if ( args.empty() ) {
        // output all current loggers and their levels
        LoggerPtr root = Logger::getRootLogger();
        reply << mmcs_client::OK;
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
        reply << mmcs_client::DONE;
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
            reply << mmcs_client::OK << mmcs_client::DONE;
        } catch ( const std::invalid_argument& e ) {
            reply << mmcs_client::FAIL << e.what() << mmcs_client::DONE;
        }
    } else {
        reply << mmcs_client::FAIL << this->description() << mmcs_client::DONE;
    }
}

void
LogLevel::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description();
    reply << ";Sets the specified loggers to the requested levels. If no logger is given,";
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
    reply << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
