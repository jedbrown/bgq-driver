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
#include "client/options/ParseEnvironment.h"

#include "common/logging.h"

#include <boost/algorithm/string.hpp>

#include <boost/bind.hpp>

#include <iostream>

namespace runjob {
namespace client {
namespace options {

ParseEnvironment::ParseEnvironment(
        boost::program_options::options_description& options,
        boost::program_options::variables_map& vm
        ) :
    _options( options )
{
    try {
        boost::program_options::store(
                boost::program_options::parse_environment(
                    options,
                    boost::bind(
                        &ParseEnvironment::convert,
                        this,
                        _1
                        )
                    ),
                vm
                );
    } catch ( const boost::program_options::error& e ) {
        std::cerr << e.what() << std::endl;
        throw;
    }
}

std::string
ParseEnvironment::convert(
        const std::string& env
        )
{
    std::string result;

    // basic idea here is to match all environment variables
    // starting with RUNJOB_ and ignore others but log a warning message
    if ( !env.compare(0, 7, "RUNJOB_") ) {
        // strip RUNJOB_ from value and convert to lowercase
        std::string arg = env.substr(7, std::string::npos);
        boost::to_lower(arg);

        // replace all underscores with dashes, a dash is not a valid environment variable
        // identifier but it is used in several parameter names
        boost::replace_all( arg, "_", "-" );

        // search our options_descriptions for this arg
        //
        // I have no idea what this approx arg is for, the documentation
        // for program_options does not describe it
        bool approx = true;
        const boost::program_options::option_description* option = _options.find_nothrow( arg, approx );
        if ( option ) {
            result = option->long_name();
        } else {
            // not a valid RUNJOB_ environment variable, ignore it
        }
    }

    return result;
}

} // options
} // client
} // runjob
