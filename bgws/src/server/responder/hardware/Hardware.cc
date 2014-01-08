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

/*! \page hardwareResource /bg/machine/<i>location</i>

This resource provides details for pieces of hardware.

- \subpage hardwareResourceGet

 */


/*! \page hardwareResourceGet GET /bg/machine/<i>location</i>

Get details for the piece of hardware at the location.

\section Authority

The user must be authenticated.

\section hardwareResourceGetResponse JSON response format

<pre>
{
  // Compute rack location (R00)

  "midplanes" : [
        {
          "location" : &quot;<i>midplane-location</i>&quot;,
          "status" : &quot;<i>hardware-status</i>&quot;,
          "URI" : &quot;\ref hardwareResource&quot;,
          "serviceCardStatus" : &quot;<i>hardware-status</i>&quot;,
          "notAvailableSwitchCount" : <i>number</i>,  // optional
          "notAvailableNodeBoardCount" : <i>number</i>,  // optional
          "notAvailableNodeCount" : <i>number</i>,  // optional
          "notAvailableDCACount" : <i>number</i>  // optional
        }, ...
    ],

  "clockCard" : {  // optional
      "location" : &quot;<i>midplane-location</i>&quot;,
      "status" : &quot;<i>hardware-status</i>&quot;,
      "serialNumber" : &quot;<i>string</i>&quot;,  // optional
      "productId" : &quot;<i>string</i>&quot;,
      "clockHz" : &quot;<i>string</i>&quot;,
      "source" : &quot;<i>string</i>&quot;, // optional
      "supplies" : [ &quot;<i>string</i>&quot;, ... ] // optional
    },

  "ioDrawers" : [ // optional
        {
          "location" : &quot;<i>io-drawer-location</i>&quot;,
          "status" : &quot;<i>hardware-status</i>&quot;,
          "URI" : &quot;\ref hardwareResource&quot;,
          "notAvailableIoNodeCount" : <i>number</i>  // optional
        }, ...
    ],

  "powerModules" : [
        {
          "location" : &quot;<i>power-module-location</i>&quot;,
          "status" : &quot;<i>hardware-status</i>&quot;,
          "serialNumber" : &quot;<i>string</i>&quot;, // optional
          "productId" : &quot;<i>string</i>&quot; // optional
        }, ...
    ],


  // Midplane location (R00-M0)

  "status" : &quot;<i>hardware-status</i>&quot;,
  "serialNumber" : &quot;<i>string</i>&quot;, // optional
  "productId" : &quot;<i>string</i>&quot;,
  "torusCoord" : [ <i>integer</i>, <i>integer</i>, <i>integer</i>, <i>integer</i> ],
  "clockSource" : &quot;<i>string</i>&quot;, // optional

  "nodeBoards" : [
        {
          "location" : &quot;<i>node-board-location</i>&quot;,
          "status" : &quot;<i>hardware-status</i>&quot;,
          "URI" : &quot;\ref hardwareResource&quot;,
          "notAvailableNodeCount" : <i>number</i>,  // optional
          "notAvailableDCACount" : <i>number</i>  // optional
        }, ...
    ],

  "serviceCard" : {
      "location" : &quot;<i>service-cardlocation</i>&quot;,
      "status" : &quot;<i>hardware-status</i>&quot;,
      "serialNumber" : &quot;<i>string</i>&quot;, // optional
      "productId" : &quot;<i>string</i>&quot;
    },

  "switches" : [
      {
        "id" : &quot;<i>switch-id</i>&quot;,
        "status" : &quot;<i>hardware-status</i>&quot;
      }
    ]


  // Node board location (R00-M0-N00)

  "status" : &quot;<i>hardware-status</i>&quot;,
  "serialNumber" : &quot;<i>string</i>&quot;, // optional
  "productId" : &quot;<i>string</i>&quot;,

  "linkModules" : [
      {
        "location" : &quot;<i>link-module-location</i>&quot;,
        "status" : &quot;<i>hardware-status</i>&quot;
        "serialNumber" : &quot;<i>string</i>&quot;, // optional
        "productId" : &quot;<i>string</i>&quot; // optional
      }, ...
    ],

  "DCAs" : [
      {
        "location" : &quot;<i>link-module-location</i>&quot;,
        "status" : &quot;<i>hardware-status</i>&quot;
        "serialNumber" : &quot;<i>string</i>&quot;, // optional
        "productId" : &quot;<i>string</i>&quot;
      }, ...
    ],

  "computeCards": [
      {
        "location" : &quot;<i>compute-card-location</i>&quot;,
        "status" : &quot;<i>hardware-status</i>&quot;
        "serialNumber" : &quot;<i>string</i>&quot;, // optional
        "productId" : &quot;<i>string</i>&quot;, // optional
        "ipAddress" : &quot;<i>string</i>&quot;, // optional
        "macAddress" : &quot;<i>string</i>&quot;, // optional
        "memorySize" : <i>integer</i>,
        "voltage" : <i>number</i>,
        "bitsteering" : <i>number</i>, // optional
        "ioLinkNode" : &quot;<i>string</i>&quot;, //optional
        "ioLinkStatus" : &quot;<i>hardware-status</i>&quot;, //optional
        "ioLinkNodeStatus" : &quot;<i>hardware-status</i>&quot; // optional
      }, ...
    ]


  // I/O rack location (Q00)

  "status" : &quot;<i>hardware-status</i>&quot;
  "serialNumber" : &quot;<i>string</i>&quot;, // optional
  "productId" : &quot;<i>string</i>&quot;,

  "clockCards" : [
      {
        "location" : &quot;<i>midplane-location</i>&quot;,
        "status" : &quot;<i>hardware-status</i>&quot;,
        "serialNumber" : &quot;<i>string</i>&quot;, // optional
        "productId" : &quot;<i>string</i>&quot;,
        "clockHz" : &quot;<i>string</i>&quot;
      }, ...
    ],

  "ioDrawers" : [ // optional
        {
          "location" : &quot;<i>io-drawer-location</i>&quot;,
          "status" : &quot;<i>hardware-status</i>&quot;,
          "URI" : &quot;\ref hardwareResource&quot;
        }, ...
    ],

  "powerModules" : [
        {
          "location" : &quot;<i>power-module-location</i>&quot;,
          "status" : &quot;<i>hardware-status</i>&quot;,
          "serialNumber" : &quot;<i>string</i>&quot;, // optional
          "productId" : &quot;<i>string</i>&quot; // optional
        }, ...
    ]


  // I/O drawer location (Q00-I0 or R00-IC)

  "status" : &quot;<i>hardware-status</i>&quot;,
  "serialNumber" : &quot;<i>string</i>&quot;, // optional
  "productId" : &quot;<i>string</i>&quot;,
  "clockSource" : &quot;<i>string</i>&quot;, // optional

  "linkModules" : [
        {
          "location" : &quot;<i>link-module-location</i>&quot;,
          "status" : &quot;<i>hardware-status</i>&quot;,
          "serialNumber" : &quot;<i>string</i>&quot;, // optional
          "productId" : &quot;<i>string</i>&quot; // optional
        }, ...
    ],

  "computeCards" : [
        {
          "location" : &quot;<i>compute-card-location</i>&quot;,
          "status" : &quot;<i>hardware-status</i>&quot;,
          "serialNumber" : &quot;<i>string</i>&quot;, // optional
          "productId" : &quot;<i>string</i>&quot;, // optional
          "ipAddress" : &quot;<i>string</i>&quot;, // optional
          "memorySize" : <i>integer</i>,
          "voltage" : <i>number</i>, // optional
          "bitSteering" : <i>integer</i>, // optional
          "cnLink1Location" : &quot;<i>compute-card-location</i>&quot;, // optional
          "cnLink1Status" : &quot;<i>hardware-status</i>&quot;, // optional
          "cnLink2Location" : &quot;<i>compute-card-location</i>&quot;, // optional
          "cnLink2Status" : &quot;<i>hardware-status</i>&quot; // optional
        }, ...
    ]

}
</pre>


\section Errors

HTTP status: 403 Forbidden
- notAuthenticated: User isn't authenticated.

 */


