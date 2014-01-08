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

#ifndef _BGMASTER_H
#define _BGMASTER_H

#ifndef _SOCKETTYPES_H
#include "SocketTypes.h"
#endif

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <hlcs/include/c_api/BGMasterAgentProtocolSpec.h>
#include "BGMasterAgentProtocol.h"
#include "BGMasterClientProtocol.h"
#include "BGAgentRep.h"
#include "BinaryController.h"
#include "Policy.h"
#include "LockingRingBuffer.h"

class Alias;
class Host;
class DBUpdater;

// * Member variables: Leading underscore, all lower case.  _my_variable.
// * Types: Start with cap, cap for each word.  MyClass
// * Methods: Start with lower case, cap each following word.  doSomethingCool()
// * Setters/getters: lower case with underscores.  verb followed by accessed variable. set_my_var()
// * Non-locking internal methods: Same as external method with _nl suffix.
// * API calls that are analogs to program commands use command naming convention and mirror
//   the command names.  All lowercase separated by underscores.

#define LOGGING_DECLARE_ID_MDC(value) \
    log4cxx::MDC _location_mdc( "ID", std::string("{") + boost::lexical_cast<std::string>(value) + "} " );

typedef boost::shared_ptr<BGAgentRep> BGAgentRepPtr;
typedef boost::shared_ptr<boost::thread> ThreadPtr;

//! There are two classes of "clients" that are completely different
//! in function and behavior.  There are "agents" and "clients".  "Agents"
//! are process monitors running remotely or locally.  "Clients" are
//! programs that query BGMaster for process status or issue control commands.

//! \brief BGMasterController is the main data structure in BGMaster.  There's only
//! one and it contains some static members for managing agents and clients.
class BGMasterController {
public:
    static std::string _version_string;
    //! \brief Registrar joins new BGAgents and clients
    //! Registrar process new clients, validates them and
    //! starts new threads within the client manager
    class Registrar {
    public:
        Registrar();
        ~Registrar();
        void run(bool agent);
        bool get_failed() { return _failed; }
        void cancel();
    private:
        //! \brief Waits for new incoming connections.
        void listenForNew(bgq::utility::PortConfiguration::Pairs& portpairs);

        //! \brief Validate a new connection and put it in the agent vector.
        void processNew(CxxSockets::SecureTCPSocketPtr sock);

        //! \brief Listen for new agents in this thread.
        boost::thread _listenerThread;

        //! \brief listener for new connections
        CxxSockets::PollingListenerSetPtr _registrationListener;
        bool _end;
        bool _failed;
        pthread_t _my_tid;
    };
    static Registrar _agent_registrar;
    static Registrar _client_registrar;
#ifdef WITH_DB
    static DBUpdater _updater;
#endif

    //! \brief ClientManager class maintains a list of all of the BGMaster clients
    class ClientManager {
    public:

        //! \brief One ClientController for each client connected to BGMaster.
        //!
        //! Client Controller owns the sockets connected to the client and all
        //! messages to and from the client flow through it.
        class ClientController : public boost::enable_shared_from_this<ClientController> {

            //! \brief Protocol management object
            BGMasterClientProtocolPtr _prot;
            bool _ending;

            //! \brief my id remote host:port
            ClientId _client_id;

            //! \brief Thread to wait for requests
            boost::thread _client_socket_poller;

            //! \brief my socket poller thread id
            pthread_t _my_tid;

            //! \brief Administrative or Normal user.
            CxxSockets::UserType _utype;

