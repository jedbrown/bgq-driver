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
 * \page jobsim Job Simulation Survival Guide
 *
 * \date Last Updated on September 7, 2011
 *
 * \version generated for driver DRIVER_NAME with revision VERSION_NUMBER.
 * 
 * \section toc Table of Contents
 *
 * - \ref Overview
 * - \ref simulation-setup
 * - \ref takeoff
 * - \ref blocks
 * - \ref running
 * - \ref limitations
 * - \ref cleanup
 * - \ref faq
 * - \ref moreinfo
 *
 * \section Overview
 *
 * Job simulation leverages the BG/Q database to simulate jobs running on essentially
 * any supported machine size.  It achieves this goal by simulating the I/O node software
 * stack with one process per I/O node.  With that in mind, even though it's conceivable
 * to simulate a large system, there are a number of limitations.
 *
 * \section simulation-setup Setup
 *
 * The following steps will need to be done only once.
 *
 * <ol>
 * <li> create your own bg.properties file </li>
 * \code
 * user@host ~> cp /bgsys/drivers/ppcfloor/utility/etc/bg.properties.ship.tpl ~/bg.properties
 * user@host ~> export BG_PROPERTIES_FILE=$HOME/bg.properties
 * \endcode
 *
 * <li> edit your bg.properties file</li>
 *     <ul>
 *     <li> add schema_name = user in the [database] section, where user is your username </li>
 *     <li> pick a machine size to simulate by changing the computeRackRows and computeRackColumns
 *          keys in the [database] section.
 *     <li> note the value of the max_user_processes key in the [%runjob.server] section, this
 *          is designed to prevent you from fork bombing a system with simulated I/O services
 *          processes forked from runjob_server.  There is no maximum value beyond what the
 *          linux kernel enforces for non-privileged users. If you change this value, ensure
 *          you know what you're doing.
 *     <li> change port numbers to be unique in the following sections </li>
 *     <ul>
 *          <li> [%runjob.server] command_listen_ports and mux_listen_ports </li>
 *          <li> [%runjob.mux] host should match the port from [%runjob.server] mux_listen_ports </li>
 *          <li> [%runjob.server.commands] host should match the port from [%runjob.server] command_listen_ports </li>
 *          <li> [%runjob.mux] local_socket should contain something specific to your uid.  For example:
 *               local_socket = runjob_mux.username </li>
 *     </ul>
 *     </ul>
 *     <li> add the following simulation settings in [runjob.server]
 *     <ul>
 *          <li> job_sim = true </li>
 *          <li> iosd_sim_path = /bgsys/drivers/ppcfloor/ramdisk/bin </li>
 *          <li> iosd_sim_name = start_job_simulation  </li>
 *          <li> iosd_id_counter_name = /tmp/runjob_server_iosd_counter </li>
 *          <li> iosd_id_counter_size = 2048 </li>
 *          <li> iosd_remove_log_files = true </li>
 *     </ul>
 *     </ul>
 *
 * <li> create a database schema </li>
 * \code
 * user@host ~> /bgsys/drivers/ppcfloor/db/schema/createBGQSchema /dbhome/bgqsysdb/sqllib bgdb0 
 * \endcode
 *
 * <li> populate your database schema </li>
 *
 * this will use the rackRows and rackColumns values that you specified in your 
 * bg.properties file to pick a machine configuration 
 * \code
 * user@host ~> source /dbhome/bgqsysdb/sqllib/db2profile
 * user@host ~> cd /bgsys/drivers/ppcfloor/db/schema
 * user@host schema> ./dbPopulate.pl --properties $HOME/bg.properties
 * \endcode
 *
 * \section takeoff Getting off the ground
 *
 * \subsection start servers
 * You'll need to start four servers for job simulation. The output should loosely resemble what is
 * shown below.
 * <ol>
 * <li> mc_serversim
 * \code
 * user@host ~> /bgsys/drivers/ppcfloor/control/sbin/mc_serversim 
 * 2010-08-07 09:16:37.369 (DEBUG) [] MCServerParams - MCServerParams setProperties
 * 2010-08-07 09:16:37.370 (DEBUG) [] MCServerParams - No CardThatTalksToMasterClockCard keyword was specified in the properties file.
 * 2010-08-07 09:16:37.370 (DEBUG) [] MCServerParams - No spill file directory specified
 * 2010-08-07 09:16:37.370 (DEBUG) [] MCServerParams - No bringup options specified
 * 2010-08-07 09:16:37.371 (WARN ) [] MCServerParams - Could not find section machinecontroller.subnet.1 in properties file /bglhome/samjmill/bg.properties
 * 2010-08-07 09:16:37.371 (WARN ) [] MCServerParams - ignoring subnet 1
 * 2010-08-07 09:16:37.371 (DEBUG) [] MCServerParams - Read 1 subnets
 * 2010-08-07 09:16:37.371 (INFO ) [queenBee] mcservermain - 
 * clientPort=1207
 * machineType=0
 * hostName=localhost
 * computeRacks=4
 * ioRacks=1
 * rows=2
 * columns=2
 * debug=99
 * autoinit=1
 * bringupOptions=
 * alteraPath=/bgsys/drivers/ppcfloor/bin/alteraImages
 * CardThatTalksToMasterClockCard=R00-M0-S
 * Subnet Row0
 *   PrimaryServer = localhost
 *   PrimaryServerInterface = eth1
 *   PrimaryServerPort = 33457
 *   PrimaryBGInterface = eth0
 *   BackupServer = localhost
 *   BackupServerInterface = eth1
 *   BackupServerPort = 33456
 *   BackupBGInterface = eth0
 *   HardwareToManage = R00, R01, R10, R11, Q01, 
 * 
 * 2010-08-07 09:16:37.371 (INFO ) [queenBee] mcservermain - Core limits: 0
 * 2010-08-07 09:16:37.371 (INFO ) [queenBee] mcservermain - File limits: 1024
 * 2010-08-07 09:16:37.376 (WARN ) [queenBee] performance - could not find key buffer_capacity in section performance
 * 2010-08-07 09:16:37.376 (WARN ) [queenBee] performance - using default capacity of 2048
 * 2010-08-07 09:16:37.377 (DEBUG) [queenBee] MCServer - MCServer initializing, version 3
 * 2010-08-07 09:16:37.377 (INFO ) [queenBee] mc - starting in simulation mode: 
 * 2010-08-07 09:16:37.377 (INFO ) [queenBee] mc - machine type=0
 * 2010-08-07 09:16:37.377 (INFO ) [queenBee] mc - rows=2, cols=2
 * 2010-08-07 09:16:37.377 (INFO ) [queenBee] mc - scPalFpgaImageFile=
 * 2010-08-07 09:16:37.377 (INFO ) [queenBee] mc - ncFpgaImageFile=
 * 2010-08-07 09:16:37.377 (INFO ) [queenBee] mc - icFpgaImageFile=
 * 2010-08-07 09:16:37.378 (INFO ) [queenBee] mc - Service Card Palomino FPGA images are being read from /bgsys/drivers/ppcfloor/bin/alteraImages/xxx.rbf
 * 2010-08-07 09:16:37.378 (INFO ) [queenBee] mc - Node Card FPGA images are being read from /bgsys/drivers/ppcfloor/bin/alteraImages/xxx.rbf
 * 2010-08-07 09:16:37.378 (INFO ) [queenBee] mc - IO board FPGA images are being read from /bgsys/drivers/ppcfloor/bin/alteraImages/xxx.rbf
 * 2010-08-07 09:16:37.378 (DEBUG) [queenBee] MCServer - MC initialized
 * 2010-08-07 09:16:37.378 (TRACE) [queenBee] mc - registering controlEventListener
 * 2010-08-07 09:16:37.378 (TRACE) [queenBee] mc - registering consoleEventListener
 * 2010-08-07 09:16:37.459 (DEBUG) [queenBee] MCServerThread - Created thread 4398102606256
 * 2010-08-07 09:16:37.459 (DEBUG) [queenBee] MCServer - bringup....
 * 2010-08-07 09:16:37.460 (DEBUG) [queenBee] MCServer - <BringupRequest bringupOptions=''>
 * </BringupRequest>
 * \endcode
 *
 * <li> mmcs_server
 * \code
 * user@host server> /bgsys/drivers/ppcfloor/hlcs/sbin/mmcs_server 
 * 2010-08-07 09:17:42.143 (WARN ) [0x40003208e60] ibm.database.tableapi.DBConnectionPool: using [database] section to initialize API is deprecated
 * 2010-08-07 09:17:45.525 (INFO ) [0x40003208e60] ibm.mc.MCServerRef: Connected to MCServer as mmcs@bgqfen4.rchland.ibm.com. Client version 3. Server version 3
 * 2010-08-07 09:17:45.995 (INFO ) [0x40003208e60] ibm.mc.MCServerRef: Connected to MCServer as @bgqfen4.rchland.ibm.com. Client version 3. Server version 3
 * \endcode
 *
 * <li> runjob_server
 * \code
 * user@host ~> /bgsys/drivers/ppcfloor/hlcs/sbin/runjob_server
 * 2010-03-11 14:21:22.645 (DEBUG) [0x400019367b0] ibm.utility.logging.Log: Logging configured.
 * 2010-03-11 14:21:22.646 (DEBUG) [0x400019367b0] ibm.utility.LoggingProgramOptions: ibm=INFO
 * 2010-03-11 14:21:22.646 (DEBUG) [0x400019367b0] ibm.utility.LoggingProgramOptions: ibm.bgsched=INFO
 * 2010-03-11 14:21:22.647 (DEBUG) [0x400019367b0] ibm.utility.LoggingProgramOptions: ibm.cios=TRACE
 * 2010-03-11 14:21:22.647 (DEBUG) [0x400019367b0] ibm.utility.LoggingProgramOptions: ibm.database=INFO
 * 2010-03-11 14:21:22.647 (DEBUG) [0x400019367b0] ibm.utility.LoggingProgramOptions: ibm.diags=DEBUG
 * 2010-03-11 14:21:22.647 (DEBUG) [0x400019367b0] ibm.utility.LoggingProgramOptions: ibm.hlcs.security=TRACE
 * 2010-03-11 14:21:22.647 (DEBUG) [0x400019367b0] ibm.utility.LoggingProgramOptions: ibm.mc=DEBUG
 * 2010-03-11 14:21:22.647 (DEBUG) [0x400019367b0] ibm.utility.LoggingProgramOptions: ibm.realtime=DEBUG
 * 2010-03-11 14:21:22.647 (DEBUG) [0x400019367b0] ibm.utility.LoggingProgramOptions: ibm.runjob=DEBUG
 * 2010-03-11 14:21:22.647 (DEBUG) [0x400019367b0] ibm.utility.LoggingProgramOptions: ibm.utility=TRACE
 * 2010-03-11 14:21:22.647 (DEBUG) [0x400019367b0] ibm.utility.LoggingProgramOptions: ibm.utility.Properties=INFO
 * 2010-03-11 14:21:22.647 (DEBUG) [0x400019367b0] ibm.utility.LoggingProgramOptions: ibm.utility.cxxsockets=WARN
 * 2010-03-11 14:21:22.647 (DEBUG) [0x400019367b0] ibm.utility.LoggingProgramOptions: ibm.utility.performance=TRACE
 * 2010-03-11 14:21:22.648 (INFO ) [0x400019367b0] ibm.runjob.AbstractOptions: using properties file /bglhome/samjmill/bg.properties
 * 2010-03-11 14:21:22.648 (DEBUG) [0x400019367b0] ibm.runjob.AbstractOptions: startup parameters: ./runjob_server 
 * 2010-03-11 14:21:22.649 (INFO ) [0x400019367b0] ibm.runjob.server.Options: missing thread-pool-size key from runjob.server section in properties file
 * 2010-03-11 14:21:22.650 (DEBUG) [0x400019367b0] ibm.runjob.AbstractOptions: Calculating number of workers from number of cpus in /proc/cpuinfo.
 * 2010-03-11 14:21:22.650 (INFO ) [0x400019367b0] ibm.runjob.AbstractOptions: calculated 4 worker threads
 * 2010-03-11 14:21:22.650 (DEBUG) [0x400019367b0] ibm.runjob.server.Options: current max number of user processes: 256
 * 2010-03-11 14:21:22.650 (INFO ) [0x400019367b0] ibm.runjob.server.Options: set max user processes to 128
 * 2010-03-11 14:21:22.651 (DEBUG) [0x400019367b0] ibm.runjob.server.Database: initializing database
 * 2010-03-11 14:21:22.651 (DEBUG) [0x400019367b0] ibm.utility.logging.Log: Logging already initialized.
 * 2010-03-11 14:21:22.816 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: extracting machine XML description
 * 2010-03-11 14:21:23.398 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: creating machine XML object
 * 2010-03-11 14:21:23.406 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: machine has 16 I/O links
 * 2010-03-11 14:21:23.406 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: link R00-M1-N04-J11 <--> R00-IC-J05
 * 2010-03-11 14:21:23.406 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: link R00-M1-N04-J06 <--> R00-IC-J07
 * 2010-03-11 14:21:23.406 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: link R00-M1-N12-J11 <--> R00-IC-J04
 * 2010-03-11 14:21:23.406 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: link R00-M1-N12-J06 <--> R00-IC-J06
 * 2010-03-11 14:21:23.406 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: link R00-M1-N08-J11 <--> R00-IC-J01
 * 2010-03-11 14:21:23.406 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: link R00-M1-N08-J06 <--> R00-IC-J03
 * 2010-03-11 14:21:23.406 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: link R00-M1-N00-J11 <--> R00-IC-J00
 * 2010-03-11 14:21:23.406 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: link R00-M1-N00-J06 <--> R00-IC-J02
 * 2010-03-11 14:21:23.406 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: link R00-M0-N08-J11 <--> R00-IC-J06
 * 2010-03-11 14:21:23.407 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: link R00-M0-N08-J06 <--> R00-IC-J07
 * 2010-03-11 14:21:23.407 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: link R00-M0-N12-J11 <--> R00-IC-J05
 * 2010-03-11 14:21:23.407 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: link R00-M0-N12-J06 <--> R00-IC-J04
 * 2010-03-11 14:21:23.407 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: link R00-M0-N00-J11 <--> R00-IC-J02
 * 2010-03-11 14:21:23.407 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: link R00-M0-N00-J06 <--> R00-IC-J03
 * 2010-03-11 14:21:23.407 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: link R00-M0-N04-J11 <--> R00-IC-J01
 * 2010-03-11 14:21:23.407 (DEBUG) [0x400019367b0] ibm.runjob.server.block.Container: link R00-M0-N04-J06 <--> R00-IC-J00
 * 2010-03-11 14:21:23.407 (INFO ) [0x400019367b0] ibm.runjob.server.sim.SharedMemory: using shared memory name '/tmp/runjob_server_iosd_counter'
 * 2010-03-11 14:21:23.407 (INFO ) [0x400019367b0] ibm.runjob.server.sim.SharedMemory: using shared memory size '2048'
 * 2010-03-11 14:21:23.408 (DEBUG) [0x400019367b0] ibm.runjob.server.sim.SharedMemory: counter value 2242
 * 2010-03-11 14:21:23.409 (DEBUG) [0x400019367b0] ibm.utility.portConfiguration.SslConfiguration: Configuration:
 *         Certificate file: '/bghome/samjmill/bgq/work/utility/etc/security/administrative.pem'
 *         Private key file: '/bghome/samjmill/bgq/work/utility/etc/security/administrative.pem'
 *         CA certificates path: not set
 *         CA certificate file: '/bghome/samjmill/bgq/work/utility/etc/security/root.pem'
 *         CA use default paths: 0
 * 2010-03-11 14:21:23.409 (DEBUG) [0x400019367b0] ibm.utility.portConfiguration.SslConfiguration: Creating context.
 * 2010-03-11 14:21:23.415 (DEBUG) [0x400019367b0] ibm.utility.UserId: getting uid for username nobody
 * 2010-03-11 14:21:23.416 (DEBUG) [0x400019367b0] ibm.utility.UserId: uid 65534
 * 2010-03-11 14:21:23.417 (DEBUG) [0x400019367b0] ibm.utility.UserId: secondary group list 2
 * 2010-03-11 14:21:23.418 (DEBUG) [0x400019367b0] ibm.utility.UserId: added group nogroup with 65534
 * 2010-03-11 14:21:23.419 (DEBUG) [0x400019367b0] ibm.utility.UserId: added group nobody with 65533
 * 2010-03-11 14:21:23.419 (DEBUG) [0x400019367b0] ibm.utility.portConfiguration.Acceptor: Looking up [127.0.0.1]:21510
 * 2010-03-11 14:21:23.420 (DEBUG) [0x400019367b0] ibm.utility.portConfiguration.SslConfiguration: Configuration:
 *         Certificate file: '/bghome/samjmill/bgq/work/utility/etc/security/administrative.pem'
 *         Private key file: '/bghome/samjmill/bgq/work/utility/etc/security/administrative.pem'
 *         CA certificates path: not set
 *         CA certificate file: '/bghome/samjmill/bgq/work/utility/etc/security/root.pem'
 *         CA use default paths: 0
 * 2010-03-11 14:21:23.420 (DEBUG) [0x400019367b0] ibm.utility.portConfiguration.SslConfiguration: Creating context.
 * 2010-03-11 14:21:23.423 (DEBUG) [0x400019367b0] ibm.utility.UserId: getting uid for username nobody
 * 2010-03-11 14:21:23.423 (DEBUG) [0x400019367b0] ibm.utility.UserId: uid 65534
 * 2010-03-11 14:21:23.423 (DEBUG) [0x400019367b0] ibm.utility.UserId: secondary group list 2
 * 2010-03-11 14:21:23.423 (DEBUG) [0x400019367b0] ibm.utility.UserId: added group nogroup with 65534
 * 2010-03-11 14:21:23.423 (DEBUG) [0x400019367b0] ibm.utility.UserId: added group nobody with 65533
 * 2010-03-11 14:21:23.423 (DEBUG) [0x400019367b0] ibm.utility.portConfiguration.Acceptor: Looking up [127.0.0.1]:27510
 * 2010-03-11 14:21:23.424 (DEBUG) [0x400019367b0] ibm.runjob.server.server: creating 4 threads
 * 2010-03-11 14:21:23.425 (DEBUG) [0x400019367b0] ibm.utility.portConfiguration.Acceptor: [127.0.0.1]:21510 -> 127.0.0.1:21510
 * 2010-03-11 14:21:23.425 (DEBUG) [0x4000beff1b0] ibm.utility.portConfiguration.Acceptor: [127.0.0.1]:27510 -> 127.0.0.1:27510
 * 2010-03-11 14:21:23.426 (DEBUG) [0x4000beff1b0] ibm.utility.portConfiguration.Acceptor: Looking up [::1]:27510
 * 2010-03-11 14:21:23.426 (DEBUG) [0x400019367b0] ibm.utility.portConfiguration.Acceptor: Looking up [::1]:21510
 * 2010-03-11 14:21:23.427 (DEBUG) [0x400019367b0] ibm.utility.portConfiguration.Acceptor: [::1]:27510 -> [::1]:27510
 * 2010-03-11 14:21:23.427 (DEBUG) [0x400019367b0] ibm.utility.portConfiguration.Acceptor: Looking up [9.5.45.48]:27510
 * 2010-03-11 14:21:23.427 (DEBUG) [0x4000a0ff1b0] ibm.utility.portConfiguration.Acceptor: [::1]:21510 -> [::1]:21510
 * 2010-03-11 14:21:23.427 (INFO ) [0x4000a0ff1b0] ibm.utility.portConfiguration.Acceptor: Accepting connections on all requested ports.
 * 2010-03-11 14:21:23.428 (INFO ) [0x4000a0ff1b0] ibm.runjob.server.CommandListener: accepting connections
 * 2010-03-11 14:21:23.429 (DEBUG) [0x4000a0ff1b0] ibm.utility.portConfiguration.Acceptor: [9.5.45.48]:27510 -> 9.5.45.48:27510
 * 2010-03-11 14:21:23.429 (INFO ) [0x4000a0ff1b0] ibm.utility.portConfiguration.Acceptor: Accepting connections on all requested ports.
 * 2010-03-11 14:21:23.429 (INFO ) [0x4000a0ff1b0] ibm.runjob.server.MuxListener: accepting connections
 * \endcode
 *
 * <li> runjob_mux
 * \code
 * user@host mux> /bgsys/drivers/ppcfloor/hlcs/sbin/runjob_mux
 * 2010-08-06 12:13:03.562 (INFO ) [0x400010c44e0] ibm.runjob.AbstractOptions: max open file descriptors: 1024
 * 2010-08-06 12:13:03.563 (INFO ) [0x400010c44e0] ibm.runjob.AbstractOptions: core file limit: 0
 * 2010-08-06 12:13:03.563 (INFO ) [0x400010c44e0] ibm.runjob.AbstractOptions: using properties file /bglhome/samjmill/bg.properties
 * 2010-08-06 12:13:03.563 (DEBUG) [0x400010c44e0] ibm.runjob.AbstractOptions: startup parameters: ./runjob_mux --properties /bglhome/samjmill/bg.properties
 * 2010-08-06 12:13:03.564 (INFO ) [0x400010c44e0] ibm.runjob.mux.Options: set local socket to runjob_mux.samjmill from properties file
 * 2010-08-06 12:13:03.565 (INFO ) [0x400010c44e0] ibm.runjob.mux.Options: missing thread_pool_size key from runjob.mux section in properties file
 * 2010-08-06 12:13:03.565 (DEBUG) [0x400010c44e0] ibm.runjob.AbstractOptions: Calculating number of workers from number of cpus in /proc/cpuinfo.
 * 2010-08-06 12:13:03.565 (INFO ) [0x400010c44e0] ibm.runjob.AbstractOptions: calculated 4 worker threads
 * 2010-08-06 12:13:03.572 (DEBUG) [0x400010c44e0] ibm.runjob.mux.Multiplexer: creating 4 threads
 * 2010-08-06 12:13:03.572 (INFO ) [0x400010c44e0] ibm.runjob.mux.client.Listener: listening for command connections at runjob_mux.samjmill
 * 2010-08-06 12:13:03.574 (DEBUG) [0x400010c44e0] ibm.runjob.mux.Plugin: using plugin path /bghome/samjmill/bgq/work/hlcs/lib/librunjob_mux_sample_plugin.so
 * 2010-08-06 12:13:03.577 (INFO ) [0x4000398f1b0] ibm.runjob.mux.CommandListener: accepting connections
 * 2010-08-06 12:13:04.080 (DEBUG) [0x40001b8f1b0] ibm.runjob.mux.server.Timer: connected to [::1]:27710
 * 2010-08-07 12:13:04.080 (DEBUG) [0x40001b8f1b0] ibm.runjob.mux.server.Connection: connected to [::1]:27710
 * \endcode
 * \section blocks Creating and Booting Blocks
 *
 * Now you are dangerous, you've created and populated a database schema.  To simulate 
 * jobs you need at least two blocks defined.  A compute block and an I/O block. For 
 * both of these tasks, you'll need to start a bg_console:
 *
 * \code
 * user@host ~> /bgsys/drivers/ppcfloor/hlcs/bin/bg_console
 * 2010-01-04 09:59:09.914 (INFO ) [0x40001cf8440] ibm.mmcs.bg_console: connecting to mmcs_server
 * 2010-01-04 09:59:09.955 (WARN ) [0x40001cf8440] ibm.utility.security.init: no properties file set to seed PRND, using default
 * 2010-01-04 09:59:09.988 (ERROR) [0x40001cf8440] ibm.mmcs.bg_console: connect error: Connection refused... mmcs_server is not started
 * 2010-01-04 09:59:10.086 (INFO ) [0x40001cf8440] ibm.mmcs.bg_console: connected to database
 * mmcs$
 * \endcode
 *
 * \subsection compute Create a Compute Block
 *
 * this command will create a node board sized block named R00-M0-N00 with 32 nodes.
 *
 * \code
 * mmcs$ gen_small_block R00-M0-N00 R00-M0 32 N00
 * \endcode
 *
 * \subsection io Create an I/O Block
 *
 * this command will create an I/O block using an entire I/O drawer with 8 nodes.
 * \code
 * mmcs$ gen_io_block io R00-ID 8
 * \endcode

 * </ol>
 *
 * \subsection Boot Boot your blocks
 *
 * The I/O block must be booted before its compute block(s).  
 *
 * <ol>
 * <li> I/O block
 * \code
 * mmcs$ allocate io
 * OK
 * mmcs$
 * \endcode
 *
 * <li> Compute block
 * \code
 * mmcs$ allocate R00-M0-N00
 * OK
 * mmcs$
 * \endcode
 *
 * \section running Running jobs
 *
 * Now you're ready to run jobs.
 *
 * \code
 * user@host ~> /bgsys/drivers/ppcfloor/hlcs/bin/runjob --block R00-M0-N00 : /bin/date
 * ...
 * \endcode
 *
 * \subsection htc Sub-Block jobs
 *
 * \image html sub_block_jobs.jpg "BlueGene/Q sub-block jobs"
 *
 * Sub-block jobs allow multiple jobs to execute simultaneously on a single compute block. Any compute
 * block may be used for this purpose, there is no boot time configuration required like HTC blocks
 * on BG/P. The only limitation for sub-block jobs is they may use a maximum of 512 nodes. The syntax
 * to request a sub-block job is provided by the \ref runjob "--corner" and \ref runjob "--shape" arguments
 * to %runjob.
 *
 * For the purposes of job simulation, sub-block jobs are largely agnostic to their environment. The
 * arbitration is fully supported in simulation mode, meaning two jobs requested the same or overlapping
 * resources should only allow one to run. The \link runjob runjob documentation\endlink displays several examples utilizing
 * the corner and shape arguments.
 *
 * \section limitations Limitations
 *
 * There are several limitations with job simulation. Some are inherent in the overall idea of simulating
 * such a large supercomputer. Others are purposefully omitted since their behavior is not needed until
 * a later date. The list of current limitations is shown below.
 *
 *  - \htmlonly<strike>\endhtmlonlyOutput (stdout, stderr) does not equal the number of compute
 *    nodes (yet).\htmlonly</strike>\endhtmlonly
 *  - \htmlonly<strike>\endhtmlonlyNot automated yet, there's still a manual step involved to notify runjob_server about I/O and
 *    compute blocks.\htmlonly</strike>\endhtmlonly
 *  - \htmlonly<strike>\endhtmlonlykilling or sending signals are not supported.\htmlonly</strike>\endhtmlonly
 *  - stdin is not supported and probably never will be
 *  - \htmlonly<strike>\endhtmlonlysingle core sub-block jobs are not supported.\htmlonly</strike>\endhtmlonly
 *  - \htmlonly<strike>\endhtmlonlymany runjob arguments are not supported, consult the documentation for an updated list.\htmlonly</strike>\endhtmlonly
 *  - \htmlonly<strike>\endhtmlonlynothing stored in the database yet, so commands like list_jobs are not supported.\htmlonly</strike>\endhtmlonly
 *  - alternative mappings via the runjob --mapping argument are not supported.
 *
 * \section faq Frequently Asked Questions
 *
 *  - When simulating large configurations (use your judgement what large is) I sometimes see the following
 *  entry in my runjob_server output before it aborts:
