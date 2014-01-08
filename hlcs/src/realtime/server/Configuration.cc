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


#include "Configuration.h"

#include "log_util.h"

#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#include <fstream>
#include <stdexcept>
#include <string>


using boost::lexical_cast;

using std::ifstream;
using std::exception;
using std::string;


LOG_DECLARE_FILE( "realtime.server" );


namespace realtime {
namespace server {


//-------------------------------------------------------------------------
// Local declarations

static unsigned calculate_workers();

static const string& getValue(
        const bgq::utility::Properties& properties,
        const string& section_name,
        const string& prop_name,
        const string& prop_desc
    )
{
    try {
        return properties.getValue( section_name, prop_name );
    } catch ( exception& e ) {
        THROW_RUNTIME_ERROR_EXCEPTION( "Couldn't get " << prop_desc << " from the configuration file. The server cannot continue." );
    }
}

static Configuration::StringPtr getOptionalValue(
        const bgq::utility::Properties& properties,
        const string& section_name,
        const string& prop_name,
        const string& prop_desc
    )
{
    try {
        return Configuration::StringPtr( new string(properties.getValue( section_name, prop_name )) );
    } catch ( exception& e ) {
        LOG_DEBUG_MSG( prop_desc << " value not in configuration file." );
        return Configuration::StringPtr();
    }
}


static unsigned parseUnsigned( const std::string& s )
{
    if ( s.empty() )  throw std::runtime_error( "invalid unsigned value, used an empty string" );
    if ( s[0] == '-' )  throw std::runtime_error( "invalid unsigned value, starts with -" );
    return lexical_cast<unsigned>( s );
}


//-------------------------------------------------------------------------
// External definitions.
//  see configuration.h


const std::string Configuration::PROPERTIES_SECTION_NAME( "realtime.server" );

const unsigned Configuration::DEFAULT_MAXIMUM_TRANSACTION_SIZE(40);

Configuration::Configuration(
        const bgq::utility::Properties& properties
    ) :
        _database_name(getValue( properties, "database", "name", "database name" )),
        _database_user(getOptionalValue( properties, "database", "user", "database user name" )),
        _database_schema_name(getOptionalValue( properties, "database", "schema_name", "database schema" ))
{
    try {
        string workers_str(properties.getValue( PROPERTIES_SECTION_NAME, "workers" ));
        try {
            _workers = parseUnsigned( workers_str );
        } catch ( std::exception& e ) {
            LOG_WARN_MSG( "The workers value is not a valid number. The workers value is '" << workers_str << "'. Will use 1." );
            _workers = 1;
        }
    } catch ( exception& e ) {
        LOG_INFO_MSG( "Could not find workers in properties file, will calculate the number of workers to use." );
        _workers = 0;
    }

    try {
        string max_xact_size_str(properties.getValue( PROPERTIES_SECTION_NAME, "maximum_transaction_size" ));

        try {
            _maximum_transaction_size = parseUnsigned( max_xact_size_str );
        } catch ( exception& e ) {
            LOG_WARN_MSG(
                    "Failed to parse maximum transaction size in properties file."
                    " The value must be a number greater than zero, but is '" << max_xact_size_str << "'."
                    " Will use the default."
                );
            _maximum_transaction_size = DEFAULT_MAXIMUM_TRANSACTION_SIZE;
        }

        if ( _maximum_transaction_size < 1 ) {
            LOG_WARN_MSG(
                    "The maximum transaction size in the BG configuration file is not valid, it must be > 0."
                    " The value must be a number greater than zero, but is " << _maximum_transaction_size << "."
                    " Will use the default."
                );
            _maximum_transaction_size = DEFAULT_MAXIMUM_TRANSACTION_SIZE;
        }

    } catch ( exception& e ) {
        LOG_INFO_MSG( "Could not find maximum transaction size in properties file, will use the default." );
        _maximum_transaction_size = DEFAULT_MAXIMUM_TRANSACTION_SIZE;
    }


    if ( _workers < 1 ) {
        _workers = calculate_workers();
    }

    LOG_INFO_MSG(
"Configuration: \n"
"\tWorkers=" << _workers << "\n"
"\tDatabase name='" << _database_name << "'\n"
"\tDatabase user name=" << (_database_user ? string() + "'" + *_database_user + "'" : "not set") << "\n"
"\tDatabase schema=" << (_database_schema_name ? string() + "'" + *_database_schema_name + "'" : "not set") << "\n"
"\tMaximum transaction size=" << _maximum_transaction_size
        );
}


//-------------------------------------------------------------------------
// Static routines

unsigned calculate_workers()
{
  unsigned workers(boost::thread::hardware_concurrency());

  LOG_INFO_MSG( "Calculated number of workers as " << workers );

  return workers;
} // calculate_workers()


} // namespace realtime::server
} // namespace realtime
