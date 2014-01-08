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

/*!
 * \mainpage BGmaster
 * \author IBM
 *
 * Copyright IBM Corp. 2010, 2012
 *
 * \section toc Table of Contents
 *
 * - \ref introduction
 * - \ref quick_start
 * - \ref concepts
 * - \ref commands
 * - \ref policies
 * - \ref other_options
 * - \ref ras
 *
 * \section introduction Introduction
 * The Blue Gene/Q distributed process manager, known as BGmaster, consists of three components,
 * bgmaster_server, bgagentd, and several client commands for interacting with bgmaster_server.
 * BGmaster is responsible for not only managing the distributed mc_server components,
 * it continues to serve to manage all other control system components as its predecessors
 * bgpmaster and bglmaster have.
 * \section quick_start Quick Start
 * This section will give you enough information to start your own bgmaster_server and bgagentd
 * and run a simulator environment. Advanced configurations for failover are not covered here.
 * - Set unique listeners: BGmaster now supports the common server port configuration
 *    syntax. It needs two listeners defined. One for client connections and one for bgagentd
 *    connections. They are defined in the [master] section as follows:
 * \code
 *    [master.server]
 *    agent_listen_ports=bgqfen1:32041
 *    client_listen_ports=bgqfen1:32042
 *
 *    [master.agent]
 *    host=bgqfen1:32041
 *
 *    [master.client]
 *    host=bgqfen1:32042
 * \endcode
 *  Choose a unique listening port for each.
 * - Set a directory for ouptut logging. Stderr and stdout from all of your servers
 *   will be piped to files generated in this directory. This will be in the [master.server]
 *   section as well. bgagentd also has its own logging directory specification. So
 *   add that in the [master.agent] section. The format looks like this in both sections:
 * \code
 *   logdir=/path/to/log/directory
 * \endcode
 * - Define an alias for each server you plan on running in the [master.binmap] section.
 *   This includes bgmaster_server itself which must be defined as "bgmaster". An alias
 *   is more than just a short name for an executable. It is a representation of an executable
 *   and the set of policies and arguments with which it is associated.
 * \code
 * bgmaster=/bglhome/willstoc/bgq-head/bgq/work/hlcs/sbin/bgmaster_server
 * mc_server=/bglhome/willstoc/bgq-head/bgq/work/control/sbin/mc_serversim_64
 * mmcs_server=/bglhome/willstoc/bgq-head/bgq/work/hlcs/sbin/mmcs_server
 * \endcode
 * - If you wish to pass command line arguments to any of your servers, they are added in
 *   the [master.binargs] section
 * \code
 * mmcs_server=--test 30003
 * \endcode
 * - The [master.startup] section has options to allow you to automatically start
 *   your servers when you start bgmaster
 * \code
 *  [master.startup]
 * # startup options for bgmaster
 * # Option to start selected servers when bgmaster starts.
 * start_servers=true
 * # comma separated list of aliases to start serially
 * start_order=mc_server,mmcs_server
 * \endcode
 * - Start an agent process.
 * \code
 * > cd <driverpath>/hlcs/sbin
 * > ./bgagentd
 * \endcode
 * - Start bgmaster_server
 * \code
 * > ./master_start bgmaster
 * \endcode
 * - If you have configured your servers to start automatically, they should both start.
 *   They can also be started with the ./master_start command and stopped with ./master_stop.
 *   The complete set of commands are listed in the SEE ALSO section of the bgmaster_server
 *   man page.
 * \code
 * > man /bgsys/drivers/ppcfloor/hlcs/man/man8/bgmaster_server.8
 * \endcode
 *
 * \section concepts Concepts
 * - Binary:  A binary is a single instance of an executable under the management of BGmaster. A
 *            binary has a unique id formulated from its IP address and process id separated by
 *            a colon. Running binary status can be queried, waited on, or stopped by commands
 *            and through the bgmaster programming API.
 * - Policy: A policy is a set of triggers and their associated behaviors, and allowable number of
 *           instances.
 *  <ul>
 *    <li> Trigger: A trigger is limited set of events that can cause a policy to enact a particular
 *                 behavior. The list of acceptable triggers are "killed", "binary" or "agent"
 *         <ul>
 *           <li> killed: This means that a binary has received a kill signal.
 *           <li> binary: This means that the binary abnormally ended
 *           <li> agent:  The agent managing the binary failed.
 *                 "Killed" and "binary" are distinguished by the signal with which they fail.
 *                  SIGTERM and SIGSTOP, for example, are "killed" triggers. SIGSEGV and SIGABRT
 *                  are considered "binary" triggers.
 *         </ul>
 *    <li>  A behavior is an action, and instructions for that action, to take when a trigger condition
 *       is detected. The components of a behavior are an "action", a retry count and an optional
 *       set of failover pairs. The supported actions are:
 *         <ul>
 *           <li> failover: Analyze the failover pairs and if the triggered binary is running on
 *                          the first host in a pair, fail it over to the second.
 *           <li> restart:  Attempt to restart the binary on the same host up to the number of times
 *                          specified in the retry count parameter.
 *           <li> cleanup:  Simply clean up internal data structures and log the error. (This is the
 *                          default.)
 *         </ul>
 *  </ul>
 * - Alias:  An alias is a short name that represents a specific executable, associated policy
 *           and its pairs of triggers and behaviors, the number of instances that it may run,
 *           the hosts on which it may run, and references the binaries that are running
 *           in association with it. \n
 * Below is a graphical representation of an Alias and its associated concepts including the list
 * of configured hosts to use, the list of actively running binaries, and the policy with its
 * associated trigger/behavior pairs.
 * \image html Alias_etc.png
 *
 * \section commands Commands
 * These are the available BGmaster commands. See the man pages for details.
 * - alias_wait:  Wait for a binary associated with a specified alias to start.
 * - binary_status:  Returns the status of one or all binaries currently under control of bgmaster_server.
 * - binary_wait:  Waits for a binary associated with the specified alias to become active.
 * - fail_over:  Force a specific binary id to be killed and restarted on another node.
 * - get_errors:  Returns up to 25 items in bgmaster_server's ring buffer of logged errors.
 * - list_agents:  Returns  a list of IDs for active agents and their associated running binaries.
 * - list_clients:  Returns a list of client IDs for active clients.
 * - master_start:  Start a controlled process or bgmaster_server.
 * - master_status:  Check to see if bgmaster_server is running.
 * - master_stop:   Stop a controlled process, bgmaster_server, or any or all bgagentd processes.
 * - refresh_config:  Start a controlled process or bgmaster_server.
 *
 * \section other_options Additional Options
 * - [master] section \n
 *                    \n
 * \code
 * db=true|false
 * # If this is set to "true", bgmaster_server will attempt to connect to the database and issue
 * # ras messages for starting, stopping, restarting, failure detection and other events for its
 * # own components and the ones it is managing.
 * \endcode
 *
 * \section policies Advanced Policy Configuration
 * Policy configuration is complex and flexible. They are documented in the bg.properties template
 * which is reproduced here.
 * \code
 * [master.policy.host_list]
 * # List of hosts on which the binary associated with the alias
 * # may start. Comma separated
 * # my_alias=bgqfen1.rchland.ibm.com,bgqfen2.rchland.ibm.com
 * mmcs_server=bgqfen1
 * mc_server=bgqfen1
 *
 * [master.policy.instances]
 * # policies for the number of instances of each alias are allowed
 * # valid entries are 0 through 65535
 * mc_server=1
 * mmcs_server=10
 *
 * [master.policy.failure]
 * # Formatted failover policy.
 * #
 * # Terminology:
 * # killed:        binary received a kill signal (not a stop message through bgmaster)
 * # binary:        binary failed
 * # agent:         the monitoring agent failed
 * # failover:      run the binary that failed on the specified backup host
 * # restart:       restart the binary on the same host
 * # cleanup:       just clean up internal structures and let the binary go
 * # retries:       number of times to attempt to restart the binary
 * # failover_from: when it fails on this host...
 * # failover_to:   where you want the failover to go
 * # trigger:       event that causes a policy to be activated. It can be "killed", "binary", or "agent".
 * #
 * # Each line has a policy_name as a key followed by a formatted policy.
 * # The policy consists of a trigger and a complex behavior.
 * # The complex behavior consists of a trigger, an action, a retry count and an
 * # optional set of '|' separated failover pairs.
 * #
 * # Format: policy_name=trigger,[failover|restart|cleanup],<retries>,<failover_from:failover_to>|etc...
 * #
 * # Example: foo=binary,failover,2,bgqfen2.rchland.ibm.com:bgqfen1.rchland.ibm.com|bgqfen2.rchland.ibm.com:bgqfen1.rchland.ibm.com
 * #
 * # The 'foo' policy will failover to bgqfen1.rchland.ibm.com if the associated
 * # binary dies on bgqfen2.rchland.ibm.com
 * # and vice versa. It will retry two times.
 * # If the agent dies, it will not failover.
 * # If failover is specified and no host is specified, it will fail over
 * # to any other available host
 * # In the examples below, a restart and a bidirectional failover policy are started.
 * sleeper_restart=killed,restart,2
 * sleeper_failover=binary,failover,2,bgqfen1.rchland.ibm.com:bgqfen2.rchland.ibm.com|bgqfen2.rchland.ibm.com:bgqfen1.rchland.ibm.com
 *
 * [master.policy.map]
 * # map named policies to an alias
 * # In this example, there is an alias that is mapped to a restart and a failover policy
 * # that are previously defined.
 * sleeper=sleeper_restart,sleeper_failover
 *
 * \endcode
 *
 * \section ras RAS Messages
 * BGmaster emits RAS messages for several events. These include the following:
 * - 00030000: "bgmaster_server has been started in process $(PID)"
 * - 00030001: "bgmaster_server process $(PID) stopped"
 * - 00030002: "bgmaster_server started binary $(BIN)"
 * - 00030003: "bgmaster_server stopped binary $(BIN)"
 * - 00030004: "bgmaster_server has detected a failure of binary $(BIN) with signal $(SIGNAL) and exit status $(ESTAT)"
 * - 00030005: "bgmaster_server has executed a restart policy for alias $(ALIAS)"
 * - 00030006: "bgmaster_server has executed a failover policy for alias $(ALIAS) from $(SOURCE) to $(TARGET)"
 * - 00030007: "bgmaster_server has detected a failure of bgagentd $(AGENT_ID)"
 * - 00030008: "bgmaster_server has been requested to end bgagentd $(AGENT_ID)"
 * - 00030009: "bgmaster_server process $(PID) has failed with signal $(SIGNAL)"
 *
 */
