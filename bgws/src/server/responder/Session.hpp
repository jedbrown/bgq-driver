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

#ifndef BGWS_RESPONDER_SESSION_HPP_
#define BGWS_RESPONDER_SESSION_HPP_


#include "../AbstractResponder.hpp"

#include "../Sessions.hpp"

#include "capena-http/http/uri/Path.hpp"

#include "chiron-json/json.hpp"

#include <string>


namespace bgws {
namespace responder {


class Session : public AbstractResponder
{
public:

    // returns true if is /bg/bgws/sessions/<session-id>
    static bool matchesUrl( const capena::http::uri::Path& request_path );

    static capena::http::uri::Path calcPath(
            const std::string& session_id,
            const capena::http::uri::Path& path_base
        );


    static json::ValuePtr calcUserJson(
            const UserInfo& user_info,
            const std::string& session_id,
            const hlcs::security::Enforcer& enforcer
        );


    Session(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _sessions(args.sessions),
            _session_ptr(args.session_ptr)
    { /* Nothing to do */ }

    capena::http::Methods _getAllowedMethods() const;

    // override
    void _doDelete();

    // override
    void _doGet();


private:


    bgws::Sessions &_sessions;

    SessionPtr _session_ptr;


    void _deleteSession(
            std::string session_id
        );
};


} } // namespace bgws::responder


#endif
