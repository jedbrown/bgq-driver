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

#ifndef BGWS_ABSTRACT_RESPONDER_HPP_
#define BGWS_ABSTRACT_RESPONDER_HPP_


#include "capena-http/server/AbstractResponder.hpp"

#include "DynamicConfiguration.hpp"
#include "types.hpp"
#include "UserInfo.hpp"

#include "capena-http/http/http.hpp"
#include "capena-http/http/uri/Path.hpp"

#include "chiron-json/fwd.hpp"

#include <hlcs/include/security/Enforcer.h>

#include <boost/optional.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <string>


namespace bgws {


class CheckUserAdminExecutor;
class BgwsServer;
class BlueGene;
class PwauthExecutor;
class ServerStats;
class Sessions;

namespace blue_gene {
    namespace diagnostics { class Runs; }
    namespace service_actions { class ServiceActions; }
}

namespace teal { class Teal; }


class AbstractResponder : public capena::server::AbstractResponder
{
public:


    struct CtorArgs {
        capena::server::RequestPtr request_ptr;
        const capena::http::uri::Path& requested_resource_path;
        UserInfo user_info;
        SessionPtr session_ptr;
        DynamicConfiguration::ConstPtr dynamic_configuration_ptr;

        BgwsServer &bgws_server;
        BlueGene &blue_gene;
        CheckUserAdminExecutor &check_user_admin_executor;
        blue_gene::diagnostics::Runs &diagnostics_runs;
        PwauthExecutor &pwauth_executor;
        ServerStats &server_stats;
        blue_gene::service_actions::ServiceActions &service_actions;
        Sessions &sessions;
        teal::Teal &teal;

        CtorArgs(
                capena::server::RequestPtr request_ptr,
                const capena::http::uri::Path& requested_resource_path,
                const UserInfo user_info,
                SessionPtr session_ptr,
                DynamicConfiguration::ConstPtr dynamic_configuration_ptr,
                BgwsServer& bgws_server,
                BlueGene& blue_gene,
                CheckUserAdminExecutor& check_user_admin_executor,
                blue_gene::diagnostics::Runs& diagnostics_runs,
                PwauthExecutor& pwauth_executor,
                ServerStats& server_stats,
                blue_gene::service_actions::ServiceActions& service_actions,
                Sessions& sessions,
                teal::Teal& teal
            ) :
                request_ptr(request_ptr),
                requested_resource_path(requested_resource_path),
                user_info(user_info),
                session_ptr(session_ptr),
                dynamic_configuration_ptr(dynamic_configuration_ptr),
                bgws_server(bgws_server),
                blue_gene(blue_gene),
                check_user_admin_executor(check_user_admin_executor),
                diagnostics_runs(diagnostics_runs),
                pwauth_executor(pwauth_executor),
                server_stats(server_stats),
                service_actions(service_actions),
                sessions(sessions),
                teal(teal)
        { /* Nothing to do */ }
    };


    typedef boost::shared_ptr<AbstractResponder> Ptr;
    typedef boost::optional<std::string> OptionalString;


    AbstractResponder(
            CtorArgs& args
        );


    const DynamicConfiguration& getDynamicConfiguration() const  { return *_dynamic_configuration_ptr; }

    const capena::http::uri::Path& getRequestedResourcePath() const  { return _requested_resource_path; }

    const UserInfo& getRequestUserInfo() const  { return _user_info; }
    const bgq::utility::UserId& getRequestUserId() const  { return _user_info.getUserId(); }
    const std::string& getRequestUserName() const  { return _user_info.getUserId().getUser(); }

    capena::server::Response& getResponse()  { return _getResponse(); }


    boost::asio::strand& strand()  { return _getStrand(); }


    /*! \brief Check that the content type in the request is valid.
     *
     *  \throws error if the content type is present and is not application/json.
     */
    void checkContentType();

    void handleError( std::exception& e );


    void handle();


    virtual capena::http::Methods getAllowedMethods() const  { return capena::http::Methods(); }

    virtual void doDelete()  { _throwMethodNotAllowed(); }
    virtual void doGet()  { _throwMethodNotAllowed(); }
    virtual void doHead()  { _throwMethodNotAllowed(); }

    virtual void doPut( json::ConstValuePtr /*val_ptr*/ )  { _throwMethodNotAllowed(); }
    virtual void doPost( json::ConstValuePtr /*val_ptr*/ )  { _throwMethodNotAllowed(); }


    ~AbstractResponder();


protected:

    // Override
    void _processRequest();


    // returns true iff the user is authenticated, i.e., not Nobody.
    bool _isUserAuthenticated() const  { return (getRequestUserInfo().getUserType() != UserInfo::UserType::Nobody); }

    // returns true iff the user is an administrator.
    bool _isUserAdministrator() const  { return (getRequestUserInfo().getUserType() == UserInfo::UserType::Administrator); }

    bool _userHasHardwareRead() const;

    const hlcs::security::Enforcer& _enforcer() const  { return _security_enforcer; }


private:

    capena::http::uri::Path _requested_resource_path;
    UserInfo _user_info;
    DynamicConfiguration::ConstPtr _dynamic_configuration_ptr;
    ServerStats &_server_stats;

    const hlcs::security::Enforcer &_security_enforcer;

    boost::posix_time::ptime _start_time;


    void _throwMethodNotAllowed();
};


} // namespace bgws


#endif