#include "Hardware.hpp"

#include "Machine.hpp"

#include "../../BlockingOperationsThreadPool.hpp"
#include "../../dbConnectionPool.hpp"
#include "../../Error.hpp"

#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <bgq_util/include/Location.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <boost/format.hpp>
#include <boost/optional.hpp>
#include <boost/throw_exception.hpp>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {
namespace hardware {


namespace statics {

boost::optional<std::string> calcClockSource( const std::string& location, cxxdb::Connection& conn )
{
    static const string SQL( "SELECT " + BGQDB::DBTClockcable::FROMLOCATION_COL + " FROM " + BGQDB::DBTClockcable().getTableName() + " WHERE " + BGQDB::DBTClockcable::TOLOCATION_COL + "=?" );

    cxxdb::QueryStatementPtr stmt_ptr(conn.prepareQuery( SQL, cxxdb::ParameterNames{ "location" } ));
    stmt_ptr->parameters()["location"].set( location );
    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

    if ( rs_ptr->fetch() ) {
        return boost::optional<std::string>(rs_ptr->columns()[BGQDB::DBTClockcable::FROMLOCATION_COL].getString());
    }

    return boost::optional<std::string>();
}

} // namespace statics



bool Hardware::matchesUrl(
        const capena::http::uri::Path& requested_resource
    )
{
    // Check is /bg/machine/<location> (parent is machine)

    if ( requested_resource.empty() ) {
        return false;
    }

    if ( requested_resource.back().empty() ) {
        return false;
    }

    return Machine::matchesUrl( requested_resource.calcParent() ); // parent is machine.
}


capena::http::Methods Hardware::_getAllowedMethods() const
{
    return { capena::http::Method::GET };
}


void Hardware::_doGet()
{

    if ( ! _isUserAuthenticated() ) {

        LOG_WARN_MSG( "Cannot get hardware info because the user isn't authenticated." );

        BOOST_THROW_EXCEPTION( Error(
                "Cannot get hardware info because the user isn't authenticated.",
                "getHardware", "notAuthenticated", Error::Data(),
                capena::http::Status::Forbidden
            ) );

    }


    const string &location_str(_getRequestedResourcePath().back());

    _blocking_operations_thread_pool.post( boost::bind(
            &Hardware::_startQuery, this,
            capena::server::AbstractResponder::shared_from_this(),
            location_str
        ) );
}


void Hardware::_startQuery(
        capena::server::ResponderPtr,
        const std::string& location_str
    )
{
    try {
        bgq::util::Location location( location_str );

        json::ValuePtr val_ptr;

        if ( location.getType() == bgq::util::Location::ComputeRack ) {
            val_ptr = _getComputeRack( location_str );
        }

        if ( location.getType() == bgq::util::Location::Midplane ) {
            val_ptr = _getMidplane( location_str );
        }

        if ( location.getType() == bgq::util::Location::NodeBoard ) {
            val_ptr = _getNodeBoard( location_str );
        }

        if ( location.getType() == bgq::util::Location::IoRack ) {
            val_ptr = _getIoRack( location_str );
        }

        if ( (location.getType() == bgq::util::Location::IoBoardOnComputeRack) ||
             (location.getType() == bgq::util::Location::IoBoardOnIoRack) ) {
            val_ptr = _getIoDrawer( location_str );
        }


        _getStrand().post( boost::bind(
                &Hardware::_queryComplete, this,
                capena::server::AbstractResponder::shared_from_this(),
                location_str, val_ptr
            ) );

    } catch ( bgq::util::LocationError& e ) {

        try {

            Error::Data data;
            data["location"] = location_str;

            BOOST_THROW_EXCEPTION( Error(
                    boost::str( boost::format( "Could not get details for hardware '%1%' because that location is not valid." ) % location_str ),
                    "getHardwareDetails", "invalidLocation", data,
                    capena::http::Status::BadRequest
                ) );

        } catch ( std::exception& e ) {

            _inCatchPostCurrentExceptionToHandlerFn();

        }

    } catch ( std::exception& e ) {

        _inCatchPostCurrentExceptionToHandlerFn();

    }
}


void Hardware::_queryComplete(
        capena::server::ResponderPtr,
        const std::string& location_str,
        json::ValuePtr val_ptr
    )
{
    try {
        if ( val_ptr ) {

            capena::server::Response &response(_getResponse());

            response.setContentTypeJson();
            response.headersComplete();

            json::Formatter()( *val_ptr, response.out() );

            return;
        }

        Error::Data data;
        data["location"] = location_str;

        BOOST_THROW_EXCEPTION( Error(
                boost::str( boost::format( "Could not get details for hardware '%1%' because that type of hardware is not supported." ) % location_str ),
                "getHardwareDetails", "invalidType", data,
                capena::http::Status::BadRequest
            ) );
    } catch ( std::exception& e ) {

        _handleError( e );

    }

}


json::ValuePtr Hardware::_getComputeRack( const std::string& location )
{
    auto conn_ptr(dbConnectionPool::getConnection());


    json::ObjectValuePtr val_ptr(json::Object::create());
    json::Object &obj(val_ptr->get());

    bool any_midplanes_found(false);

    json::Array &midplanes_arr(obj.createArray( "midplanes" ));

    {
        static const string SQL(

 "SELECT mp.location, mp.status,"
       " sc.status AS scStatus,"
       " us.c AS unavailableSwitchCount"
  " FROM bgqMidplane AS mp"
       " LEFT OUTER JOIN"
       " bgqServiceCard AS sc"
       " ON mp.location = sc.midplanePos"
       " LEFT OUTER JOIN"
       " ( SELECT midplanePos, COUNT(*) AS c"
           " FROM bgqSwitch"
           " WHERE status <> 'A'"
           " GROUP BY midplanePos"
       " ) AS us"
       " ON mp.location = us.midplanePos"
  " WHERE LEFT(mp.location,3)=?"
 " ORDER BY mp.location"

             );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "rackLocation" } ));
        stmt_ptr->parameters()["rackLocation"].set( location );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        while ( rs_ptr->fetch() ) {
            const cxxdb::Columns &cols(rs_ptr->columns());

            any_midplanes_found = true;

            json::Object& mp_obj(midplanes_arr.addObject());

            string mp_loc(cols["location"].getString());

            mp_obj.set( "location", mp_loc );
            mp_obj.set( "URI", string() + "/bg/machine/" + mp_loc );

            mp_obj.set( "status", cols["status"].getString());
            mp_obj.set( "serviceCardStatus", cols["scStatus"].getString());

            if ( cols["unavailableSwitchCount"] ) {
                mp_obj.set( "notAvailableSwitchCount", cols["unavailableSwitchCount"].as<int64_t>() );
            }
        }
    }

    if ( ! any_midplanes_found ) {
        Error::Data data;
        data["location"] = location;

        BOOST_THROW_EXCEPTION( Error(
                boost::str( boost::format( "Could not get details for rack '%1%' because it does not exist." ) % location ),
                "getRackDetails", "notFound", data,
                capena::http::Status::NotFound
            ) );
    }


    boost::optional<json::Object&> cc_obj_opt;
    string cc_location;

    {
        static const string SQL( "SELECT * FROM bgqClockCard WHERE LEFT(location,3)=?" );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "rackLocation" } ));
        stmt_ptr->parameters()["rackLocation"].set( location );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        if ( rs_ptr->fetch() ) {
            const cxxdb::Columns &cols(rs_ptr->columns());

            cc_obj_opt = obj.createObject( "clockCard" );

            cc_location = cols["location"].getString();

            cc_obj_opt->set( "location", cc_location );

            cc_obj_opt->set( "status", cols["status"].getString() );

            if ( cols["serialNumber"] )  cc_obj_opt->set( "serialNumber", cols["serialNumber"].getString() );
            if ( cols["productId"] )  cc_obj_opt->set( "productId", cols["productId"].getString() );
            if ( cols["clockHz"] )  cc_obj_opt->set( "clockHz", cols["clockHz"].as<int64_t>() );
        }
    }

    if ( cc_obj_opt ) {
        boost::optional<std::string> clock_source_opt(statics::calcClockSource( cc_location, *conn_ptr ));

        if ( clock_source_opt ) {
            cc_obj_opt->set( "source", *clock_source_opt );
        }
    }

    if ( cc_obj_opt ) {
        static const string SQL( "SELECT toLocation FROM bgqClockCable WHERE fromLocation=? ORDER BY toLocation" );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "location" } ));
        stmt_ptr->parameters()["location"].set( cc_location );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        boost::optional<json::Array&> supplies_arr_opt;

        while ( rs_ptr->fetch() ) {
            if ( ! supplies_arr_opt )  supplies_arr_opt = cc_obj_opt->createArray( "supplies" );
            supplies_arr_opt->add( rs_ptr->columns()["toLocation"].getString() );
        }
    }


    json::Array *iods_arr_p(_addIoDrawers( location, conn_ptr, obj ));
    _addPowerModules( location, conn_ptr, obj );


    { // Not available node boards on the rack

        static const string SQL(

 "SELECT midplanePos, COUNT(*) AS c"
  " FROM bgqNodeCard"
  " WHERE LEFT(midplanePos,3)=? AND status <> 'A'"
  " GROUP BY midplanePos"

            );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "rackLocation" } ));
        stmt_ptr->parameters()["rackLocation"].set( location );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        while ( rs_ptr->fetch() ) {
            string mp_loc(rs_ptr->columns()["midplanePos"].getString());

            if ( mp_loc.substr( 5, 1 ) == "0" ) {
                midplanes_arr[0]->getObject().set( "notAvailableNodeBoardCount", rs_ptr->columns()["c"].as<int64_t>() );
            } else {
                midplanes_arr[1]->getObject().set( "notAvailableNodeBoardCount", rs_ptr->columns()["c"].as<int64_t>() );
            }
        }
    }

    { // Not available nodes on the rack midplanes

        static const string SQL(

  "SELECT midplanePos, COUNT(*) AS c FROM bgqComputeNode"
   " WHERE LEFT(midplanePos,3)=? AND status <> 'A'"
   " GROUP BY midplanePos"

            );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "rackLocation" } ));
        stmt_ptr->parameters()["rackLocation"].set( location );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        while ( rs_ptr->fetch() ) {
            string mp_loc(rs_ptr->columns()["midplanePos"].getString());

            if ( mp_loc.substr( 5, 1 ) == "0" ) {
                midplanes_arr[0]->getObject().set( "notAvailableNodeCount", rs_ptr->columns()["c"].as<int64_t>() );
            } else {
                midplanes_arr[1]->getObject().set( "notAvailableNodeCount", rs_ptr->columns()["c"].as<int64_t>() );
            }
        }
    }

    { // Not available DCAs on the rack midplanes

        static const string SQL(

  "SELECT midplanePos, COUNT(*) AS c FROM bgqNodeCardDCA"
   " WHERE LEFT(midplanePos,3)=? AND status <> 'A'"
   " GROUP BY midplanePos"

            );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "rackLocation" } ));
        stmt_ptr->parameters()["rackLocation"].set( location );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        while ( rs_ptr->fetch() ) {
            string mp_loc(rs_ptr->columns()["midplanePos"].getString());

            if ( mp_loc.substr( 5, 1 ) == "0" ) {
                midplanes_arr[0]->getObject().set( "notAvailableDCACount", rs_ptr->columns()["c"].as<int64_t>() );
            } else {
                midplanes_arr[1]->getObject().set( "notAvailableDCACount", rs_ptr->columns()["c"].as<int64_t>() );
            }
        }
    }

    if ( iods_arr_p ) { // Not available I/O nodes

        static const string SQL(

 "SELECT ioPos AS location, COUNT(*) AS c FROM bgqIoNode"
  " WHERE LEFT(ioPos,3)=? AND status <> 'A'"
  " GROUP BY ioPos"

            );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "rackLocation" } ));
        stmt_ptr->parameters()["rackLocation"].set( location );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        while ( rs_ptr->fetch() ) {
            const string io_drawer_pos(rs_ptr->columns()["location"].getString());

            for ( json::Array::iterator i(iods_arr_p->begin()) ; i != iods_arr_p->end() ; ++i ) {
                json::Object &io_drawer_obj(i->get()->getObject());
                if ( io_drawer_obj["location"]->getString() != io_drawer_pos ) {
                    continue;
                }

                io_drawer_obj.set( "notAvailableIoNodeCount", rs_ptr->columns()["c"].as<int64_t>() );
                break;
            }
        }
    }

    return val_ptr;
}


