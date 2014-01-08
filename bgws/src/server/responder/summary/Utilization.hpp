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

#ifndef BGWS_RESPONDER_SUMMARY_UTILIZATION_HPP_
#define BGWS_RESPONDER_SUMMARY_UTILIZATION_HPP_


#include "../../AbstractResponder.hpp"

#include "capena-http/http/uri/Path.hpp"

#include <cstdint>


namespace bgws {
namespace responder {
namespace summary {


class Utilization : public AbstractResponder
{
public:

    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        )
    {
        static const capena::http::uri::Path URL_PATH(capena::http::uri::Path() / "summary" / "utilization");
        return (requested_resource == URL_PATH);
    }


    Utilization(
            CtorArgs& args
        );


    capena::http::Methods _getAllowedMethods() const
    {
        static const capena::http::Methods METHODS = { capena::http::Method::GET };
        return METHODS;
    }

    void _doGet();


private:

    std::uint64_t _system_cpu_count;
    BlockingOperationsThreadPool &_blocking_operations_thread_pool;


    void _doQuery(
            capena::server::ResponderPtr
        );


    void _queryComplete(
            capena::server::ResponderPtr,
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );

};


} } } // namespace bgws::responder::summary


#endif
