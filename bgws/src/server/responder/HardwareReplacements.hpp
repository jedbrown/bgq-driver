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

#ifndef BGWS_RESPONDER_HARDWARE_REPLACEMENTS_HPP_
#define BGWS_RESPONDER_HARDWARE_REPLACEMENTS_HPP_


#include "../AbstractResponder.hpp"


namespace bgws {
namespace responder {


class HardwareReplacements : public AbstractResponder
{
public:

    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        )
    {
        static const capena::http::uri::Path RESOURCE_PATH(capena::http::uri::Path() / "hardwareReplacements");
        static const capena::http::uri::Path RESOURCE_PATH_EMPTY_CHILD(RESOURCE_PATH / "");
        return (requested_resource == RESOURCE_PATH || requested_resource == RESOURCE_PATH_EMPTY_CHILD);
    }


    HardwareReplacements(
            CtorArgs& args
        ) :
            AbstractResponder( args )
    { /* Nothing to do */ }


    capena::http::Methods _getAllowedMethods() const
    {
        static const capena::http::Methods METHODS = { capena::http::Method::GET };
        return METHODS;
    }


    void _doGet();


private:

    void _checkAuthority();

};


}} // namespace bgws::responder


#endif
