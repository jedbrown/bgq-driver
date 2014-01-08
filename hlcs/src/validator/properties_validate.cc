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
#include <utility/include/cxxsockets/exception.h>
#include <utility/include/cxxsockets/ListeningSocket.h>
#include <utility/include/cxxsockets/SecureTCPSocket.h>
#include <utility/include/cxxsockets/SockAddr.h>
#include <utility/include/portConfiguration/ClientPortConfiguration.h>
#include "../master/server/MasterController.h"
#include "../master/server/LockFile.h"
#include "../master/lib/exceptions.h"

#include "mmcs/common/Properties.h"


// Bogus value to make BGMaster.o happy.
LockFile* lock_file;
bgq::utility::Properties::Ptr props;
bool dynamic = false;

// Global list of valid servers.
std::vector<std::string> valid_server_names;

void server_names() {
    std::cerr << "Valid server names are:" << std::endl;
    BOOST_FOREACH(std::string curr_server, valid_server_names) {
        std::cout << curr_server << std::endl;
    }
}


void help() {
    std::cout << "properties_validate examines the sections of the bg.properties file" << std::endl;
    std::cout << "related to server configuration.  It ensures that the" << std::endl;
    std::cout << "configuration options that must be present are there and that " << std::endl;
    std::cout << "dependencies are met.  Any failures detected here would also cause" << std::endl;
    std::cout << "servers to fail on startup."  << std::endl << std::endl;
    std::cout << "By default, configuration for all supported servers is checked." << std::endl;
    std::cout << "The --server|-s option allows specification of a single server." << std::endl;
}

bool checkPorts(std::string section, std::string port_type, bool server = true) {
    bgq::utility::PortConfiguration::Pairs portpairs;
    std::string bogus_port = "31337";
    CxxSockets::Error sockerr;
    bool success = false;
    if(!server) { // Going to be a client
        // Needs to get master location from properties and command line
        bgq::utility::ClientPortConfiguration port_config(bogus_port,
                                                          bgq::utility::ClientPortConfiguration::ConnectionType::Command);
        port_config.setProperties( props, section);
        port_config.notifyComplete();
        portpairs = port_config.getPairs();
        // It's a client.  We want to try to connect.
        BOOST_FOREACH(bgq::utility::PortConfiguration::Pair portpair, portpairs) {
            std::cerr << "Attempting to connect to " << portpair.first << ":" << portpair.second << std::endl;
            try {
                CxxSockets::SockAddr remote(AF_UNSPEC, portpair.first, portpair.second);
                CxxSockets::SecureTCPSocketPtr sock;
                if(sock) {
                    sock->Connect(remote, port_config);
                    success = true;
                } else return false;
            } catch(CxxSockets::Error& e) {
                std::cerr << "Failed to bind." << std::endl;
                sockerr = e;
            }
        }

    } else {
        // Needs to get master location from properties and command line
        bgq::utility::ServerPortConfiguration port_config(bogus_port, port_type, port_type);
        port_config.setProperties( props, section);
        port_config.notifyComplete();
        portpairs = port_config.getPairs();
        // For a server, just see if we can bind, then close.
        BOOST_FOREACH(bgq::utility::PortConfiguration::Pair portpair, portpairs) {
            std::cerr << "Attempting to listen on " << portpair.first << ":" << portpair.second << std::endl;
            try {
                CxxSockets::SockAddr local(AF_UNSPEC, portpair.first, portpair.second);
                CxxSockets::ListeningSocket sock(local, 1);
                success = true;
            } catch(CxxSockets::Error& e) {
                std::cerr << "Failed to listen." << std::endl;
                sockerr = e;
            }
        }

    }
    if(success == false) {
        std::cerr << "Failed to bind on any portpairs." << sockerr.what() << std::endl;
        return false;
    } else
        return true;
}