            //! \brief process a start request
            void doStartRequest(BGMasterClientProtocolSpec::StartRequest startreq);
            //! \brief process an agent list request
            void doAgentRequest(BGMasterClientProtocolSpec::AgentlistRequest agentreq);
            //! \brief process a wait request
            void doWaitRequest(BGMasterClientProtocolSpec::WaitRequest waitreq);
            //! \brief process a client list request
            void doClientsRequest(BGMasterClientProtocolSpec::ClientsRequest clientreq);
            //! \brief process a stop request
            void doStopRequest(BGMasterClientProtocolSpec::StopRequest stopreq);
            //! \brief process a status request
            void doStatusRequest(BGMasterClientProtocolSpec::StatusRequest statreq);
            //! \brief process an exit status request
            void doExitRequest(BGMasterClientProtocolSpec::ExitStatusRequest exitreq);
            //! \brief process a die request
            void doEnd_agentRequest(BGMasterClientProtocolSpec::End_agentRequest end_agentreq);
            //! \brief process a request to terminate bgmaster_server
            void doTermRequest(BGMasterClientProtocolSpec::TerminateRequest termreq);
            //! \brief process a request to reload config
            void doReloadRequest(BGMasterClientProtocolSpec::ReloadRequest relreq);
            //! \brief process a request to execute a failover
            void doFailRequest(BGMasterClientProtocolSpec::FailoverRequest failreq);
            //! \brief process a server status request
            void doMasterStatRequest(BGMasterClientProtocolSpec::MasterstatRequest statreq);
            //! \brief wait for an alias to start
            void doAliasWaitRequest(BGMasterClientProtocolSpec::Alias_waitRequest waitreq);
            //! \brief get the errors from the ring buffer
            void doErrorsRequest(BGMasterClientProtocolSpec::Get_errorsRequest& error_req);
            //! \brief get the history from the ring buffer
            void doHistoryRequest(BGMasterClientProtocolSpec::Get_historyRequest& history_req);
            //! \brief Start an event/error monitor
            void doMonitorRequest(BGMasterClientProtocolSpec::MonitorRequest& monreq);
            //! \brief End an event/error monitor
            void doEndmonitorRequest(BGMasterClientProtocolSpec::EndmonitorRequest& endmonreq);
            //! \brief Change/get log levels
            void doLoglevelRequest(BGMasterClientProtocolSpec::LoglevelRequest& loglevreq);
            //! \brief Get list of idle aliases
            void doGetidleRequest(BGMasterClientProtocolSpec::GetidleRequest& idlereq);
        public:

            ClientController() : _ending(false) {}
            ClientController(BGMasterClientProtocolPtr& prot,
                             std::string& ipaddr, int port);

            ~ClientController();

            //! \brief clean up client
            void cancel();

            //! \brief Poll the socket for new messages from bgmaster
            void waitMessages();

            //! \brief Start the thread that polls the socket for messages
            void startPoller();

            void processRequest();

            void setUserType(CxxSockets::UserType utype) { _utype = utype; }
            std::string getUserId() { return _prot->getResponder()->getUserId().getUser(); }
            ClientId get_client_id() { return _client_id; }
        };

        typedef boost::shared_ptr<ClientController> ClientControllerPtr;

        ClientManager();

        void startup() {}

        //! \brief end thread processing of all clients
        void cancel();

        //! \brief Add a new client to the list
        void addClient(ClientControllerPtr c);

        //! \brief Remove a client from the list
        void removeClient(ClientControllerPtr c);
        std::vector<ClientControllerPtr> getClients() { return _clients; }
        bool _ending;
    private:
        std::vector<ClientControllerPtr> _clients;
    };

    //! One instance.  Manages all bgagents
    class BGAgentManager {

    public:
        BGAgentManager();

        void startup() {}

        //! \brief set the number of agents we are allowed to have for each host
        //! \param count number of agents
        void setCount(unsigned count) { _agents_per_host = count; }

        //! \brief End processing of all agent threads
        //! \param end_agents Also end all of the agent processes
        //! \param end_binaries Also end all managed binaries
        //! \param signal to use to end binaries
        void cancel(const bool end_agents, const bool end_binaries, const unsigned signal);

        //! \brief Insert a new agent in the list
        bool addNew(BGAgentRepPtr& agent);

        //! \brief Agent is gone.  Pull it out
        void removeAgent(BGAgentRepPtr& agent);

        //! \brief Figure out which agent to run the bin
        BGAgentRepPtr pickAgent();

        //! \brief Find an agent id associated with a host
        BGAgentId findAgentId(const Host& host);

        //! \brief Find a particular agent
        BGAgentRepPtr findAgentRep(BGAgentId& aid);

        //! \brief Find an agent by the host name
        BGAgentRepPtr findAgentRep(Host& host);

        //! \brief Find a specific binary by id
        bool findBinary(const BinaryId& id, BinaryLocation& loc);

        //! \brief Find all binaries by alias
        bool findBinary(const std::string& alias, std::vector<BinaryLocation>& loc);

        //! \brief Agents are ending
        bool get_ending_agents() { return _ending_agents; }
        
