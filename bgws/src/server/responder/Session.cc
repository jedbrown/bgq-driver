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

/*! \page sessionResource /bg/bgws/sessions/<i>sessionId</i>

This resource provides operations on a session.

Use the special value "current" for the session ID to perform the operation on the current session.

- \subpage sessionResourceGet
- \subpage sessionResourceDelete

 */

/*! \page sessionResourceGet GET /bg/bgws/sessions/<i>sessionId</i>

Get session details.

Use the special value "current" for the session ID to get details for the current session
(as specified in the session ID header).


\section Authorization

The user must be authenticated and the user must be an administrator or have started the session.

\section sessionResourceGetResponse JSON response format

<pre>
{
  "sessionId" : &quot;<i>string</i>&quot;, // optional
  "user" : &quot;<i>string</i>&quot;, // optional
  "isAdministrator" : <i>Boolean</i>, //optional, false if not present.
  "hardwareRead" : <i>Boolean</i>, // optional, false if not present.
  "blockCreate" : <i>Boolean</i> // optional, false if not present.
}
</pre>

\section Errors

HTTP status: 403 Forbidden
- notAuthenticated: The user isn't authenticated.

HTTP status: 404 Not Found
- notFound: not authorized or the session doesn't exist.

 */


/*! \page sessionResourceDelete DELETE /bg/bgws/sessions/<i>sessionId</i>

End a session.

Use the special value "current" for the session ID to end the current session
(as specified in the session ID header).


\section Authorization

The user must be an administrator or have started the session.

\section Response

HTTP status: 204 No Content

\section Errors

HTTP status: 404 Not Found
- notFound: not authorized or the session doesn't exist.

 */


#include "Session.hpp"

#include "Sessions.hpp"

#include "../Error.hpp"
#include "../Sessions.hpp"

#include "capena-http/http/http.hpp"

#include "capena-http/server/exception.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>

#include <boost/throw_exception.hpp>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {


bool Session::matchesUrl( const capena::http::uri::Path& request_path )
{
    // check if the path without the last element valid for SessionsRequestHandler.

    if ( request_path.empty() )  return false;
    if ( request_path.back().empty() )  return false; // last element (session ID) can't be empty.

    return Sessions::matchesUrl( request_path.calcParent() );
}


capena::http::uri::Path Session::calcPath(
            const std::string& session_id,
            const capena::http::uri::Path& path_base
        )
{
    return (path_base / Sessions::RESOURCE_PATH / session_id);
}


json::ValuePtr Session::calcUserJson(
        const UserInfo& user_info,
        const std::string& session_id,
        const hlcs::security::Enforcer& enforcer
    )
{
    json::ObjectValuePtr obj_val_ptr(json::Object::create());
    json::Object &obj(obj_val_ptr->get());

    if ( session_id != string() ) {
        obj.set( "sessionId", session_id );
    }


    if ( ! user_info.isAnonymous() ) {
        obj.set( "user", user_info.getUserId().getUser() );
    }
    if ( user_info.isAdministrator() ) {

        obj.set( "isAdministrator", true );
        obj.set( "hardwareRead", true );
        obj.set( "blockCreate", true );

    } else {

        if ( enforcer.validate(
                     hlcs::security::Object( hlcs::security::Object::Hardware, string() ),
                     hlcs::security::Action::Read,
                     user_info.getUserId()
             ) )
        {
            obj.set( "hardwareRead", true );
        }

        if ( enforcer.validate(
                 hlcs::security::Object(
                        hlcs::security::Object::Block,
                        string() // no block ID when creating block.
                 ),
                 hlcs::security::Action::Create,
                 user_info.getUserId()
             ) )
        {
            obj.set( "blockCreate", true );
        }

    }

    return obj_val_ptr;
}



capena::http::Methods Session::_getAllowedMethods() const
{
    return { capena::http::Method::GET, capena::http::Method::DELETE };
}


