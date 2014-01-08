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

#ifndef BGWS_RESPONDER_SERVICE_ACTION_HPP_
#define BGWS_RESPONDER_SERVICE_ACTION_HPP_


#include "../AbstractResponder.hpp"

#include "ServiceActions.hpp"

#include "../Error.hpp"

#include "../blue_gene/service_actions/fwd.hpp"

#include "capena-http/http/uri/Path.hpp"

#include <exception>

#include <stdint.h>


namespace bgws {
namespace responder {


class ServiceAction : public AbstractResponder
{
public:

    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        )
    { return ((requested_resource.calcParent() == ServiceActions::RESOURCE_PATH) && (requested_resource.back() != std::string())); }

    static capena::http::uri::Path calcPath(
            const capena::http::uri::Path& path_base,
            uint64_t id
        );


    ServiceAction(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _service_actions(args.service_actions)
    { /* Nothing to do */ }


    // override
    void _doGet();

    // override
    void _doPost( json::ConstValuePtr val_ptr );


private:

    blue_gene::service_actions::ServiceActions &_service_actions;


    void _gotAttentionMessages(
            capena::server::ResponderPtr /*responder_ptr*/,
            uint64_t id,
            const std::string& attention_messages
        );


    void _end(
            uint64_t id,
            const std::string& location,
            const std::string& action,
            const std::string& status,
            Error::Data& error_data_in_out
        );

    void _ended(
            capena::server::ResponderPtr /*responder_ptr*/,
            const std::string& error_message,
            uint64_t id,
            const std::string& location,
            Error::Data error_data
        );

    void _close(
            uint64_t id,
            const std::string& location,
            const std::string& status,
            Error::Data& error_data_in_out
        );

    void _closed(
            capena::server::ResponderPtr /*responder_ptr*/,
            std::exception_ptr exc_ptr,
            uint64_t id,
            const std::string& location,
            Error::Data error_data
        );
};

} } // namespace bgws::responder

#endif
