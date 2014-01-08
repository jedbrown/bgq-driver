
# Blue Gene configuration file.


[database]

name=BGDB0
    # The name of the database containing the Blue Gene schema.

#user=bgqsysdb
    # The user to connect to the database as.
    # This value is optional. If not set, then the current user is used.

#password=xxxxxx
    # The password to use when connecting to the database.
    # This value is required if the user is set and ignored if it's not.

schema_name=bgqsysdb
    # The database schema containing the Blue Gene tables.
    # This value is optional. If not set, the default schema is used.

computeRackRows=1
computeRackColumns=1
    # Used by dbPopulate to populate the schema.


[security.ca]
    # Certificate Authority.

certificate = /bgsys/local/etc/security/ca/cert.pem
    # File containing the root CA (certificate authority) certificate.
    # This property is optional, but either certificate or
    # certificates_dir must be set or use_default_paths must be 'true'.

# certificates_dir = /bgsys/local/etc/security/root/
    # Directory containing root CA certificates.
    # This property is optional, but either certificate or
    # certificates_dir must be set or use_default_paths must be 'true'.

use_default_paths = false
    # Set to 'true' to use the system default root CA paths.
    # If this is 'false' then either certificate or certificates_dir
    # must be set.

keystore = /bgsys/local/etc/security/ca/cert.jks
    # File containing the root CA certificate in Java Keystore format.


[security.admin]
    # Administrative certificate.

certificate = /bgsys/local/etc/security/admin/cert.pem
    # File containing the Blue Gene administrative certificate.

key_file = /bgsys/local/etc/security/admin/key.pem
    # File containing the Blue Gene administrative private key.

cn = Blue Gene administrative
    # The certificate provided by the server or administrator must have this CN
    # (common name).

keystore = /bgsys/local/etc/security/admin/key.jks
    # File containing the Blue Gene administrative certificate in Java Keystore format (jks).


[security.command]
    # Command certificate.

certificate = /bgsys/local/etc/security/command/cert.pem
    # File containing the Blue Gene command certificate.

key_file = /bgsys/local/etc/security/command/key.pem
    # File containing the Blue Gene command private key.

cn = Blue Gene command
    # The certificate provided by the Blue Gene command must have this CN.


[security.blocks]
    # All of the values in this section can be refreshed by running refresh_config from bg_console.

all = bgqadmin
    # Comma separated list of users or groups allowed to execute all actions on blocks.

create =
    # Comma separated list of users or groups allowed to create blocks.

read =
    # Comma separated list of users or groups allowed to read blocks.

update =
    # Comma separated list of users or groups allowed to update blocks.

delete =
    # Comma separated list of users or groups allowed to delete blocks.

execute =
    # Comma separated list of users or groups allowed to execute blocks.

[security.jobs]
    # All of the values in this section can be refreshed by running refresh_config from bg_console.

all = bgqadmin
    # Comma separated list of users or groups allowed to execute all actions on all jobs.

read = 
    # Comma separated list of users or groups allowed to read all jobs.

execute = 
    # Comma separated list of users or groups allowed to execute all jobs.


[security.hardware]
    # All of the values in this section can be refreshed by running refresh_config from bg_console.

all = bgqadmin
    # Comma separated list of users or groups allowed to execute all actions on hardware.

read = 
    # Comma separated list of users or groups allowed to read hardware.

execute = 
    # Comma separated list of users or groups allowed to execute hardware.


[bgsched]
    # Job scheduler API configuration.

polling_interval = 3000
    # Polling interval in milliseconds when allocator monitor is in polling mode for block allocates and deallocates.

connection_pool_size = 4
    # Maximum number of connections held in the database connection pool.

# extra_connection_parameters = Debug=True;
    # Extra parameters to pass on the connection.
    # Format is NAME=VALUE pairs separated by ;.
    # Optional, default is no extra parameters.

realtime_host = [::1]:32061,127.0.0.1:32061
    # The host and port to connect to for the real-time API. The real-time server should be listening on the port.
    # The format is a comma-separated list of host:port pairs.
    # host is an IP address or host name. IP addresses containing colons must be enclosed in [].
    # port is a port number or service name.
    # The host and port are optional.
    # If the host is not specified the client will connect to localhost.
    # If the port is not specified the default port will be used (32061).


[bgsched.allocator.plugins]
    # Fully qualified paths to scheduler plugins.

IBM = /bgsys/drivers/ppcfloor/hlcs/lib/libfirstfit.so


[runjob.mux]

thread_pool_size = auto
    # Number of threads to run in the server.
    # Allowed values are integers greater than 0 or "auto".
    # "auto" means the runjob_mux will pick the number of threads to start.
    # The default is auto.

#plugin = 
    # Fully qualified path to the plugin used for communicating with a job scheduler.
    # This value can be updated by the runjob_mux_refresh_config command on the
    # Login Node where a runjob_mux process runs.

#plugin_flags = 
    # Hexadecimal flags to pass as the second argument to dlopen. If not specified,
    # the default value is 0x0001 (RTLD_LAZY). See the dlopen(3) man page for more information.

