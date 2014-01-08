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

#include "../../dbConnectionPool.hpp"
#include "../../Error.hpp"

#include "capena-http/http/http.hpp"

#include "capena-http/server/exception.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>

#include <boost/foreach.hpp>
#include <boost/throw_exception.hpp>

#include <string>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {
namespace hardware {


const capena::http::uri::Path Machine::RESOURCE_PATH( capena::http::uri::Path() / "machine" );


void Machine::doGet()
{
    if ( ! _isUserAuthenticated() ) {

        LOG_WARN_MSG( "Cannot get machine info because the user isn't authenticated." );

        BOOST_THROW_EXCEPTION( Error(
                "Cannot get machine info because the user isn't authenticated.",
                "getMachine", "notAuthenticated", Error::Data(),
                capena::http::Status::Forbidden
            ) );

    }

    _formatMachine();
}


void Machine::_formatMachine()
{
    capena::server::Response &response(getResponse());

    response.setContentTypeJson();
    response.headersComplete();

    json::ObjectValue obj_val;
    json::Object &obj(obj_val.get());

    auto conn_ptr(dbConnectionPool::getConnection());

    { // machine details, from the bgqMachine table.
        static const string SQL( "SELECT * FROM bgqMachine" );

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        rs_ptr->fetch();

        obj.set( "serialNumber", rs_ptr->columns()["serialNumber"].getString() );
        obj.set( "productId", rs_ptr->columns()["productId"].getString() );
        if ( ! rs_ptr->columns()["description"].isNull() )  obj.set( "description", rs_ptr->columns()["description"].getString() );
        obj.set( "status", rs_ptr->columns()["status"].getString() );
        obj.set( "hasEthernetGateway", rs_ptr->columns()["hasEthernetGateway"].getString() );
        obj.set( "mtu", rs_ptr->columns()["mtu"].as<int64_t>() );
        obj.set( "clockHz", rs_ptr->columns()["clockHz"].as<int64_t>() );
        if ( ! rs_ptr->columns()["bringupOptions"].isNull() )  obj.set( "bringupOptions", rs_ptr->columns()["bringupOptions"].getString() );
    }

    { // compute racks
        json::Array &racks_arr(obj.createArray( "racks" ));

        static const string SQL( "SELECT DISTINCT LEFT( location, 3 ) AS location FROM bgqMidplane ORDER BY location" );

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        while ( rs_ptr->fetch() ) {
            json::Object& rack_obj(racks_arr.addObject());

            string location(rs_ptr->columns()["location"].getString());

            rack_obj.set( "location", location );
            rack_obj.set( "URI", string() + "/bg/machine/" + location );
        }
    }

    { // I/O racks
        static const string SQL( "SELECT location, status FROM bgqIoRack ORDER BY location" );

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        json::Array *io_racks_arr_p(NULL);

        while ( rs_ptr->fetch() ) {
            if ( ! io_racks_arr_p )  io_racks_arr_p = &(obj.createArray( "ioRacks" ));

            json::Object& io_rack_obj(io_racks_arr_p->addObject());

            string location(rs_ptr->columns()["location"].getString());

            io_rack_obj.set( "location", location );
            io_rack_obj.set( "URI", string() + "/bg/machine/" + location );

            io_rack_obj.set( "status", rs_ptr->columns()["status"].getString() );
        }
    }


    json::Object *not_available_obj_p(NULL);


    { // Not available hardware

        static const string SQL(

 "SELECT location, status FROM bgqMidplane WHERE status <> 'A'"
" UNION ALL"
" SELECT location, status FROM bgqNodeCard WHERE status <> 'A'"
" UNION ALL"
" SELECT location, status FROM bgqIoRack WHERE status <> 'A'"
" UNION ALL"
" SELECT location, status FROM bgqIoDrawer WHERE status <> 'A'"

            );

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        while ( rs_ptr->fetch() ) {
            if ( ! not_available_obj_p )  not_available_obj_p = &(obj.createObject( "notAvailable" ));

            json::Object &na_obj(not_available_obj_p->createObject( rs_ptr->columns()["location"].getString() ));

            na_obj.set( "status", rs_ptr->columns()["status"].getString() );
        }
    }

    { // Not available service card

        static const string SQL(

 "SELECT midplanePos, status FROM bgqServiceCard WHERE status <> 'A'"

            );

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        while ( rs_ptr->fetch() ) {
            string midplane_pos(rs_ptr->columns()["midplanePos"].getString());

            if ( ! not_available_obj_p )  not_available_obj_p = &(obj.createObject( "notAvailable" ));

            json::Object *na_obj_p;

            json::Object::iterator i(not_available_obj_p->find( midplane_pos ));

            if ( i == not_available_obj_p->end() ) {
                na_obj_p = &(not_available_obj_p->createObject( midplane_pos ));
            } else {
                na_obj_p = &(i->second.get()->getObject());
            }

            na_obj_p->set( "serviceCardStatus", rs_ptr->columns()["status"].getString() );
        }
    }

    { // Not available switches

        static const string SQL(

 "SELECT midplanePos, COUNT(*) AS c FROM bgqSwitch WHERE status <> 'A' GROUP BY midplanePos"

            );

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        while ( rs_ptr->fetch() ) {
            string midplane_pos(rs_ptr->columns()["midplanePos"].getString());

            if ( ! not_available_obj_p )  not_available_obj_p = &(obj.createObject( "notAvailable" ));

            json::Object *na_obj_p;

            json::Object::iterator i(not_available_obj_p->find( midplane_pos ));

            if ( i == not_available_obj_p->end() ) {
                na_obj_p = &(not_available_obj_p->createObject( midplane_pos ));
            } else {
                na_obj_p = &(i->second.get()->getObject());
            }

            na_obj_p->set( "switchCount", rs_ptr->columns()["c"].as<int64_t>() );
        }
    }

    { // Not available nodes

        static const string SQL(

 "SELECT midplanePos || '-' || nodeCardPos AS location, c"
  " FROM ( SELECT midplanePos, nodeCardPos, COUNT(*) AS c FROM bgqComputeNode"
           " WHERE status <> 'A'"
           " GROUP BY midplanePos, nodeCardPos"
       " ) AS t"

            );

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        while ( rs_ptr->fetch() ) {
            if ( ! not_available_obj_p )  not_available_obj_p = &(obj.createObject( "notAvailable" ));

            const string nodecard_pos(rs_ptr->columns()["location"].getString());

            json::Object *na_obj_p;

            json::Object::iterator i(not_available_obj_p->find( nodecard_pos ));

            if ( i == not_available_obj_p->end() ) {
                na_obj_p = &(not_available_obj_p->createObject( nodecard_pos ));
            } else {
                na_obj_p = &(i->second.get()->getObject());
            }

            na_obj_p->set( "nodeCount", rs_ptr->columns()["c"].as<uint64_t>() );
        }
    }

    { // Not available I/O nodes

        static const string SQL(

 "SELECT ioPos AS location, COUNT(*) AS c FROM bgqIoNode"
  " WHERE status <> 'A'"
  " GROUP BY ioPos"

            );

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        while ( rs_ptr->fetch() ) {
            if ( ! not_available_obj_p )  not_available_obj_p = &(obj.createObject( "notAvailable" ));

            const string io_drawer_pos(rs_ptr->columns()["location"].getString());

            json::Object *na_obj_p;

            json::Object::iterator i(not_available_obj_p->find( io_drawer_pos ));

            if ( i == not_available_obj_p->end() ) {
                na_obj_p = &(not_available_obj_p->createObject( io_drawer_pos ));
            } else {
                na_obj_p = &(i->second.get()->getObject());
            }

            na_obj_p->set( "ioNodeCount", rs_ptr->columns()["c"].as<uint64_t>() );
        }
    }

    { // Not available DCAs

        static const string SQL(

 "SELECT midplanePos || '-' || nodeCardPos AS location, c"
  " FROM ( SELECT midplanePos, nodeCardPos, COUNT(*) AS c FROM bgqNodeCardDCA"
           " WHERE status <> 'A'"
           " GROUP BY midplanePos, nodeCardPos"
       " ) AS t"

            );

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        while ( rs_ptr->fetch() ) {
            if ( ! not_available_obj_p )  not_available_obj_p = &(obj.createObject( "notAvailable" ));

            const string nodecard_pos(rs_ptr->columns()["location"].getString());

            json::Object *na_obj_p;

            json::Object::iterator i(not_available_obj_p->find( nodecard_pos ));

            if ( i == not_available_obj_p->end() ) {
                na_obj_p = &(not_available_obj_p->createObject( nodecard_pos ));
            } else {
                na_obj_p = &(i->second.get()->getObject());
            }

            na_obj_p->set( "DCACount", rs_ptr->columns()["c"].as<uint64_t>() );
        }
    }

    { // Not available power modules in each compute rack.

        static const string SQL(

 "SELECT rackLoc, COUNT(*) AS c"
  " FROM ( SELECT LEFT(location,3) AS rackLoc"
           " FROM bgqBulkPowerSupply"
           " WHERE status <> 'A' AND location LIKE 'R%'"
       " ) AS t"
  " GROUP BY rackLoc"

            );

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        while ( rs_ptr->fetch() ) {
            if ( ! not_available_obj_p )  not_available_obj_p = &(obj.createObject( "notAvailable" ));

            const string rack_loc(rs_ptr->columns()["rackLoc"].getString());

            json::Object *na_obj_p;

            json::Object::iterator i(not_available_obj_p->find( rack_loc ));

            if ( i == not_available_obj_p->end() ) {
                na_obj_p = &(not_available_obj_p->createObject( rack_loc ));
            } else {
                na_obj_p = &(i->second.get()->getObject());
            }

            na_obj_p->set( "powerModuleCount", rs_ptr->columns()["c"].as<uint64_t>() );
        }
    }

    json::Formatter()( obj_val, response.out() );
}


}}} // namespace bgws::responder::hardware
