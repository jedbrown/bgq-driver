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

/*! \page errorDocument %Error document

%Error document

<b> JSON response format: </b> <br>

<pre>
{
  "text" : &quot;<i>error text</i>&quot;,
  "operation" : &quot;<i>operation name</i>&quot;,
  "id" : &quot;<i>error ID</i>&quot;,
  "data" : { &quot;<i>data ID</i>&quot; : &quot;<i>data value</i>&quot;, ... }
}
</pre>

 */


#include "Error.hpp"

#include "chiron-json/json.hpp"

#include <boost/foreach.hpp>


namespace bgws {


Error::Error(
        const std::string& text,
        const std::string& operation,
        const std::string& id,
        const Data& data,
        capena::http::Status http_status
    ) :
        capena::server::exception::Error(
                http_status,
                text
            ),
        _operation(operation),
        _id(id),
        _data(data)
{
    json::ObjectValue obj_val;
    json::Object &err_obj(obj_val.get());

    err_obj.set( "text", text );
    err_obj.set( "operation", _operation );
    err_obj.set( "id", _id );

    json::Object &data_obj(err_obj.createObject( "data" ));

    BOOST_FOREACH( const Data::value_type& data , getData() ) {
        data_obj.set( data.first, data.second );
    }

    std::ostringstream response_oss;

    json::Formatter()( obj_val, response_oss );

    setResponseStr( response_oss.str() );
}


void Error::_setHeaders( capena::server::Response& response ) const
{
    response.setContentTypeJson();
}


} // namespace bgws
