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

#ifndef BGWS_RESPONDER_BGWS_SERVER_HPP_
#define BGWS_RESPONDER_BGWS_SERVER_HPP_


#include "../AbstractResponder.hpp"

#include "../ServerStats.hpp"
#include "../types.hpp"

#include "capena-http/http/uri/Path.hpp"


namespace bgws {
namespace responder {


class BgwsServer : public AbstractResponder
{
public:

    static const capena::http::uri::Path &RESOURCE_PATH;


    static bool matchesUrl( const capena::http::uri::Path& requested_resource )
    { return (requested_resource == RESOURCE_PATH); }


    BgwsServer(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _bgws_server(args.bgws_server),
            _server_stats(args.server_stats)
    { /* Nothing to do */ }

    // override
    capena::http::Methods _getAllowedMethods() const  {
        return { capena::http::Method::GET, capena::http::Method::POST };
    }

    // override
    void _doGet();

    // override
    void _doPost( json::ConstValuePtr val_ptr );


private:

    bgws::BgwsServer &_bgws_server;
    ServerStats &_server_stats;


    void _gotStatistics(
            capena::server::ResponderPtr /*shared_ptr*/,
            const ServerStats::Snapshot& snapshot
        );

};


}} // namespace bgws::responder


#endif
