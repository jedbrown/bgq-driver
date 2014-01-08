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

/*! \page multiWildcardFormat Multi-wildcard String Format

Some query string parameters can contain multiple patterns containing wildcard characters.

- Each pattern is separated by space.
- - (minus) prefix on a string indicates those values should not be in the result.
- * (asterisk) matches any string of characters.
- ? (question mark) matches any character.

Examples:
- "MCH3601" matches "MCH3601".
- "MCH*" matches "MCH3601", "MCH3602", etc.
- "-MCH*" matches everything but a string starting with MCH.
- "MCH3601 CPF3201" matches "MCH3601" or "CPF3201"
- "MCH* -MCH3601" matches "MCH3602" but not "MCH3601"

In a URL, the ' ' (space) will be escaped as '+'.

 */


#include "MultiWildcardOption.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <boost/algorithm/string.hpp>


using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


MultiWildcardOption::_Pattern::_Pattern( const std::string &pattern_str )
{
    // check for easy case, no wildcards.
    if ( pattern_str.find( '*' ) == string::npos && pattern_str.find( '?' ) == string::npos )
    {
        use_like = false;
        pattern = pattern_str;
        return;
    }

    use_like = true;
    pattern = pattern_str;

    boost::algorithm::replace_all( pattern, "*", "%" ); // * glob becomes % SQL.
    boost::algorithm::replace_all( pattern, "?", "_" ); // ? glob becomes _ SQL.
}


MultiWildcardOption::MultiWildcardOption(
        const std::string& name,
        unsigned max_length,
        BGQDB::DBObj::ColumnType::Value col_type
    ) :
        _name(name),
        _max_length(max_length),
        _col_type(col_type)
{
    // Nothing to do.
}


void MultiWildcardOption::addTo(
        boost::program_options::options_description& desc_in_out
    )
{
    desc_in_out.add_options()
            ( _name.c_str(), boost::program_options::value<string>()->notifier( boost::bind( &MultiWildcardOption::_notifyValue, this, _1 ) ) )
        ;
}


void MultiWildcardOption::addTo(
        WhereClause& wc_in_out,
        cxxdb::ParameterNames& parameter_names_in_out,
        const std::string& col_name
    ) const
{
    string sql = "";
    unsigned param_no = 0;

    if ( ! _neg_patterns.empty() ) {

        sql += "(";

        bool need_and(false);
        BOOST_FOREACH( const _Patterns::value_type &pattern , _neg_patterns ) {
            bool do_rtrim(false);
            bool do_cast(false);

            if ( pattern.use_like && _col_type == BGQDB::DBObj::ColumnType::Char ) {
                bool ends_in_pct((! pattern.pattern.empty()) && (pattern.pattern[pattern.pattern.size() - 1] == '%'));

                if ( ends_in_pct ) {
                    do_rtrim = false;
                    do_cast = false;
                } else {
                    bool internal_pct(pattern.pattern.find( '%' ) != string::npos );

                    if ( internal_pct ) {
                        do_rtrim = true;
                        do_cast = false;
                    } else {
                        do_rtrim = false;
                        do_cast = true;
                    }
                }
            }


            if ( need_and )  sql += " AND ";
            else need_and = true;

            if ( do_rtrim ) { sql += "RTRIM(" + col_name + ") "; }
            else { sql += col_name + " "; }

            if ( pattern.use_like ) {
                sql +=  "NOT LIKE ";
                if ( do_cast ) {
                    sql += "CAST(? AS CHAR(" + lexical_cast<string>(_max_length) + "))";
                } else {
                    sql += "?";
                }
            } else {
                sql += "<> ?";
            }


            parameter_names_in_out.push_back( _name + "_" + lexical_cast<string>(++param_no) );
        }

        sql += ")";

        if ( ! _inc_patterns.empty() ) sql += " AND ";
    }

    if ( ! _inc_patterns.empty() ) {
        sql += "(";

        bool need_or(false);
        BOOST_FOREACH( const _Patterns::value_type &pattern , _inc_patterns ) {
            bool do_rtrim(false);
            bool do_cast(false);

            if ( pattern.use_like && _col_type == BGQDB::DBObj::ColumnType::Char ) {
                bool ends_in_pct((! pattern.pattern.empty()) && (pattern.pattern[pattern.pattern.size() - 1] == '%'));

                if ( ends_in_pct ) {
                    do_rtrim = false;
                    do_cast = false;
                } else {
                    bool internal_pct(pattern.pattern.find( '%' ) != string::npos );

                    if ( internal_pct ) {
                        do_rtrim = true;
                        do_cast = false;
                    } else {
                        do_rtrim = false;
                        do_cast = true;
                    }
                }
            }


            if ( need_or )  sql += " OR ";
            else need_or = true;

            if ( do_rtrim ) { sql += "RTRIM(" + col_name + ") "; }
            else { sql += col_name + " "; }

            if ( pattern.use_like ) {
                sql +=  "LIKE ";
                if ( do_cast ) {
                    sql += "CAST(? AS CHAR(" + lexical_cast<string>(_max_length) + "))";
                } else {
                    sql += "?";
                }
            } else {
                sql += "= ?";
            }


            parameter_names_in_out.push_back( _name + "_" + lexical_cast<string>(++param_no) );
        }

        sql += ")";
    }

    if ( ! sql.empty() ) {

        LOG_DEBUG_MSG( "sql=" << sql );

        wc_in_out.add( sql );
    }
}


void MultiWildcardOption::bindParameters(
        cxxdb::Parameters& parameters_in_out
    ) const
{
    unsigned param_no = 0;

    BOOST_FOREACH( const _Patterns::value_type &pattern , _neg_patterns ) {
        parameters_in_out[_name + "_" + lexical_cast<string>(++param_no)].set( pattern.pattern );
    }

    BOOST_FOREACH( const _Patterns::value_type &pattern , _inc_patterns ) {
        parameters_in_out[_name + "_" + lexical_cast<string>(++param_no)].set( pattern.pattern );
    }
}


void MultiWildcardOption::_notifyValue( const std::string& s )
{
    if ( s.empty() )  return;

    // split on ' '.

    typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;

    boost::char_separator<char> sep(" ");

    Tokenizer tok( s, sep );

    for ( Tokenizer::iterator i(tok.begin()) ; i != tok.end() ; ++i ) {
        string pattern_str(*i);

        if ( pattern_str.empty() )  continue;

        if ( pattern_str[0] == '-' ) {
            pattern_str = pattern_str.substr( 1 );
            if ( pattern_str.empty() )  continue;

            // Skip the pattern if the length is too long.
            if ( pattern_str.size() > _max_length ) {
                LOG_WARN_MSG( "The length of the " << _name << " option is longer than allowed, the option will be ignored. allowed=" << _max_length << " pattern='" << pattern_str << "'" );
                continue;
            }

            _neg_patterns.push_back( _Pattern( pattern_str ) );

        } else {

            // Skip the pattern if the length is too long.
            if ( pattern_str.size() > _max_length ) {
                LOG_WARN_MSG( "The length of the " << _name << " option is longer than allowed, the option will be ignored. allowed=" << _max_length << " pattern='" << pattern_str << "'" );
                continue;
            }

            _inc_patterns.push_back( _Pattern( pattern_str ) );

        }
    }
}


} // namespace bgws
