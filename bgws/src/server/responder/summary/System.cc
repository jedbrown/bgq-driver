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

/*! \page systemSummaryResource /bg/summary/system

This resource provides summary information related to the Blue Gene system.

- \subpage systemSummaryResourceGet

 */

/*! \page systemSummaryResourceGet GET /bg/summary/system

Get summary information related to the Blue Gene system.

\section Authority

The user must be authenticated.

\section systemSummaryResourceGetResponse JSON response format

<pre>
{
  "alertCount" : <i>number</i>, // optional
  "jobCount": <i>number</i>, // number of jobs running on the system
  "jobCpuCount": <i>number</i>, // number of CPUs taken up by jobs running on the system
  "mps": [ // optional
    &quot;<i>location</i>&quot;: {
      "status": &quot;<i>string</i>&quot;, // optional

      "nodeBoards": { // optional
        &quot;<i>node-board-position</i>&quot;: &quot;<i>string</i>&quot;
      }
    }
  ],
  "hardwareNotifications": [  // optional
    {
      "location": &quot;<i>location</i>&quot;,
      "status": &quot;<i>status</i>&quot; // optional -- either status or nodeCount are set
      "nodeCount": <i>number</i> // optional -- either status or nodeCount are set
    }, ...
  ],
  "diagnostics": [ // optional
    {
      "location": &quot;<i>location</i>&quot;
    }, ...
  ],
  "serviceActions": [ // optional
    {
      "id": <i>number</i>,
      "location": &quot;<i>location</i>&quot;,
      "action": &quot;<i>action</i>&quot;,
      "prepareUser": &quot;<i>string</i>&quot;,
      "endUser": &quot;<i>string</i>&quot; // optional
    }, ...
  ]
}
</pre>

- status is one of
  - AVAILABLE: The hardware is available.
  - BLOCK_BUSY: A block on the hardware is in the process of booting or freeing.
  - BLOCK_READY: A block on the hardware is ready to run a job.
  - JOB_RUNNING: A job is running on the hardware.
  - DIAGNOSTICS: Diagnostics is running on the hardware.
  - SERVICE_ACTION: A service action is in progress on the hardware.
  - NOT_AVAILABLE: The hardware is not availble.


\section Errors

HTTP status: 403 Forbidden
- notAuthenticated: The user isn't authenticated.

 */


#include "System.hpp"

#include "../../blue_gene.hpp"
#include "../../dbConnectionPool.hpp"
#include "../../Error.hpp"
#include "../../sqlStrings.gen.hpp"

#include "capena-http/server/exception.hpp"
#include "capena-http/server/Response.hpp"

#include <utility/include/Log.h>

#include <boost/algorithm/string.hpp>

