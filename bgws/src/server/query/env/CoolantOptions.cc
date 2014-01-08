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

#include "CoolantOptions.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>


namespace bgws {
namespace query {
namespace env {


CoolantOptions::CoolantOptions()
    :
        AbstractOptions( BGQDB::DBTCoolantenvironment::LOCATION_SIZE ),
        _inlet_flow_rate( "inletFlowRate" ),
        _outlet_flow_rate( "outletFlowRate" ),
        _coolant_pressure( "coolantPressure" ),
        _diff_pressure( "diffPressure" ),
        _inlet_coolant_temp( "inletCoolantTemp" ),
        _outlet_coolant_temp( "outletCoolantTemp" ),
        _dewpoint_temp( "dewpointTemp" ),
        _ambient_temp( "ambientTemp" ),
        _ambient_humidity( "ambientHumidity" ),
        _system_power( "systemPower" ),
        _shutoff_cause( "shutoffCause" )
{
    // Nothing to do.
}


void CoolantOptions::_addExtraMappings(
        SortInfo::IdToCol& id_to_col_in_out
    )
{
    id_to_col_in_out["inletFlowRate"] = _QUAL + BGQDB::DBTCoolantenvironment::INLETFLOWRATE_COL;
    id_to_col_in_out["outletFlowRate"] = _QUAL + BGQDB::DBTCoolantenvironment::OUTLETFLOWRATE_COL;
    id_to_col_in_out["coolantPressure"] = _QUAL + BGQDB::DBTCoolantenvironment::COOLANTPRESSURE_COL;
    id_to_col_in_out["diffPressure"] = _QUAL + BGQDB::DBTCoolantenvironment::DIFFPRESSURE_COL;
    id_to_col_in_out["inletCoolantTemp"] = _QUAL + BGQDB::DBTCoolantenvironment::INLETCOOLANTTEMP_COL;
    id_to_col_in_out["outletCoolantTemp"] = _QUAL + BGQDB::DBTCoolantenvironment::OUTLETCOOLANTTEMP_COL;
    id_to_col_in_out["dewpointTemp"] = _QUAL + BGQDB::DBTCoolantenvironment::DEWPOINTTEMP_COL;
    id_to_col_in_out["ambientTemp"] = _QUAL + BGQDB::DBTCoolantenvironment::AMBIENTTEMP_COL;
    id_to_col_in_out["ambientHumidity"] = _QUAL + BGQDB::DBTCoolantenvironment::AMBIENTHUMIDITY_COL;
    id_to_col_in_out["systemPower"] = _QUAL + BGQDB::DBTCoolantenvironment::SYSTEMPOWER_COL;
    id_to_col_in_out["shutoffCause"] = _QUAL + BGQDB::DBTCoolantenvironment::SHUTOFFCAUSE_COL;
}


void CoolantOptions::_addOptions(
        boost::program_options::options_description& desc
    )
{
    _inlet_flow_rate.addTo( desc );
    _outlet_flow_rate.addTo( desc );
    _coolant_pressure.addTo( desc );
    _diff_pressure.addTo( desc );
    _inlet_coolant_temp.addTo( desc );
    _outlet_coolant_temp.addTo( desc );
    _dewpoint_temp.addTo( desc );
    _ambient_temp.addTo( desc );
    _ambient_humidity.addTo( desc );
    _system_power.addTo( desc );
    _shutoff_cause.addTo( desc );
}


void CoolantOptions::_addOptions(
        WhereClause& where_clause_in_out,
        cxxdb::ParameterNames& parameter_names_in_out
    ) const
{
    _inlet_flow_rate.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTCoolantenvironment::INLETFLOWRATE_COL );
    _outlet_flow_rate.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTCoolantenvironment::OUTLETFLOWRATE_COL );
    _coolant_pressure.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTCoolantenvironment::COOLANTPRESSURE_COL );
    _diff_pressure.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTCoolantenvironment::DIFFPRESSURE_COL );
    _inlet_coolant_temp.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTCoolantenvironment::INLETCOOLANTTEMP_COL );
    _outlet_coolant_temp.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTCoolantenvironment::OUTLETCOOLANTTEMP_COL );
    _dewpoint_temp.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTCoolantenvironment::DEWPOINTTEMP_COL );
    _ambient_temp.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTCoolantenvironment::AMBIENTTEMP_COL );
    _ambient_humidity.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTCoolantenvironment::AMBIENTHUMIDITY_COL );
    _system_power.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTCoolantenvironment::SYSTEMPOWER_COL );
    _shutoff_cause.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTCoolantenvironment::SHUTOFFCAUSE_COL );
}


void CoolantOptions::_bindParameters(
        cxxdb::Parameters& parameters_in_out
    )
{
    _inlet_flow_rate.bindParameters( parameters_in_out );
    _outlet_flow_rate.bindParameters( parameters_in_out );
    _coolant_pressure.bindParameters( parameters_in_out );
    _diff_pressure.bindParameters( parameters_in_out );
    _inlet_coolant_temp.bindParameters( parameters_in_out );
    _outlet_coolant_temp.bindParameters( parameters_in_out );
    _dewpoint_temp.bindParameters( parameters_in_out );
    _ambient_temp.bindParameters( parameters_in_out );
    _ambient_humidity.bindParameters( parameters_in_out );
    _system_power.bindParameters( parameters_in_out );
    _shutoff_cause.bindParameters( parameters_in_out );
}


} } } // namespace bgws::query::env