\verbatim
2010-05-03 02:57:06.668 (ERROR) [0x4000c81f1b0] R00-IC-J00:ibm.runjob.server.sim.Iosd: could not fork 11 (Resource temporarily unavailable)
\endverbatim
 *  This is due to a safety belt mechanism design to prevent you from fork bombing our development
 *  environments. The runjob_server unconditionally sets the maximum number of user processes with
\verbatim
setrlimit( RLIMIT_NPROC, value )
\endverbatim
 *  during job simulation when it starts up. You can tailor this value to whatever you like using the
 *  max_user_processes key in the [%runjob.server] section of your properties file. The default value is
 *  256. If you run a vnc, you will probably want to increase this value. Ensure you know what you're doing
 *  before increasing this to a large value. This value can be set to 0 to inherit the ulimit configuration.
 *
 * \section cleanup Cleaning Up
 *
 * Be a good citizen to other users of our front end nodes, ensure any stray processes are cleaned
 * up after you are finished with job simulation. Normally these processes should be cleaned up
 * for you, but it's a good idea to check for any stray processes just in case.
 * \code
 * user@host ~> killall -KILL iosd
 * user@host ~> killall -KILL jobctld
 * user@host ~> killall -KILL stdiod
 * \endcode
 *
 * Also remove your temporary iosd directories and log files
 * \code
 * user@host ~> rm -rf /tmp/cios*
 * \endcode
 *
 * \section moreinfo More Information
 *
 * - \ref runjob::server::sim::Iosd
 * - \ref runjob::server::sim::Inotify
 * - \ref runjob::server::sim::SharedMemory
 * - \ref runjob_server
 * - \ref runjob_mux
 * - \ref runjob
 */


