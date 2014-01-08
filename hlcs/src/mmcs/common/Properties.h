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

/*!
 * \file Properties.h
 * \brief Conveys MMCS server startup options to other classes.
 */

#ifndef MMCS_COMMON_PROPERTIES_H_
#define MMCS_COMMON_PROPERTIES_H_

#include <utility/include/Properties.h>

#include <boost/utility.hpp>

#include <stdexcept>
#include <string>
#include <map>
#include <vector>

namespace mmcs {
namespace common {

class Subnet;

// various key values - case sensitive
#define MMCS_LOGDIR                     "log_dir"               // name of the I/O node logging directory, from --log-ionodes option
#define MMCS_VERSION                    "mmcs_version"          // version of the current bg_console, mmcs_server, mmcs_lite
#define MC_SERVER_IP                    "mc_server_ip"          // IP address for the mcServer
#define MC_SERVER_PORT                  "mc_server_port"        // IP port for the mcServer
#define BRINGUP_OPTS                    "bringupOptions"        // Bringup options for mc
#define BRINGUP                         "bringup"               // Bringup options for mc
#define MMCS_PROCESS                    "mmcs_process"          // Type of MMCS process: bg_console, mmcs_server, mmcs_lite
#define DFT_TGTSET_TYPE                 "dft_tgtset_type"       // default targetset type
#define POLL_ENVS                       "poll_envs"
#define BLOCKGOVERNOR_MAX_CONCURRENT    "max_concurrent"        // number of block transactions that can be running at one time
#define BLOCKGOVERNOR_INTERVAL          "interval"              // length of a measurement interval in seconds
#define BLOCKGOVERNOR_MAX_TRAN_RATE     "max_tran_rate"         // number of block transactions that can be started in one interval
#define BLOCKGOVERNOR_REPEAT_INTERVAL   "repeat_interval"       // minimum seconds between repeat of a transaction
#define MMCS_BOOT_OPTIONS               "mmcs_boot_options"     // options to pass to mc on each boot request
#define MMCS_ENVS_SC_INTERVAL_SECONDS   "sc_interval_seconds"
#define MMCS_ENVS_IO_INTERVAL_SECONDS   "io_interval_seconds"
#define MMCS_ENVS_NC_INTERVAL_SECONDS   "nc_interval_seconds"
#define WAIT_BOOT_FREE_TIME             "wait_boot_free_time"
#define POLL_DB                         "poll_db"
#define RECONNECT_BLOCKS                "reconnect_blocks"
#define FILE_EXIT                       "eof_exit"
#define MASTER_MON                      "master_mon"
#define AUTO_RECONNECT                  "AUTO_RECONNECT"
#define MMCS_PERFDATA_INTERVAL_SECONDS  "perfdata_interval_seconds"
#define MMCS_ENVS_BULK_INTERVAL_SECONDS "bulk_interval_seconds"
#define MMCS_ENVS_COOLANT_INTERVAL_SECONDS  "coolant_interval_seconds"
#define MMCS_ENVS_OPT_INTERVAL_SECONDS  "optical_interval_seconds"
#define NO_SHELL                        "no_shell"
#define FREE_IO_TARGETS                 "free_io_targets"
#define MINIMUM_BOOT_WAIT               "minimum_boot_wait"
#define CN_BOOT_SLOPE                   "cn_boot_slope"
#define IO_BOOT_SLOPE                   "io_boot_slope"
#define THREAD_STACK_SIZE               "thread_stack_size"


class Properties : boost::noncopyable
{
public:
    enum object {
        lite,
        console,
        server
    };

public:
    static std::string& getProperty(const char* key) { return (*getProp())[std::string(key)]; }
    static std::string& getProperty(const std::string& key) { return (*getProp())[key]; }
    static void setProperty(const char* key, const std::string& value);
    static void addExternal(const std::string& name, const std::string& description);
    static void init(const std::string& filename, object mmcs = server);
    static void read(object mmcs, bool bail);
    static void reload();
    static int buildSubnets();
    static int getSubnets(std::string& hwstring);
    static void getSubnetNames(std::vector<std::string>& subnet_names);
    static bgq::utility::Properties::Ptr getProperties();
    static void setProperties(bgq::utility::Properties::Ptr p) { _properties = p; }
    typedef std::map<std::string, std::string> Map;

    static Map* getExternalCmds();
private:
    static Map* getProp();
private:
    static Map* _prop;
    static Map _externalCmds;
    static bgq::utility::Properties::Ptr _properties;
public:
    static std::vector<Subnet> _subnets;
};

} } // namespace mmcs::common

#endif
