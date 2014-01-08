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

#include "MediaType.hpp"

#include <boost/bind.hpp>
#include <boost/throw_exception.hpp>

#include <boost/spirit/include/classic_insert_at_actor.hpp>

#include <boost/spirit/include/classic.hpp>

#include <iostream>
#include <stdexcept>
#include <string>


using boost::bind;

using boost::spirit::classic::assign_a;
using boost::spirit::classic::ch_p;
using boost::spirit::classic::insert_at_a;
using boost::spirit::classic::parse_info;

using std::string;


namespace capena {
namespace http {


static MediaType createJsonUtf8()
{
    MediaType::Parameters params;
    params["charset"] = "UTF-8";

    return MediaType( "application", "json", params );
}


const MediaType MediaType::JSON( "application", "json" );
const MediaType MediaType::JSON_UTF8(createJsonUtf8());


MediaType MediaType::parse( const std::string& str )
{
    MediaType ret;

    string param_name, param_value;

    boost::spirit::classic::rule<> r =
            (+~ch_p('/'))[assign_a( ret._type )] >> ch_p('/') >> (+~ch_p(';'))[assign_a( ret._subtype )] >>
            *(ch_p(';') >> *ch_p(' ') >>
                ((+~ch_p('='))[assign_a( param_name )] >> ch_p('=') >>
                 (+~ch_p(';'))[assign_a( param_value )]
                )[insert_at_a( ret._parameters, param_name, param_value )]
             );

    parse_info<const char*> p_info(boost::spirit::classic::parse( str.c_str(), r ));

    if ( ! p_info.full ) {
        // Failed to parse the full document.

        BOOST_THROW_EXCEPTION( std::invalid_argument( string() + "invalid argument to MediaType: " + str ) );
    }

    return ret;
}


MediaType::MediaType(
        const std::string& type,
        const std::string& subtype,
        const Parameters& parameters
    ) :
        _type(type),
        _subtype(subtype),
        _parameters(parameters)
{
    // Nothing to do.
}


bool MediaType::equivalent( const MediaType& other ) const
{
    return ((_type == other._type) && (_subtype == other._subtype));
}


std::ostream& operator<<( std::ostream& os, const MediaType& ct )
{
    os << ct.getType() << '/' << ct.getSubtype();
    for ( MediaType::Parameters::const_iterator i(ct.getParameters().begin()) ; i != ct.getParameters().end() ; ++i ) {
        os << "; " << i->first << "=" << i->second;
    }
    return os;
}


} } // namespace capena::http
