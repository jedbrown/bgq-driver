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

#ifndef JSON_CHIRON_JSON_HPP_
#define JSON_CHIRON_JSON_HPP_


#include "exception.hpp"
#include "Array.hpp"
#include "Formatter.hpp"
#include "functions.hpp"
#include "Object.hpp"
#include "Parser.hpp"
#include "Value.hpp"


/*! \mainpage

Chiron is a JSON library.
It provides classes for manipulating JSON objects,
parsing JSON text,
and formatting JSON objects to text.
Chiron is named because it's the first name that starts with C in the
Wikipedia entry for Jason (from Greek mythology) on the day I was picking a
name.

JSON is defined at:<br>
http://json.org/

The classes are in the <code>json</code> namespace.

JSON objects are represented by the <code>json::Object</code> class.
A <code>json::Object</code> is a
<code>std::map<std::string,json::ValuePtr></code> from member name to a JSON value.
<code>ValuePtr</code> is a shared pointer to a <code>Value</code>.
<code>Value</code> is a base class for the different actual value types.

There are several types of values that a JSON object or array can contain
- string
- number
- object
- array
- Boolean
- null

These types are represented by the <code>json::Value</code> class.
The user of this library will typically use the <code>json::Value::create</code> methods to
create Values of the desired type.

For convenience, <code>json::Object</code> has methods to
- Set members to different types of values, like
  <code>void json::Object::set( const string& name, const string& value )</code>
- Create empty containers as members, like
  <code>json::Object& json::Object::createObject( const string& name )</code>
- Query if a member is defined,
  <code>bool json::Object::contains( const string& name ) const</code>
- Query if a member is a type, like
  <code>bool json::Object::isString( const string& name ) const</code>
- Get a member as a type, like
  <code>const std::string& json::Object::getString( const string& name ) const</code>

Arrays in JSON are represented using <code>json::Array</code>.
This is just a typedef for <code>std::vector&lt;json::ValuePtr&gt;</code>.
Use the <code>json::Value::create</code> methods to create the values in an array.

For parsing a JSON string, use <code>json::Parser</code>.

The following code provides an example of parsing a JSON document.

\code

#include <chiron-json/json.hpp>

#include <iostream>
#include <string>

int main( int argc, char *argv )
{
    std::string json_str(

"{ \"menu\" : {"
    " \"header\" : \"SVG Viewer\","
    " \"items\" : [ "
        " { \"id\" : \"Open\" },"
        " null"
    " ]"
"}}"

     );

    json::ValuePtr value_ptr(json::Parser()( json_str ));
    const json::Object &obj(value_ptr->getObject());

    const json::Object &menu_obj(obj.getObject( "menu" ));

    std::cout << "Header: " << menu_obj.getString( "header" ) << "\n\n";

    json::Array &items_arr(menu_obj.getArray( "items" ));

    for ( json::Array::const_iterator i(items_arr.begin()) ; i != items_arr.end() ; ++i ) {
        if ( i->isNull() ) {
            std::cout << "----\n";
            continue;
        }

        std::cout << i->getObject().getString( "id" ) << "\n";
    }

}


\endcode

For formatting a JSON object to a string, use <code>json::Formatter</code>.

The following code provides an example for creating a JSON document.

\code

#include <chiron-json/json.hpp>

int main( int argc, char *argv )
{
    json::ObjectValue obj_val;
    json::Object &obj(obj_val.get());
    json::Object &menu_obj(obj.createObject( "menu" ));
    menu_obj.set( "header", "SVG Viewer" );

    json::Array &items_arr(menu_obj.createArray( "items" ));

    json::Object &open_obj(items_arr.addObject());
    open_obj.set( "id", "open" );

    json::Object &open_new_obj(items_arr.addObject());
    open_new_obj.set( "id", "OpenNew" );
    open_new_obj.set( "label", "Open New" );

    items_arr.push_back( json::Value::createNull() );

    json::Formatter()( obj_val, std::cout );
}

\endcode

 */


/*! \namespace json
 *
 * \brief Namespace for the Chiron-JSON library.
 *
 */

#endif
