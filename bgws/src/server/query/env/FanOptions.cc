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

#include "FanOptions.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>


namespace bgws {
namespace query {
namespace env {


FanOptions::FanOptions()
    :
        AbstractOptions( BGQDB::DBTFanenvironment::LOCATION_SIZE ),
        _fan_number( "fan" ),
        _rpms( "rpms" )
{
    // Nothing to do.
}


void FanOptions::_addExtraMappings(
        SortInfo::IdToCol& id_to_col_in_out
    )
{
    id_to_col_in_out["fan"] = _QUAL + BGQDB::DBTFanenvironment::FANNUMBER_COL;
    id_to_col_in_out["rpms"] = _QUAL + BGQDB::DBTFanenvironment::RPMS_COL;
}


void FanOptions::_addOptions(
        boost::program_options::options_description& desc
    )
{
    _fan_number.addTo( desc );
    _rpms.addTo( desc );
}


void FanOptions::_addOptions(
        WhereClause& where_clause_in_out,
        cxxdb::ParameterNames& parameter_names_in_out
    ) const
{
    _fan_number.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTFanenvironment::FANNUMBER_COL );
    _rpms.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTFanenvironment::RPMS_COL );
}


void FanOptions::_bindParameters(
        cxxdb::Parameters& parameters_in_out
    )
{
    _fan_number.bindParameters( parameters_in_out );
    _rpms.bindParameters( parameters_in_out );
}


} } } // namespace bgws::query::env