#include <string>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {
namespace summary {


bool System::matchesUrl(
        const capena::http::uri::Path& requested_resource
    )
{
    static const capena::http::uri::Path URL_PATH(capena::http::uri::Path() / "summary" / "system");
    return (requested_resource == URL_PATH);
}


capena::http::Methods System::_getAllowedMethods() const
{
    return { capena::http::Method::GET };
}


void System::_doGet()
{

    // The user must be authenticated.
    if ( ! _isUserAuthenticated() ) {
        LOG_WARN_MSG( "Cannot get system summary because the user isn't authenticated." );

        BOOST_THROW_EXCEPTION( Error(
                "Cannot get system summary because the user isn't authenticated.",
                "getSystemSummary", "notAuthenticated", Error::Data(),
                capena::http::Status::Forbidden
            ) );
    }


    capena::server::Response &response(_getResponse());

    auto conn_ptr(dbConnectionPool::getConnection());

    response.setContentTypeJson();
    response.headersComplete();

    json::ObjectValue obj_val;
    json::Object &obj(obj_val.get());

    uint64_t job_count, job_cpus;

    _calcJobSummary( *conn_ptr, job_count, job_cpus );

    obj.set( "jobCount", job_count ); // Number of jobs running on the system.
    obj.set( "jobCpuCount", job_cpus ); // Number of CPUs taken up by jobs running on the system.

    _addAlertSummary( *conn_ptr, obj );

    _addMidplaneStatus( *conn_ptr, obj );

    _addHardwareNotifications( *conn_ptr, obj );

    _addDiagnostics( *conn_ptr, obj );
    _addServiceActions( *conn_ptr, obj );

    json::Formatter()( obj_val, response.out() );
}


void System::_calcJobSummary(
        cxxdb::Connection &db_conn,
        uint64_t &job_count_out,
        uint64_t &job_cpus_out
    ) const
{
    job_count_out = 0;
    job_cpus_out = 0;

    cxxdb::ResultSetPtr rs_ptr(db_conn.query(

 "SELECT rj.job_count, rj.job_cpus + snj.job_cpus AS job_cpus"
  " FROM ( SELECT COUNT(*) AS job_count,"
                " COALESCE( SUM( nodesUsed ), 0 ) AS job_cpus"
           " FROM bgqJob"
       " ) AS rj,"
       " ( SELECT COUNT(*) AS job_cpus"
           " FROM ( SELECT DISTINCT LEFT( corner, 14 ) AS n_loc"
                    " FROM bgqjob"
                    " WHERE nodesUsed = 0"
                " ) AS a"
       " ) AS snj"

        ));

    if ( ! rs_ptr->fetch() ) {
        job_count_out = 0;
        job_cpus_out = 0;
        return;
    }

    job_count_out = rs_ptr->columns()["job_count"].as<uint64_t>();
    job_cpus_out = rs_ptr->columns()["job_cpus"].as<uint64_t>();
}


void System::_addAlertSummary(
        cxxdb::Connection& db_conn,
        json::Object& obj_in_out
    )
{
    cxxdb::ResultSetPtr rs_ptr(db_conn.query(

 "WITH open_alerts AS ("

" SELECT \"rec_id\""
  " FROM x_tealalertlog"
  " WHERE \"state\" <> 2 OR \"state\" IS NULL"

" )"

" SELECT COUNT(*) AS c"
  " FROM open_alerts AS o"
       " LEFT OUTER JOIN"
       " x_tealalert2alert AS r"
       " ON o.\"rec_id\" = r.\"t_alert_recid\" AND r.\"assoc_type\" = 'D'"
  " WHERE r.\"assoc_type\" IS NULL"

        ));

    if ( ! rs_ptr->fetch() ) {
        return;
    }

    uint64_t alert_count(rs_ptr->columns()["c"].as<uint64_t>());

    if ( alert_count == 0 )  return;

    obj_in_out.set( "alertCount", alert_count );
}


void System::_addMidplaneStatus(
        cxxdb::Connection &db_conn,
        json::Object& obj_in_out
    )
{
    cxxdb::ResultSetPtr rs_ptr(db_conn.query(
            sql::SUMMARY_MACHINE_USAGE // queries/summaryMachineUsage.txt
        ));

    json::Object *mps_obj_p(NULL);

    while ( rs_ptr->fetch() ) {

        const cxxdb::Columns &cols(rs_ptr->columns());

        if ( ! mps_obj_p )  mps_obj_p = &(obj_in_out.createObject( "mps" ));

        int row_status(cols["state"].as<int>());

        string status_str(
                row_status == 0 ? "AVAILABLE" :
                row_status == 1 ? "BLOCK_BUSY" :
                row_status == 2 ? "BLOCK_READY" :
                row_status == 3 ? "JOB_RUNNING" :
                row_status == 4 ? "DIAGNOSTICS" :
                row_status == 5 ? "SERVICE_ACTION" :
                row_status == 6 ? "NOT_AVAILABLE" :
                "UNKNOWN"
            );

        string mp_location(cols["mp_location"].getString());

        json::Object *mp_obj_p;

        if ( mps_obj_p->contains( mp_location ) ) {
            mp_obj_p = &((*mps_obj_p)[mp_location]->getObject());
        } else {
            mp_obj_p = &(mps_obj_p->createObject( mp_location ));
        }

        if ( cols["nodeCardPos"].isNull() ) {
            // No node board pos, so set the status for the midplane.

            mp_obj_p->set( "status", status_str );

        } else {
            // Node board pos, so set the status for the node board.

            string nb_pos(cols["nodeCardPos"].getString());

            json::Object *nbs_obj_p;
            if ( mp_obj_p->contains( "nodeBoards" ) ) {
                nbs_obj_p = &(mp_obj_p->getObject( "nodeBoards" ));
            } else {
                nbs_obj_p = &(mp_obj_p->createObject( "nodeBoards" ));
            }

            nbs_obj_p->set( nb_pos, status_str );
        }
    }
}


void System::_addHardwareNotifications(
        cxxdb::Connection& db_conn,
        json::Object& obj_in_out
    )
{
    cxxdb::ResultSetPtr rs_ptr(db_conn.query(
            sql::SUMMARY_HARDWARE_PROBLEMS // queries/summaryHardwareProblems.txt
        ));

    json::Array *nots_arr_p(NULL);

    while ( rs_ptr->fetch() ) {
        if ( ! nots_arr_p )  nots_arr_p = &(obj_in_out.createArray( "hardwareNotifications" ));

        const cxxdb::Columns &cols(rs_ptr->columns());

        json::Object &hw_obj(nots_arr_p->addObject());
        hw_obj.set( "location", boost::algorithm::trim_copy( cols["location"].getString() ) );
        if ( cols["status"] )  hw_obj.set( "status", cols["status"].getString() );
        if ( cols["nodeCount"] ) hw_obj.set( "nodeCount", cols["nodeCount"].as<uint64_t>() );
    }
}


void System::_addDiagnostics(
        cxxdb::Connection& db_conn,
        json::Object& obj_in_out
    )
{
    cxxdb::ResultSetPtr rs_ptr(db_conn.query(
            sql::SUMMARY_DIAGNOSTICS // queries/summaryDiagnostics.txt
        ));

    json::Array *diags_arr_p(NULL);

    while ( rs_ptr->fetch() ) {
        if ( ! diags_arr_p )  diags_arr_p = &(obj_in_out.createArray( "diagnostics" ));

        const cxxdb::Columns &cols(rs_ptr->columns());

        json::Object &diag_obj(diags_arr_p->addObject());

        string block_id(cols["blockId"].getString());

        diag_obj.set( "block", block_id );
        if ( boost::algorithm::starts_with( block_id, blue_gene::diagnostics::HARDWARE_BLOCK_PREFIX ) ) {
            diag_obj.set( "location", block_id.substr( blue_gene::diagnostics::HARDWARE_BLOCK_PREFIX.size() ) );
        }
        diag_obj.set( "runId", cols["runId"].as<uint64_t>() );
    }
}


void System::_addServiceActions(
        cxxdb::Connection& db_conn,
        json::Object& obj_in_out
    )
{
    cxxdb::ResultSetPtr rs_ptr(db_conn.query(
            sql::SUMMARY_SERVICE_ACTIONS // queries/summaryServiceActions.txt
        ));

    json::Array *sas_arr_p(NULL);

    while ( rs_ptr->fetch() ) {
        if ( ! sas_arr_p )  sas_arr_p = &(obj_in_out.createArray( "serviceActions" ));

        const cxxdb::Columns &cols(rs_ptr->columns());

        json::Object &sa_obj(sas_arr_p->addObject());

        sa_obj.set( "id", cols["id"].as<uint64_t>() );
        sa_obj.set( "location", cols["location"].getString() );
        sa_obj.set( "action", cols["serviceAction"].getString() );
        sa_obj.set( "prepareUser", cols["usernamePrepareForService"].getString() );
        if ( cols["usernameEndServiceAction"] )  sa_obj.set( "endUser", cols["usernameEndServiceAction"].getString() );
    }
}


} } } // namespace bgws::responder::summary