json::ValuePtr Hardware::_getMidplane( const std::string& location )
{
    auto conn_ptr(dbConnectionPool::getConnection());

    json::ObjectValuePtr val_ptr(json::Object::create());
    json::Object &obj(val_ptr->get());

    {
        static const string SQL( "SELECT * FROM bgqMidplane WHERE location=?" );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "location" } ));
        stmt_ptr->parameters()["location"].set( location );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        if ( ! rs_ptr->fetch() ) {
            Error::Data data;
            data["location"] = location;

            BOOST_THROW_EXCEPTION( Error(
                    boost::str( boost::format( "Could not get details for midplane '%1%' because it does not exist." ) % location ),
                    "getMidplaneDetails", "notFound", data,
                    capena::http::Status::NotFound
                ) );
        }

        if ( rs_ptr->columns()["serialNumber"] )  obj.set( "serialNumber", rs_ptr->columns()["serialNumber"].getString() );
        obj.set( "productId", rs_ptr->columns()["productId"].getString() );
        obj.set( "status", rs_ptr->columns()["status"].getString() );

        json::Array &coord_arr(obj.createArray( "torusCoord" ));
        coord_arr.add( rs_ptr->columns()["torusA"].as<uint64_t>() );
        coord_arr.add( rs_ptr->columns()["torusB"].as<uint64_t>() );
        coord_arr.add( rs_ptr->columns()["torusC"].as<uint64_t>() );
        coord_arr.add( rs_ptr->columns()["torusD"].as<uint64_t>() );
    }


    // Include the clock source.
    boost::optional<std::string> clock_source_opt(statics::calcClockSource( location, *conn_ptr ) );
    if ( clock_source_opt ) {
        obj.set( "clockSource", *clock_source_opt );
    }


    { // Node Boards
        static const string SQL(

"SELECT nc.location, nc.status, un.c AS unavailableNodeCount, ud.c AS unavailableDCACount"
 " FROM bgqNodeCard AS nc"
      " LEFT OUTER JOIN"
      " ( SELECT midplanePos || '-' || nodeCardPos AS location, COUNT(*) AS c"
          " FROM bgqComputeNode"
          " WHERE midplanePos=? AND status <> 'A'"
          " GROUP BY midplanePos, nodeCardPos"
      " ) AS un"
      " ON nc.location = un.location"
      " LEFT OUTER JOIN"
      " ( SELECT midplanePos || '-' || nodeCardPos AS location, COUNT(*) AS c"
           " FROM bgqNodeCardDCA"
           " WHERE midplanePos=? AND status <> 'A'"
           " GROUP BY midplanePos, nodeCardPos"
      " ) AS ud"
      " ON nc.location = ud.location"
 " WHERE nc.midplanePos=?"
 " ORDER BY location"

            );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "midplanePos", "midplanePos", "midplanePos" } ));
        stmt_ptr->parameters()["midplanePos"].set( location );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());


        json::Array &nbs_arr(obj.createArray( "nodeBoards" ));

        while ( rs_ptr->fetch() ) {
            json::Object &nb_obj(nbs_arr.addObject());

            string nb_loc(rs_ptr->columns()["location"].getString());

            nb_obj.set( "location", nb_loc );
            nb_obj.set( "status", rs_ptr->columns()["status"].getString() );

            nb_obj.set( "URI", string() + "/bg/machine/" + nb_loc );

            if ( rs_ptr->columns()["unavailableNodeCount"] ) {
                nb_obj.set( "notAvailableNodeCount", rs_ptr->columns()["unavailableNodeCount"].as<int64_t>() );
            }

            if ( rs_ptr->columns()["unavailableDCACount"] ) {
                nb_obj.set( "notAvailableDCACount", rs_ptr->columns()["unavailableDCACount"].as<int64_t>() );
            }
        }
    }

    {
        static const string SQL( "SELECT * FROM bgqServiceCard WHERE midplanePos=?" );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "midplanePos" } ));
        stmt_ptr->parameters()["midplanePos"].set( location );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        if ( rs_ptr->fetch() ) {
            json::Object &sc_obj(obj.createObject( "serviceCard" ));

            string sc_loc(rs_ptr->columns()["location"].getString());

            sc_obj.set( "location", sc_loc );
            sc_obj.set( "status", rs_ptr->columns()["status"].getString() );
            if ( rs_ptr->columns()["serialnumber"] ) {
                sc_obj.set( "serialNumber", rs_ptr->columns()["serialnumber"].getString() );
            }
            sc_obj.set( "productId", rs_ptr->columns()["productId"].getString() );
        }
    }

    {
        static const string SQL( "SELECT switchId, status FROM bgqSwitch WHERE midplanePos=? ORDER BY switchId" );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "midplanePos" } ));
        stmt_ptr->parameters()["midplanePos"].set( location );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        json::Array &switches_arr(obj.createArray( "switches" ));

        while ( rs_ptr->fetch() ) {
            json::Object &switch_obj(switches_arr.addObject());

            switch_obj.set( "id", rs_ptr->columns()["switchId"].getString() );
            switch_obj.set( "status", rs_ptr->columns()["status"].getString() );
        }
    }

    return val_ptr;
}