local_socket = runjob_mux
    # Name of the local AF_UNIX socket to use when listening for runjob
    # connections. It is placed in the abstract namespace, see man unix(7) for more information.

host = [::1]:25510,127.0.0.1:25510
    # The host and port to connect to. The runjob_server should be listening on the port.
    # The format is a comma-separated list of host:port pairs.
    # host is an IP address or host name. IP addresses containing colons must be enclosed in [].
    # Link local ipv6 addresses must include the interface name appended after a % character.
    # port is a port number or service name.
    # The host and port are optional.
    # If the host is not specified the client will connect to localhost.
    # If the port is not specified the default port will be used (25510).

command_listen_ports = [::1]:26510,127.0.0.1:26510
    # The ports that the runjob_mux will listen for connections on.
    # The format is a comma-separated list of host:port pairs.
    # host is an IP address or host name. IP addresses containing colons must be enclosed in [].
    # Link local ipv6 addresses must include the interface name appended after a % character.
    # port is a port number or service name.
    # The host and port are optional.
    # If the host is not specified the server will listen on any interface.
    # If the port is not specified the default port will be used (26510).

performance_counter_interval = 30
    # Number of seconds to wait between sending performance counter statistics to
    # the runjob_server for persistent storage in the database.
    # Default is 15 seconds if this value is not specified.
    # This value can be updated by the runjob_mux_refresh_config command on the
    # Login Node where a runjob_mux process runs.

server_connection_interval = 10
    # Number of seconds to wait between connection attempts to the runjob_server
    # when it is unavailable. Default value is 10 seconds if not specified here.
    # This value can be updated by the runjob_mux_refresh_config command on the
    # Login Node where a runjob_mux process runs.

client_output_buffer_size = 512
    # Maximum number of kilobytes of stdout or stderr messages to queue up
    # per client. Messages received after this value is reached will be 
    # dropped. Default value is 512 kilobytes if not specified here.
    # This value can be updated by the runjob_mux_refresh_config command on the
    # Login Node where a runjob_mux process runs.


[runjob.mux.commands]

host = [::1]:26510,127.0.0.1:26510
    # The host and port to connect to. The runjob_mux should be listening on the port.
    # The format is a comma-separated list of host:port pairs.
    # host is an IP address or host name. IP addresses containing colons must be enclosed in [].
    # Link local ipv6 addresses must include the interface name appended after a % character.
    # port is a port number or service name.
    # The host and port are optional.
    # If the host is not specified the client will connect to localhost.
    # If the port is not specified the default port will be used (24510).


[runjob.server]

thread_pool_size = auto
    # Number of threads to run in the server.
    # Allowed values are integers greater than 0 or "auto".
    # "auto" means the runjob_server will pick the number of threads to start.
    # The default is auto.

mux_listen_ports = [::1]:25510,127.0.0.1:25510
command_listen_ports = [::1]:24510,127.0.0.1:24510
    # The ports that the runjob_server will listen for connections on.
    # The format is a comma-separated list of host:port pairs.
    # host is an IP address or host name. IP addresses containing colons must be enclosed in [].
    # Link local ipv6 addresses must include the interface name appended after a % character.
    # port is a port number or service name.
    # The host and port are optional.
    # If the host is not specified the server will listen on any interface.
    # If the port is not specified the default port will be used (25510 and 24510).

io_connection_interval_increment = 5
    # Number of seconds to increment a progressive timeout when
    # retrying connection attempts to the CIOS (common I/O services) daemons.
    # This value can be updated by the runjob_server_refresh_config command.

io_connection_interval_max = 120
    # Maximum number of seconds to wait between connection
    # attempts to the CIOS daemons.
    # This value can be updated by the runjob_server_refresh_config command.

performance_counter_interval = 30
    # Number of seconds to wait between inserting performance counter statistics 
    # into the database for persistent storage. The default is 30 seconds if this value
    # is not specified.
    # This value can be updated by the runjob_server_refresh_config command.

connection_pool_size = 20
    # The maximum number of connections held in the runjob_server database connection pool.
    # If not specified, the maximum is 20.

# extra_connection_parameters = Debug=True;
    # Extra parameters to pass on the connection.
    # Format is NAME=VALUE pairs separated by ;.
    # Optional, default is no extra parameters.

control_action_heartbeat = 60
    # The number of seconds to wait between checking for progress of a job that has been terminated
    # due to a RAS event with a control action. If not specified, this value is 60.
    # This value can be updated by the runjob_server_refresh_config command.


[runjob.server.commands]

host = [::1]:24510,127.0.0.1:24510
    # The host and port to connect to. The runjob_server should be listening on the port.
    # The format is a comma-separated list of host:port pairs.
    # host is an IP address or host name. IP addresses containing colons must be enclosed in [].
    # Link local ipv6 addresses must include the interface name appended after a % character.
    # port is a port number or service name.
    # The host and port are optional.
    # If the host is not specified the client will connect to localhost.
    # If the port is not specified the default port will be used (24510).


[bg_console]
    # bg_console configuration
