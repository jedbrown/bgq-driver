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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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


#include "SystemSummary.hpp"

#include "chiron-json/json.hpp"

#include "../BlockingOperationsThreadPool.hpp"
#include "../blue_gene.hpp"
#include "../dbConnectionPool.hpp"
#include "../sqlStrings.gen.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <utility/include/Log.h>

#include <boost/foreach.hpp>

#include <boost/algorithm/string.hpp>

#include <string>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace query {


namespace statics {


namespace statuses {
static const string Available("AVAILABLE");
static const string BlockBusy("BLOCK_BUSY");
static const string BlockReady("BLOCK_READY");
static const string JobRunning("JOB_RUNNING");
static const string Diagnostics("DIAGNOSTICS");
static const string ServiceAction("SERVICE_ACTION");
static const string NotAvailable("NOT_AVAILABLE");
static const string Unknown("UNKNOWN");


const string& calcStatusStr( int status_code )
{
    const string &status_str(
            status_code == 0 ? Available :
            status_code == 1 ? BlockBusy :
            status_code == 2 ? BlockReady :
            status_code == 3 ? JobRunning :
            status_code == 4 ? Diagnostics :
            status_code == 5 ? ServiceAction :
            status_code == 6 ? NotAvailable :
            Unknown
        );

    return status_str;
}

}

}


SystemSummary::Ptr SystemSummary::create(
        BlockingOperationsThreadPool& blocking_operations_thread_pool
    )
{
    return Ptr( new SystemSummary(
            blocking_operations_thread_pool
        ) );
}


SystemSummary::SystemSummary(
        BlockingOperationsThreadPool& blocking_operations_thread_pool
    ) :
        _strand( blocking_operations_thread_pool.getIoService() ),
        _blocking_operations_thread_pool(blocking_operations_thread_pool)
{
    // Nothing to do.
}


void SystemSummary::start(
        CompleteCb complete_cb
    )
{
    _complete_cb = complete_cb;

    _obj_val_ptr = json::Object::create();


    _blocking_operations_thread_pool.post( boost::bind(
            &SystemSummary::_doJobSummaryQuery, shared_from_this()
        ) );

    _blocking_operations_thread_pool.post( boost::bind(
            &SystemSummary::_doAlertSummaryQuery, shared_from_this()
        ) );

    _blocking_operations_thread_pool.post( boost::bind(
            &SystemSummary::_doMidplaneStatusQuery, shared_from_this()
        ) );

    _blocking_operations_thread_pool.post( boost::bind(
            &SystemSummary::_doIoStatusQuery, shared_from_this()
        ) );

    _blocking_operations_thread_pool.post( boost::bind(
            &SystemSummary::_doHardwareNotificationsQuery, shared_from_this()
        ) );

    _blocking_operations_thread_pool.post( boost::bind(
            &SystemSummary::_doDiagnosticsQuery, shared_from_this()
        ) );

    _blocking_operations_thread_pool.post( boost::bind(
            &SystemSummary::_doServiceActionQuery, shared_from_this()
        ) );

}


SystemSummary::~SystemSummary()
{
    _complete_cb(
            _exc_ptr,
            _obj_val_ptr
        );
}