json::ValuePtr Hardware::_getNodeBoard( const std::string& location )
{
    string mp_pos(location.substr( 0, 6 ));
    string nb_pos(location.substr( 7, 3 ));

    auto conn_ptr(dbConnectionPool::getConnection());

    json::ObjectValuePtr val_ptr(json::Object::create());
    json::Object &obj(val_ptr->get());

    {
        static const string SQL( "SELECT * FROM bgqNodeCard WHERE midplanePos=? AND position=?" );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "midplanePos", "position" } ));
        stmt_ptr->parameters()["midplanePos"].set( mp_pos );
        stmt_ptr->parameters()["position"].set( nb_pos );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        if ( ! rs_ptr->fetch() ) {
            Error::Data data;
            data["location"] = location;

            BOOST_THROW_EXCEPTION( Error(
                    boost::str( boost::format( "Could not get details for node board '%1%' because it does not exist." ) % location ),
                    "getNodeBoardDetails", "notFound", data,
                    capena::http::Status::NotFound
                ) );
        }

        if ( rs_ptr->columns()["serialNumber"] )  obj.set( "serialNumber", rs_ptr->columns()["serialNumber"].getString() );
        obj.set( "productId", rs_ptr->columns()["productId"].getString() );
        obj.set( "status", rs_ptr->columns()["status"].getString() );
    }

    {
        static const string SQL( "SELECT * FROM bgqLinkChip WHERE midplanePos=? AND nodeCardPos=? ORDER BY location" );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "midplanePos", "nodeCardPos" } ));
        stmt_ptr->parameters()["midplanePos"].set( mp_pos );
        stmt_ptr->parameters()["nodeCardPos"].set( nb_pos );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        json::Array &lms_arr(obj.createArray( "linkModules" ));

        while ( rs_ptr->fetch() ) {
            json::Object &lm_obj(lms_arr.addObject());

            lm_obj.set( "location", rs_ptr->columns()["location"].getString() );
            lm_obj.set( "status", rs_ptr->columns()["status"].getString() );
            if ( rs_ptr->columns()["serialNumber"] )  lm_obj.set( "serialNumber", rs_ptr->columns()["serialNumber"].getString() );
            if ( rs_ptr->columns()["productId"] )  lm_obj.set( "productId", rs_ptr->columns()["productId"].getString() );
        }
    }

    {
        static const string SQL( "SELECT location, status, serialNumber, productId FROM bgqNodeCardDCA WHERE midplanePos=? AND nodeCardPos=? ORDER BY location" );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "midplanePos", "nodeCardPos" } ));
        stmt_ptr->parameters()["midplanePos"].set( mp_pos );
        stmt_ptr->parameters()["nodeCardPos"].set( nb_pos );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        json::Array &dcas_arr(obj.createArray( "DCAs" ));

        while ( rs_ptr->fetch() ) {
            json::Object &dca_obj(dcas_arr.addObject());

            dca_obj.set( "location", rs_ptr->columns()["location"].getString() );
            dca_obj.set( "status", rs_ptr->columns()["status"].getString() );

            if ( rs_ptr->columns()["serialNumber"] ) {
                dca_obj.set( "serialNumber", rs_ptr->columns()["serialNumber"].getString() );
            }

            dca_obj.set( "productId", rs_ptr->columns()["productId"].getString() );
        }
    }

    {
        static const string SQL(

 "SELECT cn.*,"
       " l.ion AS ioLinkNode, l.status AS ioLinkStatus, l.ionStatus AS ioLinkNodeStatus"
  " FROM bgqComputeNode AS cn"
       " LEFT OUTER JOIN"
       " bgqCnIoLink AS l"
       " ON cn.location = l.source"
  " WHERE midplanePos=? AND nodeCardPos=?"
 " ORDER BY location"

            );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "midplanePos", "nodeCardPos" } ));
        stmt_ptr->parameters()["midplanePos"].set( mp_pos );
        stmt_ptr->parameters()["nodeCardPos"].set( nb_pos );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        json::Array &ccs_arr(obj.createArray( "computeCards" ));

        while ( rs_ptr->fetch() ) {
            json::Object &cc_obj(ccs_arr.addObject());


            const auto &cols(rs_ptr->columns());

            cc_obj.set( "location", cols["location"].getString() );
            cc_obj.set( "status", cols["status"].getString() );
            if ( cols["serialNumber"] )  cc_obj.set( "serialNumber", cols["serialNumber"].getString() );
            if ( cols["productId"] )  cc_obj.set( "productId", cols["productId"].getString() );
            if ( cols["ipaddress"] )  cc_obj.set( "ipAddress", cols["ipaddress"].getString() );
            if ( cols["macaddress"] )  cc_obj.set( "macAddress", cols["macaddress"].getString() );
            cc_obj.set( "memorySize", cols["memorySize"].as<int64_t>() );
            if ( cols["voltage"] )  cc_obj.set( "voltage", cols["voltage"].as<double>() );
            if ( cols["bitsteering"].as<int64_t>() != -1 )  cc_obj.set( "bitsteering", cols["bitsteering"].as<int64_t>() );

            if ( cols["ioLinkNode"] )  cc_obj.set( "ioLinkNode", cols["ioLinkNode"].getString() );
            if ( cols["ioLinkStatus"] )  cc_obj.set( "ioLinkStatus", cols["ioLinkStatus"].getString() );
            if ( cols["ioLinkNodeStatus"] )  cc_obj.set( "ioLinkNodeStatus", cols["ioLinkNodeStatus"].getString() );

        }
    }

    return val_ptr;
}


