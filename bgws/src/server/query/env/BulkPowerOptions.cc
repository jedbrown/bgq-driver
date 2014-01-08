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

#include "BulkPowerOptions.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>


namespace bgws {
namespace query {
namespace env {


BulkPowerOptions::BulkPowerOptions()
    :
      AbstractOptions( BGQDB::DBTBulkpowerenvironment::LOCATION_SIZE ),
      _input_voltage( "inputVoltage" ),
      _input_current( "inputCurrent" ),
      _output_voltage( "outputVoltage" ),
      _output_current( "outputCurrent" )
{
    // Nothing to do.
}


void BulkPowerOptions::_addExtraMappings(
        SortInfo::IdToCol& id_to_col_in_out
    )
{
    id_to_col_in_out["inputVoltage"] = _QUAL + BGQDB::DBTBulkpowerenvironment::INPUTVOLTAGE_COL;
    id_to_col_in_out["inputCurrent"] = _QUAL + BGQDB::DBTBulkpowerenvironment::INPUTCURRENT_COL;
    id_to_col_in_out["outputVoltage"] = _QUAL + BGQDB::DBTBulkpowerenvironment::OUTPUTVOLTAGE_COL;
    id_to_col_in_out["outputCurrent"] = _QUAL + BGQDB::DBTBulkpowerenvironment::OUTPUTCURRENT_COL;
}


void BulkPowerOptions::_addOptions(
        boost::program_options::options_description& desc
    )
{
    _input_voltage.addTo( desc );
    _input_current.addTo( desc );
    _output_voltage.addTo( desc );
    _output_current.addTo( desc );
}


void BulkPowerOptions::_addOptions(
        WhereClause& where_clause_in_out,
        cxxdb::ParameterNames& parameter_names_in_out
    ) const
{
    _input_voltage.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTBulkpowerenvironment::INPUTVOLTAGE_COL );
    _input_current.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTBulkpowerenvironment::INPUTCURRENT_COL );
    _output_voltage.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTBulkpowerenvironment::OUTPUTVOLTAGE_COL );
    _output_current.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTBulkpowerenvironment::OUTPUTCURRENT_COL );
}


void BulkPowerOptions::_bindParameters(
        cxxdb::Parameters& parameters_in_out
    )
{
    _input_voltage.bindParameters( parameters_in_out );
    _input_current.bindParameters( parameters_in_out );
    _output_voltage.bindParameters( parameters_in_out );
    _output_current.bindParameters( parameters_in_out );
}

} } } // namespace bgws::query::env