        //! \brief get a COPY of the agent vector.  
        std::vector<BGAgentRepPtr> get_agent_list() { return _agents; }

    private:

        //! \brief vector of BGAgents
        std::vector<BGAgentRepPtr> _agents;
        boost::mutex _agent_manager_mutex;
        bool _ending_agents;
        unsigned int _agents_per_host;
    };

    //! brief constructor must start all main threads.
    BGMasterController();
    ~BGMasterController() {}

    void startup(bgq::utility::Properties::Ptr& props);
    static void putRAS(unsigned int id, std::map<std::string, std::string>& details);
    static bool get_master_terminating() { return _master_terminating; }
    static bool get_end_requested() { return _end_requested; }

    //! \brief log error message and save it to the ring buffer
    //! \param msg the error message the handle
    static void handleErrorMessage(std::string msg);
    //! \brief get the current messages in the ring buffer
    //! \param messages A vector of messages to be filled and returned
    static void getErrorMessages(std::vector<std::string>& messages);
    static void addHistoryMessage(std::string message);
    static void getHistoryMessages(std::vector<std::string>& messages);
    static void stopThreads(const bool end_agents, const bool end_binaries, const unsigned signal);
    static void setProps(bgq::utility::Properties::Ptr& p) { _master_props = p; }

    //! \brief Build the policies from the config files by calling the various internal methods.
    static void buildPolicies(std::ostringstream& failmsg);
    static boost::posix_time::ptime get_start_time() { return _start_time; }
    static BGAgentManager& get_agent_manager() { return _agent_manager; }
    static ClientManager& get_client_manager() { return _client_manager; }
    static std::string& get_master_logdir() { return _master_logdir; }
    static AliasList _aliases;
private:
    //! Parse alias args from config file.
    static void buildArgs(bgq::utility::Properties::Section& args);

    //! Parse the hosts in the config and add them to the correct Alias.
    static void buildHostList(bgq::utility::Properties::Section& hosts,
                              std::vector<std::string>& exclude_list,
                              std::ostringstream& failmsg);
    //! Parse the instances in the config and add them to the correct Alias.
    static void buildInstances(bgq::utility::Properties::Section& instances,
                               std::vector<std::string>& exclude_list,
                               std::ostringstream& failmsg);
    //! Parse the failover behaviors.
    static void buildFailover(bgq::utility::Properties::Section& failover,
                              std::multimap<Policy::Trigger, Policy::Behavior>& behaviors);
    //! Add the failover behaviors to the correct Aliases.
    static void addBehaviors(bgq::utility::Properties::Section& failmap,
                             std::multimap<Policy::Trigger, Policy::Behavior>& behaviors,
                             std::ostringstream& failmsg);

    //! Build the list of alias to start by default
    static void buildStartList(bgq::utility::Properties::Section& startlist);

    //! Map defined user ids to aliases
    static void buildUidList(bgq::utility::Properties::Section& uidlist);

    //! Map defined user ids to aliases
    static void buildLogDirs(bgq::utility::Properties::Section& logdirs);

    //! \bug Cannot start servers configured with "localhost" or "127.0.0.1" for some reason.
    static void waitHosts();

    static void startServers(std::map<std::string,std::string>& failed_aliases, BGAgentRepPtr agent);

    static ClientManager _client_manager;
    static BGAgentManager _agent_manager;

    static std::vector<boost::shared_ptr<Alias> > _start_list;
    static std::vector<Host> _configured_hosts;

    //! \brief policy builder mutex
    static boost::mutex _policy_build_mutex;

    //! \brief static properties object
    static bgq::utility::Properties::Ptr _master_props;

    //! \brief static end flag
    static bool _master_terminating;
    //! \brief This just means that an end request has arrived.
    static bool _end_requested;
    static bool _start_servers;
    static std::string _master_logdir;
    static boost::barrier _start_barrier;
    static pid_t _master_pid;
    static bool _master_db;
    static bool _stop_once;
    static bool _start_once;
    //! \brief error message ring buffer
    static LockingStringRingBuffer _err_buff;
    //! \brief history ring buffer
    static LockingStringRingBuffer _history_buff;
    static boost::posix_time::ptime _start_time;
    static std::vector<BGMasterClientProtocolPtr> _monitor_prots;
};

#endif
