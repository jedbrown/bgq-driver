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

/*! \page error_Authentication Authentication Errors

These errors may occur when the server is authenticating the user.

The <em>operation</em> field in the \ref errorDocument "error response" is set to <b>authentication</b>.
The <em>id</em> field is set as below.


\section invalidSession

This error is returned if the session ID provided on the request doesn't match a valid session.

The session ID is provided in the X-Bgws-Session-Id header.

The session may have timed out, or the server may have been restarted.
The typical recovery is to \ref sessionsResourcePost "start a new session".


\section noUserId

This error is returned if no user ID is provided on the request when it's required because the command certificate was presented.

The User ID must be provided in the X-Bgws-User-Id header if the client connected with the command certificate.

This indicates a configuration problem, application error, or authority problem.

 */


#include "ResponderFactory.hpp"

#include "BgwsServer.hpp"
#include "Error.hpp"

#include "utility/base64.hpp"

#include "responder/Alert.hpp"
#include "responder/Alerts.hpp"
#include "responder/BgwsServer.hpp"
#include "responder/Block.hpp"
#include "responder/Blocks.hpp"
#include "responder/HardwareReplacements.hpp"
#include "responder/Job.hpp"
#include "responder/Jobs.hpp"
#include "responder/Logging.hpp"
#include "responder/PerformanceMonitoring.hpp"
#include "responder/Ras.hpp"
#include "responder/RasDetails.hpp"
#include "responder/ServiceAction.hpp"
#include "responder/ServiceActions.hpp"
#include "responder/Session.hpp"
#include "responder/Sessions.hpp"

#include "responder/diagnostics/Block.hpp"
#include "responder/diagnostics/Blocks.hpp"
#include "responder/diagnostics/Locations.hpp"
#include "responder/diagnostics/Run.hpp"
#include "responder/diagnostics/Runs.hpp"
#include "responder/diagnostics/Testcases.hpp"

#include "responder/environmentals/BulkPower.hpp"
#include "responder/environmentals/Coolant.hpp"
#include "responder/environmentals/Fan.hpp"
#include "responder/environmentals/IoCard.hpp"
#include "responder/environmentals/LinkChip.hpp"
#include "responder/environmentals/Node.hpp"
#include "responder/environmentals/NodeCard.hpp"
#include "responder/environmentals/Optical.hpp"
#include "responder/environmentals/ServiceCard.hpp"
#include "responder/environmentals/ServiceCardTemp.hpp"

#include "responder/hardware/Hardware.hpp"
#include "responder/hardware/Machine.hpp"

#include "responder/summary/Jobs.hpp"
#include "responder/summary/Machine.hpp"
#include "responder/summary/Ras.hpp"
#include "responder/summary/System.hpp"
#include "responder/summary/Utilization.hpp"

#include "capena-http/server/exception.hpp"
#include "capena-http/server/Request.hpp"

#include <utility/include/Log.h>

#include <boost/make_shared.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>
#include <string>


using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


