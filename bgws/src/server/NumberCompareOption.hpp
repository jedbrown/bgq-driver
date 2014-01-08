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

/*! \page numberComparisonFormat Number Comparison Format

This type of query string parameter allows you to specify the comparison operator and the number.

The format of the comparison operator is [operator]&lt;number&gt;.

<i>operator</i> is one of
- "eq" : Equality
- "gt" : Greater than
- "lt" : Less than
- "ge" : Greater than or equal to
- "le" : Less than or equal to

If the operator is not present, Equality is used.

The <i>number</i> may be a double-precision floating point or an integer.
The c++ streams library is used to parse the number string,
so if the number is a double it must be in that format
and
if the number is an integer it must be in that format.

For example, if the argument is gt5.0 the comparison will be for the value is greater than 5.0.

 */

#ifndef BGWS_NUMBER_COMPARE_OPTION_HPP_
#define BGWS_NUMBER_COMPARE_OPTION_HPP_


#include "WhereClause.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <stdexcept>
#include <string>


namespace bgws {


template <typename T>
class LexicalCastValueParser
{
public:
    T operator()( const std::string& num_str ) {
        return boost::lexical_cast<T>( num_str );
    }
};


template <
        typename T,
        typename ValueParser = LexicalCastValueParser<T>
    >
class NumberCompareOption
{
public:

    NumberCompareOption( const std::string& name )
        : _name(name), _comparison(NoValue)
    { /* Nothing to do */ }

    void addTo(
            boost::program_options::options_description& desc_in_out
        )
    {
        desc_in_out.add_options()
                ( _name.c_str(), boost::program_options::value<std::string>()->notifier( boost::bind( &NumberCompareOption<T,ValueParser>::_notifyValue, this, _1 ) ) )
            ;
    }

    void addTo(
            WhereClause& wc_in_out,
            cxxdb::ParameterNames& parameter_names_in_out,
            const std::string& col_name
        ) const
    {
        if ( _comparison == NoValue )  return;

        std::string op(_comparison == Equal ? "=" :
                  _comparison == Greater ? ">" :
                  _comparison == Less ? "<" :
                  _comparison == GreaterEqual ? ">=" :
                  _comparison == LessEqual ? "<=" :
                  "=");

        wc_in_out.add( col_name + " " + op + " ?" );
        parameter_names_in_out.push_back( _name );
    }

    void bindParameters(
            cxxdb::Parameters& parameters_in_out
        )
    {
        if ( _comparison == NoValue )  return;

        parameters_in_out[_name].cast( _value );
    }


private:

    enum _Comparison {
        NoValue,
        Equal,
        Greater,
        Less,
        GreaterEqual,
        LessEqual
    };


    std::string _name;

    _Comparison _comparison;
    T _value;


    void _notifyValue( const std::string& s )
    {
        _comparison = NoValue;

        std::string num_part(s);

        if ( s.length() > 2 ) {
            std::string pfx(s.substr( 0, 2 ));

            _comparison = (pfx == "eq" ? Equal :
                           pfx == "gt" ? Greater :
                           pfx == "lt" ? Less :
                           pfx == "ge" ? GreaterEqual :
                           pfx == "le" ? LessEqual :
                           NoValue);

            // Found a prefix, remove the prefix from the num_part.
            if ( _comparison != NoValue ) {
                num_part = s.substr( 2 );
            }
        }

        try {
            _value = ValueParser()( num_part );

            // Valid num part and no comparison, change comparison to Equal.
            if ( _comparison == NoValue )  _comparison = Equal;
        } catch ( std::bad_cast& e ) {
            _comparison = NoValue;
        }
    }

};


typedef NumberCompareOption<double> DoubleCompareOption;
typedef NumberCompareOption<SQLINTEGER> IntegerCompareOption;


} // namespace bgws

#endif