host = [::1]:32031,127.0.0.1:32031
    # The host and port to connect to. The mmcs_server should be listening on the port.
    # The format is a comma-separated list of host:port pairs.
    # host is an IP address or host name. IP addresses containing colons must be enclosed in [].
    # Link local ipv6 addresses must include the interface name appended after a % character.
    # port is a port number or service name.
    # The host and port are optional.
    # If the host is not specified the client will connect to localhost.
    # If the port is not specified the default port will be used (32031).

command_path = /bgsys/drivers/ppcfloor/bin:/bgsys/drivers/ppcfloor/sbin
    # Colon separated list of directories to search for external console commands

[bg_console.external.commands]
    # List of external commands bg_console can run and their arguments

alias_wait = alias_wait [alias] [ --timeout seconds ] [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]
binary_status = binary_status [binary id] [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]
binary_wait = binary_wait [ binary id ] [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]
fail_over = fail_over [ binary id ] [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]
list_agents = list_agents [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]
list_clients = list_clients [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]
master_start = master_start [ alias ] | [ bgmaster ] [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]
master_status = master_status [ alias ] | [ bgmaster ] [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]
master_stop = master_stop [ alias ] | [ "bgmaster" ] | [ --binary binary id ] | [ --agent agent id ] [ --signal signal number ] [ --help ] [ --properties filename ]
monitor_master = monitor_master [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]
get_history = get_history [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]
get_errors = get_errors [ --properties filename ] [ --host host:port ] [ --verbose verbosity ]
kill_job = kill_job <id> -signal
list_jobs = list_jobs
job_status = job_status <id>
grant_job_authority = grant_job_authority <id> <username> <read|execute>
revoke_job_authority = revoke_job_authority <id> <username> <read|execute>
list_job_authority = list_job_authority <id> 
start_tool = start_tool <id> --tool path
end_tool = end_tool <id>
tool_status = tool_status <id>
delete_block = delete_block <id>
locate_rank = locate_rank <id> <rank>
dump_proctable = dump_proctable <id>


[bg_console.external.command.categories]
    # This is a map of help categories to external commands.
    # Use help from bg_console to see a detailed description of each category.

DEFAULT = get_errors,get_history,master_status,monitor_master,list_agents,list_clients,alias_wait,binary_wait,binary_status,list_jobs,job_status,locate_rank,dump_proctable,list_job_authority,tool_status
USER = kill_job,grant_job_authority,revoke_job_authority,delete_block,end_tool,start_tool
ADMIN = fail_over,master_start,master_stop
SPECIAL =


[mmcs]

boot_options =
listen_ports = [::1]:32031,127.0.0.1:32031
    # The ports that the mmcs_server will listen for connections on.
    # The format is a comma-separated list of host:port pairs.
    # host is an IP address or host name. IP addresses containing colons must be enclosed in [].
    # Link local ipv6 addresses must include the interface name appended after a % character.
    # port is a port number or service name.
    # The host and port are optional.
    # If the host is not specified mmcs_server will listen on localhost.
    # If the port is not specified the default port will be used (32031).

shutdown_timeout = 90
    # seconds

wait_boot_free_time = 20
    # maximum wait time for a boot to complete (minutes)

minimum_boot_wait = 360
    # minimum boot wait time (seconds)

reconnect_blocks = false
    # reconnect

log_dir = /bgsys/logs/BGQ
    # I/O log

connection_pool_size = 10
    # Maximum number of connections held in the database connection pool.

cn_boot_slope = .017
    # Linear boot timeout slope for compute blocks

io_boot_slope = .017
    # Linear boot timeout slope for IO blocks

[mmcs.envs]

sc_interval_seconds = 1800
io_interval_seconds = 300
nc_interval_seconds = 300
bulk_interval_seconds = 300
coolant_interval_seconds = 300
optical_interval_seconds = 3600
    # Polling intervals in seconds for
    # service cards (sc)
    # I/O boards (io) 
    # node cards (nc)
    # bulk power modules
    # coolant monitors
    # optical modules
    # For all intervals, specifying a 0 or omitting the value turns off polling for that type of hardware.
    # All of these values can be refreshed by running refresh_config mmcs_server from bg_console.
    # Default values are shown, valid values can range between 60 and 3600 seconds, values outside
    # outside of this range use the default value.

perfdata_interval_seconds = 300
    # polling interval for performance data
    # Specifying a 0 or omitting the value turns off polling.
    # Default value is shown, valid values can range between 60 and 1800 seconds, values outside
    # of this range use the default value.


[mmcs.blockgovernor]
    # The block governor manages the rate at which mmcs_server can poll the database.
max_concurrent = 0
    # number of block transactions that can be running at one time

interval = 10
    # length of a measurement interval in seconds

max_tran_rate = 5
    # number of block transactions that can be started in one interval
    
repeat_interval = 15
    # minimum seconds between repeat of a transaction


[realtime.server]

