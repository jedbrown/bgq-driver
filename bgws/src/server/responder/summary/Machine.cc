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

/*! \page summaryMachineResource /bg/summary/machine

This resource provides information about the Blue Gene machine.

- \subpage summaryMachineResourceGet

 */

/*! \page summaryMachineResourceGet GET /bg/summary/machine

Get a information about the machine.

\section Authority

The user must be authenticated.

\section summaryMachineResourceGetResponse JSON response format

<pre>
{
  "name" : &quot;<i>string</i>&quot;, // the name provided in the BG configuration file as <i>machine_name</i>.
  "rows" : <i>number</i>,
  "columns" : <i>number</i>,
  "midplanes" : [ [ [ [ &quot;<i>midplane_location</i>&quot;, ... ], ... ], ... ], ... ], ... ], // provides coordinates of each midplane in the torus.
  "ioDrawers" : [ &quot;<i>io_drawer_location</i>&quot;, ... ],
  "systemCpuCount": <i>number</i>, // number of CPUs on the system
  "measurementSystem": "<i>string</i>" // Configured measurement system, either US or SI -- Added in V1R1M1. Optional.
}
</pre>


\section Errors

HTTP status: 403 Forbidden
- notAuthenticated: The user isn't authenticated.

 */


#include "Machine.hpp"

#include "../../Error.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/BGQDBlib.h>

#include <utility/include/Log.h>

#include <boost/foreach.hpp>
#include <boost/throw_exception.hpp>

#include <string>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {
namespace summary {


void Machine::_doGet()
{
    // The user must be authenticated.
    if ( ! _isUserAuthenticated() ) {
        LOG_WARN_MSG( "Cannot get machine summary because the user isn't authenticated." );

        BOOST_THROW_EXCEPTION( Error(
                "Cannot get machine summary because the user isn't authenticated.",
                "getMachineSummary", "notAuthenticated", Error::Data(),
                capena::http::Status::Forbidden
            ) );
    }

    json::ObjectValue obj_val;
    json::Object &obj(obj_val.get());

    const BlueGene::MachineInfo &machine_info(_blue_gene.getMachineInfo());

    const DynamicConfiguration &config(_getDynamicConfiguration());

    obj.set( "name", config.getMachineName() );
    if ( ! config.getMeasurementSystem().empty() ) {
        obj.set( "measurementSystem", config.getMeasurementSystem() );
    }

    obj.set( "rows", machine_info.rows );
    obj.set( "columns", machine_info.columns );
    obj.set( "systemCpuCount", machine_info.midplane_count * BGQDB::Nodes_Per_Midplane ); // Number of CPUs on the system.

    json::Array &midplanes_arr(obj.createArray( "midplanes" ));

    for ( unsigned a_i(0) ; a_i < machine_info.mp_locations.shape()[BGQDB::Dimension::A] ; ++a_i ) {
        json::Array &a_arr(midplanes_arr.addArray());
        for ( unsigned b_i(0) ; b_i < machine_info.mp_locations.shape()[BGQDB::Dimension::B] ; ++b_i ) {
            json::Array &b_arr(a_arr.addArray());
            for ( unsigned c_i(0) ; c_i < machine_info.mp_locations.shape()[BGQDB::Dimension::C] ; ++c_i ) {
                json::Array &c_arr(b_arr.addArray());
                for ( unsigned d_i(0) ; d_i < machine_info.mp_locations.shape()[BGQDB::Dimension::D] ; ++d_i ) {
                    c_arr.add( machine_info.mp_locations[a_i][b_i][c_i][d_i] );
                }
            }
        }
    }

    json::Array &io_drawers_arr(obj.createArray( "ioDrawers" ));

    BOOST_FOREACH( const string& io_drawer_location, machine_info.io_drawer_locations ) {
        io_drawers_arr.add( io_drawer_location );
    }

    capena::server::Response &response(_getResponse());

    response.setContentTypeJson();
    response.headersComplete();
    json::Formatter()( obj_val, response.out() );
}


} } } // namespace bgws::responder::summary
