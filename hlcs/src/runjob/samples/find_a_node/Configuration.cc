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

#include "logging.h"

LOG_DECLARE_FILE( find_a_node::log );

namespace find_a_node {

const std::string Configuration::DefaultLocation = "/bgsys/local/etc/find_a_node.properties";
const std::string Configuration::EnvironmentVariable = "FIND_A_NODE_CONFIGURATION";

Configuration::Configuration() :
    _location( DefaultLocation ),
    _file()
{
    if ( getenv(EnvironmentVariable.c_str()) ) {
        _location = getenv( EnvironmentVariable.c_str() );
    }
    LOG_DEBUG_MSG( "using configuration: " << _location );
    _file = bgq::utility::Properties::create( _location );
}

} // find_a_node
