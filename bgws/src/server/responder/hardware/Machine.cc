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

/*! \page machineResource /bg/machine

This resource provides Blue Gene hardware status.


- \subpage machineResourceGet

 */

/*! \page machineResourceGet GET /bg/machine

Get machine hardware status.

\section Authority

The user must be authenticated.

\section machineResourceGetResponse JSON response format

<pre>
{
  "status": &quot;<i>hardware-status</i>&quot;,
  "serialNumber" : &quot;<i>string</i>&quot;,
  "productId" : &quot;<i>string</i>&quot;,
  "description" : &quot;<i>string</i>&quot;,  // optional
  "hasEthernetGateway" : &quot;<i>T|F</i>&quot;,
  "mtu" : &quot;integer&quot;,
  "clockHz" : &quot;integer&quot;,
  "bringupOptions" : &quot;<i>string</i>&quot;,  // optional

  "racks" : [
        {
          "location": &quot;<i>rack-location</i>&quot;,
          "URI": &quot;\ref hardwareResource&quot;
        }, ...
    ],

  "ioRacks" : [  // optional
        {
          "location": &quot;<i>io-rack-location</i>&quot;,
          "status": &quot;<i>hardware-status</i>&quot;,
          "URI": &quot;\ref hardwareResource&quot;
        }, ...
    ],

  "notAvailable" : {  // optional
      &quot;<i>rack-location</i>&quot; : {  // optional
        "powerModuleCount" : <i>number</i>  // optional
      },
      &quot;<i>midplane-location</i>&quot; : { // optional
        "status" : &quot;<i>hardware-status</i>&quot; // optional
        "serviceCardStatus" : &quot;<i>hardware-status</i>&quot;,  // optional
        "switchCount" : <i>number</i>  // optional
      },
      &quot;<i>nodeboard-location</i>&quot; : {  // optional
        "status" : &quot;<i>hardware-status</i>&quot;  // optional
        "nodeCount" : <i>integer</i>,  // optional
        "DCACount" : <i>integer</i>  // optional
      },
      &quot;<i>io-rack-location</i>&quot; : {  // optional
        "status" : &quot;<i>hardware-status</i>&quot;
      },
      &quot;<i>io-drawer-location</i>&quot; : {  // optional
        "status" : &quot;<i>hardware-status</i>&quot;  // optional
        "ioNodeCount" : <i>integer</i>  // optional
      },
      ...
    }

}
</pre>


\section Errors

HTTP status: 403 Forbidden
- notAuthenticated: User isn't authenticated.

 */


#include "Machine.hpp"

#include "../../Error.hpp"

#include "../../query/Machine.hpp"

#include "capena-http/http/http.hpp"

#include "capena-http/server/exception.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>

#include <boost/throw_exception.hpp>


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {
namespace hardware {


const capena::http::uri::Path Machine::RESOURCE_PATH( capena::http::uri::Path() / "machine" );


void Machine::_doGet()
{
    if ( ! _isUserAuthenticated() ) {

        LOG_WARN_MSG( "Cannot get machine info because the user isn't authenticated." );

        BOOST_THROW_EXCEPTION( Error(
                "Cannot get machine info because the user isn't authenticated.",
                "getMachine", "notAuthenticated", Error::Data(),
                capena::http::Status::Forbidden
            ) );

    }


    query::Machine::Ptr query_ptr(query::Machine::create(
            _blocking_operations_thread_pool
        ));

    query_ptr->start( _getStrand().wrap( boost::bind(
            &Machine::_queryComplete, this,
            capena::server::AbstractResponder::shared_from_this(),
            _1, _2
        ) ) );
}


void Machine::_queryComplete(
        capena::server::ResponderPtr,
        std::exception_ptr exc_ptr,
        json::ObjectValuePtr obj_val_ptr
    )
{
    try {

        if ( exc_ptr != 0 )  std::rethrow_exception( exc_ptr );


        capena::server::Response &response(_getResponse());

        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( *obj_val_ptr, response.out() );

    } catch ( std::exception& e ) {

        _handleError( e );

    }
}


}}} // namespace bgws::responder::hardware
