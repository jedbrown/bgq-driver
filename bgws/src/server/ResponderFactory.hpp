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

#ifndef BGWS_RESPONDER_FACTORY_HPP_
#define BGWS_RESPONDER_FACTORY_HPP_


#include "AbstractResponder.hpp"
#include "DynamicConfiguration.hpp"
#include "types.hpp"
#include "UserInfo.hpp"

#include "capena-http/http/uri/Path.hpp"

#include "capena-http/server/fwd.hpp"

#include <utility/include/UserId.h>

#include <boost/shared_ptr.hpp>


namespace bgws {


/*! \brief Creates Responder for the request. */
class ResponderFactory
{
public:

    ResponderFactory(
            BgwsServer& bgws_server,
            BlueGene& blue_gene,
            CheckUserAdminExecutor& check_user_admin_executor,
            blue_gene::diagnostics::Runs& diagnostics_runs,
            DynamicConfiguration::ConstPtr dynamic_configuration_ptr,
            PwauthExecutor& pwauth_executor,
            ServerStats& server_stats,
            blue_gene::service_actions::ServiceActions& service_actions,
            Sessions& sessions,
            teal::Teal& teal,
            BlockingOperationsThreadPool& blocking_operations_thread_pool
        );


    /*! \brief Called during refresh config. */
    void setNewDynamicConfiguration( DynamicConfiguration::ConstPtr dynamic_configuration_ptr )  { _dynamic_configuration_ptr = dynamic_configuration_ptr; }


    /*! \brief This function is called by the capena-http library when a request comes in, it checks some things in the request and returns the responder for the request, or exception. */
    capena::server::ResponderPtr createResponder(
            capena::server::RequestPtr request_ptr,
            bgq::utility::portConfig::UserType::Value conn_user_type
        );


private:

    BgwsServer &_bgws_server;
    BlueGene &_blue_gene;
    CheckUserAdminExecutor &_check_user_admin_executor;
    blue_gene::diagnostics::Runs &_diagnostics_runs;
    DynamicConfiguration::ConstPtr _dynamic_configuration_ptr;
    PwauthExecutor &_pwauth_executor;
    ServerStats &_server_stats;
    blue_gene::service_actions::ServiceActions &_service_actions;
    Sessions &_sessions;
    teal::Teal &_teal;
    BlockingOperationsThreadPool &_blocking_operations_thread_pool;


    void _calcRequestedResource(
            const capena::http::uri::Path& request_path,
            DynamicConfiguration::ConstPtr config_ptr,
            capena::http::uri::Path* requested_resource_out
        );

    UserInfo _calcRequestUser(
            capena::server::RequestPtr request_ptr,
            bgq::utility::portConfig::UserType::Value conn_user_type,
            SessionPtr* session_ptr_out
        );

    bgq::utility::UserId::ConstPtr _getUserIdFromRequestHeader(
            capena::server::RequestPtr request_ptr
        ) const;

    AbstractResponder::Ptr _createResponderForResource(
            const capena::http::uri::Path& requested_resource,
            AbstractResponder::CtorArgs& responder_ctor_args
        );

};


} // namespace bgws


#endif
