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
#include "mux/handlers/LogLevel.h"

#include "common/commands/LogLevel.h"
#include "common/logging.h"

#include "mux/CommandConnection.h"
#include "mux/Options.h"

#include <utility/include/ScopeGuard.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::mux::log );

namespace runjob {
namespace mux {
namespace handlers {

LogLevel::LogLevel() :
    CommandHandler()
{

}

void
LogLevel::handle(
        const runjob::commands::Request::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    const runjob::commands::request::LogLevel::Ptr logLevelMessage(
        boost::static_pointer_cast<runjob::commands::request::LogLevel>(request)
    );

    // create response
    runjob::commands::response::LogLevel::Ptr response( new runjob::commands::response::LogLevel );
    bgq::utility::ScopeGuard writeGuard(
            boost::bind(
                &CommandConnection::write,
                connection,
                response
                )
            );

    if ( logLevelMessage->getArgs().empty() ) {
        using namespace log4cxx;

        // output all loggers
        LoggerPtr root = Logger::getRootLogger();
        BOOST_FOREACH( LoggerPtr i, root->getLoggerRepository()->getCurrentLoggers() ) {
            if ( i && i->getLevel() ) {
                // add logger name and level to response
                runjob::commands::response::LogLevel::Logger result;
                result._name = i->getName();
                result._level = i->getLevel()->toString();
                response->addLogger( result );
            }
        }
    } else {
        // set logging level
        try {
            bgq::utility::LoggingProgramOptions lpo( std::string("ibm.") + runjob::mux::log );
            lpo.notifier( logLevelMessage->getArgs() );
            lpo.apply();
        } catch ( const std::invalid_argument& e ) {
            LOG_WARN_MSG( e.what() );
            response->setError( runjob::commands::error::logging_level_invalid );
            response->setMessage( e.what() );
        }
    }
}

} // handlers
} // mux
} // runjob
