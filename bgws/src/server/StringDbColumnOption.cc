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


#include "StringDbColumnOption.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>

#include <string>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


StringDbColumnOption::StringDbColumnOption(
        const std::string& name,
        unsigned max_length
    ) :
        _name(name),
        _max_length(max_length),
        _no_value(true)
{
    // Nothing to do.
}


void StringDbColumnOption::addTo(
        boost::program_options::options_description& desc_in_out
    )
{
    desc_in_out.add_options()
            ( _name.c_str(), boost::program_options::value<string>()->notifier( boost::bind( &StringDbColumnOption::_notifyValue, this, _1 ) ) )
        ;
}


void StringDbColumnOption::addTo(
        WhereClause& wc_in_out,
        cxxdb::ParameterNames& parameter_names_in_out,
        const std::string& col_name
    ) const
{
    if ( _no_value )  return;

    wc_in_out.add( col_name + " = ?" );

    parameter_names_in_out.push_back( _name );
}


void StringDbColumnOption::bindParameters(
        cxxdb::Parameters& parameters_in_out
    ) const
{
    if ( _no_value )  return;

    parameters_in_out[_name].set( _value );
}


void StringDbColumnOption::_notifyValue( const std::string& s )
{
    if ( s.empty() ) {
        _no_value = true;
        return;
    }

    if ( s.size() > _max_length ) {
        LOG_WARN_MSG( "The length of the " << _name << " option is longer than allowed, the option will be ignored. allowed=" << _max_length << " value='" << s << "'" );
        _no_value = true;
        return;
    }

    _no_value = false;

    _value = s;
}


} // namespace bgws