namespace statics {

typedef bool (*MatchesUrlFn)( const capena::http::uri::Path& );
typedef AbstractResponder::Ptr (*CreateResponderFn)( AbstractResponder::CtorArgs& );

typedef std::pair<MatchesUrlFn,CreateResponderFn> MatcherToCreator;
typedef std::vector<MatcherToCreator> MatcherToCreators;


template<class T>
AbstractResponder::Ptr createResponder( AbstractResponder::CtorArgs& a )
{
    return boost::make_shared<T>( boost::ref( a ) );
}


static MatcherToCreators createMatcherToCreators()
{
    MatcherToCreators ret;

    ret.push_back( MatcherToCreator( &responder::Alert::matchesUrl, &(createResponder<responder::Alert>) ) );
    ret.push_back( MatcherToCreator( &responder::Alerts::matchesUrl, &(createResponder<responder::Alerts>) ) );
    ret.push_back( MatcherToCreator( &responder::BgwsServer::matchesUrl, &(createResponder<responder::BgwsServer>) ) );
    ret.push_back( MatcherToCreator( &responder::Block::matchesUrl, &(createResponder<responder::Block>) ) );
    ret.push_back( MatcherToCreator( &responder::Blocks::matchesUrl, &(createResponder<responder::Blocks>) ) );
    ret.push_back( MatcherToCreator( &responder::HardwareReplacements::matchesUrl, &(createResponder<responder::HardwareReplacements>) ) );
    ret.push_back( MatcherToCreator( &responder::diagnostics::Block::matchesUrl, &(createResponder<responder::diagnostics::Block>) ) );
    ret.push_back( MatcherToCreator( &responder::diagnostics::Blocks::matchesUrl, &(createResponder<responder::diagnostics::Blocks>) ) );
    ret.push_back( MatcherToCreator( &responder::diagnostics::Locations::matchesUrl, &(createResponder<responder::diagnostics::Locations>) ) );
    ret.push_back( MatcherToCreator( &responder::diagnostics::Run::matchesUrl, &(createResponder<responder::diagnostics::Run>) ) );
    ret.push_back( MatcherToCreator( &responder::diagnostics::Runs::matchesUrl, &(createResponder<responder::diagnostics::Runs>) ) );
    ret.push_back( MatcherToCreator( &responder::diagnostics::Testcases::matchesUrl, &(createResponder<responder::diagnostics::Testcases>) ) );
    ret.push_back( MatcherToCreator( &responder::Job::matchesUrl, &(createResponder<responder::Job>) ) );
    ret.push_back( MatcherToCreator( &responder::Jobs::matchesUrl, &(createResponder<responder::Jobs>) ) );
    ret.push_back( MatcherToCreator( &responder::Logging::matchesUrl, &(createResponder<responder::Logging>) ) );
    ret.push_back( MatcherToCreator( &responder::PerformanceMonitoring::matchesUrl, &(createResponder<responder::PerformanceMonitoring>) ) );
    ret.push_back( MatcherToCreator( &responder::Ras::matchesUrl, &(createResponder<responder::Ras>) ) );
    ret.push_back( MatcherToCreator( &responder::RasDetails::matchesUrl, &(createResponder<responder::RasDetails>) ) );
    ret.push_back( MatcherToCreator( &responder::ServiceAction::matchesUrl, &(createResponder<responder::ServiceAction>) ) );
    ret.push_back( MatcherToCreator( &responder::ServiceActions::matchesUrl, &(createResponder<responder::ServiceActions>) ) );
    ret.push_back( MatcherToCreator( &responder::Session::matchesUrl, &(createResponder<responder::Session>) ) );
    ret.push_back( MatcherToCreator( &responder::Sessions::matchesUrl, &(createResponder<responder::Sessions>) ) );
    ret.push_back( MatcherToCreator( &responder::environmentals::BulkPower::matchesUrl, &(createResponder<responder::environmentals::BulkPower>) ) );
    ret.push_back( MatcherToCreator( &responder::environmentals::Coolant::matchesUrl, &(createResponder<responder::environmentals::Coolant>) ) );
    ret.push_back( MatcherToCreator( &responder::environmentals::Fan::matchesUrl, &(createResponder<responder::environmentals::Fan>) ) );
    ret.push_back( MatcherToCreator( &responder::environmentals::IoCard::matchesUrl, &(createResponder<responder::environmentals::IoCard>) ) );
    ret.push_back( MatcherToCreator( &responder::environmentals::LinkChip::matchesUrl, &(createResponder<responder::environmentals::LinkChip>) ) );
    ret.push_back( MatcherToCreator( &responder::environmentals::Node::matchesUrl, &(createResponder<responder::environmentals::Node>) ) );
    ret.push_back( MatcherToCreator( &responder::environmentals::NodeCard::matchesUrl, &(createResponder<responder::environmentals::NodeCard>) ) );
    ret.push_back( MatcherToCreator( &responder::environmentals::Optical::matchesUrl, &(createResponder<responder::environmentals::Optical>) ) );
    ret.push_back( MatcherToCreator( &responder::environmentals::ServiceCard::matchesUrl, &(createResponder<responder::environmentals::ServiceCard>) ) );
    ret.push_back( MatcherToCreator( &responder::environmentals::ServiceCardTemp::matchesUrl, &(createResponder<responder::environmentals::ServiceCardTemp>) ) );
    ret.push_back( MatcherToCreator( &responder::hardware::Machine::matchesUrl, &(createResponder<responder::hardware::Machine>) ) );
    ret.push_back( MatcherToCreator( &responder::hardware::Hardware::matchesUrl, &(createResponder<responder::hardware::Hardware>) ) );
    ret.push_back( MatcherToCreator( &responder::summary::Jobs::matchesUrl, &(createResponder<responder::summary::Jobs>) ) );
    ret.push_back( MatcherToCreator( &responder::summary::Machine::matchesUrl, &(createResponder<responder::summary::Machine>) ) );
    ret.push_back( MatcherToCreator( &responder::summary::Ras::matchesUrl, &(createResponder<responder::summary::Ras>) ) );
    ret.push_back( MatcherToCreator( &responder::summary::System::matchesUrl, &(createResponder<responder::summary::System>) ) );
    ret.push_back( MatcherToCreator( &responder::summary::Utilization::matchesUrl, &(createResponder<responder::summary::Utilization>) ) );

    return ret;
}


static CreateResponderFn calcCreateResponderFn( const capena::http::uri::Path& requested_resource )
{
    static MatcherToCreators matcher_to_creator = createMatcherToCreators();


    for ( MatcherToCreators::iterator i(matcher_to_creator.begin()) ; i != matcher_to_creator.end() ; ++i ) {
        MatchesUrlFn &muf(i->first);
        if ( muf( requested_resource ) ) {
            return i->second;
        }
    }
    return CreateResponderFn();
}

}


