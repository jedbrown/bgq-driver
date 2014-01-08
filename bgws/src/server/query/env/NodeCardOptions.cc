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

#include "NodeCardOptions.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>


namespace bgws {
namespace query {
namespace env {


NodeCardOptions::NodeCardOptions()
    :
        AbstractOptions( BGQDB::DBTNodecardenvironment::LOCATION_SIZE ),
        _voltage_v08( "v08" ),
        _voltage_v14( "v14" ),
        _voltage_v25( "v25" ),
        _voltage_v33( "v33" ),
        _voltage_v120p( "v120p" ),
        _voltage_v15( "v15" ),
        _voltage_v09( "v09" ),
        _voltage_v10( "v10" ),
        _voltage_v33p( "v33p" ),
        _voltage_v12a( "v12a" ),
        _voltage_v12b( "v12b" ),
        _voltage_v18( "v18" ),
        _voltage_v25p( "v25p" ),
        _voltage_v12p( "v12p" ),
        _voltage_v18p( "v18p" ),
        _temp_monitor_0( "temp0" ),
        _temp_monitor_1( "temp1" )
{
    // Nothing to do.
}


void NodeCardOptions::_addExtraMappings(
        SortInfo::IdToCol& id_to_col_in_out
    )
{
    id_to_col_in_out["v08"] = _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV08_COL;
    id_to_col_in_out["v14"] = _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV14_COL;
    id_to_col_in_out["v25"] = _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV25_COL;
    id_to_col_in_out["v33"] = _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV33_COL;
    id_to_col_in_out["v120p"] = _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV120P_COL;
    id_to_col_in_out["v15"] = _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV15_COL;
    id_to_col_in_out["v09"] = _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV09_COL;
    id_to_col_in_out["v10"] = _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV10_COL;
    id_to_col_in_out["v33p"] = _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV33P_COL;
    id_to_col_in_out["v12a"] = _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV12A_COL;
    id_to_col_in_out["v12b"] = _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV12B_COL;
    id_to_col_in_out["v18"] = _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV18_COL;
    id_to_col_in_out["v25p"] = _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV25P_COL;
    id_to_col_in_out["v12p"] = _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV12P_COL;
    id_to_col_in_out["v18p"] = _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV18P_COL;
    id_to_col_in_out["temp0"] = _QUAL + BGQDB::DBTNodecardenvironment::TEMPMONITOR0_COL;
    id_to_col_in_out["temp1"] = _QUAL + BGQDB::DBTNodecardenvironment::TEMPMONITOR1_COL;
}


void NodeCardOptions::_addOptions(
        boost::program_options::options_description& desc
    )
{
    _voltage_v08.addTo( desc );
    _voltage_v14.addTo( desc );
    _voltage_v25.addTo( desc );
    _voltage_v33.addTo( desc );
    _voltage_v120p.addTo( desc );
    _voltage_v15.addTo( desc );
    _voltage_v09.addTo( desc );
    _voltage_v10.addTo( desc );
    _voltage_v33p.addTo( desc );
    _voltage_v12a.addTo( desc );
    _voltage_v12b.addTo( desc );
    _voltage_v18.addTo( desc );
    _voltage_v25p.addTo( desc );
    _voltage_v12p.addTo( desc );
    _voltage_v18p.addTo( desc );
    _temp_monitor_0.addTo( desc );
    _temp_monitor_1.addTo( desc );
}


void NodeCardOptions::_addOptions(
        WhereClause& where_clause_in_out,
        cxxdb::ParameterNames& parameter_names_in_out
    ) const
{
    _voltage_v08.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV08_COL );
    _voltage_v14.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV14_COL );
    _voltage_v25.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV25_COL );
    _voltage_v33.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV33_COL );
    _voltage_v120p.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV120P_COL );
    _voltage_v15.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV15_COL );
    _voltage_v09.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV09_COL );
    _voltage_v10.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV10_COL );
    _voltage_v33p.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV33P_COL );
    _voltage_v12a.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV12A_COL );
    _voltage_v12b.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV12B_COL );
    _voltage_v18.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV18_COL );
    _voltage_v25p.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV25P_COL );
    _voltage_v12p.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV12P_COL );
    _voltage_v18p.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::VOLTAGEV18P_COL );
    _temp_monitor_0.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::TEMPMONITOR0_COL );
    _temp_monitor_1.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTNodecardenvironment::TEMPMONITOR1_COL );
}


void NodeCardOptions::_bindParameters(
        cxxdb::Parameters& parameters_in_out
    )
{
    _voltage_v08.bindParameters( parameters_in_out );
    _voltage_v14.bindParameters( parameters_in_out );
    _voltage_v25.bindParameters( parameters_in_out );
    _voltage_v33.bindParameters( parameters_in_out );
    _voltage_v120p.bindParameters( parameters_in_out );
    _voltage_v15.bindParameters( parameters_in_out );
    _voltage_v09.bindParameters( parameters_in_out );
    _voltage_v10.bindParameters( parameters_in_out );
    _voltage_v33p.bindParameters( parameters_in_out );
    _voltage_v12a.bindParameters( parameters_in_out );
    _voltage_v12b.bindParameters( parameters_in_out );
    _voltage_v18.bindParameters( parameters_in_out );
    _voltage_v25p.bindParameters( parameters_in_out );
    _voltage_v12p.bindParameters( parameters_in_out );
    _voltage_v18p.bindParameters( parameters_in_out );
    _temp_monitor_0.bindParameters( parameters_in_out );
    _temp_monitor_1.bindParameters( parameters_in_out );
}

} } } // namespace bgws::query::env
