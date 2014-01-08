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

#ifndef MASTER_BGMASTER_CLIENT_API_H_
#define MASTER_BGMASTER_CLIENT_API_H_


#include "../common/Ids.h"
#include "../common/types.h"

#include <utility/include/Properties.h>

#include <utility/include/portConfiguration/PortConfiguration.h>

#include <map>
#include <string>
#include <vector>


//! \brief client API for BGMaster.
class BGMasterClient
{
private:
    const ClientProtocolPtr _prot;

public:
    //! \brief Constructor
    BGMasterClient();

    //! \brief Connect to bgmaster_server and initiate protocol
    //! \param props Properties
    //! \param portpairs host:port pairs to use to connect to bgmaster_server.
    //! \returns Id assigned to this client
    ClientId connectMaster(
            const bgq::utility::Properties::ConstPtr& props,
            const bgq::utility::PortConfiguration::Pairs& portpairs
            ) const;

    //! \brief Tell bgmaster_server to start a specific binary based on its configured alias
    //! Starts all servers if an empty alias string is specified.
    //! \param alias Description used in config file for the binary
    //! \param loc Optional pointer to an ID object for the agent on which you wish to start the binary.
    //! \return A binary ID object for the successfully started binary.
    BinaryId start(const std::string& alias, const BGAgentId* loc = 0) const;

    //! \brief blocking wait for the specified binary to terminate
    //! \param bid The binary to wait for.
    //! \return The exit status for the binary.
    int wait_for_terminate(BinaryId bid) const;

    //! \brief blocking wait for the specified alias to =start=
    //! \param alias Name of the alias to wait for as defined in the properties file
    //! \param timeout Time to wait.
    //! \return The ID of the binary associated with the started alias
    BinaryId alias_wait(const std::string& alias, const unsigned timeout) const;

    //! \brief Cause all binaries matching 'alias' to shut down
    //! \param alias Description used in config file for the binary.
    //! \param errormsg Returns with 'ok' or an error message from bgmaster_server
    void stop(const std::string& alias, const int signal, std::string& errormsg) const;

    //! \brief Cause a specific binary to shut down by binary id
    //! \param id Specific binary ide to shut down
    //! \param errormsg Returns with 'ok' or an error message from bgmaster_server
    void stop(const BinaryId& id, const int signal, std::string& errormsg) const;

    //! \brief Cause all binaries matching all 'aliases' to shut down
    //! \param aliases Vector of aliases to stop
    //! \param errormsg Returns with 'ok' or an error message from bgmaster_server
    void stop(const std::vector<std::string>& aliases, const int signal, std::string& errormsg) const;

    //! \brief Cause a set of binaries to shut down by binary id
    //! \param ids Vector of binary ids to stop
    //! \param errormsg Returns with 'ok' or an error message from bgmaster_server
    void stop(const std::vector<BinaryId>& ids, const int signal, std::string& errormsg) const;

    //! \brief Cause a set of binaries to shut down by a set of aliases and a set of ids
    //! \param ids vector of binary ids to stop
    //! \param aliases Vector of aliases to stop
    //! \param errormsg Returns with 'ok' or an error message from bgmaster_server
    void stop(const std::vector<BinaryId>& ids, const std::vector<std::string>& aliases, const int signal, std::string& errormsg) const;

    //! \brief Get the current contents of bgmaster_server's error buffer
    //! \param empty vector of strings to be filled.
    void get_errors(std::vector<std::string>& error_vec) const;

    //! \brief Get the current contents of bgmaster_server's history buffer
    //! \param history_vec empty vector of strings to be filled.
    void get_history(std::vector<std::string>& history_vec) const;

    //! \brief Force selected binaries to fail over according to policy
    //! \param bins vector of binary ids to move.
    //! \param trigger Trigger to use for the failover event.
    //! Any binaries that do not (or no longer) exist are returned in the same vector.
    void fail_over(std::vector<BinaryId>& bins, const std::string& trigger) const;

    //! \brief Force bgmaster to attempt to end, gracefully first then forcefully
    //! \param master_only Only end bgmaster_server, not managed binaries.
    //! \param signal Signal to send to kill managed binaries.
    //  Note:  This does NOT kill associated agents.  Just the bgmaster_server.
    void end_master(bool master_only, int signal = 15) const;

    //! \brief Get status of specified binaries (or all if empty).
    //! \param ids Map.  Associated statuses will be filled out for supplied binary ids.
    void status(std::map<BinaryId, boost::shared_ptr<BinaryController>, Id::Comp>& ids) const;

    //! \brief Get the status of bgmaster_server
    //! \param start_time is when bgmaster_server started.
    //! \param version is the version string for bgmaster_server
    //! \param properties is the properties path for bgmaster_server
    //! \returns pid of bgmaster_server
    int master_status(std::string& start_time, std::string& version, std::string& properties) const;

    //! \brief Get a list of the aliases without running binaries.
    //! \param aliases is an empty vector that is returned filled with the alias list.
    void idle_aliases(std::vector<std::string>& aliases) const;

    //! \brief Get a map of all bgagents and their binaries
    //! \param amap Map of all agents and their associated binary controllers will be returned here.
    void get_agents(std::map<BGAgentId, std::vector<BinaryControllerPtr>, Id::Comp>& amap) const;

    //! \brief Get all attached clients
    //! \param clients Vector of all client ids attached to bgmaster_server.
    typedef std::string ClientUID;
    typedef std::map<ClientId, ClientUID> ClientAndUserMap;
    void get_clients(ClientAndUserMap& clients) const;

    //! \brief reload the config file
    //! \param config_file Full path to the new config file (optional)
    void reload_config(const std::string& config_file) const;

    //! \brief monitor bgmaster_server for events for the life of the client process.
    void event_monitor() const;

    //! \brief end bgmaster_server monitor
    void end_monitor() const;

    typedef std::string LogName;
    typedef std::string LogLevel;

    //! \brief Change/display bgmaster_server logging levels.
    void log_level(
            const std::vector<std::string>& input,      //!< [in] loggers to change
            std::map<LogName, LogLevel>& loglevels      //!< [out] current logging levels
            ) const;
};

#endif
