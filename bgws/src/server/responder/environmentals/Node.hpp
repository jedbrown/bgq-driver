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

#ifndef BGWS_RESPONDER_ENVIRONMENTALS_NODE_HPP_
#define BGWS_RESPONDER_ENVIRONMENTALS_NODE_HPP_


#include "../../AbstractResponder.hpp"

#include "capena-http/http/uri/Path.hpp"


namespace bgws {
namespace responder {
namespace environmentals {


class Node : public AbstractResponder
{
public:

    static const capena::http::uri::Path RESOURCE_PATH;

    static bool matchesUrl( const capena::http::uri::Path& requested_resource )
    { return (requested_resource == RESOURCE_PATH); }


    Node(
            CtorArgs& args
        ) :
            AbstractResponder( args )
    { /* Nothing to do */ }

    // override
    capena::http::Methods _getAllowedMethods() const  {
        return { capena::http::Method::GET };
    }

    // override
    void _doGet();

};


} } } // namespace bgws::responder::environmentals

#endif
