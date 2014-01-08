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

#ifndef BGWS_RESPONDER_JOB_HPP_
#define BGWS_RESPONDER_JOB_HPP_


#include "../AbstractResponder.hpp"

#include "Jobs.hpp"

#include <stdint.h>


namespace bgws {
namespace responder {


class Job : public AbstractResponder
{
public:


    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        )
    { return (requested_resource.calcParent() == Jobs::RESOURCE_PATH && requested_resource != Jobs::RESOURCE_PATH_EMPTY_CHILD); }


    static capena::http::uri::Path calcUri(
            const capena::http::uri::Path& path_base,
            uint64_t job_id
        );


    Job(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _properties_ptr(args.dynamic_configuration_ptr->getPropertiesPtr())
    { /* Nothing to do */ }


    capena::http::Methods _getAllowedMethods() const  { return { capena::http::Method::GET }; }


    void _doGet();


private:

    const bgq::utility::Properties::ConstPtr _properties_ptr;

};


}} // namespace bgws::responder

#endif
