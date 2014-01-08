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

#include "ServiceCardOptions.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/program_options.hpp>

#include <string>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace query {
namespace env {


ServiceCardOptions::ServiceCardOptions()
    :
        AbstractOptions( BGQDB::DBTServicecardenvironment::LOCATION_SIZE ),
        _voltage_V12P( "v12p" ),
        _voltage_V12R5( "v12r5" ),
        _voltage_V15P( "v15p" ),
        _voltage_V25P( "v25p" ),
        _voltage_V33P( "v33p" ),
        _voltage_V50P( "v50p" )
{
    // Nothing to do.
}


void ServiceCardOptions::_addExtraMappings(
        SortInfo::IdToCol& id_to_col_in_out
    )
{
    id_to_col_in_out["v12p"] = _QUAL + BGQDB::DBTServicecardenvironment::VOLTAGEV12P_COL;
    id_to_col_in_out["v12r5"] = _QUAL + BGQDB::DBTServicecardenvironment::VOLTAGEV12R5_COL;
    id_to_col_in_out["v15p"] = _QUAL + BGQDB::DBTServicecardenvironment::VOLTAGEV15P_COL;
    id_to_col_in_out["v25p"] = _QUAL + BGQDB::DBTServicecardenvironment::VOLTAGEV25P_COL;
    id_to_col_in_out["v33p"] = _QUAL + BGQDB::DBTServicecardenvironment::VOLTAGEV33P_COL;
    id_to_col_in_out["v50p"] = _QUAL + BGQDB::DBTServicecardenvironment::VOLTAGEV50P_COL;
}


void ServiceCardOptions::_addOptions(
        boost::program_options::options_description& desc
    )
{
    _voltage_V12P.addTo( desc );
    _voltage_V12R5.addTo( desc );
    _voltage_V15P.addTo( desc );
    _voltage_V25P.addTo( desc );
    _voltage_V33P.addTo( desc );
    _voltage_V50P.addTo( desc );
}


void ServiceCardOptions::_addOptions(
        WhereClause& where_clause_in_out,
        cxxdb::ParameterNames& parameter_names_in_out
    ) const
{
    _voltage_V12P.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTServicecardenvironment::VOLTAGEV12P_COL );
    _voltage_V12R5.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTServicecardenvironment::VOLTAGEV12R5_COL );
    _voltage_V15P.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTServicecardenvironment::VOLTAGEV15P_COL );
    _voltage_V25P.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTServicecardenvironment::VOLTAGEV25P_COL );
    _voltage_V33P.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTServicecardenvironment::VOLTAGEV33P_COL );
    _voltage_V50P.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTServicecardenvironment::VOLTAGEV50P_COL );
}


void ServiceCardOptions::_bindParameters(
        cxxdb::Parameters& parameters_in_out
    )
{
    _voltage_V12P.bindParameters( parameters_in_out );
    _voltage_V12R5.bindParameters( parameters_in_out );
    _voltage_V15P.bindParameters( parameters_in_out );
    _voltage_V25P.bindParameters( parameters_in_out );
    _voltage_V33P.bindParameters( parameters_in_out );
    _voltage_V50P.bindParameters( parameters_in_out );
}


} } } // namespace bgws::query::env
