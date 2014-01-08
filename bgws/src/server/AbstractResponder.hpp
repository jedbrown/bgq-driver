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

#include "blue_gene/diagnostics/types.hpp"

#include "blue_gene/service_actions/fwd.hpp"

#include "teal/fwd.hpp"

#include "capena-http/http/http.hpp"
#include "capena-http/http/uri/Path.hpp"

#include "chiron-json/fwd.hpp"

#include <hlcs/include/security/Enforcer.h>

#include <boost/optional.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <exception>
#include <string>


namespace bgws {


/*! \brief Base class for BGWS responders.
 *
 *  Contains some common behavior for responders
 *  and handy functions for the responders.
 *
 *  Common behavior:
 *  - Notifying server stats of responder stats (see constructor & destructor)
 *  - Logs end of request & time to process (destructor)
 *  - _processRequest handling
 *     - Validates that the HTTP method for the request is allowed by the resource
 *     - Validates content-type in request is application/json -- BGWS only allows JSON data in request.
 *     - Parses JSON data in request -- BGWS only allows JSON data in request.
 *     - Calls __doGet(), __doPost(), etc.
 *
 *  Derived classes override these methods:
 *  - _getAllowedMethods() -- with the HTTP methods the resource allows.
 *  - _doDelete(), _doGet(), _doPut(), _doPost() -- to generate the response, depending on which methods are allowed
 *
 */
class AbstractResponder : public capena::server::AbstractResponder
{
public:

    /*! \brief Groups the object references that get passed in on the constructor. */
    struct CtorArgs {
        capena::server::RequestPtr request_ptr;
        const capena::http::uri::Path &requested_resource_path;
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
        BlockingOperationsThreadPool &blocking_operations_thread_pool;


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
                teal::Teal& teal,
                BlockingOperationsThreadPool &blocking_operations_thread_pool
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
                teal(teal),
                blocking_operations_thread_pool(blocking_operations_thread_pool)
        { /* Nothing to do */ }
    };


    typedef boost::shared_ptr<AbstractResponder> Ptr;


    /*! \brief Constructor. */
    AbstractResponder(
            CtorArgs& args
        );

    ~AbstractResponder();


protected:

    /*! \brief Override to provide the list of methods this resource allows. */
    virtual capena::http::Methods _getAllowedMethods() const  { return capena::http::Methods(); }

    /*! \brief Override to implement the DELETE method for this resource. */
    virtual void _doDelete()  { _throwMethodNotAllowed(); }

    /*! \brief Override to implement the GET method for this resource. */
    virtual void _doGet()  { _throwMethodNotAllowed(); }

    /*! \brief Override to implement the HEAD method for this resource. */
    virtual void _doHead()  { _throwMethodNotAllowed(); }

    /*! \brief Override to implement the PUT method for this resource. */
    virtual void _doPut( json::ConstValuePtr /*val_ptr*/ )  { _throwMethodNotAllowed(); }

    /*! \brief Override to implement the POST method for this resource. */
    virtual void _doPost( json::ConstValuePtr /*val_ptr*/ )  { _throwMethodNotAllowed(); }


    const capena::http::uri::Path& _getRequestedResourcePath() const  { return _requested_resource_path; }


    const UserInfo& _getRequestUserInfo() const  { return _user_info; }

    const std::string& getRequestUserName() const  { return _user_info.getUserId().getUser(); }

    const bgq::utility::UserId& _getRequestUserId() const  { return _user_info.getUserId(); }

    /*! \brief returns true iff the user is authenticated, i.e., not Nobody. */
    bool _isUserAuthenticated() const  { return (_getRequestUserInfo().getUserType() != UserInfo::UserType::Nobody); }

    /*! \brief returns true iff the user is an administrator. */
    bool _isUserAdministrator() const  { return (_getRequestUserInfo().getUserType() == UserInfo::UserType::Administrator); }

    /*! \brief returns true iff the user has hardware READ authority. */
    bool _userHasHardwareRead() const;


    const DynamicConfiguration& _getDynamicConfiguration() const  { return *_dynamic_configuration_ptr; }
    const hlcs::security::Enforcer& _getEnforcer() const  { return _security_enforcer; }


    /*! \brief Call this to set the response to an error response. */
    void _handleError( std::exception& e );

    /*! \brief Post to my strand to write an empty JSON array as result. */
    void _postEmptyResult();

    /*! \brief Called in catch block, posts call to _handleErrorCb with current_exception. */
    void _inCatchPostCurrentExceptionToHandlerFn();

    /*! \brief Overrides and implements, derived classes should not override. */
    void _processRequest();


private:

    capena::http::uri::Path _requested_resource_path;
    UserInfo _user_info;
    DynamicConfiguration::ConstPtr _dynamic_configuration_ptr;
    ServerStats &_server_stats;

    const hlcs::security::Enforcer &_security_enforcer;

    boost::posix_time::ptime _start_time;


    /*! \brief Check that the content type in the request is valid.
     *
     *  \throws error if the content type is present and is not application/json.
     */
    void _checkContentType();


    void _handle();

    void _throwMethodNotAllowed();


    void _emptyResult(
            capena::server::ResponderPtr
        );


    void _handleErrorCb(
            capena::server::ResponderPtr,
            std::exception_ptr exc_ptr
        );

};


} // namespace bgws


#endif
