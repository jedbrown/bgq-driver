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

#ifndef MASTER_MASTER_CONTROLLER_H_
#define MASTER_MASTER_CONTROLLER_H_


#include "Behavior.h"
#include "DBUpdater.h"
#include "LockingRingBuffer.h"
#include "Policy.h"
#include "types.h"

#include <utility/include/Properties.h>

#include <utility/include/cxxsockets/Host.h>

#include <boost/date_time.hpp>
#include <boost/thread.hpp>

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

#include <sys/types.h>


// * Member variables: Leading underscore, all lower case.  _my_variable.
// * Types: Start with cap, cap for each word.  MyClass
// * Methods: Start with lower case, cap each following word.  doSomethingCool()
// * Setters/getters: lower case with underscores.  verb followed by accessed variable. set_my_var()
// * Non-locking internal methods: Same as external method with _nl suffix.
// * API calls that are analogs to program commands use command naming convention and mirror
//   the command names.  All lowercase separated by underscores.


//! There are two classes of "clients" that are completely different
//! in function and behavior.  There are "agents" and "clients".  "Agents"
//! are process monitors running remotely or locally.  "Clients" are
//! programs that query BGMaster for process status or issue control commands.

//! \brief MasterController is the main data structure in BGMaster.  There's only
//! one and it contains some static members for managing agents and clients.
class MasterController
{
public:
    static std::string _version_string;

    static Registrar _agent_registrar;
    static Registrar _client_registrar;
    static DBUpdater _updater;

    MasterController(
            const bgq::utility::Properties::Ptr& properties
            );

    void startup(int signal_fd);
    static void putRAS(unsigned int id, const std::map<std::string, std::string>& details);

    static bool get_master_terminating() { return _master_terminating; }
    static void set_master_terminating()  { _master_terminating = true; }

    static bool get_end_requested() { return _end_requested; }
    static void set_end_requested()  { _end_requested = true; }

    static std::vector<ClientProtocolPtr>& get_monitor_prots()  { return _monitor_prots; }

    //! \brief log error message and save it to the ring buffer
    //! \param msg the error message the handle
    static void handleErrorMessage(const std::string& msg);
    //! \brief get the current messages in the ring buffer
    //! \param messages A vector of messages to be filled and returned
    static void getErrorMessages(std::vector<std::string>& messages);
    static void addHistoryMessage(const std::string& message);
    static void getHistoryMessages(std::vector<std::string>& messages);
    static void stopThreads(bool end_binaries, int signal);
    static const bgq::utility::Properties::Ptr& getProps()  { return _props; }

    static bool isStartServers() { return _start_servers; }

    static void startServers(std::map<std::string,std::string>& failed_aliases, AgentRepPtr agent);

    static void waitStartBarrier()  { _start_barrier.wait(); }

    //! \brief Build the policies from the config files by calling the various internal methods.
    static void buildPolicies(std::ostringstream& failmsg);
    static const boost::posix_time::ptime& get_start_time() { return _start_time; }
    static AgentManager& get_agent_manager() { return _agent_manager; }
    static ClientManager& get_client_manager() { return _client_manager; }
    static const std::string& get_master_logdir() { return _master_logdir; }
    static AliasList _aliases;

    //! \brief monitor_prots container mutex
    static boost::mutex _monitor_prots_mutex;

private:
    //! Parse alias args from config file.
    static void buildArgs(const bgq::utility::Properties::Section& args);

    //! Parse the hosts in the config and add them to the correct Alias.
    static void buildHostList(const bgq::utility::Properties::Section& hosts,
                              const std::vector<std::string>& exclude_list,
                              std::ostringstream& failmsg);
    //! Parse the instances in the config and add them to the correct Alias.
    static void buildInstances(const bgq::utility::Properties::Section& instances,
                               const std::vector<std::string>& exclude_list,
                               std::ostringstream& failmsg);
    //! Parse the failover behaviors.
    static void buildFailover(const bgq::utility::Properties::Section& failover,
                              std::multimap<Policy::Trigger, Behavior>& behaviors);
    //! Add the failover behaviors to the correct Aliases.
    static void addBehaviors(const bgq::utility::Properties::Section& failmap,
                             std::multimap<Policy::Trigger, Behavior>& behaviors);

    //! Build the list of alias to start by default
    static void buildStartList(const bgq::utility::Properties::Section& startlist);

    //! Map defined user ids to aliases
    static void buildUidList(const bgq::utility::Properties::Section& uidlist);

    //! Map defined user ids to aliases
    static void buildLogDirs(const bgq::utility::Properties::Section& logdirs);

    static ClientManager _client_manager;
    static AgentManager _agent_manager;

    //! \brief policy builder mutex
    static boost::mutex _policy_build_mutex;

    //! \brief properties object
    static bgq::utility::Properties::Ptr _props;

    //! \brief static end flag
    static bool _master_terminating;
    //! \brief This just means that an end request has arrived.
    static bool _end_requested;
    static bool _start_servers;
    static std::string _master_logdir;
    static boost::barrier _start_barrier;
    static bool _master_db;
    static bool _stop_once;
    static bool _start_once;
    //! \brief error message ring buffer
    static LockingStringRingBuffer _err_buff;
    //! \brief history ring buffer
    static LockingStringRingBuffer _history_buff;
    static boost::posix_time::ptime _start_time;
    static std::vector<ClientProtocolPtr> _monitor_prots;

private:
    const pid_t _pid;
};

#endif