listen_ports = [::1]:32061,127.0.0.1:32061
    # The ports that the real-time server will listen for connections on.
    # The format is a comma-separated list of host:port pairs.
    # host is an IP address or host name. IP addresses containing colons must be enclosed in [].
    # port is a port number or service name.
    # The host and port are optional.
    # If the host is not specified the server will listen on any interface.
    # If the port is not specified the default port will be used (32061).

command_listen_ports = localhost:32062
    # The ports that the real-time server will listen for command connections on.
    # The format is a comma-separated list of host:port pairs.
    # host is an IP address or host name. IP addresses containing colons must be enclosed in [].
    # port is a port number or service name.
    # The host and port are optional.
    # If the host is not specified the server will listen on any interface.
    # If the port is not specified the default port will be used (32062).

workers = 0
    # The number of worker threads to start.
    # 0 indicates to let the server pick, otherwise must be a positive number.

maximum_transaction_size = 40
    # The number of operations in a transaction before the server enters large-transaction mode.
    # When the server is in large-transaction mode, clients will not receive real-time events.
    # The default is 40.


[realtime.server.command]
    # Real-time server commands (status, logging, etc.) configuration options.

host = [::1]:32062,127.0.0.1:32062
    # The host and port to connect to.
    # The real-time server should be listening for command connections on the port.
    # The format is a comma-separated list of host:port pairs.
    # host is an IP address or host name. IP addresses containing colons must be enclosed in [].
    # port is a port number or service name.
    # The host and port are optional.
    # If the host is not specified the client will connect to localhost.
    # If the port is not specified the default port will be used (32062).


[machinecontroller]
    # Low Level Control system (LLCS) configuration parameters for mc_server.

clientPort = 1206 
    # The TCP port that mc_server will listen on.

machineType = 0

hostName = 127.0.0.1
    # This is the IP address/name that the external apps use to talk to mcServer (e.g. mmcs, baremetal, etc.)
#PrimaryMcHostName = 172.16.2.8  
    # This is the IP address/name that the SubnetMcs need to use to talk to the PrimaryMc (not mcServer).

bringup = true
    # Bringup the hardware when mc_server starts. Valid values are true or false.
    # If not specified, the default value is false.

spillFileDir =
    # Fully qualified path to the RAS spill file directory.
    # If not specified, no spill file is used.

alteraPath = /bgsys/drivers/ppcfloor/ctrlnet/bin/icon/fpgaImages
    # Fully qualified path to the images for the iCon/Palominos (FPGAs).

CardThatTalksToMasterClockCard = R00-M0-S
    # The location of the card that communicates with / controls the master clock card.

bringupOptions=
    # Options to be used when bringing up the machine (indicates how to initialize this machine). 

secure = true
    # Use SSL for incoming connections. Defaults to false if not specified here.

kernel_shutdown_timeout = 60
    # Number of seconds compute and I/O kernels have to complete shutdown  
    # The default is 60.

io_link_shutdown_timeout = 180
    # Number of seconds to complete I/O link shutdown  
    # The default is 180.

[machinecontroller.subnet.0]
    # This is the header that indicates the start of the parameters for the first SubnetMc.

Name = Subnet1
    # Identifier for this SubnetMc.  Unique identifier for this subnet, will be used when logging messages, RAS events, etc

PrimaryServer = 127.0.0.1
    # The machine name / IP address of the machine running this SubnetMc process.

PrimaryServerInterface = lo
    # The ethernet adapter that the Primary MC should use to talk to the Subnet MC for this subnet.

PrimaryServerPort = 33456
    # The ethernet port that the Subnet MC for this subnet will be listening on.

PrimaryBGInterface = eth1
    # The ethernet adapter on the Subnet Service Node to use to talk to the BG hardware.

BackupServer = localhost
BackupServerInterface = eth1 
BackupServerPort = 33456       
BackupBGInterface = eth0 

HardwareToManage = R00,Q01
    # List of the RACKS that are allowed to communicate via this subnet (i.e. the racks expected to be on this subnet).


[master.server]
    # bgmaster_server configuration options

agent_listen_ports = [::1]:32041,127.0.0.1:32041
client_listen_ports = [::1]:32042,127.0.0.1:32042
    # The ports that the bgmaster_server will listen for connections on.
    # The format is a comma-separated list of host:port pairs.
    # host is an IP address or host name. IP addresses containing colons must be enclosed in [].
    # Link local ipv6 addresses must include the interface name appended after a % character.
    # port is a port number or service name.
    # The host and port are optional.
    # If the host is not specified the server will listen on any interface.
    # If the port is not specified the default port will be used (32041 and 32042).

max_agents_per_host=1
logdir = /bgsys/logs/BGQ
    # Default logging location for bgmaster_server and all managed binaries.
logdirEdramChargePump=/bgsys/logs/BGQ/diags/bringup
    # Default logging location for any eDRAMChargePump logs.

[master.client]

host = [::1]:32042,127.0.0.1:32042
    # The host and port to connect to. The bgmaster_server should be listening on the port.
    # The format is a comma-separated list of host:port pairs.
    # host is an IP address or host name. IP addresses containing colons must be enclosed in [].
    # port is a port number or service name.
    # The host and port are optional.
    # If the host is not specified the client will connect to localhost.
    # If the port is not specified the default port will be used (32042).


