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

#include "LinkChipOptions.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>


namespace bgws {
namespace query {
namespace env {


LinkChipOptions::LinkChipOptions()
    :
        AbstractOptions( BGQDB::DBTLinkchipenvironment::LOCATION_SIZE ),
        _asic_temp( "temp" )
{
    // Nothing to do.
}


void LinkChipOptions::_addExtraMappings(
        SortInfo::IdToCol& id_to_col_in_out
    )
{
    id_to_col_in_out["temp"] = _QUAL + BGQDB::DBTLinkchipenvironment::ASICTEMP_COL;
}


void LinkChipOptions::_addOptions(
        boost::program_options::options_description& desc
    )
{
    _asic_temp.addTo( desc );
}


void LinkChipOptions::_addOptions(
        WhereClause& where_clause_in_out,
        cxxdb::ParameterNames& parameter_names_in_out
    ) const
{
    _asic_temp.addTo( where_clause_in_out, parameter_names_in_out, _QUAL + BGQDB::DBTLinkchipenvironment::ASICTEMP_COL );
}


void LinkChipOptions::_bindParameters(
        cxxdb::Parameters& parameters_in_out
    )
{
    _asic_temp.bindParameters( parameters_in_out );
}


} } } // namespace bgws::query::env
