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

#ifndef BGWS_RESPONDER_JOBS_HPP_
#define BGWS_RESPONDER_JOBS_HPP_


#include "../AbstractResponder.hpp"

#include "capena-http/http/uri/Path.hpp"

#include <db/include/api/filtering/JobFilter.h>
#include <db/include/api/filtering/JobSort.h>

#include <string>

#include <stdint.h>


namespace bgws {
namespace responder {


class Jobs : public AbstractResponder
{
public:


    static const capena::http::uri::Path &RESOURCE_PATH;
    static const capena::http::uri::Path RESOURCE_PATH_EMPTY_CHILD;


    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        )
    { return (requested_resource == RESOURCE_PATH || requested_resource == RESOURCE_PATH_EMPTY_CHILD); }


    Jobs(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _properties_ptr(args.dynamic_configuration_ptr->getPropertiesPtr())
    { /* Nothing to do */ }


    capena::http::Methods getAllowedMethods() const  { return { capena::http::Method::GET }; }


    void doGet();


private:

    const bgq::utility::Properties::ConstPtr _properties_ptr;


    void _calcJobFilterAndSort(
            bool* no_jobs_out,
            BGQDB::filtering::JobFilter& job_filter_out,
            BGQDB::filtering::JobSort& job_sort_out,
            std::string& service_location_out
        ) const;

    void _getServiceJobs( const std::string& location );
};

}} // namespace bgws::responder

#endif
