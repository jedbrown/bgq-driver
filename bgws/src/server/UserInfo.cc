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

#include "UserInfo.hpp"


namespace bgws {


boost::shared_ptr<const UserInfo> UserInfo::_ANONYMOUS_ADMINISTRATOR_PTR;
boost::shared_ptr<const UserInfo> UserInfo::_NOBODY_PTR;


void UserInfo::initialize()
{
    _ANONYMOUS_ADMINISTRATOR_PTR.reset( new UserInfo(
            bgq::utility::UserId::Ptr( new bgq::utility::UserId( 0 ) ),
            UserType::Administrator,
            true // anonymous
        ) );

    _NOBODY_PTR.reset( new UserInfo(
            bgq::utility::UserId::Ptr( new bgq::utility::UserId( "nobody" ) ),
            UserType::Nobody,
            true // anonymous
        ) );
}


UserInfo::UserInfo(
        bgq::utility::UserId::ConstPtr user_id_ptr,
        UserType user_type,
        bool is_anonymous
    ) :
        _is_anonymous(is_anonymous),
        _user_id_ptr(user_id_ptr),
        _user_type(user_type)
{
    // Nothing to do.
}


std::ostream& operator<<( std::ostream& os, const UserInfo& user_info )
{
    if ( user_info.isAdministrator() ) {
        if ( user_info.isAnonymous() ) {
            os << "anonymous administrator";
        } else { // not anonymous administrator
            os << "administrator '" << user_info.getUserId().getUser() << "'";
        }
    } else { // not administrator.
        if ( user_info.isAnonymous() ) {
            os << "anonymous user";
        } else { // not anonymous user.
            os << "user '" << user_info.getUserId().getUser() << "'";
        }
    }

    return os;
}


} // namespace bgws
