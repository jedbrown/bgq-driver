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

#ifndef BGWS_RESPONDER_HARDWARE_MACHINE_HPP_
#define BGWS_RESPONDER_HARDWARE_MACHINE_HPP_


#include "../../AbstractResponder.hpp"

#include "capena-http/http/uri/Path.hpp"


namespace bgws {
namespace responder {
namespace hardware {


class Machine : public AbstractResponder
{
public:

    static const capena::http::uri::Path RESOURCE_PATH;


    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        )
    {
        static const capena::http::uri::Path RESOURCE_PATH_EMPTY_CHILD(RESOURCE_PATH / "");
        return (requested_resource == RESOURCE_PATH || requested_resource == RESOURCE_PATH_EMPTY_CHILD);
    }


    Machine(
            CtorArgs& args
        ) :
            AbstractResponder( args )
    { /* Nothing to do */ }


    capena::http::Methods _getAllowedMethods() const  { return { capena::http::Method::GET }; }

    void _doGet();


private:

    void _formatMachine();

};


}}} // namespace bgws::responder::hardware


#endif