ResponderFactory::ResponderFactory(
        BgwsServer& bgws_server,
        BlueGene& blue_gene,
        CheckUserAdminExecutor& check_user_admin_executor,
        blue_gene::diagnostics::Runs& diagnostics_runs,
        DynamicConfiguration::ConstPtr dynamic_configuration_ptr,
        PwauthExecutor& pwauth_executor,
        ServerStats& server_stats,
        blue_gene::service_actions::ServiceActions& service_actions,
        Sessions& sessions,
        teal::Teal& teal
    ) :
        _bgws_server(bgws_server),
        _blue_gene(blue_gene),
        _check_user_admin_executor(check_user_admin_executor),
        _diagnostics_runs(diagnostics_runs),
        _dynamic_configuration_ptr(dynamic_configuration_ptr),
        _pwauth_executor(pwauth_executor),
        _server_stats(server_stats),
        _service_actions(service_actions),
        _sessions(sessions),
        _teal(teal)
{
    // Nothing to do.
}


capena::server::ResponderPtr ResponderFactory::createResponder(
        capena::server::RequestPtr request_ptr,
        bgq::utility::portConfig::UserType::Value conn_user_type
    )
{
    LOG_DEBUG_MSG( "Creating responder for " << request_ptr->getUrl().getPath() << " conn_user_type=" << conn_user_type );

    DynamicConfiguration::ConstPtr config_ptr(_dynamic_configuration_ptr);

    capena::http::uri::Path requested_resource;

    _calcRequestedResource(
            request_ptr->getUrl().getPath(),
            config_ptr,
            &requested_resource
        );

    SessionPtr session_ptr;

    UserInfo user_info(_calcRequestUser(
            request_ptr,
            conn_user_type,
            &session_ptr
        ));

    AbstractResponder::CtorArgs responder_ctor_args(
            request_ptr,
            requested_resource,
            user_info,
            session_ptr,
            config_ptr,
            _bgws_server,
            _blue_gene,
            _check_user_admin_executor,
            _diagnostics_runs,
            _pwauth_executor,
            _server_stats,
            _service_actions,
            _sessions,
            _teal
        );

    capena::server::ResponderPtr ret(
            _createResponderForResource(
                        requested_resource,
                        responder_ctor_args
                    )
        );

    if ( ! ret ) {
        BOOST_THROW_EXCEPTION( Error( string() +
                "unexpected request path '" + request_ptr->getUrl().getPath().toString() + "'",
                "processRequest", "unexpectedRequestPath", Error::Data(),
                capena::http::Status::NotFound
            ) );
    }

    // Won't get here if ret is NULL!
    return ret;
}


void ResponderFactory::_calcRequestedResource(
        const capena::http::uri::Path& request_path,
        DynamicConfiguration::ConstPtr config_ptr,
        capena::http::uri::Path* requested_resource_out
    )
{
    // Check if path_base is start of request_path, if not, shouldn't have gotten here because apache or bgws are misconfigured.

    const capena::http::uri::Path &path_base(config_ptr->getPathBase());

    if ( ! request_path.isDescendantOf( path_base ) ) {
        // The request isn't under /bg, return 404 Not Found.

        BOOST_THROW_EXCEPTION( Error( string() +
                "unexpected request path '" + request_path.toString() + "' not under " + path_base.toString(),
                "processRequest", "requestPathNotUnderBase", Error::Data(),
                capena::http::Status::NotFound
            ) );
    }

    // Calculate the requested resource by stripping the path_base from the request path.
    *requested_resource_out = capena::http::uri::Path(
            request_path.begin() + path_base.size(),
            request_path.end()
        );
}


