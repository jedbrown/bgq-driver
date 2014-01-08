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

#ifndef BGWS_SESSION_HPP_
#define BGWS_SESSION_HPP_


#include "types.hpp"
#include "UserInfo.hpp"

#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <string>


namespace bgws {


class Session : boost::noncopyable
{
public:

    Session(
            const std::string& session_id,
            const std::string& username,
            UserInfo::UserType user_type,
            boost::asio::io_service& io_service,
            Sessions& sessions
        );


    void resetTimeout();


    const std::string& getId() const  { return _session_id; }

    const UserInfo& getUserInfo() const  { return *_user_info_ptr; }

    const std::string& getUsername() const  { return _user_info_ptr->getUserId().getUser(); }
    const bgq::utility::UserId& getUserId() const  { return _user_info_ptr->getUserId(); }

    /*! \brief Check is user authorized to read.
     *
     * User can read the session if user is administrator or they're the user that created the session.
     *
     * \return true iff the user can read the session.
     */
    bool isAuthorizedRead( const UserInfo& user_info ) const;

    /*! \brief Check is user authorized to end the session.
     *
     * User can end the session if user is administrator or they're the user that created the session.
     *
     * \return true iff the user can end the session.
     */
    bool isAuthorizedEnd( const UserInfo& user_info ) const;

    const boost::posix_time::ptime& getStartTime() const  { return _start_time; }


private:

    typedef boost::shared_ptr<boost::asio::deadline_timer> _TimerPtr;


    void _startTimer();

    bool _userIsAdministratorOrCreator( const UserInfo& user_info ) const;


    boost::asio::io_service &_io_service;
    Sessions &_sessions;

    std::string _session_id;

    boost::shared_ptr<UserInfo> _user_info_ptr;

    boost::posix_time::ptime _start_time;

    _TimerPtr _timer_ptr;
};


} // namespace bgws

#endif