void Session::_doDelete()
{
    const string &session_id(_getRequestedResourcePath().back());

    _deleteSession(
            session_id
        );
}


void Session::_doGet()
{
    const string &session_id(_getRequestedResourcePath().back());

    Error::Data error_data;
    error_data["sessionId"] = session_id;

    if ( ! _isUserAuthenticated() ) {
        LOG_WARN_MSG( "Cannot get session details because the user isn't authenticated." );

        BOOST_THROW_EXCEPTION( Error(
                "Cannot get session details because the user isn't authenticated.",
                "getSession", "notAuthenticated", error_data,
                capena::http::Status::Forbidden
            ) );
    }

    SessionPtr session_ptr;

    if ( session_id == "current" ) {

        session_ptr = _session_ptr;

    } else {

        if ( _sessions.checkSession( session_id, &session_ptr ) ) {

            // The session id is valid.
            LOG_DEBUG_MSG( "Session is valid, user=" << session_ptr->getUserId().getUser() << " session ID=" << session_id );

            if ( _isUserAdministrator() ) {
                LOG_DEBUG_MSG( "User is administrator so can look at this session." );
            } else if ( _getRequestUserId().getUid() == session_ptr->getUserId().getUid() ) {
                LOG_DEBUG_MSG( "User is administrator so can look at this session." );
            } else {

                LOG_WARN_MSG( "User " << _getRequestUserInfo() << " tried to get " << session_ptr->getUserInfo() << " session info!" );

                BOOST_THROW_EXCEPTION( Error(
                        "Could get session detaions for '" + session_id + " because the session does not exist.",
                        "getSession", "notFound", error_data,
                        capena::http::Status::NotFound
                    ) );

            }

        } else {

            BOOST_THROW_EXCEPTION( Error(
                    "Could get session detaions for '" + session_id + " because the session does not exist.",
                    "getSession", "notFound", error_data,
                    capena::http::Status::NotFound
                ) );

        }

    }

    const UserInfo &user_info(session_ptr ? session_ptr->getUserInfo() : _getRequestUserInfo());

    json::ValuePtr val_ptr(calcUserJson(
            user_info,
            session_ptr ? session_ptr->getId() : string(),
            _getEnforcer()
        ));

    auto &response(_getResponse());

    response.setContentTypeJson();
    response.headersComplete();
    json::Formatter()( *val_ptr, response.out() );
}


void Session::_deleteSession(
        std::string session_id
    )
{
    LOG_DEBUG_MSG( "Requested to delete session " << session_id );

    Error::Data error_data;
    error_data["sessionId"] = session_id;

    capena::server::Response &response(_getResponse());


    if ( session_id == "current" ) {
        if ( ! _session_ptr ) {
            // This is a temporary session so just return.

            response.setStatus( capena::http::Status::NoContent );
            response.headersComplete();
            return;
        }

        session_id = _session_ptr->getId();
    }


    bgws::Sessions::Result::Value result;

    _sessions.end(
            session_id,
            _getRequestUserInfo(),
            &result
        );

    if ( result == bgws::Sessions::Result::Ended ) {
        // The session was successfully ended.
        response.setStatus( capena::http::Status::NoContent );
        response.headersComplete();
        return;
    }

    if ( result == bgws::Sessions::Result::NotAuthorized ) {
        LOG_WARN_MSG( "Requested to delete session " << session_id << " " << " by " << _getRequestUserInfo() << " but the session is not owned by the user." );

        BOOST_THROW_EXCEPTION( Error(
                "Could not end session '" + session_id + " because the session does not exist.",
                "endSession", "notFound", error_data,
                capena::http::Status::NotFound
            ) );

    }


    LOG_WARN_MSG( "Requested to delete session " << session_id << " but the session is not known to the server." );

    BOOST_THROW_EXCEPTION( Error(
            "Could not end session '" + session_id + " because the session does not exist.",
            "endSession", "notFound", error_data,
            capena::http::Status::NotFound
        ) );

}


} // namespace bgws::responder
} // namespace bgws
