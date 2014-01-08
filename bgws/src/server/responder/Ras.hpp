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

#ifndef BGWS_RESPONDER_RAS_HPP_
#define BGWS_RESPONDER_RAS_HPP_


#include "../AbstractResponder.hpp"

#include "capena-http/http/uri/Path.hpp"


namespace bgws {
namespace responder {


class Ras : public AbstractResponder
{
public:

    static const capena::http::uri::Path &RESOURCE_PATH;
    static const capena::http::uri::Path RESOURCE_PATH_EMPTY_CHILD;


    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        );


    Ras(
            CtorArgs& args
        ) :
            AbstractResponder( args )
    { /* Nothing to do */ }


    capena::http::Methods getAllowedMethods() const;

    void doGet();


private:

    void _checkAuthority() const;

};


} } // namespace bgws::responder

#endif