bool doBGMaster() {
    std::cout << "Evaluating bgmaster properties...." << std::endl;
    std::string logger("master");
    MasterController* bgm = 0;
    bgm = new(MasterController);

    std::ostringstream failmsg;
    try {
        bgm->buildPolicies(failmsg);
    } catch (exceptions::ConfigError& e) {
        std::cerr << "BGmaster Configuration error detected. " << e.errcode << " " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    } catch (std::runtime_error& e) {
        std::cerr << "BGmaster Configuration error detected. " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Try master logdir.
    std::string master_logdir = props->getValue("master.server", "logdir");
    if(access(master_logdir.c_str(), R_OK|W_OK) < 0) {
        std::cerr << "Directory " << master_logdir
                  << " inaccessible to this user.  Make sure "
                  << "that it exists and that bgmaster_server's user ID can reach it." << std::endl;
        exit(EXIT_FAILURE);
    }

    if(dynamic) {
        bool good = false;
        good = checkPorts("master.server", "agent");
        if(good == false) return false;
        good = checkPorts("master.client", "client", false);
        return good;
    }
    return true;
}

bool doMMCS() {
    std::cout << "Evaluating mmcs properties...." << std::endl;
    std::string logger("mmcs");
    mmcs::common::Properties mmcsprops;
    mmcsprops.setProperties(props);
    mmcs::common::Properties::object server_object = mmcs::common::Properties::server;
    mmcs::common::Properties::object console_object = mmcs::common::Properties::console;
    try {
        mmcsprops.read(server_object, true);
        mmcsprops.read(console_object, true);
    } catch (mmcs::common::Properties::PropertiesException& e) {
        std::cerr << "MMCS Configuration error detected. " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    return true;
}

// These are special rules that may cross components.
bool doOthers() {
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
    } catch(const std::invalid_argument& e) {
        // We're supposed to reach here. Just catch and be quiet.
    }

    // Now get the alias list from master.binmap
    bgq::utility::Properties::Section binmap = props->getValues("master.binmap");
    // Finally, make sure that each subnet name is in master.binmap.
    BOOST_FOREACH(std::string& curr_subnet, subnet_names) {
        bool found = false;
        BOOST_FOREACH(bgq::utility::Properties::Pair& key_val, binmap) {
            if(key_val.first == curr_subnet) found = true;
        }
        if(!found) {
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

int main(int argc, const char** argv) {
    std::vector<std::string> servers;
    valid_server_names.push_back("bgmaster_server");
    valid_server_names.push_back("mmcs_server");

    namespace po = boost::program_options;
    po::options_description options;
    options.add_options()
        ("help,h", po::bool_switch(), "this help text")
        ("server,s", po::value(&servers), "server name")
        ;

    po::options_description hidden;
    hidden.add_options()
        ("dynamic,d", po::bool_switch(&dynamic), "validate port configurations")
        ;

    // add properties and verbose options
    bgq::utility::Properties::ProgramOptions propertiesOptions;
    propertiesOptions.addTo( options );
    bgq::utility::LoggingProgramOptions lpo( "ibm" );
    lpo.addTo( options );

    po::options_description both;
    both.add( options );
    both.add( hidden );

    // parse --properties before everything else
    try {
        po::command_line_parser cmd_line( argc, const_cast<char**>(argv) );
        cmd_line.allow_unregistered();
        cmd_line.options( both );
        po::variables_map vm;
        po::store( cmd_line.run(), vm );
        po::notify( vm );

        // create properties and initialize logging
        props = bgq::utility::Properties::create( propertiesOptions.getFilename() );
        bgq::utility::initializeLogging(*props, lpo);
    } catch(const std::runtime_error& e) {
        std::cerr << "Error reading properties: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    // parse the rest of the args
    po::variables_map vm;
    po::command_line_parser cmd_line( argc, const_cast<char**>(argv) );
    cmd_line.options( both );
    try {
        po::store( cmd_line.run(), vm );

        // notify variables_map that we are done processing options
        po::notify( vm );
    } catch ( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        exit( EXIT_FAILURE );
    }

    if ( vm["help"].as<bool>() ) {
        std::cout << argv[0] << std::endl;
        std::cout << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << options << std::endl;
        help();
        server_names();
        exit(EXIT_SUCCESS);
    }

    std::cout << "validating " << props->getFilename() << std::endl;

    if(servers.size() == 0) {
        servers = valid_server_names;
    }

    BOOST_FOREACH(std::string curr_server, servers) {
        if(curr_server == "bgmaster_server") {
            if(doBGMaster() == false) {
                std::cerr << "Error on dynamic bgmaster configuration check." << std::endl;
                exit(EXIT_FAILURE);
            }
        } else if(curr_server == "mmcs_server") {
            if(doMMCS() == false) {
                std::cerr << "Error on dynamic mmcs configuration check." << std::endl;
                exit(EXIT_FAILURE);
            }
        } else {
            std::cerr << "Invalid server name \"" << curr_server << "\".  ";
            server_names();
            exit(EXIT_FAILURE);
        }
    }
    if(doOthers() == false) {
        exit(EXIT_FAILURE);
    }
    std::cout << "No errors detected for servers ";
    BOOST_FOREACH(std::string curr_server, servers) {
        std::cout << curr_server << " ";
    }
    std::cout << std::endl;
}