[master.agent]
    # bgagentd configuration options

logdir = /bgsys/logs/BGQ
host = [::1]:32041,127.0.0.1:32041
    # The host and port to connect to. The bgmaster_server server should be listening on the port.
    # The format is a comma-separated list of host:port pairs.
    # host is an IP address or host name. IP addresses containing colons must be enclosed in [].
    # port is a port number or service name.
    # The host and port are optional.
    # If the host is not specified the client will connect to localhost.
    # If the port is not specified the default port will be used (32041).

agentuid = bgqadmin


[master.binmap]
    # Maps an alias to a binary path.

bgmaster_server = /bgsys/drivers/ppcfloor/hlcs/sbin/bgmaster_server
bgws_server = /bgsys/drivers/ppcfloor/bgws/sbin/bgws_server
realtime_server = /bgsys/drivers/ppcfloor/hlcs/sbin/bg_realtime_server
runjob_server = /bgsys/drivers/ppcfloor/hlcs/sbin/runjob_server
runjob_mux = /bgsys/drivers/ppcfloor/hlcs/sbin/runjob_mux
mc_server = /bgsys/drivers/ppcfloor/control/sbin/mc_server_64
Subnet1 = /bgsys/drivers/ppcfloor/control/sbin/SubnetMc_64
mmcs_server = /bgsys/drivers/ppcfloor/hlcs/sbin/mmcs_server
teal_server = /opt/teal/ibm/teal/teal.py
teal_bg = /opt/teal/ibm/bgq/bgq.py

[master.logdirs]
    # Maps aliases to logging directories.  By default, all binaries will log to logdir
    # in the [master.server] section.
#runjob_server = /bgsys/logs/BGQ/
teal_bg = /bgsys/logs/BGQ/teal/
teal_server = /bgsys/logs/BGQ/teal/

[master.binargs]
    # List of arguments for each alias specified in the [master.binmap] section.
bgmaster_server =
bgws_server =
realtime_server =
runjob_server =
runjob_mux =
mc_server =
Subnet1 =
mmcs_server =
teal_server = --realtime --logfile /dev/stdout
teal_bg = --logfile /dev/stdout

[master.user]
    # Maps an alias to a user id under which it should run.
    # If an alias is not specified, it runs as the agentuid value in [master.agent].

bgws_server = bgws
realtime_server = bgqsysdb


[master.policy.host_list]
    # Comma separated list of hosts on which the binary associated with the alias
    # may start.

bgmaster_server = localhost
bgws_server = localhost
realtime_server = localhost
runjob_server = localhost
runjob_mux = localhost
mc_server = localhost
Subnet1 = localhost
mmcs_server = localhost
teal_server = localhost
teal_bg = localhost


[master.policy.instances]
    # Policies for the number of instances of each alias are allowed.
    # Valid entries are 0 through 65535, the default if not specified is 1.

bgmaster_server = 1
bgws_server = 1
realtime_server = 1
runjob_server = 1
runjob_mux = 1
mc_server = 1
Subnet1 = 1
mmcs_server = 1
teal_server = 1
teal_bg = 1


[master.policy.failure]
    # Formatted failover policy.
    #
    # Terminology:
    # killed:        binary received a kill signal (not a stop message through bgmaster)
    # binary:        binary failed 
    # agent:         the monitoring agent failed  
    # failover:      run the binary that failed on the specified backup host
    # restart:       restart the binary on the same host  
    # cleanup:       just clean up internal structures and let the binary go
    # retries:       number of times to attempt to restart the binary
    # failover_from: when it fails on this host...
    # failover_to:   where you want the failover to go
    # trigger:       event that causes a policy to be activated.  It can be "killed", "binary", or "agent".
    #
    # Each line has a policy_name as a key followed by a formatted policy.
    # The policy consists of a trigger and a complex behavior.  
    # The complex behavior consists of a trigger, an action, a retry count and an
    # optional set of '|' separated failover pairs.
    #
    # Format: policy_name=trigger,[failover|restart|cleanup],<retries>,<failover_from:failover_to>|etc...
    #
    # Example: foo=binary,failover,2,bgqfen2.rchland.ibm.com:bgqfen1.rchland.ibm.com|bgqfen2.rchland.ibm.com:bgqfen1.rchland.ibm.com
    #
    # The 'foo' policy will failover to bgqfen1.rchland.ibm.com if the associated 
    # binary dies on bgqfen2.rchland.ibm.com
    # and vice versa.  It will retry two times.
    # If the agent dies, it will not failover.  
    # If failover is specified and no host is specified, it will fail over 
    # to any other available host
agent_restart = agent,restart,3
server_restart = binary,restart,3
server_kill_restart = killed,restart,3

[master.policy.map]
    # Map comma separated list of named policies to an alias
    # alias=alias_restart,alias_failover
