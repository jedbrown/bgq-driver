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


/*! \page loggingResource /bg/bgwsServer/logging

This resource provides operations on logging configuration for the BGWS server.

- \subpage loggingResourceGet
- \subpage loggingResourcePut

 */

/*! \page loggingResourceGet GET /bg/bgwsServer/logging

Get current logging configuration for the BGWS server.

\section Authority

The user must be a Blue Gene administrator.

\section loggingResourceGetResponse JSON response format

<pre>
{
  &quot;<i>loggerName</i>&quot; : &quot;<i>level</i>&quot;,
  ...
}
</pre>

\section Errors

HTTP status: 403 Forbidden
- authority: The user doesn't have authority.

 */

/*! \page loggingResourcePut PUT /bg/bgwsServer/logging

Update the logging level for the web services server. The changes are not persistent.

\section Authority

The user must be a Blue Gene administrator.

\section loggingResourcePutData JSON request data format

<pre>
{
  &quot;<i>loggerName</i>&quot; : &quot;<i>level</i>&quot;,
  ...
}
</pre>

level is one of
- "OFF",
- "FATAL"
- "ERROR"
- "WARN"
- "INFO"
- "DEBUG"
- "TRACE",
- "ALL"

If a level is given that isn't one of the above values then the level for the logger will be set to DEBUG.


\section Response

HTTP status: 204 No Content

\section Errors

HTTP status: 400 Bad Request
- inputWrongType: The posted data wasn't a JSON object.

HTTP status: 403 Forbidden
- authority: The user doesn't have authority.

 */


#include "Logging.hpp"

#include "../constants.hpp"
#include "../Error.hpp"
#include "../ras.hpp"

#include "common/common.hpp"

#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>

#include <boost/throw_exception.hpp>


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {


const capena::http::uri::Path &Logging::RESOURCE_PATH(::bgws::common::resource_path::LOGGING);


bool Logging::matchesUrl(
        const capena::http::uri::Path& request_path
    )
{
    return (request_path == RESOURCE_PATH);
}


capena::http::Methods Logging::_getAllowedMethods() const
{
    return { capena::http::Method::GET, capena::http::Method::PUT };
}


void Logging::_doGet()
{
    if ( ! _isUserAdministrator() ) {
        LOG_WARN_MSG( "Could not get BGWS logging configuration because " << _getRequestUserInfo() << " doesn't have authority (must be administrator)." );

        ras::postAdminAuthorityFailure( _getRequestUserInfo(), "get logging configuration" );

        BOOST_THROW_EXCEPTION( Error(
                "Could not get BGWS logging configuration because the user doesn't have authority.",
                "getBgwsServerLogging", "authority", Error::Data(),
                capena::http::Status::Forbidden
            ) );

        return;
    }


    log4cxx::LoggerList loggers(log4cxx::Logger::getRootLogger()->getLoggerRepository()->getCurrentLoggers());

    _formatLoggingConfiguration( loggers );
}


void Logging::_doPut( json::ConstValuePtr val_ptr )
{
    if ( ! _isUserAdministrator() ) {
        LOG_WARN_MSG( "Could not set BGWS logging configuration because " << _getRequestUserInfo() << " doesn't have authority (must be administrator)." );

        ras::postAdminAuthorityFailure( _getRequestUserInfo(), "set logging configuration" );

        BOOST_THROW_EXCEPTION( Error(
                "Could not set BGWS logging configuration because the user doesn't have authority.",
                "setBgwsServerLogging", "authority", Error::Data(),
                capena::http::Status::Forbidden
            ) );

        return;
    }


    capena::server::Response &response(_getResponse());

    _LoggingInfo logging_info(_parseLoggingUpdate( val_ptr ));

    std::ostringstream oss;

    oss << "Updating BGWS server logging configuration\n";
    bgq::utility::LoggingProgramOptions::Strings strings;
    for ( _LoggingInfo::const_iterator i(logging_info.begin()) ; i != logging_info.end() ; ++i ) {
        oss << "\tSetting '" << i->first << "' logger to '" << i->second << "'\n";
        strings.push_back( i->first + "=" + i->second );
    }

    bgq::utility::LoggingProgramOptions lpo( "ibm.bgws" );
    try {
        lpo.notifier( strings );
        lpo.apply();
    } catch ( const std::exception& e ) {
        BOOST_THROW_EXCEPTION(
                Error(
                    e.what(),
                    "configureLogging", "inputWrongType", Error::Data(),
                    capena::http::Status::BadRequest
                    )
                );
    }

    LOG_INFO_MSG_FORCED( oss.str() );

    response.setStatus( capena::http::Status::NoContent );
    response.headersComplete();
}


void Logging::_formatLoggingConfiguration(
        const log4cxx::LoggerList& loggers
    )
{
    capena::server::Response &response(_getResponse());

    response.setContentTypeJson();
    response.headersComplete();

    json::ObjectValue logging_value;
    json::Object &logging_obj(logging_value.get());

    for ( log4cxx::LoggerList::const_iterator i(loggers.begin()) ; i != loggers.end() ; ++i ) {
        if ( (*i)->getLevel() ) {
            logging_obj.set( (*i)->getName(), (*i)->getLevel()->toString() );
        }
    }

    json::Formatter()( logging_value, response.out() );
}


Logging::_LoggingInfo Logging::_parseLoggingUpdate(
        json::ConstValuePtr val_ptr
    )
{
    _LoggingInfo ret;

    try {

        const json::Object &obj(val_ptr->getObject());

        for ( json::Object::const_iterator i(obj.begin()) ; i != obj.end() ; ++i ) {
            try {

                ret[i->first] = i->second->getString();
                LOG_DEBUG_MSG( "parseLogging: " << i->first << "=" << i->second->getString() );

            } catch ( const json::WrongType& e ) {

                LOG_WARN_MSG( "Received invalid input on configure logging request, value for '" << i->first << "' is not a string." );

            }
        }

    } catch ( const json::WrongType& e ) {

        LOG_WARN_MSG( "Received invalid input on configure logging request, document is not a JSON object." );

        BOOST_THROW_EXCEPTION( Error(
                "Could not set BGWS logging configuration because the input was not a JSON object.",
                "configureLogging", "inputWrongType", Error::Data(),
                capena::http::Status::BadRequest
            ) );

    }

    return ret;
}


} } // namespace bgws::responder
