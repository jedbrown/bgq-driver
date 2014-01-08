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

#include "OpticalOptions.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>


namespace bgws {
namespace query {
namespace env {


OpticalOptions::OpticalOptions()
    :
        AbstractOptions( BGQDB::DBTOpticalenvironment::LOCATION_SIZE ),
        _channel( "channel" ),
        _power( "power" )
{
    // Nothing to do.
}


void OpticalOptions::_addExtraMappings(
        SortInfo::IdToCol& id_to_col_in_out
    )
{
    id_to_col_in_out["channel"] = _QUAL + BGQDB::DBTOpticalenvironment::CHANNEL_COL;
    id_to_col_in_out["power"] = _QUAL + BGQDB::DBTOpticalenvironment::POWER_COL;
}


void OpticalOptions::_addOptions(
        boost::program_options::options_description& desc
    )
{
    _channel.addTo( desc );
    _power.addTo( desc );
}


void OpticalOptions::_addOptions(
        WhereClause& where_clause_in_out,
        cxxdb::ParameterNames& parameter_names_in_out
    ) const
{
    _channel.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTOpticalenvironment::CHANNEL_COL );
    _power.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTOpticalenvironment::POWER_COL );
}


void OpticalOptions::_bindParameters(
        cxxdb::Parameters& parameters_in_out
    )
{
    _channel.bindParameters( parameters_in_out );
    _power.bindParameters( parameters_in_out );
}

} } } // namespace bgws::query::env