Subnet1 = agent_restart,server_restart
mc_server = agent_restart,server_restart
realtime_server = agent_restart,server_restart
mmcs_server = agent_restart,server_restart
bgws_server = agent_restart,server_restart
runjob_server = agent_restart,server_restart
runjob_mux = agent_restart,server_restart
teal_server = agent_restart,server_restart
teal_bg = agent_restart,server_restart

[master.startup]
    # startup options for bgmaster_server

start_servers=true
    # Option to start selected servers when bgmaster_server starts.

start_order=Subnet1,mc_server,realtime_server,mmcs_server,bgws_server,runjob_server,runjob_mux,teal_server,teal_bg
    # Comma separated list of aliases to start serially.


[performance]

mode = basic
    # Control System performance monitoring mode.
    # Possible values are none, basic, and extended.
    # Default value is none if not specified or if the value is malformed.
    # This value can be refreshed by refresh_config from bg_console.

buffer_capacity = 2048
    # Number of entries to create in the circular buffer used by persistent
    # statistic sets. The default value is 2048 entries. Must be a positive 
    # number, otherwise the default value is used.


[bgws]
    # Settings for Blue Gene Web Services (bgws)

#machine_name = My Blue Gene
    # Machine name. User interfaces can display this so users know what machine they're connected to.
    # Defaults to the system hostname.
    # This value can be refreshed by refresh_config bgws_server from bg_console.

listen_ports = [::1]:32071,127.0.0.1:32071
    # The ports that the BGWS server will listen for connections on.
    # The format is a comma-separated list of host:port pairs.
    # host is an IP address or host name. IP addresses containing colons must be enclosed in [].
    # port is a port number or service name.
    # The host and port are optional.
    # If the host is not specified the server will listen on any interface.
    # If the port is not specified the default port will be used (32071).

path_base = /bg
    # Path on the web server to the BGWS resources.
    # Defaults to /bg.
    # This value can be refreshed by refresh_config bgws_server from bg_console.

thread_pool_size = auto
    # Number of threads to run in the server.
    # Allowed values are integers greater than 0 or "auto".
    # "auto" means the BGWS server will pick the number of threads to start.
    # The default is auto.

connection_pool_size = 20
    # Maximum number of connections held in the BGWS server database connection pool.

# extra_connection_parameters = Debug=True;
    # Extra parameters to pass on the connection.
    # Format is NAME=VALUE pairs separated by ;.
    # Optional, default is no extra parameters.

session_timeout = 3600
    # Number of seconds of inactivity before a session will be discarded.
    # The default is 3600.
    # This value can be refreshed by refresh_config bgws_server from bg_console.

user_authentication_exe = /usr/local/libexec/pwauth
    # Path to the program to run to authenticate users.
    # The default is /usr/local/libexec/pwauth
    # This value can be refreshed by refresh_config bgws_server from bg_console.

check_user_admin_exe = /bgsys/drivers/ppcfloor/bgws/libexec/checkUserAdmin
    # Path to the program to run to check if a user is an administrator.
    # If not set, all users will be non-administrators.
    # This value can be refreshed by refresh_config bgws_server from bg_console.

diagnostics_exe = /bgsys/drivers/ppcfloor/diags/bin/rundiags.py
    # Path to the program to run for diagnostics.
    # The default is /bgsys/drivers/ppcfloor/diags/bin/rundiags.py
    # This value can be refreshed by refresh_config bgws_server from bg_console.

service_action_exe = /bgsys/drivers/ppcfloor/baremetal/bin/ServiceAction
    # Path to the program to run for service actions.
    # The default is /bgsys/drivers/ppcfloor/baremetal/bin/ServiceAction
    # This value can be refreshed by refresh_config bgws_server from bg_console.

teal_chalert_exe = /opt/teal/bin/tlchalert
    # Path to the program to close TEAL alerts, tlchalert.
    # The default is /opt/teal/bin/tlchalert
    # This value can be refreshed by refresh_config bgws_server from bg_console.

teal_rmalert_exe = /opt/teal/bin/tlrmalert
    # Path to the program to remove TEAL alerts, tlrmalert.
    # The default is /opt/teal/bin/tlrmalert
    # This value can be refreshed by refresh_config bgws_server from bg_console.


[bgws.commands]
    # Settings for the BGWS commands.

base_url = https://localhost:32071/bg
    # base URL to BGWS server, defaults to https://localhost:32071/bg


[log_merge]

log_dir = /bgsys/logs/BGQ
    # The directories used when no arguments are given to log_merge.
    # Defaults to /bgsys/logs/BGQ
    # To use multiple directories, separate each directory with "," (comma) like "/bgsys/logs/BGQ,/bgsys/logs/BGQ/ssn1".


[cios]
    # Settings for Common I/O Services.

large_region_size = 1048576
    # Size in bytes of large memory regions.
    # Valid values are 65536 to 8388608.
    # If large_region_size is invalid or not specified, the default value is used (1048576).

num_large_regions = 256
    # Number of large memory regions to allocate on I/O node.

max_connected_nodes = 256
    # Maximum number of compute nodes an I/O node will accept.
    # Valid values are numbers >128.


[cios.iosd]
    # Settings for CIOS I/O services daemon.

