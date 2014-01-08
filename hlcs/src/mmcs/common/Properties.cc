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

#include "Properties.h"

#include "Subnet.h"

#include <control/include/mcServer/defaults.h>

#include <utility/include/Properties.h>
#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>

namespace mmcs {
namespace common {

// static storage
Properties::Map* Properties::_prop = NULL;
Properties::Map Properties::_externalCmds;
bgq::utility::Properties::Ptr Properties::_properties;
std::vector<Subnet> Properties::_subnets;

LOG_DECLARE_FILE( "mmcs.common" );

void
Properties::init(
        const std::string& filename,
        object mmcs
        )
{
    // Create property object
    _properties = bgq::utility::Properties::create(filename);

    try {
        Properties::read( mmcs, true );
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void
Properties::reload()
{
    read(Properties::server, false);
}

int
Properties::buildSubnets()
{
    LOG_TRACE_MSG(__FUNCTION__);
    int i = 0;
    try {
        while (true) {
            std::string subnet = "machinecontroller.subnet.";
            std::string prop_key = "Name.";
            subnet += boost::lexical_cast<std::string>(i);
            prop_key += boost::lexical_cast<std::string>(i);
            Subnet mmcs_subnet(_properties, subnet);
            _subnets.push_back(mmcs_subnet);
            mmcs_subnet.output();
            ++i;
        }
    } catch (const std::invalid_argument& e) {
        LOG_DEBUG_MSG("Reached end of subnet list: " << e.what());
    }
    return 0;
}

int
Properties::getSubnets(
        std::string& hwstring
        )
{
    int i = 0;
    try {
        bool init = false; // Are we setting up properties or just getting a list of subnet hardware?
        if (hwstring == "init") {
            init = true;
        }

        if (init) buildSubnets();

        bool first = true;
        while (true) {  // Exception will end this loop.
            std::string subnet = "machinecontroller.subnet.";
            std::string prop_key = "Name.";
            subnet += boost::lexical_cast<std::string>(i);
            prop_key += boost::lexical_cast<std::string>(i);
            const std::string subnet_id = _properties->getValue(subnet, "Name");
            const std::string hw = _properties->getValue(subnet, "HardwareToManage");
            if (init) {
                LOG_DEBUG_MSG("Adding subnet " << prop_key << ":" << subnet_id << " to properties.");
                setProperty(prop_key.c_str(), subnet_id);
                LOG_DEBUG_MSG("Adding subnet hardware " << subnet_id << ":" << hw << " to properties.");
                setProperty(subnet_id.c_str(), hw);
            } else {
                if (first)
                    hwstring += hw;
                else
                    hwstring = hwstring + "," + hw;
                first = false;
            }
            ++i;
        }
    } catch (const std::invalid_argument& e) {
        LOG_DEBUG_MSG("Reached end of subnet list: " << e.what());
    }
    return i;
}

void
Properties::getSubnetNames(
        std::vector<std::string>& subnet_names
        )
{
    int i = 0;
    try {
        while (true) {  // Exception will end this loop.
            std::string subnet = "machinecontroller.subnet.";
            std::string prop_key = "Name.";
            subnet += boost::lexical_cast<std::string>(i);
            prop_key += boost::lexical_cast<std::string>(i);
            std::string subnet_id = _properties->getValue(subnet, "Name");
            subnet_names.push_back(subnet_id);
            ++i;
        }
    } catch (const std::invalid_argument& e) {
        LOG_DEBUG_MSG("Reached end of subnet list: " << e.what());
    }
}

bool
validate_key(
        const std::string& key,
        const std::string& val
        )
{
    try {
        if (key == WAIT_BOOT_FREE_TIME                ||
            key == MINIMUM_BOOT_WAIT                  ||
            key == MMCS_ENVS_SC_INTERVAL_SECONDS      ||
            key == MMCS_ENVS_IO_INTERVAL_SECONDS      ||
            key == MMCS_ENVS_NC_INTERVAL_SECONDS      ||
            key == MMCS_ENVS_BULK_INTERVAL_SECONDS    ||
            key == MMCS_ENVS_COOLANT_INTERVAL_SECONDS ||
            key == MMCS_ENVS_OPT_INTERVAL_SECONDS     ||
            key == MMCS_PERFDATA_INTERVAL_SECONDS     ||
            key == BLOCKGOVERNOR_MAX_CONCURRENT       ||
            key == BLOCKGOVERNOR_INTERVAL             ||
            key == BLOCKGOVERNOR_MAX_TRAN_RATE        ||
            key == BLOCKGOVERNOR_REPEAT_INTERVAL      ||
            key == "connection_pool_size") {
            // Make sure a number is a number.
            boost::lexical_cast<unsigned int>(val);
            if (val[0] == '-')  // And that it isn't negative.
                throw std::exception();
        }
        if (key == CN_BOOT_SLOPE || key == IO_BOOT_SLOPE) {
            // And a floater floats
            boost::lexical_cast<float>(val);
        }
        if (key == RECONNECT_BLOCKS ||
            key == MASTER_MON       ||
            key == FREE_IO_TARGETS) {
            // A boolean is boolean
            if (val != "true" && val != "false")
                throw std::exception();
        }
    } catch (const std::exception& e) {
        LOG_ERROR_MSG("Invalid value " << val << " for key " << key);
        throw std::runtime_error( "Invalid value " + val + " for key " + key );
    }
    return true;
}

void
Properties::read(
        const object mmcs,
        const bool bail
        )
{
    // Vector of the sections we like
    typedef bgq::utility::Properties::Section Section;
    typedef std::vector<Section> Sections;
    Sections sections;

    if (mmcs == server || mmcs == console) {
        // mmcs section
        Section mmcs_props = _properties->getValues("mmcs");
        sections.push_back(mmcs_props);
    }

    if (mmcs == console) {
        // bg_console
        Section bg_console_props = _properties->getValues("bg_console");
        sections.push_back(bg_console_props);

        // External Commands don't get added to properties, they get put in the properties' external
        // command list.
        Section bg_console_commands;
        try {
            bg_console_commands = _properties->getValues("bg_console.external.commands");
        } catch (const std::invalid_argument& e) {
            if (bail) {
                // No second chances if we're set to get out on error!
                throw std::runtime_error(e.what());
            }
            // Bad section.  Try the old name.
            std::cerr << "Properties section not found. Trying obsolete section name \"commands\". Please update properties to current template." << std::endl;
            bg_console_commands = _properties->getValues("commands");
        }
        for (Section::const_iterator it = bg_console_commands.begin(); it != bg_console_commands.end(); ++it) {
            Properties::addExternal(it->first, it->second);
        }

        Section command_cats;
        try {
            command_cats = _properties->getValues("bg_console.external.command.categories");
        } catch (const std::invalid_argument& e) {
            if (bail) {
                // No second chances if we're set to get out on error!
                throw std::runtime_error(e.what());
            }
            // Bad section.  Try the old name.
            std::cerr << "Properties section not found. Trying obsolete section name \"command.categories\".  Please update properties to current template." << std::endl;
            command_cats = _properties->getValues("command.categories");
        }
        sections.push_back(command_cats);
    }

    if (mmcs == lite) {
        // mmcs section
        Section mmcs_props = _properties->getValues("mmcs");
        sections.push_back(mmcs_props);

        // mmcs_lite section
        Section mmcs_lite_props = _properties->getValues("mmcs_lite");
        sections.push_back(mmcs_lite_props);

        // machinecontroller section
        Section mc_props = _properties->getValues("machinecontroller");
        sections.push_back(mc_props);

        try {
            assert(_prop != NULL);
            // get mcserver hostname and port
            const std::string host = _properties->getValue("machinecontroller", "hostName");
            const std::string port = _properties->getValue("machinecontroller", "clientPort");
            setProperty(MC_SERVER_IP, host);
            setProperty(MC_SERVER_PORT, port);

            // get compute rows and columns
            const std::string rows = _properties->getValue("database", "computeRackRows");
            const std::string columns = _properties->getValue("database", "computeRackColumns");
            setProperty("rackRows", rows);
            setProperty("rackColumns", columns);
        } catch (const std::invalid_argument& e) {
            LOG_WARN_MSG(e.what());
        }
    }

    if (mmcs == server) {
        // Set logdir by default without affecting mmcs_lite
        setProperty(MMCS_LOGDIR, "/bgsys/logs/BGQ");

        // mmcs governor section
        Section mmcs_governor_props;
        try {
             mmcs_governor_props = _properties->getValues("mmcs.blockgovernor");
        } catch (const std::invalid_argument& e) {
            throw std::runtime_error(e.what());
        }

        sections.push_back(mmcs_governor_props);

        // mmcs env mon section
        Section mmcs_env_props;
        try {
            mmcs_env_props = _properties->getValues("mmcs.envs");
        } catch (const std::invalid_argument& e) {
            throw std::runtime_error(e.what());
        }

        sections.push_back(mmcs_env_props);

        // Get mcserver hostname, port, security, and bringup.
        try {
            const std::string host = _properties->getValue("machinecontroller", "hostName");
            const std::string port = _properties->getValue("machinecontroller", "clientPort");
            setProperty(MC_SERVER_IP, host);
            setProperty(MC_SERVER_PORT, port);
            // get bringup options
            const std::string bringup_options = _properties->getValue("machinecontroller", "bringupOptions");
            setProperty(BRINGUP_OPTS, bringup_options);
            const std::string bringup = _properties->getValue("machinecontroller", "bringup");
            setProperty(BRINGUP, bringup);
        } catch (const std::invalid_argument& e) {
            throw std::runtime_error(e.what());
        }

        std::string init = "init";
        getSubnets(init);
    }

    // Iterate through our sections
    for (std::vector<Section>::const_iterator section = sections.begin(); section != sections.end(); ++section) {
        for (Section::const_iterator i = section->begin(); i != section->end(); ++i) {
            try {
                validate_key(i->first, i->second);
                Properties::setProperty(i->first.c_str(), i->second);
            } catch ( const std::exception& e ) {
                if (bail)
                    throw;
                // already logged error, continue on
            }
        }
    }

    if (mmcs == server && Properties::getProperty(BRINGUP) == "true" && Properties::getProperty(RECONNECT_BLOCKS) == "true") {
        throw std::runtime_error("Cannot reconnect to blocks if bringup=true in [machinecontroller] section.");
    }
    if (mmcs == server && Properties::getProperty(BRINGUP) == "false" && Properties::getProperty(RECONNECT_BLOCKS) == "false") {
        throw std::runtime_error("Incorrect configuration, [machinecontroller] bringup and [mmcs] reconnect_blocks cannot both be false.");
    }

    if (mmcs == lite) {
        // we never want an I/O node log directory, override whatever came from the properties file
        setProperty(MMCS_LOGDIR, "");
    }
}

Properties::Map*
Properties::getExternalCmds()
{
    return &_externalCmds;
}

std::map<std::string, std::string>*
Properties::getProp()
{
    if (!_prop)
    {
        _prop = new std::map<std::string, std::string>;
        _prop->insert(std::make_pair(WAIT_BOOT_FREE_TIME, "20"));         // 20 minute maximum
        _prop->insert(std::make_pair(MC_SERVER_IP, "127.0.0.1"));     // default mc server ip address
        _prop->insert(std::make_pair(MC_SERVER_PORT, boost::lexical_cast<std::string>(mc_server::ClientPort)));    // default mc server ip
        _prop->insert(std::make_pair(DFT_TGTSET_TYPE, "targetset=perm")); // default targetset type
        _prop->insert(std::make_pair(BLOCKGOVERNOR_MAX_CONCURRENT, "0"));    // disable block governor
        _prop->insert(std::make_pair(BLOCKGOVERNOR_INTERVAL, "0"));       // disable block governor
        _prop->insert(std::make_pair(BLOCKGOVERNOR_MAX_TRAN_RATE, "0"));  // disable block governor
        _prop->insert(std::make_pair(BLOCKGOVERNOR_REPEAT_INTERVAL, "0"));// disable block governor
        _prop->insert(std::make_pair(POLL_ENVS, "true"));
        _prop->insert(std::make_pair(FILE_EXIT, "false"));
        _prop->insert(std::make_pair(MMCS_LOGDIR, ""));
        _prop->insert(std::make_pair(NO_SHELL, "false"));
        _prop->insert(std::make_pair(MASTER_MON, "true"));
        _prop->insert(std::make_pair(FREE_IO_TARGETS, "true"));
        _prop->insert(std::make_pair(POLL_DB, "true"));
        _prop->insert(std::make_pair(RECONNECT_BLOCKS, "true"));
        _prop->insert(std::make_pair(MINIMUM_BOOT_WAIT, "60"));
        _prop->insert(std::make_pair(CN_BOOT_SLOPE, ".017"));
        _prop->insert(std::make_pair(IO_BOOT_SLOPE, ".017"));
        _prop->insert(std::make_pair("bypass_envs", "false"));
        _prop->insert(std::make_pair("bringup", "false"));
        _prop->insert(std::make_pair(THREAD_STACK_SIZE, "1048576"));
    }
    return _prop;
}

void
Properties::setProperty(
        const char* key,
        const std::string& value
        )
{
    LOG_DEBUG_MSG("Setting " << key << "=" << value);
    (*getProp())[std::string(key)] = value;
}

void
Properties::addExternal(
        const std::string& name,
        const std::string& description
        )
{
    LOG_DEBUG_MSG("Adding external command " << name << " " << description);
    _externalCmds[name] = description;
}

bgq::utility::Properties::Ptr
Properties::getProperties()
{
    return _properties;
}

} } // namespace mmcs::common
