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


#include "TestcasesOptions.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <boost/program_options.hpp>


namespace bgws {
namespace query {
namespace diagnostics {


TestcasesOptions::TestcasesOptions(
        const capena::http::uri::Query& query
    ) :
        block_id( "blockId", BGQDB::DBTDiagtests::BLOCKID_SIZE ),
        hardware_status( "hardwareStatus", BGQDB::DBTDiagtests::HARDWARESTATUS_SIZE ),
        location( "location", BGQDB::DBTDiagresults::LOCATION_SIZE ),
        testcase( "testcase", BGQDB::DBTDiagtests::TESTCASE_SIZE )
{
    namespace po = boost::program_options;

    po::options_description desc;

    desc.add_options()
            ( "runId", po::value( &run_id_option ) )
        ;

    block_id.addTo( desc );
    hardware_status.addTo( desc );
    location.addTo( desc );
    testcase.addTo( desc );

    po::variables_map vm;
    po::store( po::command_line_parser( query.calcArguments() ).options( desc ).allow_unregistered().run(), vm );
    po::notify( vm );
}


} } } // namespace bgws::query:: diagnostics
