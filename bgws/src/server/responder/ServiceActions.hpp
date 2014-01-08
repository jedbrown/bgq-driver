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

#ifndef BGWS_RESPONDER_SERVICE_ACTIONS_HPP_
#define BGWS_RESPONDER_SERVICE_ACTIONS_HPP_


#include "../AbstractResponder.hpp"

#include "../RequestRange.hpp"

#include "../blue_gene/service_actions/fwd.hpp"

#include "capena-http/http/uri/Path.hpp"

#include "chiron-json/fwd.hpp"

#include <db/include/api/cxxdb/fwd.h>

#include <stdint.h>


namespace bgws {
namespace responder {


class ServiceActions : public AbstractResponder
{
public:

    static const capena::http::uri::Path RESOURCE_PATH;
    static const capena::http::uri::Path RESOURCE_PATH_SLASH_EMPTY;


    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        )
    { return ((requested_resource == RESOURCE_PATH) || (requested_resource == RESOURCE_PATH_SLASH_EMPTY)); }


    static void setCommonFields(
            json::Object& obj,
            const cxxdb::Columns& cols
        );


    ServiceActions(
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


    void _query(
            cxxdb::Connection& conn,
            const RequestRange& req_range,
            uint64_t* row_count_out,
            cxxdb::ResultSetPtr* rs_ptr_out
        );

    void _started(
            capena::server::ResponderPtr /*responder_ptr*/,
            const std::string& location,
            std::exception_ptr exc_ptr,
            const std::string& service_action_id
        );


    void _gotAttentionMessagesSnapshot(
            capena::server::ResponderPtr /*responder_ptr*/,
            const blue_gene::service_actions::AttentionMessagesMap& attention_messages
        );

};


} } // namespace bgws::responder

#endif
