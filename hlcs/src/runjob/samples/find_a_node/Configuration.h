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
#ifndef RUNJOB_SAMPLE_FIND_A_NODE_CONFIGURATION_H_
#define RUNJOB_SAMPLE_FIND_A_NODE_CONFIGURATION_H_

#include <utility/include/Properties.h>

#include <string>

namespace find_a_node {

class Configuration {
public:
    static const std::string DefaultLocation;
    static const std::string EnvironmentVariable;

public:
    Configuration();

    const std::string& getValue(
            const std::string& section,
            const std::string& key
            ) const
    {
        return _file->getValue( section, key );
    }

private:
    std::string _location;
    bgq::utility::Properties::ConstPtr _file;
};

} // find_a_node

#endif