UserInfo ResponderFactory::_calcRequestUser(
        capena::server::RequestPtr request_ptr,
        bgq::utility::portConfig::UserType::Value conn_user_type,
        SessionPtr* session_ptr_out
    )
{
    // If session is in header, check if is valid, if is valid then request user is the session user.
    // If connected with command certificate then user is in X-Bgws-User-Id header.
    // If connected with administrative certificate, then X-Bgws-User-Id is optional, if present, use it, otherwise use root.
    // Otherwise, no user, so use Nobody.

    static const std::string SESSION_ID_HEADER_NAME_NORM("X-BGWS-SESSION-ID");

    {
        capena::server::Headers::const_iterator i(request_ptr->getHeaders().find( SESSION_ID_HEADER_NAME_NORM ));

        if ( i != request_ptr->getHeaders().end() ) {
            // Found session ID header...
            const string &session_id(i->second);

            // Found session ID in request, check to see if it's in sessions.

            SessionPtr session_ptr;

            if ( _sessions.checkSession( session_id, &session_ptr ) ) {
                // The session id valid.
                LOG_DEBUG_MSG( "Session is valid, user=" << session_ptr->getUserId().getUser() << " session ID=" << session_id );

                *session_ptr_out = session_ptr;
                return session_ptr->getUserInfo();
            }

            // Didn't find the session, respond with an error.

            LOG_WARN_MSG( "Client provided invalid session, session ID=" << session_id );

            BOOST_THROW_EXCEPTION( Error(
                    "Invalid session ID provided on request",
                    "authentication",
                    "invalidSession",
                    Error::Data(),
                    capena::http::Status::BadRequest
                ) );
        }
    }


    static const std::string USER_ID_HEADER_NAME_NORM("X-BGWS-USER-ID");

    capena::server::Headers::const_iterator user_id_header_i(request_ptr->getHeaders().find( USER_ID_HEADER_NAME_NORM ));


    if ( conn_user_type == bgq::utility::portConfig::UserType::Normal ) {

        bgq::utility::UserId::ConstPtr user_id_ptr(_getUserIdFromRequestHeader( request_ptr ));

        if ( ! user_id_ptr ) {

            BOOST_THROW_EXCEPTION( Error(
                    "No user ID provided on request when required",
                    "authentication",
                    "noUserId",
                    Error::Data(),
                    capena::http::Status::BadRequest
                ) );

        }

        // Found User ID in request, assign it to the request.

        return UserInfo(
                user_id_ptr,
                UserInfo::UserType::Regular,
                false // not anonymous
            );
    }

    if ( conn_user_type == bgq::utility::portConfig::UserType::Administrator ) {

        bgq::utility::UserId::ConstPtr user_id_ptr(_getUserIdFromRequestHeader( request_ptr ));

        if ( ! user_id_ptr ) {
            LOG_DEBUG_MSG( "Administrative connection, no user ID in header." );

            return UserInfo::getAnonymousAdministrator();
        }

        return UserInfo(
                user_id_ptr,
                UserInfo::UserType::Administrator,
                false // not anonymous
            );
    }


    // No session ID or User ID available, so user is nobody.

    return UserInfo::getNobody();
}


bgq::utility::UserId::ConstPtr ResponderFactory::_getUserIdFromRequestHeader(
        capena::server::RequestPtr request_ptr
    ) const
{
    static const std::string USER_ID_HEADER_NAME_NORM("X-BGWS-USER-ID");

    capena::server::Headers::const_iterator user_id_header_i(request_ptr->getHeaders().find( USER_ID_HEADER_NAME_NORM ));

    if ( user_id_header_i == request_ptr->getHeaders().end() ) {
        return bgq::utility::UserId::ConstPtr();
    }

    const string &user_id_base64(user_id_header_i->second);

    utility::Bytes user_id_bytes(utility::base64::decode( user_id_base64 ));

    std::vector<char> user_id_buf( user_id_bytes.begin(), user_id_bytes.end() );

    bgq::utility::UserId::Ptr user_id_ptr( new bgq::utility::UserId( user_id_buf ) );
    LOG_DEBUG_MSG( "User ID from request header = '" << user_id_ptr->getUser() << "'" );

    return user_id_ptr;
}


AbstractResponder::Ptr ResponderFactory::_createResponderForResource(
        const capena::http::uri::Path& requested_resource,
        AbstractResponder::CtorArgs& responder_ctor_args
    )
{
    statics::CreateResponderFn crf(statics::calcCreateResponderFn( requested_resource ));

    if ( ! crf ) {
        // Didn't match any of the matcher functions...
        return AbstractResponder::Ptr();
    }

    return crf( responder_ctor_args );
}


} // namespace bgws
