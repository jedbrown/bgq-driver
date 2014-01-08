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

#ifndef BGWS_RESPONDER_RAS_DETAILS_HPP_
#define BGWS_RESPONDER_RAS_DETAILS_HPP_


#include "../AbstractResponder.hpp"

#include "../Error.hpp"

#include "capena-http/http/http.hpp"

#include <stdint.h>


namespace bgws {
namespace responder {


class RasDetails : public AbstractResponder
{
public:

    static bool matchesUrl( const capena::http::uri::Path& requested_resource );

    static capena::http::uri::Path calcPath( const capena::http::uri::Path& path_base, uint64_t recid );


    RasDetails(
            CtorArgs& args
        ) :
            AbstractResponder( args )
    { /* Nothing to do */ }


    capena::http::Methods _getAllowedMethods() const
    {
        return { capena::http::Method::GET };
    }


    // override
    void _doGet();


private:

    void _checkAuthority(
            const Error::Data& error_data
        ) const;

};


} } // namespace bgws::responder


#endif
