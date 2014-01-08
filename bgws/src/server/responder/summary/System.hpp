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

#ifndef BGWS_RESPONDER_SUMMARY_SYSTEM_HPP_
#define BGWS_RESPONDER_SUMMARY_SYSTEM_HPP_


#include "../../AbstractResponder.hpp"

#include "../../BlueGene.hpp"

#include "capena-http/http/uri/Path.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/cxxdb/cxxdb.h>


namespace bgws {
namespace responder {
namespace summary {


class System : public AbstractResponder
{
public:

    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        );


    System(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _blue_gene(args.blue_gene)
    { /* Nothing to do */ }


    capena::http::Methods getAllowedMethods() const;

    void doGet();


private:

    const BlueGene &_blue_gene;


    void _calcJobSummary(
            cxxdb::Connection& db_conn,
            uint64_t& job_count_out,
            uint64_t& job_cpus_out
        ) const;

    void _addAlertSummary(
            cxxdb::Connection& db_conn,
            json::Object& obj_in_out
        );

    void _addMidplaneStatus(
            cxxdb::Connection& db_conn,
            json::Object& obj_in_out
        );

    void _addHardwareNotifications(
            cxxdb::Connection& db_conn,
            json::Object& obj_in_out
        );

    void _addDiagnostics(
            cxxdb::Connection& db_conn,
            json::Object& obj_in_out
        );

    void _addServiceActions(
            cxxdb::Connection& db_conn,
            json::Object& obj_in_out
        );

};


} } } // namespace bgws::responder::summary


#endif