void SystemSummary::_doJobSummaryQuery()
{
    if ( _exc_ptr != 0 )  return;

    try {

        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query(

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

        uint64_t job_count;
        uint64_t job_cpus;

        if ( ! rs_ptr->fetch() ) {
            job_count = 0;
            job_cpus = 0;
        } else {
            job_count = rs_ptr->columns()["job_count"].as<uint64_t>();
            job_cpus = rs_ptr->columns()["job_cpus"].as<uint64_t>();
        }

        _strand.post( boost::bind(
                &SystemSummary::_jobSummaryQueryComplete, shared_from_this(),
                job_count,
                job_cpus
            ) );

    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}

void SystemSummary::_jobSummaryQueryComplete(
        uint64_t job_count,
        uint64_t job_cpus
    )
{
    if ( _exc_ptr != 0 )  return;

    try {

        _obj_val_ptr->get().set( "jobCount", job_count ); // Number of jobs running on the system.
        _obj_val_ptr->get().set( "jobCpuCount", job_cpus ); // Number of CPUs taken up by jobs running on the system.

    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}

void SystemSummary::_doAlertSummaryQuery()
{
    if ( _exc_ptr != 0 )  return;

    try {

        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query(

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

        uint64_t alert_count(0);

        if ( rs_ptr->fetch() ) {
            alert_count = rs_ptr->columns()["c"].as<uint64_t>();
        }

        _strand.post( boost::bind(
                &SystemSummary::_alertSummaryQueryComplete, shared_from_this(),
                alert_count
            ) );

    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void SystemSummary::_alertSummaryQueryComplete(
        uint64_t alert_count
    )
{
    if ( _exc_ptr != 0 )  return;

    try {

        if ( alert_count != 0 ) {
            _obj_val_ptr->get().set( "alertCount", alert_count );
        }

    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}

void SystemSummary::_doMidplaneStatusQuery()
{
    if ( _exc_ptr != 0 )  return;

    try {

        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query(
                sql::SUMMARY_MACHINE_USAGE // queries/summaryMachineUsage.txt
            ));

        _strand.post( boost::bind(
                &SystemSummary::_midplaneStatusQueryComplete, shared_from_this(),
                conn_ptr, rs_ptr
            ) );

    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void SystemSummary::_midplaneStatusQueryComplete(
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    if ( _exc_ptr != 0 )  return;

    try {

        json::Object *mps_obj_p(NULL);

        while ( rs_ptr->fetch() ) {

            const cxxdb::Columns &cols(rs_ptr->columns());

            if ( ! mps_obj_p )  mps_obj_p = &(_obj_val_ptr->get().createObject( "mps" ));

            string mp_location(cols["mp_location"].getString());

            json::Object *mp_obj_p;

            if ( mps_obj_p->contains( mp_location ) ) {
                mp_obj_p = &((*mps_obj_p)[mp_location]->getObject());
            } else {
                mp_obj_p = &(mps_obj_p->createObject( mp_location ));
            }

            if ( ! cols["nodeCardPos"] ) {
                // No node board pos, so set the status for the midplane.

                int state(cols["state"] ? cols["state"].as<int>() : -1);
                int pt_state(cols["pt_state"] ? cols["pt_state"].as<int>() : -1);

                LOG_DEBUG_MSG( "For " << mp_location << " state=" << state << " pt_state=" << pt_state );

                if ( state != -1 && pt_state != -1 ) {
                    // used both for block and passthru,
                    // set the midplane to pt_state, set the node boards to state!

                    mp_obj_p->set( "status", statics::statuses::calcStatusStr(pt_state) );

                    json::Object *nbs_obj_p;
                    if ( mp_obj_p->contains( "nodeBoards" ) ) {
                        nbs_obj_p = &(mp_obj_p->getObject( "nodeBoards" ));
                    } else {
                        nbs_obj_p = &(mp_obj_p->createObject( "nodeBoards" ));
                    }

                    const string &nb_status_str(statics::statuses::calcStatusStr(state));

                    BOOST_FOREACH( const string& nb_pos, blue_gene::ALL_NB_POS ) {
                        nbs_obj_p->set( nb_pos, nb_status_str );
                    }

                } else if ( state != -1 ) {
                    // midplane in use and no passthru.

                    mp_obj_p->set( "status", statics::statuses::calcStatusStr( state ) );

                } else {
                    // passthru only.
                    // set nodeBoards to empty if no nodeboards yet.

                    mp_obj_p->set( "status", statics::statuses::calcStatusStr( pt_state ) );

                    if ( ! mp_obj_p->contains( "nodeBoards" ) ) {
                        mp_obj_p->createObject( "nodeBoards" );
                    }
                }

            } else {
                // Node board pos, so set the status for the node board.

                string nb_pos(cols["nodeCardPos"].getString());

                json::Object *nbs_obj_p;
                if ( mp_obj_p->contains( "nodeBoards" ) ) {
                    nbs_obj_p = &(mp_obj_p->getObject( "nodeBoards" ));
                } else {
                    nbs_obj_p = &(mp_obj_p->createObject( "nodeBoards" ));
                }

                int row_status(cols["state"].as<int>());
                nbs_obj_p->set( nb_pos, statics::statuses::calcStatusStr( row_status ) );
            }
        }

    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }

}

void SystemSummary::_doIoStatusQuery()
{
    if ( _exc_ptr != 0 )  return;

    try {

        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query(
                sql::SUMMARY_IO_STATUS // queries/summaryIoStatus.txt
            ));

        _strand.post( boost::bind(
                &SystemSummary::_ioStatusQueryComplete, shared_from_this(),
                conn_ptr, rs_ptr
            ) );

    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }

}

void SystemSummary::_ioStatusQueryComplete(
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    if ( _exc_ptr != 0 )  return;

    try {

        // Result is like:
        // LOCATION  STATUSCODE
        // where LOCATION is an I/O rack or I/O drawer.
        // and status code is
        //   1=BLOCK_READY, 2=BLOCK_BUSY, 3=AVAILABLE, 4=DIAGNOSTICS, 5=SERVICE_ACTION, 6=NOT_AVAILABLE

        static const string code_to_status[] = {
                string(),
                string(statics::statuses::BlockReady),
                string(statics::statuses::BlockBusy),
                string(statics::statuses::Available),
                string(statics::statuses::Diagnostics),
                string(statics::statuses::ServiceAction),
                string(statics::statuses::NotAvailable)
            };


        json::Object *racks_obj_p(0);


        while ( rs_ptr->fetch() ) {

            if ( ! racks_obj_p )  racks_obj_p = &(_obj_val_ptr->get().createObject( "racks" ));

            const cxxdb::Columns &cols(rs_ptr->columns());

            string loc(cols["location"].getString());
            int code(cols["statusCode"].as<int>());

            if ( loc.size() == 3 ) { // it's an I/O rack.

                json::Object *rack_obj_p;

                if ( racks_obj_p->contains( loc ) ) {
                    rack_obj_p = &((*racks_obj_p)[loc]->getObject()); // It's already there.
                } else {
                    rack_obj_p = &(racks_obj_p->createObject( loc )); // Create if it's not there.
                }

                rack_obj_p->set( "status", code_to_status[code] );

            } else { // it's an I/O drawer.

                string rack_loc(loc.substr( 0, 3 ));

                json::Object *rack_obj_p;

                if ( racks_obj_p->contains( rack_loc ) ) {
                    rack_obj_p = &((*racks_obj_p)[rack_loc]->getObject()); // It's already there.
                } else {
                    rack_obj_p = &(racks_obj_p->createObject( rack_loc )); // Create if it's not there.
                }

                json::Object *drawers_obj_p;

                if ( rack_obj_p->contains( "drawers" ) ) {
                    drawers_obj_p = &((*rack_obj_p)["drawers"]->getObject());
                } else {
                    drawers_obj_p = &(rack_obj_p->createObject( "drawers" ));
                }

                drawers_obj_p->set( loc.substr( 4 ), code_to_status[code] );
            }

        }

    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }

}

void SystemSummary::_doHardwareNotificationsQuery()
{
    if ( _exc_ptr != 0 )  return;

    try {
        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query(
                sql::SUMMARY_HARDWARE_PROBLEMS // queries/summaryHardwareProblems.txt
            ));

        _strand.post( boost::bind(
                &SystemSummary::_hardwareNotificationsQueryComplete, shared_from_this(),
                conn_ptr, rs_ptr
            ) );

    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }

}


void SystemSummary::_hardwareNotificationsQueryComplete(
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    if ( _exc_ptr != 0 )  return;

    try {
        json::Array *nots_arr_p(NULL);

        while ( rs_ptr->fetch() ) {
            if ( ! nots_arr_p )  nots_arr_p = &(_obj_val_ptr->get().createArray( "hardwareNotifications" ));

            const cxxdb::Columns &cols(rs_ptr->columns());

            json::Object &hw_obj(nots_arr_p->addObject());
            hw_obj.set( "location", boost::algorithm::trim_copy( cols["location"].getString() ) );
            if ( cols["status"] )  hw_obj.set( "status", cols["status"].getString() );
            if ( cols["nodeCount"] ) hw_obj.set( "nodeCount", cols["nodeCount"].as<uint64_t>() );
        }

    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }

}

void SystemSummary::_doDiagnosticsQuery()
{
    if ( _exc_ptr != 0 )  return;

    try {
        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query(
                sql::SUMMARY_DIAGNOSTICS // queries/summaryDiagnostics.txt
            ));

        _strand.post( boost::bind(
                &SystemSummary::_diagnosticsQueryComplete, shared_from_this(),
                conn_ptr, rs_ptr
            ) );

    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void SystemSummary::_diagnosticsQueryComplete(
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    if ( _exc_ptr != 0 )  return;

    try {

        json::Array *diags_arr_p(NULL);

        while ( rs_ptr->fetch() ) {
            if ( ! diags_arr_p )  diags_arr_p = &(_obj_val_ptr->get().createArray( "diagnostics" ));

            const cxxdb::Columns &cols(rs_ptr->columns());

            json::Object &diag_obj(diags_arr_p->addObject());

            string block_id(cols["blockId"].getString());

            diag_obj.set( "block", block_id );
            if ( boost::algorithm::starts_with( block_id, blue_gene::diagnostics::HARDWARE_BLOCK_PREFIX ) ) {
                diag_obj.set( "location", block_id.substr( blue_gene::diagnostics::HARDWARE_BLOCK_PREFIX.size() ) );
            }
            diag_obj.set( "runId", cols["runId"].as<uint64_t>() );
        }

    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }

}


void SystemSummary::_doServiceActionQuery()
{
    if ( _exc_ptr != 0 )  return;

    try {
        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query(
                sql::SUMMARY_SERVICE_ACTIONS // queries/summaryServiceActions.txt
            ));

        _strand.post( boost::bind(
                &SystemSummary::_serviceActionQueryComplete, shared_from_this(),
                conn_ptr, rs_ptr
            ) );

    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }

}


void SystemSummary::_serviceActionQueryComplete(
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    if ( _exc_ptr != 0 )  return;

    try {

        json::Array *sas_arr_p(NULL);

        while ( rs_ptr->fetch() ) {
            if ( ! sas_arr_p )  sas_arr_p = &(_obj_val_ptr->get().createArray( "serviceActions" ));

            const cxxdb::Columns &cols(rs_ptr->columns());

            json::Object &sa_obj(sas_arr_p->addObject());

            sa_obj.set( "id", cols["id"].as<uint64_t>() );
            sa_obj.set( "location", cols["location"].getString() );
            sa_obj.set( "action", cols["serviceAction"].getString() );
            sa_obj.set( "prepareUser", cols["usernamePrepareForService"].getString() );
            if ( cols["usernameEndServiceAction"] )  sa_obj.set( "endUser", cols["usernameEndServiceAction"].getString() );
        }

    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }

}


} } // namespace bgws::query
