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

#include "Sessions.hpp"

#include "utility/base64.hpp"

#include <utility/include/Log.h>

#include <boost/foreach.hpp>

#include <stdexcept>
#include <vector>

#include <stdint.h>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


std::string Sessions::_generateSessionId()
{
    utility::Bytes bytes( 21 );

    utility::getRandomBytes( bytes );

    string session_id_str(_genSessionIdFromBytes( bytes ));

    return session_id_str;
}


std::string Sessions::_genSessionIdFromBytes( const utility::Bytes& bytes )
{
    string ret(utility::base64::encode( bytes ));

    // base64 uses / and + which must be escaped in URLs.
    // It's easier if users don't have to change session IDs,
    // so switch these chars to - and _ which don't need to be escaped.

    for ( unsigned i(0) ; i < ret.size() ; ++i ) {
        if ( ret[i] == '/' )  ret[i] = '-';
        else if ( ret[i] == '+' ) ret[i] = '_';
    }

    return ret;
}


Sessions::Sessions(
        boost::asio::io_service& io_service,
        DynamicConfiguration::ConstPtr dynamic_configuration_ptr
    ) :
        _io_service(io_service),
        _dynamic_configuration_ptr(dynamic_configuration_ptr)
{
    // Nothing to do.
}


void Sessions::createSession(
        const std::string& username,
        UserInfo::UserType user_type,
        SessionPtr* session_ptr_out
    )
{
    LOG_DEBUG_MSG( "Creating session for " << username );

    boost::lock_guard<boost::mutex> lg( _mtx );

    std::string session_id(_generateSessionId());

    SessionPtr session_ptr( new Session(
            session_id,
            username,
            user_type,
            _io_service,
            *this
        ) );

    _id_to_session[session_id] = session_ptr;

    LOG_DEBUG_MSG( "Created session " << session_ptr->getId() << " for " << username );

    LOG_INFO_MSG( "Started session \"" << session_ptr->getUserInfo() << "@" << boost::posix_time::to_iso_string( session_ptr->getStartTime() ) << "\". Now have " << _id_to_session.size() << " active sessions." );

    *session_ptr_out = session_ptr;
}


bool Sessions::checkSession(
        const std::string& session_id,
        SessionPtr* session_ptr_out
    )
{
    boost::lock_guard<boost::mutex> lg( _mtx );

    _IdToSession::const_iterator i(_id_to_session.find( session_id ));

    if ( i == _id_to_session.end() ) {
        return false;
    }

    i->second->resetTimeout();

    *session_ptr_out = i->second;
    return true;
}


void Sessions::end(
        const std::string& session_id,
        const UserInfo& user_info,
        Result::Value* result_out
    )
{
    boost::lock_guard<boost::mutex> lg( _mtx );

    _IdToSession::iterator i(_id_to_session.find( session_id ));

    if ( i == _id_to_session.end() ) {
        *result_out = Result::NotFound;
        return;
    }

    SessionPtr session_ptr(i->second);

    if ( ! session_ptr->isAuthorizedEnd( user_info ) ) {
        LOG_WARN_MSG( "User " << user_info.getUserId().getUser() << " tried to end session created by " << session_ptr->getUsername() );
        *result_out = Result::NotAuthorized;
        return;
    }

    // Session exists and user is authorized to end it.

    _id_to_session.erase( i );

    LOG_INFO_MSG( "User " << user_info.getUserId().getUser() << " ended session \"" << session_ptr->getUserInfo() << "@" << boost::posix_time::to_iso_string( session_ptr->getStartTime() ) << "\". Now have " << _id_to_session.size() << " active sessions." );
    LOG_DEBUG_MSG( "Ended session ID=" << session_ptr->getId() );

    *result_out = Result::Ended;
}


Sessions::SessionInfos Sessions::getSessionInfos(
        const UserInfo& user_info
    )
{
    boost::lock_guard<boost::mutex> lg( _mtx );

    SessionInfos ret;

    BOOST_FOREACH( const _IdToSession::value_type& s_pair, _id_to_session ) {
        if ( s_pair.second->isAuthorizedRead( user_info ) ) {
            ret.push_back( s_pair.first );
        }
    }

    return ret;
}


void Sessions::sessionTimeoutHandler(
        const boost::system::error_code& ec,
        const std::string& session_id
    )
{
    if ( ec ) {
        // probably canceled.
        return;
    }

    boost::lock_guard<boost::mutex> lg( _mtx );

    _IdToSession::iterator i(_id_to_session.find( session_id ));

    if ( i == _id_to_session.end() ) {
        return;
    }

    SessionPtr session_ptr(i->second);

    _id_to_session.erase( i );

    LOG_INFO_MSG( "Session \"" << session_ptr->getUserInfo() << "@" << boost::posix_time::to_iso_string( session_ptr->getStartTime() ) << "\" timed out. Now have " << _id_to_session.size() << " active sessions." );
}


} // namespace bgws
