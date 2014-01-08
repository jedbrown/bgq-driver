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

#ifndef BGWS_RESPONDER_SESSIONS_HPP_
#define BGWS_RESPONDER_SESSIONS_HPP_


#include "../AbstractResponder.hpp"

#include "../types.hpp"

#include "capena-http/http/uri/Path.hpp"

#include <string>


namespace bgws {
namespace responder {


class Sessions : public AbstractResponder
{
public:

    static const capena::http::uri::Path RESOURCE_PATH;
    static const capena::http::uri::Path RESOURCE_PATH_EMPTY_CHILD;


    static bool matchesUrl( const capena::http::uri::Path& request_path );


    Sessions(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _check_user_admin_executor(args.check_user_admin_executor),
            _pwauth_executor(args.pwauth_executor),
            _sessions(args.sessions)
    { /* Nothing to do */ }


    capena::http::Methods _getAllowedMethods() const;

    // override
    void _doGet();

    // override
    void _doPost( json::ConstValuePtr val_ptr );


private:


    CheckUserAdminExecutor &_check_user_admin_executor;
    PwauthExecutor &_pwauth_executor;
    bgws::Sessions &_sessions;

    std::string _username;


    void _handlePwauthComplete(
            capena::server::ResponderPtr shared_ptr,
            bool authenticated
        );

    void _handleCheckUserAdminComplete(
            capena::server::ResponderPtr shared_ptr,
            UserInfo::UserType user_type
        );

    void _parseSessionPostData(
            json::ConstValuePtr val_ptr,
            std::string& username_out,
            std::string& password_out
        );
};


} // namespace bgws::responder
} // namespace bgws

#endif
