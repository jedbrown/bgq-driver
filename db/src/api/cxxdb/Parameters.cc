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

#include "cxxdb/Parameters.h"

#include "cxxdb/exceptions.h"
#include "cxxdb/sql_util.h"
#include "cxxdb/StatementHandle.h"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>

#include <boost/algorithm/string.hpp>

#include <sqlext.h>

#include <stdexcept>
#include <string>

using boost::lexical_cast;

using std::logic_error;
using std::string;

LOG_DECLARE_FILE( "database" );

namespace cxxdb {

Parameters::Parameters(
        StatementHandle& stmt_handle,
        const ParameterNames& parameter_names
    )
{
    ParameterInfos infos(stmt_handle.getParameterInfos());

    LOG_TRACE_MSG( "Number of parameters is " << infos.size() );

    _param_ptrs.resize( infos.size() );

    for ( unsigned i(0) ; i < infos.size() ; ++i ) {
        string name;
        if ( i < parameter_names.size() ) {
            name = boost::to_upper_copy( parameter_names[i] );
        }

        if ( name.empty() ) {
            _param_ptrs[i] = Parameter::Ptr( new Parameter(
                    stmt_handle,
                    i+1,
                    infos[i]
                ) );
        } else {
            _NameToParameter::iterator name_i(_name_to_parameter.find( name ));

            if ( name_i == _name_to_parameter.end() ) {
                // It's not in the map.

                Parameter::Ptr param_ptr( new Parameter(
                        stmt_handle,
                        i+1,
                        infos[i]
                    ) );

                _param_ptrs[i] = param_ptr;
                _name_to_parameter[name] = param_ptr;

            } else {

                // Found the name in the map already...

                // Make sure the type is compatible.

                const ParameterInfo &other_info(name_i->second->getInfo());

                if ( other_info.data_type != infos[i].data_type ||
                     other_info.size != infos[i].size ||
                     other_info.decimal_digits != infos[i].decimal_digits ||
                     other_info.nullable != infos[i].nullable
                   )
                {
                    CXXDB_THROW_EXCEPTION( logic_error( "Type for parameter '" + name + "' doesn't match the previous use of the name." ) );
                }

                // Types are compatible, add the index.
                // Returns non-zero if now multiple indexes with the same name so should remove the other one.

                unsigned prev_index(name_i->second->addIndex( i+1 ));

                if ( prev_index != 0 ) {
                    _param_ptrs[prev_index-1].reset();
                }

            }
        }
    }
}


Parameter& Parameters::operator[]( unsigned param_index )
{
    if ( (param_index == 0) ||
         ((param_index-1) >= _param_ptrs.size()) ) {
        CXXDB_THROW_EXCEPTION( logic_error( string() + "Parameter index " + lexical_cast<string>(param_index) + " out of range." ) );
    }

    if ( ! _param_ptrs[param_index-1] ) {
        // This parameter marker is named for multiple parameter markers, so confusing if allow to set on multiple.
        CXXDB_THROW_EXCEPTION( logic_error( string() + "Parameter index " + lexical_cast<string>(param_index) + " shared so must reference by name." ) );
    }

    return *(_param_ptrs[param_index-1]);
}


Parameter& Parameters::operator[]( const std::string& name )
{
    _NameToParameter::iterator i(_name_to_parameter.find( boost::to_upper_copy( name ) ));

    if ( i == _name_to_parameter.end() ) {
        // The name is not known.
        CXXDB_THROW_EXCEPTION( logic_error( string() + "Parameter by name of '" + name + "' doesn't match any marker names." ) );
    }

    return *(i->second);
}


} // namespace cxxdb
