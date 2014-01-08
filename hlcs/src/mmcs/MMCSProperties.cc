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
/* (C) Copyright IBM Corp.  2005, 2011                              */
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
#include <fstream>
#include <stdexcept>
#include "MMCSProperties.h"

#include <control/include/mcServer/MCServerPorts.h>

#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/cxxsockets/SocketTypes.h>
#include <utility/include/Properties.h>
#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>

// static storage
MMCSProperties::Map* MMCSProperties::_prop = NULL;
MMCSProperties::Map MMCSProperties::_externalCmds;
bgq::utility::Properties::Ptr MMCSProperties::_properties;
std::vector<MMCSSubnet> MMCSProperties::_subnets;

LOG_DECLARE_FILE( "mmcs" );

void
MMCSProperties::init(const std::string& filename, object mmcs)
{
    // Create property object
    _properties = bgq::utility::Properties::create(filename);

    // Initialize logging
    const std::string default_logger( "ibm.mmcs" );
    bgq::utility::LoggingProgramOptions logging_program_options( default_logger );
    bgq::utility::initializeLogging( *_properties, logging_program_options, "mmcs" );
    // Override 'mc' logger so we don't fill the log with messages from MCServerRef
    log4cxx::LevelPtr mclevel = log4cxx::Level::toLevel("ERROR");
    log4cxx::LoggerPtr mclogger = log4cxx::Logger::getLogger("ibm.mc");
    log4cxx::LoggerPtr mcreflogger = log4cxx::Logger::getLogger("ibm.mc.MCServerRef");
    mclogger->setLevel(mclevel);
    mcreflogger->setLevel(mclevel);

    // Initialize cxxsockets
    CxxSockets::setProperties(boost::const_pointer_cast<bgq::utility::Properties>(_properties));

    // Initialize RAS
    RasEventHandlerChain::setProperties(_properties);
    try {
        MMCSProperties::read( mmcs );
    } catch(MMCSProperties::MMCSPropertiesException& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void MMCSProperties::reload() {
    RasEventHandlerChain::reinitialized();
    RasEventHandlerChain::setProperties(_properties);
    RasEventHandlerChain::initChain();
    read(MMCSProperties::server, false);
}

int MMCSProperties::buildSubnets() {
    LOG_INFO_MSG(__FUNCTION__ );
    int i = 0;
    try {
        while(true) {
            std::string subnet = "machinecontroller.subnet.";
            std::string prop_key = "Name.";
            subnet += boost::lexical_cast<std::string>(i);
            prop_key += boost::lexical_cast<std::string>(i);
            MMCSSubnet mmcs_subnet(_properties, subnet);
            _subnets.push_back(mmcs_subnet);
            LOG_DEBUG_MSG(mmcs_subnet);
            ++i;
        }
    } catch (const std::invalid_argument& e) {
        LOG_DEBUG_MSG("Reached end of subnet list " << e.what());
    }
    return 0;
}

int MMCSProperties::getSubnets(std::string& hwstring) {
    int i = 0;
    try {
        bool init = false; // Are we setting up properties or just getting a list of subnet hardware?
        if(hwstring == "init") init = true;

        if(init) buildSubnets();

        bool first = true;
        while(true) {  // Exception will end this loop.
            std::string subnet = "machinecontroller.subnet.";
            std::string prop_key = "Name.";
            subnet += boost::lexical_cast<std::string>(i);
            prop_key += boost::lexical_cast<std::string>(i);
            std::string subnet_id = _properties->getValue(subnet, "Name");
            std::string hw = _properties->getValue(subnet, "HardwareToManage");
            if(init) {
                LOG_DEBUG_MSG("Adding subnet " << prop_key << ":" << subnet_id << " to properties.");
                setProperty(prop_key.c_str(), subnet_id);
                LOG_DEBUG_MSG("Adding subnet hardware " << subnet_id << ":" << hw << " to properties.");
                setProperty(subnet_id.c_str(), hw);
            } else {
                if(first) hwstring += hw;
                else hwstring = hwstring + "," + hw;
                first = false;
            }
            ++i;
        }
    } catch (const std::invalid_argument& e) {
        LOG_DEBUG_MSG("Reached end of subnet list " << e.what());
    }
    return i;
}

void MMCSProperties::getSubnetNames(std::vector<std::string>& subnet_names) {
    int i = 0;
    try {
        while(true) {  // Exception will end this loop.
            std::string subnet = "machinecontroller.subnet.";
            std::string prop_key = "Name.";
            subnet += boost::lexical_cast<std::string>(i);
            prop_key += boost::lexical_cast<std::string>(i);
            std::string subnet_id = _properties->getValue(subnet, "Name");
            subnet_names.push_back(subnet_id);
            ++i;
        }
    } catch (const std::invalid_argument& e) {
        LOG_DEBUG_MSG("Reached end of subnet list " << e.what());
    }
}

bool validate_key(std::string key, std::string val) {
    try {
        if(key == "shutdown_timeout"           ||
           key == "wait_boot_free_time"        ||
           key == "minimum_boot_wait"          ||
           key == "sc_interval_seconds"        ||
           key == "io_interval_seconds"        ||
           key == "nc_interval_seconds"        ||
           key == "bulk_interval_seconds"      ||
           key == "coolant_interval_seconds"   ||
           key == "optical_interval_seconds"   ||
           key == "perfdata_interval_seconds"  ||
           key == "max_concurrent"             ||
           key == "interval"                   ||
           key == "max_tran_rate"              ||
           key == "repeat_interval"            ||
           key == "connection_pool_size") {
            // Make sure a number is a number.
            boost::lexical_cast<unsigned int>(val);
            if(val[0] == '-')  // And that it isn't negative.
                throw std::exception();
        }
        if(key == "cn_boot_slope" ||
           key == "io_boot_slope") {
            // And a floater floats
            boost::lexical_cast<float>(val);
        }
        if(key == "reconnect_blocks" ||
           key == "runjob_connect"   ||
           key == "master_mon"       ||
           key == "free_io_targets") {
            // A boolean is boolean
            if(val != "true" && val != "false")
                throw std::exception();
        }
    } catch(std::exception& e) {
        LOG_ERROR_MSG("Invalid value " << val << " for key " << key);
        throw std::runtime_error( "Invalid value " + val + " for key " + key );
    }
    return true;
}

void
MMCSProperties::read(object mmcs, bool bail)
{
    // Vector of the sections we like
    std::vector<bgq::utility::Properties::Section> sections;

    if(mmcs == server || mmcs == console) {
        // mmcs section
        bgq::utility::Properties::Section mmcs_props = _properties->getValues("mmcs");
        sections.push_back(mmcs_props);
        bgq::utility::Properties::Section runjob_server_section = _properties->getValues("runjob.server");
        bool found = false;
        for(bgq::utility::Properties::Section::iterator it = runjob_server_section.begin();
            it != runjob_server_section.end(); ++it) {
            if(it->first == "command_listen_ports") {
                found = true;
                std::string clp = it->second;
                MMCSProperties::setProperty("RUNJOB_PORTS", clp);
            }
        }
        if(!found)  {
            std::ostringstream failmsg;
            failmsg << "No runjob server listener configured";
            LOG_WARN_MSG(failmsg.str());
            if(bail) throw MMCSPropertiesException(MMCSProperties::WARN, failmsg.str());
        }
    }

    if(mmcs == console) {
        // mmcs_console
        bgq::utility::Properties::Section mmcs_console_props = _properties->getValues("bg_console");
        sections.push_back(mmcs_console_props);
        for(bgq::utility::Properties::Section::iterator it = mmcs_console_props.begin();
            it != mmcs_console_props.end(); ++it) {
            if(it->first == "host") {
                std::string host = it->second;
                MMCSProperties::setProperty("HOST_PORTS", host);
            }
        }

        // External Commands don't get added to properties, they get put in the properties' external
        // command list.
        bgq::utility::Properties::Section mmcs_console_commands;
        try {
            mmcs_console_commands = _properties->getValues("bg_console.external.commands");
        } catch(std::invalid_argument& e) {
            if(bail) {
                // No second chances if we're set to get out on error!
                throw MMCSPropertiesException(MMCSProperties::WARN, e.what());
            }
            // Bad section.  Try the old name.
            std::cerr << "Properties section not found.  Trying obsolete section name \"commands\".  Please update properties to current template." << std::endl;
            mmcs_console_commands = _properties->getValues("commands");
        }
        for(bgq::utility::Properties::Section::iterator it = mmcs_console_commands.begin();
            it != mmcs_console_commands.end(); ++it) {
            std::string key = (*it).first;
            std::string value = (*it).second;
            LOG_DEBUG_MSG("Adding external command " << key << " " << value);
            MMCSProperties::addExternal(key, value);
        }

        bgq::utility::Properties::Section command_cats;
        try {
            command_cats = _properties->getValues("bg_console.external.command.categories");
        } catch(std::invalid_argument& e) {
            if(bail) {
                // No second chances if we're set to get out on error!
                throw MMCSPropertiesException(MMCSProperties::WARN, e.what());
            }
            // Bad section.  Try the old name.
            std::cerr << "Properties section not found.  Trying obsolete section name \"command.categories\".  Please update properties to current template." << std::endl;
            command_cats = _properties->getValues("command.categories");
        }
        sections.push_back(command_cats);
    }

    if(mmcs == lite) {
        // mmcs section
        bgq::utility::Properties::Section mmcs_props = _properties->getValues("mmcs");
        sections.push_back(mmcs_props);

        // mmcs_lite section
        bgq::utility::Properties::Section mmcs_lite_props = _properties->getValues("mmcs_lite");
        sections.push_back(mmcs_lite_props);

        // machinecontroller section
        bgq::utility::Properties::Section mc_props = _properties->getValues("machinecontroller");
        sections.push_back(mc_props);

        try {
            assert(_prop != NULL);
            // get mcserver hostname and port
            std::string host = _properties->getValue("machinecontroller", "hostName");
            std::string port = _properties->getValue("machinecontroller", "clientPort");
            setProperty(MC_SERVER_IP, host);
            setProperty(MC_SERVER_PORT, port);

            // get compute rows and columns
            std::string rows = _properties->getValue("database", "computeRackRows");
            std::string columns = _properties->getValue("database", "computeRackColumns");
            setProperty("rackRows", rows);
            setProperty("rackColumns", columns);
        } catch (const std::invalid_argument& e) {
            LOG_WARN_MSG(e.what());
        }
    }

    if(mmcs == server) {
        // Set logdir by default without affecting mmcs_lite
        setProperty(MMCS_LOGDIR, "/bgsys/logs/BGQ");

        // mmcs governor section
        bgq::utility::Properties::Section mmcs_governor_props;
        try {
             mmcs_governor_props = _properties->getValues("mmcs.blockgovernor");
        } catch (std::invalid_argument& e) {
            throw MMCSPropertiesException(MMCSProperties::FATAL, e.what());
        }

        sections.push_back(mmcs_governor_props);

        // mmcs env mon section
        bgq::utility::Properties::Section mmcs_env_props;
        try {
            mmcs_env_props = _properties->getValues("mmcs.envs");
        } catch (std::invalid_argument& e) {
            throw MMCSPropertiesException(MMCSProperties::FATAL, e.what());
        }

        sections.push_back(mmcs_env_props);

        // Get mcserver hostname, port, security, and bringup.
        try {
            std::string host = _properties->getValue("machinecontroller", "hostName");
            std::string port = _properties->getValue("machinecontroller", "clientPort");
            setProperty(MC_SERVER_IP, host);
            LOG_DEBUG_MSG("setting property mc_server_ip to " << host); 
            setProperty(MC_SERVER_PORT, port);
            LOG_DEBUG_MSG("setting property mc_server_port to " << port);
            // get bringup options
            std::string bringup_options = _properties->getValue("machinecontroller", "bringupOptions");
            LOG_DEBUG_MSG("setting property bringupOptions to " << bringup_options);
            setProperty(BRINGUP_OPTS, bringup_options);
            std::string bringup = _properties->getValue("machinecontroller", "bringup");
            setProperty(BRINGUP, bringup);
            LOG_DEBUG_MSG("setting property bringup to " << bringup);
        } catch (const std::invalid_argument& e) {
            throw MMCSPropertiesException(MMCSProperties::FATAL, e.what());
        }

        try {
            std::string secure = _properties->getValue("machinecontroller", "secure");
            setProperty(SECURE_MC, secure);
            LOG_DEBUG_MSG("setting property secure_mc to " << secure);
        } catch (const std::invalid_argument& e) {
            LOG_WARN_MSG("Could not get secure property from bg.properties." << e.what());
            setProperty(SECURE_MC, "true");
        }

        std::string init = "init";
        getSubnets(init);
    }

    // Iterate through our sections
    for(std::vector<bgq::utility::Properties::Section>::iterator section_it = sections.begin();
        section_it != sections.end(); ++section_it) {
        for(bgq::utility::Properties::Section::iterator it = (*section_it).begin();
            it != (*section_it).end(); ++it) {
            std::string key = (*it).first;
            std::string value = (*it).second;
            LOG_DEBUG_MSG("setting property " << key << " to value " << value);
            MMCSProperties::setProperty(key.c_str(), value);
            // Check integer properties for +/- validity.
            validate_key(key, value);
        }
    }

    if(mmcs == server && MMCSProperties::getProperty(BRINGUP) == "true" && MMCSProperties::getProperty(RECONNECT_BLOCKS) == "true") {
        throw MMCSPropertiesException(MMCSProperties::FATAL, "Cannot reconnect to blocks if bringup=true in [machinecontroller] section");
    }
    if(mmcs == server && MMCSProperties::getProperty(BRINGUP) == "false" && MMCSProperties::getProperty(RECONNECT_BLOCKS) == "false") {
        throw MMCSPropertiesException(MMCSProperties::FATAL, "Illegal configuration.  [machinecontroller] bringup and [mmcs] reconnect_blocks cannot both be false.");
    }

    if(mmcs == lite) {
        // we never want an I/O node log directory, override whatever came from the properties file
        setProperty(MMCS_LOGDIR, "");
    }
}

MMCSProperties::Map* MMCSProperties::getExternalCmds() {
    return &_externalCmds;
}

std::map<std::string, std::string>*
MMCSProperties::getProp()
{
    if (!_prop)
    {
        _prop = new std::map<std::string, std::string>;
        setProperty(SHUTDOWN_TIMEOUT, "90");         // default shutdown timeout is 90 seconds
        setProperty(LISTEN_PORTS, "127.0.0.1:32031");
        setProperty(HOST_PORTS, "127.0.0.1:32031");
        setProperty(MMCS_SERVER_IP, "127.0.0.1");     // default mmcs server ip address
        setProperty(WAIT_BOOT_FREE_TIME, "20");         // 20 minute maximum
        setProperty(NODE_TIME_MULTIPLIER, "120");       // 120 sec/ion default
        setProperty(MMCS_SERVER_PORT, "32031");     // default mmcs server ip
        setProperty(MC_SERVER_IP, "127.0.0.1");     // default mc server ip address
        setProperty(MC_SERVER_PORT, CLIENT_PORT_STR);    // default mc server ip
        setProperty(DFT_TGTSET_TYPE, "targetset=perm"); // default targetset type
        setProperty(BLOCKGOVERNOR_MAX_CONCURRENT, "0");    // disable block governor
        setProperty(BLOCKGOVERNOR_INTERVAL, "0");       // disable block governor
        setProperty(BLOCKGOVERNOR_MAX_TRAN_RATE, "0");  // disable block governor
        setProperty(BLOCKGOVERNOR_REPEAT_INTERVAL, "0");// disable block governor
        setProperty(MMCS_INTERNAL_RATIOS, "false");     // disable internal ratio support
        setProperty(POWER_CHECK_BYPASS, "false");       // Do NOT bypass the hardware power tests
        setProperty(POLL_ENVS, "true");
        setProperty(SECURE_CONSOLE, "true");
        setProperty(SECURE_MC, "false");
        setProperty(FILE_EXIT, "false");
        setProperty(MMCS_LOGDIR, "");
        setProperty(NO_SHELL, "false");
        setProperty(BOOT_COMPLETE, "true");
        setProperty(RUNJOB_CONNECT, "true");
        setProperty(MASTER_MON, "true");
        setProperty(FREE_IO_TARGETS, "true");
        setProperty(POLL_DB, "true");
        setProperty(RECONNECT_BLOCKS, "true");
        setProperty(MINIMUM_BOOT_WAIT, "60");
        setProperty(CN_BOOT_SLOPE, ".017");
        setProperty(IO_BOOT_SLOPE, ".017");
        setProperty("bypass_envs", "false");
        setProperty("bringup", "false");
    }
    return _prop;
}

void
MMCSProperties::setProperty(
        const char* key,
        const std::string& value
        )
{
    LOG_TRACE_MSG("setting " << key << "=" << value);
    (*getProp())[std::string(key)] = value;
}

bgq::utility::Properties::Ptr
MMCSProperties::getProperties()
{
    return _properties;
}