listen_port = 7001
    # Port number for control system connection.
    # If listen_port is invalid or not specified, the default value is used (7001).

jobctl_daemon_path = /sbin/jobctld
    # Path to job control daemon executable.
    # There is one job control daemon per I/O node.

stdio_daemon_path = /sbin/stdiod
    # Path to standard I/O daemon executable.
    # There is one standard I/O daemon per I/O node.

sysio_daemon_path = /sbin/sysiod
    # Path to system I/O daemon executable.
    # There is one system I/O daemon for each compute node being serviced by the I/O node. 

toolctl_daemon_path = /sbin/toolctld
    # Path to tool control daemon executable.
    # There is one tool control daemon for each compute node being serviced by the I/O node. 

max_service_restarts = 5
    # Maximum number of times a service is restarted.
    # Valid values are 0 to 65535.
    # If max_service_restarts is invalid or not specified, the default value is used (5).


[cios.jobctld]
    # Settings for CIOS job control daemon.

listen_port = 7002
    # Port number for control system connection.
    # If listen_port is invalid or not specified, the default port is used (7002).

#job_prolog_program_path = 
    # Path to prolog program to run before a job is started.
    # The prolog program must be accessible from I/O nodes.
    # This property is optional and if it is not specified, a prolog program is not run.

#job_epilog_program_path = 
    # Path to epilog program to run after a job has ended.
    # The epilog program must be accessible from I/O nodes.
    # This property is optional and if it is not specified an epilog program is not run.

job_prolog_program_timeout = -1
    # Number of seconds to wait for job prolog program to complete.
    # Valid values are a positive number or -1 to wait forever.
    # If job_prolog_program_timeout is invalid or not specified, the default value is used (-1).

job_epilog_program_timeout = -1
    # Number of seconds to wait for job epilog program to complete.
    # Valid values are a positive number or -1 to wait forever.
    # If job_epilog_program_timeout is invalid or not specified, the default value is used (-1).


[cios.stdiod]
    # Settings for CIOS standard I/O daemon.

listen_port = 7003
    # Port number for control system connection.
    # If listen_port is invalid or not specified, the default port is used (7003).

send_buffer_size = 262144
    # Size in bytes of send buffer for control system connection.
    # A larger size allows more standard output and standard error data to be buffered on the I/O node by TCP.
    # If send_buffer_size is invalid or not specified, the default value is used (262144).


[cios.sysiod]
    # Settings for CIOS system I/O daemon.

posix_mode = false
    # Run I/O operations using Posix rules.
    # When true each I/O operation initiated from a compute node completes atomically.
    # When false one I/O operation initiated from a compute node may cause multiple I/O operations on the I/O node.
    # When posix_mode is invalid or not specified, the default value is used (false).

log_job_statistics = false
    # Log system I/O statistics from a job when it ends.
    # When true, information about I/O operations is recorded in the I/O node log file.
    # If log_job_statistics is invalid or not specified, the default value is used (false).

log_function_ship_errors = false
    # Log information about a function shipped operation when the operation returns an error.
    # When true, the I/O node log file may become large if they are many errors from function shipped operations.
    # If log_function_ship_errors is invalid or not specified, the default value is used (false).



[bghealthmond]

bghealthmond_enabled=1
        # Enables (1) or disables (0) the BG Health Monitor Daemon

bghealthmond_verbose=0
        # Enables (1) or disables (0) verbose output from the daemon

bghealthmond_frequency=120
        # Number of seconds to wait between samples of the system state.

bghealthmond_log_path=/var/log
        # Path to dump log files to.

bghealthmond_memory_threshold=256000
        # Nomber of kilobytes of memory to use as the minimum on the node before throwing a RAS event.

bghealthmond_neterr_threshold=0
        # Number of network errors that may occurr before registering an event.

bghealthmond_netdrp_threshold=1
        # Percent of total packets sent/received to tolerate before throwing an error.

bghealthmond_loadavg_threshold=100
        # Five minute load average threshold.

bghealthmond_of_threshold=90
        # Percent of the total possible open file descriptors to tolerate before throwing an error.

bghealthmond_nfsretrans_threshold=0
        # Number of retransmissions to tolerate before throwing an error.



[ras]
    # Reliability, Availability, and Serviceability (RAS) configuration and settings.
    # All of the values in this section can be refreshed by running refresh_config from bg_console.

environment = PROD
    # RAS environment.

filter = /bgsys/drivers/ppcfloor/ras/etc/ras_environment_filter.xml
    # RAS environment filter file.
    # This file is used to alter RAS events.

md_install_path = /bgsys/drivers/ppcfloor
    # Root path to RAS metadata files.
    # The metadata files are located in <md_install_path>/component-name/ras/metadata

decoder_install_path = /bgsys/drivers/ppcfloor
    # Root path to RAS decoder files.
    # The decode files are located in <decoder_install_path>/component-name/ras/decoder


[logging]
    # log4cxx configuration. For more information, see http://logging.apache.org/log4cxx/index.html

