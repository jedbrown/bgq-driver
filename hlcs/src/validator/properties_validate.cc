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

#include <iostream>
#include <sstream>
#include <boost/scope_exit.hpp>
#include <boost/foreach.hpp>
#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/portConfiguration/ClientPortConfiguration.h>
#include "../master/server/MasterController.h"
#include "../master/server/LockFile.h"
#include "../master/lib/exceptions.h"
#include "../mmcs/common/Properties.h"


// Bogus value to make bgmaster translation units happy.
LockFile* lock_file;
bgq::utility::Properties::Ptr props;

// Global list of valid servers.
std::vector<std::string> valid_server_names;

void
server_names()
{
    std::cerr << "Valid server names are:" << std::endl;
    BOOST_FOREACH(const std::string& curr_server, valid_server_names) {
        std::cout << curr_server << std::endl;
    }
}


void
help()
{
    std::cout << "properties_validate examines the sections of the bg.properties file" << std::endl;
    std::cout << "related to server configuration.  It ensures that the" << std::endl;
    std::cout << "configuration options that must be present are there and that " << std::endl;
    std::cout << "dependencies are met.  Any failures detected here would also cause" << std::endl;
    std::cout << "servers to fail on startup."  << std::endl << std::endl;
    std::cout << "By default, configuration for all supported servers is checked." << std::endl;
    std::cout << "The --server|-s option allows specification of specific servers." << std::endl;
}

void
doBGMaster()
{
    std::cout << "Evaluating bgmaster properties...." << std::endl;
    std::string logger("master");
    MasterController bgm( props );

    std::ostringstream failmsg;
    try {
        bgm.buildPolicies(failmsg);
    } catch (const exceptions::ConfigError& e) {
        std::cerr << "BGmaster Configuration error detected. " << e.errcode << " " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    } catch (const std::runtime_error& e) {
        std::cerr << "BGmaster Configuration error detected. " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void
doMMCS()
{
    std::cout << "Evaluating mmcs properties...." << std::endl;
    std::string logger("mmcs");
    mmcs::common::Properties mmcsprops;
    mmcsprops.setProperties(props);
    mmcs::common::Properties::object server_object = mmcs::common::Properties::server;
    mmcs::common::Properties::object console_object = mmcs::common::Properties::console;
    try {
        mmcsprops.read(server_object, true);
        mmcsprops.read(console_object, true);
    } catch (const std::runtime_error& e) {
        std::cerr << "MMCS Configuration error detected. " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

bool
doOthers()
{
    std::cout << "Evaluating other rules..." << std::endl;
    // First rule is that all subnets must match their bgmaster alias name to their
    // machinecontroller name.
    std::vector<std::string> subnet_names;
    int i = 0;
    try {
        while(true) {  // Exception will end this loop.
            std::string subnet = "machinecontroller.subnet.";
            subnet += boost::lexical_cast<std::string>(i);
            std::string subnet_id = props->getValue(subnet, "Name");
            subnet_names.push_back(subnet_id);
            ++i;
        }
    } catch (const std::invalid_argument& e) {
        // We're supposed to reach here. Just catch and be quiet.
    }

    // Now get the alias list from master.binmap
    bgq::utility::Properties::Section binmap = props->getValues("master.binmap");
    // Finally, make sure that each subnet name is in master.binmap.
    BOOST_FOREACH(const std::string& curr_subnet, subnet_names) {
        bool found = false;
        BOOST_FOREACH(const bgq::utility::Properties::Pair& key_val, binmap) {
            if (key_val.first == curr_subnet) found = true;
        }
        if (!found) {
            std::cerr << "Error:" << std::endl;
            std::cerr << curr_subnet << " not found in [master.binmap] section.  "
                      << "Check bg.properties to ensure that " << curr_subnet
                      << " is configured to start a SubnetMc process and that "
                      << "its alias matches its subnet ID." << std::endl;
            return false;
        }
    }
    return true;
}

int
main(int argc, const char** argv)
{
    std::vector<std::string> servers;
    valid_server_names.push_back("bgmaster_server");
    valid_server_names.push_back("mmcs_server");

    namespace po = boost::program_options;
    po::options_description options;
    options.add_options()
        ("help,h", po::bool_switch(), "this help text")
        ("server,s", po::value(&servers), "server name")
        ;

    // add properties and verbose options
    bgq::utility::Properties::ProgramOptions propertiesOptions;
    propertiesOptions.addTo( options );
    bgq::utility::LoggingProgramOptions lpo( "ibm" );
    lpo.addTo( options );

    po::positional_options_description positionals;
    positionals.add( "properties", 1 );

    // parse --properties before everything else
    try {
        po::command_line_parser cmd_line( argc, const_cast<char**>(argv) );
        cmd_line.allow_unregistered();
        cmd_line.options( options );
        cmd_line.positional( positionals );
        po::variables_map vm;
        po::store( cmd_line.run(), vm );
        po::notify( vm );

        // create properties and initialize logging
        props = bgq::utility::Properties::create( propertiesOptions.getFilename() );
        bgq::utility::initializeLogging(*props, lpo);
    } catch ( const std::runtime_error& e ) {
        std::cerr << "Error reading properties: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    // parse the rest of the args
    po::variables_map vm;
    po::command_line_parser cmd_line( argc, const_cast<char**>(argv) );
    cmd_line.options( options );
    try {
        po::store( cmd_line.run(), vm );

        // notify variables_map that we are done processing options
        po::notify( vm );
    } catch ( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        exit( EXIT_FAILURE );
    }

    if ( vm["help"].as<bool>() ) {
        std::cout << argv[0] << " [path to properties file]" << std::endl;
        std::cout << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << options << std::endl;
        help();
        server_names();
        exit(EXIT_SUCCESS);
    }

    std::cout << "validating " << props->getFilename() << std::endl;

    if (servers.size() == 0) {
        servers = valid_server_names;
    }

    BOOST_FOREACH(const std::string& curr_server, servers) {
        if (curr_server == "bgmaster_server") {
            doBGMaster();
        } else if (curr_server == "mmcs_server") {
            doMMCS();
        } else {
            std::cerr << "Invalid server name \"" << curr_server << "\".  ";
            server_names();
            exit(EXIT_FAILURE);
        }
    }
    if (!doOthers()) {
        exit(EXIT_FAILURE);
    }
    std::cout << "No errors detected for servers ";
    BOOST_FOREACH(const std::string& curr_server, servers) {
        std::cout << curr_server << " ";
    }
    std::cout << std::endl;
}
