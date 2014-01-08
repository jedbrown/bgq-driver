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


/*! \page wildcardFormat Wildcard String Format

Some query string parameters can contain wildcard characters that match any string or any character.

- * (asterisk) matches any string of characters.
- ? (question mark) matches any character.

For example, location=R00-M0-* matches locations like "R00-M0-N00", "R00-M0-N01".

 */


#include "WildcardOption.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/algorithm/string.hpp>


using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


WildcardOption::WildcardOption(
        const std::string& name,
        unsigned max_length,
        BGQDB::DBObj::ColumnType::Value col_type
    ) :
        _name(name),
        _max_length(max_length),
        _col_type(col_type),
        _no_value(true)
{
    // Nothing to do.
}


void WildcardOption::addTo(
        boost::program_options::options_description& desc_in_out
    )
{
    desc_in_out.add_options()
            ( _name.c_str(), boost::program_options::value<string>()->notifier( boost::bind( &WildcardOption::_notifyValue, this, _1 ) ) )
        ;
}


void WildcardOption::addTo(
        WhereClause& wc_in_out,
        cxxdb::ParameterNames& parameter_names_in_out,
        const std::string& col_name
    ) const
{
    if ( _no_value )  return;


    bool do_rtrim(false);
    bool do_cast(false);

    if ( _use_like && _col_type == BGQDB::DBObj::ColumnType::Char ) {
        bool ends_in_pct((! _value.empty()) && (_value[_value.size() - 1] == '%'));

        if ( ends_in_pct ) {
            do_rtrim = false;
            do_cast = false;
        } else {
            bool internal_pct(_value.find( '%' ) != string::npos );

            if ( internal_pct ) {
                do_rtrim = true;
                do_cast = false;
            } else {
                do_rtrim = false;
                do_cast = true;
            }
        }
    }


    string sql;

    if ( do_rtrim ) { sql += "RTRIM(" + col_name + ") "; }
    else { sql += col_name + " "; }

    if ( _use_like ) {
        sql +=  "LIKE ";
        if ( do_cast ) {
            sql += "CAST(? AS CHAR(" + lexical_cast<string>(_max_length) + "))";
        } else {
            sql += "?";
        }
    } else {
        sql += "= ?";
    }

    LOG_DEBUG_MSG( "sql=" << sql );

    wc_in_out.add( sql );

    parameter_names_in_out.push_back( _name );
}


void WildcardOption::bindParameters(
        cxxdb::Parameters& parameters_in_out
    ) const
{
    if ( _no_value )  return;

    parameters_in_out[_name].set( _value );
}


void WildcardOption::_notifyValue( const std::string& s )
{
    if ( s.empty() ) {
        _no_value = true;
        return;
    }

    if ( s.size() > _max_length ) {
        LOG_WARN_MSG( "The length of the " << _name << " option is longer than allowed, the option will be ignored. allowed=" << _max_length << " length=" << s.size() );
        _no_value = true;
        return;
    }

    _no_value = false;

    // check for easy case, no wildcards.
    if ( s.find( '*' ) == string::npos && s.find( '?' ) == string::npos )
    {
        _use_like = false;
        _value = s;
        return;
    }

    _use_like = true;

    _value = s;
    boost::algorithm::replace_all( _value, "*", "%" ); // * glob becomes % SQL.
    boost::algorithm::replace_all( _value, "?", "_" ); // ? glob becomes _ SQL.
}


} // namespace bgws