log4j.logger.ibm = INFO, default
    # ibm logger - Root for logging done by IBM components ('level' progression - TRACE, DEBUG, INFO, WARN, ERROR, FATAL)
log4j.additivity.ibm = false

log4j.logger.ibm.utility                = INFO   # Utility functions
log4j.logger.ibm.utility.cxxsockets     = WARN   # CxxSockets library
log4j.logger.ibm.bgsched                = INFO   # Scheduler APIs
log4j.logger.ibm.security               = INFO   # Security
log4j.logger.ibm.database               = INFO   # Database layer
log4j.logger.ibm.realtime               = INFO   # Real-time server
log4j.logger.ibm.runjob                 = WARN   # job submission
log4j.logger.ibm.runjob.mux             = INFO   # job submission multiplexer
log4j.logger.ibm.runjob.server          = INFO   # job submission server
log4j.logger.ibm.mc                     = DEBUG  # MC (machine controller)
log4j.logger.ibm.boot                   = DEBUG  # MC boot lib
log4j.logger.ibm.bgqconfig              = INFO   # BGQ config
log4j.logger.ibm.icon                   = DEBUG  # iCon
log4j.logger.ibm.bgws                   = INFO   # Web Services
log4j.logger.ibm.mmcs                   = INFO   # MMCS server
log4j.logger.ibm.ras                    = WARN   # RAS
log4j.logger.ibm.master                 = INFO   # BGmaster
log4j.logger.ibm.cios                   = WARN   # Common I/O Services
log4j.logger.ibm.teal                   = INFO   # TEAL


[logging.default]

log4j.appender.default = org.apache.log4j.ConsoleAppender
log4j.appender.default.layout = org.apache.log4j.PatternLayout
log4j.appender.default.layout.ConversionPattern = %d{yyyy-MM-dd HH:mm:ss.SSS} (%-5p) [%t] %c: %m%n


[logging.mc]

log4j.appender.default = org.apache.log4j.ConsoleAppender
log4j.appender.default.layout = org.apache.log4j.PatternLayout
log4j.appender.default.layout.ConversionPattern = %d{yyyy-MM-dd HH:mm:ss.SSS} (%-5p) [%X{subnet}] %c{1} - %m%n


[logging.mmcs]

log4j.appender.default = org.apache.log4j.ConsoleAppender
log4j.appender.default.layout = org.apache.log4j.PatternLayout
log4j.appender.default.layout.ConversionPattern=%d{yyyy-MM-dd HH:mm:ss.SSS} (%-5p) [%t] %X{blockId}%X{user}%X{FD}%X{FT}%c: %m%n


[logging.runjob]

log4j.appender.default = org.apache.log4j.ConsoleAppender
log4j.appender.default.layout = org.apache.log4j.PatternLayout
log4j.appender.default.layout.ConversionPattern = %d{yyyy-MM-dd HH:mm:ss.SSS} (%-5p) [%t] %X{block}%X{job}%X{location}%X{user}%c: %m%n


[logging.master]

log4j.appender.default=org.apache.log4j.ConsoleAppender
log4j.appender.default.layout=org.apache.log4j.PatternLayout
log4j.appender.default.layout.ConversionPattern=%d{yyyy-MM-dd HH:mm:ss.SSS} (%-5p) [%t] %X{ID}%X{ALIAS}%X{FD}%X{FT}%c: %m%n

[logging.cios]

log4j.appender.default=org.apache.log4j.ConsoleAppender
log4j.appender.default.layout=org.apache.log4j.PatternLayout
log4j.appender.default.layout.ConversionPattern=%d{yyyy-MM-dd HH:mm:ss.SSS} (%-5p) [%t] %X{service}%X{pid}%c: %m%n

[baremetal]

TxOpticsFirmwareLevel = B11
TxOpticsFirmwareFilename = /home/bgqadmin/fw/avago/deuce328Tx-0B11-F6B0.a90
RxOpticsFirmwareLevel = B12
RxOpticsFirmwareFilename = /home/bgqadmin/fw/avago/deuce328Rx-0B12-38B5.a90

CloseAllAlertsOnServiceBulkPowerModule = false
CloseAllAlertsOnServiceClockCard = false
CloseAllAlertsOnServiceIoDrawer = false
CloseAllAlertsOnServiceMidplane = false
CloseAllAlertsOnServiceNodeBoard = false
CloseAllAlertsOnServiceNodeDCA = false
CloseAllAlertsOnServiceRack = false
    # The CloseAllAlertsOnServiceXXX options indicate how alerts are to be closed following a successful
    # completion of a Service Action performed by the ServiceXXX command.
    # Set to false to close alerts on only those replaced devices on or in the serviced hardware.
    # Set to true to close alerts on all devices on or in the serviced hardware even if they were not replaced.
    # For example, if ServiceNodeBoard was used to replace a compute card and if CloseAllAlertsOnServiceNodeBoard 
    # was set to false, only those alerts on the replaced compute card are closed. If it was set to true,
    # alerts on the node board and all the devices on the board (e.g., compute card, link chips, optical module,
    # node DCA) are closed even if they were not replaced.