json::ValuePtr Hardware::_getIoRack( const std::string& location )
{
    auto conn_ptr(dbConnectionPool::getConnection());

    json::ObjectValuePtr val_ptr(json::Object::create());
    json::Object &obj(val_ptr->get());

    {
        static const string SQL( "SELECT * FROM bgqIoRack WHERE location=?" );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "location" } ));
        stmt_ptr->parameters()["location"].set( location );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        if ( ! rs_ptr->fetch() ) {
            Error::Data data;
            data["location"] = location;

            BOOST_THROW_EXCEPTION( Error(
                    boost::str( boost::format( "Could not get details for I/O rack '%1%' because it does not exist." ) % location ),
                    "getIoRackDetails", "notFound", data,
                    capena::http::Status::NotFound
                ) );
        }

        if ( rs_ptr->columns()["serialNumber"] )  obj.set( "serialNumber", rs_ptr->columns()["serialNumber"].getString() );
        obj.set( "productId", rs_ptr->columns()["productId"].getString() );
        obj.set( "status", rs_ptr->columns()["status"].getString() );
    }

    {
        static const string SQL( "SELECT * FROM bgqClockCard WHERE LEFT(location,3)=? ORDER BY location" );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "rackLocation" } ));
        stmt_ptr->parameters()["rackLocation"].set( location );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        json::Array &ccs_arr(obj.createArray( "clockCards" ));

        while ( rs_ptr->fetch() ) {
            json::Object &cc_obj(ccs_arr.addObject());

            string cc_loc(rs_ptr->columns()["location"].getString());

            cc_obj.set( "location", cc_loc );

            cc_obj.set( "status", rs_ptr->columns()["status"].getString() );

            if ( rs_ptr->columns()["serialNumber"] )  cc_obj.set( "serialNumber", rs_ptr->columns()["serialNumber"].getString() );
            if ( rs_ptr->columns()["productId"] )  cc_obj.set( "productId", rs_ptr->columns()["productId"].getString() );
            if ( rs_ptr->columns()["clockHz"] )  cc_obj.set( "clockHz", rs_ptr->columns()["clockHz"].as<int64_t>() );
        }
    }

    _addIoDrawers( location, conn_ptr, obj );
    _addPowerModules( location, conn_ptr, obj );

    return val_ptr;
}


