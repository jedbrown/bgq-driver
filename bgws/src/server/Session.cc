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

#include "Session.hpp"

#include "Sessions.hpp"

#include <boost/bind.hpp>
#include <boost/date_time.hpp>


namespace bgws {


Session::Session(
        const std::string& session_id,
        const std::string& username,
        UserInfo::UserType user_type,
        boost::asio::io_service& io_service,
        Sessions& sessions
    ) :
        _io_service(io_service),
        _sessions(sessions),
        _session_id(session_id),
        _start_time(boost::posix_time::second_clock::local_time())
{
    _user_info_ptr.reset( new UserInfo(
            bgq::utility::UserId::Ptr( new bgq::utility::UserId( username ) ),
            user_type,
            false // not anonymous
        ) );


    _startTimer();
}


void Session::resetTimeout()
{
    _timer_ptr->cancel();

    _startTimer();
}


bool Session::isAuthorizedRead( const UserInfo& user_info ) const
{
    return _userIsAdministratorOrCreator( user_info );
}


bool Session::isAuthorizedEnd( const UserInfo& user_info ) const
{
    return _userIsAdministratorOrCreator( user_info );
}


void Session::_startTimer()
{
    _timer_ptr.reset( new boost::asio::deadline_timer( _io_service, boost::posix_time::seconds( _sessions.getTimeoutSecs() ) ) );

    _timer_ptr->async_wait( boost::bind( &Sessions::sessionTimeoutHandler, &_sessions, boost::asio::placeholders::error, _session_id ) );
}


bool Session::_userIsAdministratorOrCreator( const UserInfo& user_info ) const
{
    if ( user_info.isAdministrator() )  return true;
    if ( user_info.getUserId().getUser() == _user_info_ptr->getUserId().getUser() )  return true;

    return false;
}


} // namespace bgws
