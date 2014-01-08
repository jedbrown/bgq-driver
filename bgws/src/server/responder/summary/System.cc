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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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

/*! \page systemSummaryResource /bg/summary/system

This resource provides summary information related to the Blue Gene system.

- \subpage systemSummaryResourceGet

 */

/*! \page systemSummaryResourceGet GET /bg/summary/system

Get summary information related to the Blue Gene system.

\section Authority

The user must be authenticated.

\section systemSummaryResourceGetResponse JSON response format

<pre>
{
  "alertCount" : <i>number</i>, // optional
  "jobCount": <i>number</i>, // number of jobs running on the system
  "jobCpuCount": <i>number</i>, // number of CPUs taken up by jobs running on the system
  "mps": { // optional
    &quot;<i>midplane-location</i>&quot;: {
      "status": &quot;<i>status-value</i>&quot;, // optional

      "nodeBoards": { // optional
        &quot;<i>node-board-position</i>&quot;: &quot;<i>status-value</i>&quot;
      }, ...
    }
  },
  "racks": { // optional -- New in V1R2M0
    &quot;<i>rack-location</i>&quot;: {
      "status":  &quot;<i>status-value</i>&quot;, // optional, default status is AVAILABLE

      "drawers": { // optional
        &quot;<i>io-drawer-position</i>&quot;: &quot;<i>status-value</i>&quot; // default status is BLOCK_READY
      }, ...
    }, ...
  },
  "hardwareNotifications": [  // optional
    {
      "location": &quot;<i>location</i>&quot;,
      "status": &quot;<i>status</i>&quot; // optional -- either status or nodeCount are set
      "nodeCount": <i>number</i> // optional -- either status or nodeCount are set
    }, ...
  ],
  "diagnostics": [ // optional
    {
      "location": &quot;<i>location</i>&quot;
    }, ...
  ],
  "serviceActions": [ // optional
    {
      "id": <i>number</i>,
      "location": &quot;<i>location</i>&quot;,
      "action": &quot;<i>action</i>&quot;,
      "prepareUser": &quot;<i>string</i>&quot;,
      "endUser": &quot;<i>string</i>&quot; // optional
    }, ...
  ]
}
</pre>

- <i>status-value</i> is one of
  - AVAILABLE: The hardware is available.
  - BLOCK_BUSY: A block on the hardware is in the process of booting or freeing.
  - BLOCK_READY: A block on the hardware is ready to run a job.
  - JOB_RUNNING: A job is running on the hardware.
  - DIAGNOSTICS: Diagnostics is running on the hardware.
  - SERVICE_ACTION: A service action is in progress on the hardware.
  - NOT_AVAILABLE: The hardware is not availble.


\section Errors

HTTP status: 403 Forbidden
- notAuthenticated: The user isn't authenticated.

 */


#include "System.hpp"

#include "../../BlockingOperationsThreadPool.hpp"
#include "../../Error.hpp"

#include "../../query/SystemSummary.hpp"

#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {
namespace summary {


bool System::matchesUrl(
        const capena::http::uri::Path& requested_resource
    )
{
    static const capena::http::uri::Path URL_PATH(capena::http::uri::Path() / "summary" / "system");
    return (requested_resource == URL_PATH);
}


capena::http::Methods System::_getAllowedMethods() const
{
    return { capena::http::Method::GET };
}


void System::_doGet()
{

    // The user must be authenticated.
    if ( ! _isUserAuthenticated() ) {
        LOG_WARN_MSG( "Cannot get system summary because the user isn't authenticated." );

        BOOST_THROW_EXCEPTION( Error(
                "Cannot get system summary because the user isn't authenticated.",
                "getSystemSummary", "notAuthenticated", Error::Data(),
                capena::http::Status::Forbidden
            ) );
    }


    query::SystemSummary::Ptr query_ptr(
            query::SystemSummary::create(
                    _blocking_operations_thread_pool
                )
        );

    query_ptr->start(
            boost::bind(
                    &System::_queryComplete, this,
                    capena::server::AbstractResponder::shared_from_this(),
                    _1, _2
                )
        );

}


void System::_queryComplete(
        capena::server::ResponderPtr,
        std::exception_ptr exc_ptr,
        json::ObjectValuePtr val_ptr
    )
{
    try {

        if ( exc_ptr != 0 ) {
            std::rethrow_exception( exc_ptr );
        }


        capena::server::Response &response(_getResponse());

        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( *val_ptr, response.out() );

    } catch ( std::exception& e ) {

        _handleError( e );

    }
}


} } } // namespace bgws::responder::summary