json::ValuePtr Hardware::_getIoDrawer( const std::string& location )
{
    auto conn_ptr(dbConnectionPool::getConnection());

    json::ObjectValuePtr val_ptr(json::Object::create());
    json::Object &obj(val_ptr->get());

    {
        static const string SQL( "SELECT * FROM bgqIoDrawer WHERE location=?" );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "location" } ));
        stmt_ptr->parameters()["location"].set( location );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        if ( ! rs_ptr->fetch() ) {
            Error::Data data;
            data["location"] = location;

            BOOST_THROW_EXCEPTION( Error(
                    boost::str( boost::format( "Could not get details for I/O drawer '%1%' because it does not exist." ) % location ),
                    "getIoDrawerDetails", "notFound", data,
                    capena::http::Status::NotFound
                ) );
        }

        if ( rs_ptr->columns()["serialNumber"] )  obj.set( "serialNumber", rs_ptr->columns()["serialNumber"].getString() );
        obj.set( "productId", rs_ptr->columns()["productId"].getString() );
        obj.set( "status", rs_ptr->columns()["status"].getString() );
    }


    // Include the clock source.
    boost::optional<std::string> clock_source_opt(statics::calcClockSource( location, *conn_ptr ) );
    if ( clock_source_opt ) {
        obj.set( "clockSource", *clock_source_opt );
    }


    {
        static const string SQL( "SELECT * FROM bgqIoLinkChip WHERE ioPos=? ORDER BY location" );

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "iodLocation" } ));
        stmt_ptr->parameters()["iodLocation"].set( location );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        json::Array &lms_arr(obj.createArray( "linkModules" ));

        while ( rs_ptr->fetch() ) {
            json::Object &lm_obj(lms_arr.addObject());

            lm_obj.set( "location", rs_ptr->columns()["location"].getString() );
            lm_obj.set( "status", rs_ptr->columns()["status"].getString() );

            if ( rs_ptr->columns()["serialNumber"] )  lm_obj.set( "serialNumber", rs_ptr->columns()["serialNumber"].getString() );
            if ( rs_ptr->columns()["productId"] )  lm_obj.set( "productId", rs_ptr->columns()["productId"].getString() );
        }
    }

    {
        static const string SQL(

 "WITH si AS ("

" SELECT source, ion, status, DENSE_RANK() OVER ( PARTITION BY ion ORDER BY source ) AS num"
  " FROM bgqCnIoLink"
  " WHERE LEFT(ion,6) = CAST( ? AS CHAR(6))"

" )"

" SELECT n.location, n.serialNumber, n.productId, n.status, n.memorySize, n.voltage, n.bitSteering,"
       " c.itemValue AS ipAddress,"
       " si1.source AS cnLink1Location, si1.status AS cnLink1Status,"
       " si2.source AS cnLink2Location, si2.status AS cnLink2Status"
  " FROM bgqIoNode AS n"
       " LEFT OUTER JOIN"
       " bgqNetConfig AS c"
       " ON n.location = c.location AND c.interface = 'external1' AND c.itemName = 'ipv4address'"
       " LEFT OUTER JOIN"
       " si AS si1"
       " ON n.location = si1.ion AND si1.num = 1"
       " LEFT OUTER JOIN"
       " si AS si2"
       " ON n.location = si2.ion AND si2.num = 2"
  " WHERE n.ioPos=?"
  " ORDER BY n.location"

            );


        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "iodLocation", "iodLocation" } ));
        stmt_ptr->parameters()["iodLocation"].set( location );
        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        json::Array &ccs_arr(obj.createArray( "computeCards" ));

        while ( rs_ptr->fetch() ) {
            json::Object &cc_obj(ccs_arr.addObject());

            const auto &cols(rs_ptr->columns());

            cc_obj.set( "location", cols["location"].getString() );
            cc_obj.set( "status", cols["status"].getString() );
            if ( cols["serialNumber"] )  cc_obj.set( "serialNumber", cols["serialNumber"].getString() );
            if ( cols["productId"] )  cc_obj.set( "productId", cols["productId"].getString() );
            if ( cols["ipaddress"] )  cc_obj.set( "ipAddress", cols["ipaddress"].getString() );
            cc_obj.set( "memorySize", cols["memorySize"].as<int64_t>() );
            if ( cols["voltage"] )  cc_obj.set( "voltage", cols["voltage"].as<double>() );
            if ( cols["bitsteering"].as<int64_t>() != -1 )  cc_obj.set( "bitSteering", cols["bitsteering"].as<int64_t>() );

            if ( cols["cnLink1Location"] )  cc_obj.set( "cnLink1Location", cols["cnLink1Location"].getString() );
            if ( cols["cnLink1Status"] )  cc_obj.set( "cnLink1Status", cols["cnLink1Status"].getString() );

            if ( cols["cnLink2Location"] )  cc_obj.set( "cnLink2Location", cols["cnLink2Location"].getString() );
            if ( cols["cnLink2Status"] )  cc_obj.set( "cnLink2Status", cols["cnLink2Status"].getString() );
        }
    }

    return val_ptr;
}


