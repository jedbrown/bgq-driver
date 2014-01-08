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

#ifndef BGWS_RESPONDER_DIAGNOSTICS_BLOCK_HPP_
#define BGWS_RESPONDER_DIAGNOSTICS_BLOCK_HPP_


#include "../../AbstractResponder.hpp"

#include "capena-http/http/http.hpp"


namespace bgws {
namespace responder {
namespace diagnostics {


class Block : public AbstractResponder
{
public:

    static bool matchesUrl( const capena::http::uri::Path& requested_resource );

    static capena::http::uri::Path calcUri(
            const capena::http::uri::Path& path_base,
            const std::string& block_id
        );


    Block(
            CtorArgs& args
        ) :
            AbstractResponder( args )
    { /* Nothing to do */ }


    capena::http::Methods _getAllowedMethods() const  { return { capena::http::Method::GET }; }


    // override
    void _doGet();


private:

};

} } } // namespace bgws::responder::diagnostics


#endif
