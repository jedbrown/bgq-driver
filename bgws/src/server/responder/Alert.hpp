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

#ifndef BGWS_RESPONDER_ALERT_HPP_
#define BGWS_RESPONDER_ALERT_HPP_


#include "../AbstractResponder.hpp"

#include "Alerts.hpp"

#include "../Error.hpp"

#include "../teal/fwd.hpp"

#include <db/include/api/cxxdb/fwd.h>

#include <stdint.h>


namespace bgws {
namespace responder {


class Alert : public AbstractResponder
{
public:

    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        )
    {
        return (requested_resource.calcParent() == Alerts::RESOURCE_PATH && requested_resource.back() != std::string());
    }

    static capena::http::uri::Path calcPath(
            const capena::http::uri::Path& path_base,
            teal::Id id
        );


    Alert(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _teal(args.teal)
    { /* Nothing to do */ }

    capena::http::Methods _getAllowedMethods() const
    {
        static const capena::http::Methods METHODS = { capena::http::Method::GET, capena::http::Method::POST, capena::http::Method::DELETE };
        return METHODS;
    }

    // override
    void _doGet();

    // override
    void _doPost( json::ConstValuePtr val_ptr );

    // override
    void _doDelete();


private:

    static const std::string _GET_OPERATION_NAME;
    static const std::string _OPERATE_OPERATION_NAME;
    static const std::string _CLOSE_OPERATION_NAME;
    static const std::string _REMOVE_OPERATION_NAME;


    teal::Teal &_teal;


    void _calcRecordId(
            const std::string& operation_name,
            Error::Data& error_data_in_out,
            std::string& record_id_str_out,
            teal::Id& record_id_out
        );

    void _checkGetAuthority();

    void _checkPostAuthority();

    void _checkDeleteAuthority();

    void _closed(
            capena::server::ResponderPtr /*shared_ptr*/,
            teal::Id record_id,
            std::exception_ptr& exc_ptr
        );

    void _removed(
            capena::server::ResponderPtr /*shared_ptr*/,
            teal::Id record_id,
            std::exception_ptr exc_ptr
        );
};


}} // namespace bgws::responder


#endif