void Hardware::_addPowerModules( const std::string& rack_location, cxxdb::ConnectionPtr conn_ptr, json::Object& obj )
{
    json::Array &pms_arr(obj.createArray( "powerModules" ));

    static const string SQL( "SELECT * FROM bgqBulkPowerSupply WHERE LEFT(location,3)=? ORDER BY location" );

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "rackLocation" } ));
    stmt_ptr->parameters()["rackLocation"].set( rack_location );
    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

    while ( rs_ptr->fetch() ) {
        json::Object& pm_obj(pms_arr.addObject());

        string pm_loc(rs_ptr->columns()["location"].getString());

        pm_obj.set( "location", pm_loc );

        pm_obj.set( "status", rs_ptr->columns()["status"].getString() );

        if ( rs_ptr->columns()["serialNumber"] )  pm_obj.set( "serialNumber", rs_ptr->columns()["serialNumber"].getString() );
        if ( rs_ptr->columns()["productId"] )  pm_obj.set( "productId", rs_ptr->columns()["productId"].getString() );
    }
}


json::Array* Hardware::_addIoDrawers( const std::string& rack_location, cxxdb::ConnectionPtr conn_ptr, json::Object& obj )
{
    static const string SQL( "SELECT location, status FROM bgqIoDrawer WHERE LEFT(location,3)=? ORDER BY location" );

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "rackLocation" } ));
    stmt_ptr->parameters()["rackLocation"].set( rack_location );
    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

    json::Array *iods_arr_p(NULL);

    while ( rs_ptr->fetch() ) {
        if ( ! iods_arr_p )  iods_arr_p = &(obj.createArray( "ioDrawers" ));

        json::Object& iod_obj(iods_arr_p->addObject());

        string iod_loc(rs_ptr->columns()["location"].getString());

        iod_obj.set( "location", iod_loc );
        iod_obj.set( "URI", string() + "/bg/machine/" + iod_loc );

        iod_obj.set( "status", rs_ptr->columns()["status"].getString());
    }

    return iods_arr_p;
}


}}} // namespace bgws::responder::hardware
