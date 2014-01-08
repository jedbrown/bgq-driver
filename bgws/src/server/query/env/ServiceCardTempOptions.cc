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

#include "ServiceCardTempOptions.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>


namespace bgws {
namespace query {
namespace env {


ServiceCardTempOptions::ServiceCardTempOptions()
    :
        AbstractOptions( BGQDB::DBTServicecardtemp::LOCATION_SIZE ),
        _onboard_temp( "temp" )
{
    // Nothing to do.
}


void ServiceCardTempOptions::_addExtraMappings(
        SortInfo::IdToCol& id_to_col_in_out
    )
{
    id_to_col_in_out["temp"] = _QUAL + BGQDB::DBTServicecardtemp::ONBOARDTEMP_COL;
}


void ServiceCardTempOptions::_addOptions(
        boost::program_options::options_description& desc
    )
{
    _onboard_temp.addTo( desc );
}


void ServiceCardTempOptions::_addOptions(
        WhereClause& where_clause_in_out,
        cxxdb::ParameterNames& parameter_names_in_out
    ) const
{
    _onboard_temp.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTServicecardtemp::ONBOARDTEMP_COL );
}


void ServiceCardTempOptions::_bindParameters(
        cxxdb::Parameters& parameters_in_out
    )
{
    _onboard_temp.bindParameters( parameters_in_out );
}


} } } // namespace bgws::query::env
