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

#ifndef BGWS_RESPONDER_DIAGNOSTICS_BLOCKS_HPP_
#define BGWS_RESPONDER_DIAGNOSTICS_BLOCKS_HPP_


#include "../../AbstractResponder.hpp"

#include "capena-http/http/http.hpp"


namespace bgws {
namespace responder {
namespace diagnostics {


class Blocks : public AbstractResponder
{
public:

    static const capena::http::uri::Path RESOURCE_PATH;
    static const capena::http::uri::Path RESOURCE_PATH_EMPTY_CHILD;


    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        )
    { return (requested_resource == RESOURCE_PATH || requested_resource == RESOURCE_PATH_EMPTY_CHILD); }


    Blocks(
            CtorArgs& args
        ) :
            AbstractResponder( args )
    { /* Nothing to do */ }


    capena::http::Methods _getAllowedMethods() const  { return { capena::http::Method::GET }; }


    // override
    void _doGet();

};

} } } // namespace bgws::responder::diagnostics

#endif
