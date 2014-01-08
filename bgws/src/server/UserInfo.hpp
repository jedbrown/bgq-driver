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

#ifndef BGWS_USER_INFO_HPP_
#define BGWS_USER_INFO_HPP_


#include <utility/include/UserId.h>

#include <iosfwd>


namespace bgws {


class UserInfo
{
public:

    enum class UserType {
        Administrator,
        Regular,
        Nobody
    };


    static void initialize();

    static const UserInfo& getAnonymousAdministrator()  { return *_ANONYMOUS_ADMINISTRATOR_PTR; }
    static const UserInfo& getNobody()  { return *_NOBODY_PTR; }


    UserInfo(
            bgq::utility::UserId::ConstPtr user_id_ptr,
            UserType user_type,
            bool is_anonymous
        );

    const bgq::utility::UserId& getUserId() const  { return *_user_id_ptr; }
    bgq::utility::UserId::ConstPtr getUserIdPtr() const  { return _user_id_ptr; }

    UserType getUserType() const  { return _user_type; }
    bool isAdministrator() const  { return (_user_type == UserType::Administrator); }

    bool isAnonymous() const  { return _is_anonymous; }
        // No username is available.
        //  - true if no user name given (if provided admin cert can still be administrator)


private:

    static boost::shared_ptr<const UserInfo> _ANONYMOUS_ADMINISTRATOR_PTR;
    static boost::shared_ptr<const UserInfo> _NOBODY_PTR;


    bool _is_anonymous;
    bgq::utility::UserId::ConstPtr _user_id_ptr;
    UserType _user_type;


};


std::ostream& operator<<( std::ostream& os, const UserInfo& user_info );


} // namespace bgws


#endif
