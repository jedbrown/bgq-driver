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

#ifndef JSON_PARSER_HPP_
#define JSON_PARSER_HPP_


/*! \file
 *  \brief class Parser.
 */


#include "fwd.hpp"
#include "Value.hpp"

#include <boost/spirit/include/classic.hpp>

#include <stack>
#include <string>

#include <ext/rope>


namespace json {


/*!
 * \brief Parses a JSON string.
 */
class Parser
{
public:

    /*! \brief Constructor. */
    Parser();

    /*!
     * \brief Parse the JSON string
     *
     * This method can only be called from one thread at a time.
     *
     * \throw json::ParseError The string was not a valid JSON document.
     */
    ValuePtr operator()( const std::string& str );


private:

    typedef std::pair<std::string,ValuePtr> _Pair;
    typedef std::stack<_Pair> _ContainerStack;
    typedef __gnu_cxx::rope<char> CurStr;


    ValuePtr _cur_value_ptr;

    CurStr _cur_str; // String that's being parsed.

    std::string _member_name; // Name of pair.

    _ContainerStack _containers;


    boost::spirit::classic::rule<> _jstring_r;
    boost::spirit::classic::rule<> _digits_r;
    boost::spirit::classic::rule<> _dj_int_r;
    boost::spirit::classic::rule<> _frac_r;
    boost::spirit::classic::rule<> _exp_r;
    boost::spirit::classic::rule<> _number_r;
    boost::spirit::classic::rule<> _value_r;
    boost::spirit::classic::rule<> _empty_array_r;
    boost::spirit::classic::rule<> _array_r;
    boost::spirit::classic::rule<> _object_r;
    boost::spirit::classic::rule<> _pair_r;
    boost::spirit::classic::rule<> _empty_object_r;


    void _strbegin( char /*c*/ )  { _cur_str = CurStr(); }

    void _strchr( char c )  { _cur_str += c; }
    void _strbackspace( char /*c*/ )  { _cur_str += '\b'; }
    void _strformfeed( char /*c*/ )  { _cur_str += '\f'; }
    void _strnewline( char /*c*/ )  { _cur_str += '\n'; }
    void _strcarriagereturn( char /*c*/ )  { _cur_str += '\r'; }
    void _strtab( char /*c*/ )  { _cur_str += '\t'; }
    void _strunicode( const char *str, const char *end );

    void _memberName( const char *str, const char *end );

    void _string( const char *str, const char *end );

    void _number( const char *str, const char *end );

    void _emptyObject( const char *str, const char *end );

    void _objectStart( char c );

    void _emptyArray( const char *str, const char *end );

    void _arrayStart( char c );

    void _containerEnd( char c );

    void _booleanTrue( const char *str, const char *end );

    void _booleanFalse( const char *str, const char *end );

    void _null( const char *str, const char *end );

    void _putValue( ValuePtr ptr );
};


} // namespace json {

#endif
