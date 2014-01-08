#!/usr/bin/python
# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# Licensed Materials - Property of IBM                             
#                                                                  
# Blue Gene/Q                                                      
#                                                                  
# (C) Copyright IBM Corp.  2011, 2011                              
#                                                                  
# US Government Users Restricted Rights -                          
# Use, duplication or disclosure restricted                        
# by GSA ADP Schedule Contract with IBM Corp.                      
#                                                                  
# This software is available to you under the                      
# Eclipse Public License (EPL).                                    
#                                                                  
# ================================================================ 
#                                                                  
# end_generated_IBM_copyright_prolog                               

import os
import sys
import readline
import pwd
import pdb
from socket import *
from ctypes import *

class Addrmap:
    _iface = ""
    _ip = ""
    
    def __init__(self):
        self._iface = ""
        self._ip = ""

    def Print(self):
        return self._iface + "=" + self._ip

def getInput(prompt):
    retval = 'n'
    retval = raw_input(prompt)
    if retval == "quit":
        sys.exit(1)
    elif retval == "done":
        doEndProcessing()
    return retval

def getifaddrs():
    # just enough wrapper for c library getifaddrs()
    # to get the addresses.
    addrmaplist = []
    class in_addr(Union):
        _fields_ = [("s_addr", c_uint32)]
    class in6_u(Union):
        _fields_ = [("u6_addr8",  (c_uint8 * 16)),
                    ("u6_addr16", (c_uint16 * 8)),
                    ("u6_addr32", (c_uint32 * 4))]
        
    class in6_addr(Union):
        _fields_ = [("in6_u", in6_u)]

    class sockaddr(Structure):
        _fields_ = [("sa_family", c_uint16 ),
                    ("sa_data",   (c_uint8 * 14))]
    class sockaddr_in(Structure):
        _fields_ = [("sin_family", c_short),
		    ("sin_port",   c_ushort),
		    ("sin_addr",   in_addr),
		    ("sin_zero",   (c_char * 8))]
    class sockaddr_in6(Structure):
        _fields_ = [("sin6_family", c_short),
                    ("sin6_port", c_ushort),
                    ("sin6_flowinfo", c_uint32),
                    ("sin6_addr", in6_addr),
                    ("sin6_scope_id", c_uint32)]
            

    class ifa_ifu_u(Union):
        _fields_ = [("ifu_broadaddr", c_void_p),
                    ("ifu_dstaddr",   c_void_p)]

    class ifaddrs(Structure):
        _fields_ = [("ifa_next",    c_void_p),
                    ("ifa_name",    c_char_p),
                    ("ifa_flags",   c_uint),
                    ("ifa_addr",    c_void_p),
                    ("ifa_netmask", c_void_p),
                    ("ifa_ifu",     ifa_ifu_u),
                    ("ifa_data",    c_void_p)]
        
    libc = CDLL("libc.so.6")
    ifaddrptr = c_void_p(None)
    result = libc.getifaddrs(pointer(ifaddrptr))
    if result:
        return None
    ifa = ifaddrs.from_address(ifaddrptr.value)
    result = {}
    while True:
        addtolist = True
        newaddrmap = Addrmap()
        # get the sockaddr pointer
        sa = sockaddr.from_address(ifa.ifa_addr)
        if sa.sa_family == AF_INET:
            sockaddrin = sockaddr_in.from_address(ifa.ifa_addr)
            newaddrmap._ip = inet_ntop(sockaddrin.sin_family,sockaddrin.sin_addr)
        elif sa.sa_family == AF_INET6: # IPv6
            sockaddrin6 = sockaddr_in6.from_address(ifa.ifa_addr)
            sockaddrin6 = sockaddr_in6.from_address(ifa.ifa_addr)
            newaddrmap._ip = inet_ntop(sockaddrin6.sin6_family,sockaddrin6.sin6_addr)
        else:
            addtolist = False

        newaddrmap._iface = ifa.ifa_name
        if addtolist == True:
            addrmaplist.append(newaddrmap)

        if ifa.ifa_next:
            ifa = ifaddrs.from_address(ifa.ifa_next)
        else:
            #libc.freeifaddrs(pointer(ifaddrptr))
            return addrmaplist

class Action:
    "Action enumeration for a Policy's Behavior"
    INVALID_ACTION, FAILOVER, RESTART, CLEANUP = range(4)

class Trigger:
    "Trigger enumeration for a Policy"
    INVALID_TRIGGER, KILL_REQUESTED, BINARY_ABEND, AGENT_ABEND = range(4)

class HostPair:
    "Failover pair class"
    _host_pair = []
    def __init__(self):
        _host_pair = ["localhost", "localhost"]

class Behavior:
    "A class representing a behavior to execute when a policy is triggered"
    _action = Action.INVALID_ACTION
    _host_pairs = []
    _retries = 3 # "Threes are good."
    def Print(self):
        if self._action == Action.FAILOVER:
            print "    action=FAILOVER"
        elif self._action == Action.RESTART:
            print "    action=RESTART"
        elif self._action == Action.CLEANUP:
            print "    action=CLEANUP"
        print "    retries=" + str(self._retries)
        if self._action == Action.FAILOVER:
            pairstring = "    host pairs:"
            for pair in  self._host_pairs:
                pairstring = pairstring + " " +  pair._host_pair[0] + "->" + pair._host_pair[1]
            print pairstring
        
class Policy:
    "A class representing a policy"
    _trigger = Trigger.INVALID_TRIGGER
    _behavior = Behavior()
    _name = "default"
    def __init__(self):
        default_behavior = Behavior()
        default_behavior._host_pairs = ["localhost","localhost"]
        self._behavior = default_behavior

    def Print(self):
        print self._name + ":"
        if self._trigger == Trigger.KILL_REQUESTED:
            print "    trigger=KILL_REQUESTED" 
        elif self._trigger == Trigger.BINARY_ABEND:
            print "    trigger=BINARY_ABEND"
        elif self._trigger == Trigger.AGENT_ABEND:
            print "    trigger=AGENT_ABEND"
        else:
            print "    trigger=INVALID_TRIGGER"
        self._behavior.Print()
 
    def setupPolicy(self):
        # First define a trigger.
        trigger = "0"
        while trigger != "k" and trigger != "b" and trigger != "a":
            print "A policy needs a trigger.  A trigger is what causes it"
            print "to act.  BGmaster supports 'killed', 'binary' or 'agent'"
            print "triggers."
            print
            print "A 'killed' trigger is activated when a binary exits with"
            print "a non-zero exit code or when a user signal kills it."
            print 
            print "A 'binary' trigger is activated when an operating system"
            print "signal is sent to the process (SIGSEGV, for example)."
            print
            print "An 'agent' trigger is activated when an agent failure"
            print "is detected."
            print
            print "Which kind of trigger would you like to use for this policy?"
            
            triggerprompt = prompt + "[k/b/a] "
            trigger = getInput(triggerprompt);
            if trigger != "k" and trigger != "b" and trigger != "a":
                print "FAIL.  Please provide a valid trigger."
        os.system('clear')

        action = "0"
        while action != "r" and action != "f":
            print "Now we need to define the behavior to associate with the"
            print "new trigger."
            print
            print "A behavior consists of an action to take, a retry count"
            print "and optional pairs of hosts for failover."
            print
            print "An action can be either 'restart' or 'failover'."
            print "Are you creating a restart or a failover action?"
            actionprompt = prompt + "[r/f]"
            action = getInput(actionprompt)
            if action != "r" and action != "f":
                print "FAIL.  Please provide a valid action."
        os.system('clear')

        retries = "x"
        while retries.isdigit() == False:
            print "How many retries would you like to allow for this policy?"
            retries = getInput(prompt)
        os.system('clear')
        
        hostpairs = []
        if action == "f":
            print "You've chosen a failover policy.  This requires at least"
            print "one failover pair."
            while True:
                print "Enter a host name or IP address of a host to fail FROM"
                fromhost = getInput(prompt)
                print "Enter a host name or IP address of a host to fail TO"
                tohost = getInput(prompt)
                hp = HostPair()
                hp._host_pair = [ fromhost, tohost ]
                hostpairs.append(hp)
                print "Would you like to define another failover pair for this policy?(y/n)"
                yn = getInput(promptynb)
                if yn == "n" or len(yn) == 0:
                    break
                if yn == "back":
                    return True
        os.system('clear')

        # Define the behavior.
        behavior = Behavior()
        # First the action
        if action == "r":
            behavior._action = Action.RESTART
        elif action == "f":
            behavior._action = Action.FAILOVER
        else:
            print "Invalid action " + action + " selected."
            return None
        # Then the host pairs
        behavior._host_pairs = hostpairs
        self._behavior = behavior

        # Now get the trigger and set up the policy.
        if trigger == "k":
            self._trigger = Trigger.KILL_REQUESTED
        elif trigger == "b":
            self._trigger = Trigger.BINARY_ABEND
        elif trigger == "a":
            self._trigger = Trigger.AGENT_ABEND
        else:
            print "Invalid trigger " + trigger + " selected."
            return None

        print "Finally, the policy needs a descriptive name."
        print "A good name usually includes the trigger and"
        print "the action. Ex: 'killed_restart'."
        new_name = getInput(prompt)
        if len(new_name) == 0:
            self._name = "UNNAMED"
        else:
            self._name = new_name
        os.system('clear')

class SubnetSn:
    "A class representing a subnet service node's configuration"
    _subnet_id = ""
    _subnet_num = 0
    _logdir = "/bgsys/logs/BGQ"
    _primary_ip = "0"
    _primary_port = 0
    _primary_iface = "lo"
    _primary_bgiface = "lo"
    _backup = False
    _backup_ip = "0"
    _backup_iface = "lo"
    _backup_port = 0
    _backup_bgiface = "lo"

    def printSubnet(self):
        subnet_str = "[machinecontroller.subnet." + str(self._subnet_num) + "]\n"
        subnet_str = subnet_str + "Name = " + self._subnet_id + "\n"

        subnet_str = subnet_str + "PrimaryServer = " + self._primary_ip + "\n"
        subnet_str = subnet_str + "PrimaryServerInterface = " + self._primary_iface + "\n"
        subnet_str = subnet_str + "PrimaryServerPort = " + str(self._primary_port) + "\n"
        subnet_str = subnet_str + "PrimaryBGInterface = " + self._primary_bgiface + "\n"
        if self._backup == True:
            subnet_str = subnet_str + "BackupServer = " + self._backup_ip + "\n"
            subnet_str = subnet_str + "BackupServerInterface = " + self._backup_iface + "\n"
            subnet_str = subnet_str + "BackupServerPort = " + str(self._backup_port) + "\n"
            subnet_str = subnet_str + "BackupBGInterface = " + self._backup_bgiface + "\n"
        outfile.write(subnet_str + "\n")

    def getSubnetInfo(self):
        os.system('clear')
        subnetprompt = "{" + self._subnet_id + "}" + prompt
        print "Configuring " + self._subnet_id
        outstr = "Where do you want this subnet to log?"
        print outstr
        logprompt = subnetprompt + "[" + self._logdir + "]"
        self._logdir = getInput(logprompt)
            
        outstr = "What is the primary IP address or host name for this subnet?"
        print outstr
        defaultip = "127.0.0.1"
        newip = getInput(subnetprompt + "[" + defaultip + "] ")
        if len(newip) == 0:
            self._primary_ip = defaultip
        else:
            self._primary_ip = newip
            
        print "Enter the primary TCP port that the Subnet MC for this subnet will listen on."
        defaultport = 33456
        pport = getInput(subnetprompt + "[" + str(defaultport) + "] ")
        if len(pport) != 0:
            try:
                self._primary_port = int(pport)
            except ValueError:
                print "Invalid port number " + pport + ". Try again."
        else:
            self._primary_port = defaultport

        print "Enter the ethernet adapter that the Primary MC should use to talk"
        print "to the Subnet MC for this subnet."
        defaultiface = "lo"
        iface = getInput(subnetprompt + "[" + defaultiface + "] ")
        if len(iface) == 0:
            self._primary_iface = defaultiface
        else:
            self._primary_iface = iface
            
        print "Enter the ethernet adapter on the Subnet Service Node to use to"
        print "talk to the BG hardware."
        defaultiface = "eth1"
        iface = getInput(subnetprompt + "[" + defaultiface + "] ")
        if len(iface) == 0:
            self._primary_bgiface = defaultiface
        else:
            self._primary_bgiface = iface

        print "NOTE: mc_server currently NEEDS to have a backup specified."
        print "If it doesn't, the subnet will be IGNORED.  It is STRONGLY"
        print "suggested that you respond 'y' here and specify a backup"
        print "even if it is the same as the primary."
        print "Will there be a backup for this SubnetMc for failover?"
        subnetpromptyn = subnetprompt + "(y/[n])"
        yn = getInput(subnetpromptyn)
        if yn == "y":
            self._backup = True
            print "Enter the host or IP address of the machine that can run the"
            print "backup SubnetMc process"
            defaultip = "127.0.0.1"
            newip = getInput(subnetprompt + "[" + defaultip + "] ")
            if len(newip) == 0:
                self._backup_ip = defaultip
            else:
                self._backup_ip = newip
            
            print "Enter the ethernet adapter for the backup SubnetMc process"
            defaultiface = "eth1"
            iface = getInput(subnetprompt + "[" + defaultiface + "] ")
            if len(iface) == 0:
                self._backup_iface = defaultiface
            else:
                self._backup_iface = iface
            
            print "Enter the TCP port number that the Subnet MC for this subnet"
            print "will listen on when running on the back SSN."
            defaultport = 33456
            pport = getInput(subnetprompt + "[" + str(defaultport) + "] ")
            if len(pport) != 0:
                try:
                    self._backup_port = int(pport)
                except ValueError:
                    print "Invalid port number " + pport + ". Try again."
            else:
                self._primary_port = defaultport

            print "Enter the ethernet adapter on the backup SSN to use to talk to"
            print "the BG hardware."
            defaultiface = "eth0"
            iface = getInput(subnetprompt + "[" + defaultiface + "] ")
            if len(iface) == 0:
                self._backup_bgiface = defaultiface
            else:
                self._backup_bgiface = iface

class Sn:
    "A class representing the Primary or Backup service node's configuration"
    _agentaddrlist = []
    _clientaddrlist = []
    _agentport = 32041
    _clientport = 32042
    _defaultlogdir = "/bgsys/logs/BGQ"

    def getSnInfo(self):
        """Configure bgmaster_server parameters."""
        os.system('clear')
        print "Configuring bgmaster_server"
        # Get list of all IP addresses on the machine.
        addrs = getifaddrs()
        print "Enter the number of each address on which bgmaster_server "
        print "should listen for =agents=.  When you are done, type"
        print "Enter to stop adding new addresses.  Type \"h\" to get"
        print "help with this question."

        defaultaddrm4 = Addrmap()
        defaultaddrm4._ip = "127.0.0.1"
        defaultaddrm4._iface = "lo"
        defaultaddrm6 = Addrmap()
        defaultaddrm6._ip = "::1"
        defaultaddrm6._iface = "lo"

        num = "127.0.0.1"
        usedaddrnums = []
        # Get the addr/port pairs for agent listeners.
        while len(num) != 0:
            addrnum = 0
            for addr in addrs:
                if str(addrnum) not in usedaddrnums:
                    print "(" + str(addrnum) + ") " + addr.Print()
                addrnum = addrnum + 1
                
            num = getInput(prompt)
            
            if num == 'h':
                print "bgmaster_server communicates with bgagentd"
                print "processes on the service node, subnet service nodes,"
                print "front end nodes and login nodes to run BGQ servers."
                print "The bgagentd processes connect to bgmaster server so"
                print "they must know how to find it on the network."
                print "Select all IP addresses connected to networks that"
                print "bgagentd processes will use to contact bgmaster_server."
                print "Consider how your BGQ nodes are connected carefully."
                print "NOTE: If only loopback addresses are configured (::1,"
                print "127.0.0.1, and 'loopback'), you will not be able to"
                print "run bgagents on remote hosts which means you will "
                print "not be able to connect SubnetMc process or runjob_muxes."
            if num != 'h' and len(num) != 0:
                if num.isdigit() == True:
                    self._agentaddrlist.append(addrs[int(num)])
                    usedaddrnums.append(num)
                else:
                    num + " is an invalid entry.  Try again."
        os.system('clear')
        if len(self._agentaddrlist) == 0:
            print "No agent listen addresses configured!  Will use loopback."
            print
            self._agentaddrlist.append(defaultaddrm4)
            self._agentaddrlist.append(defaultaddrm6)

        # Get the addr/port pairs for client listeners.
        print "Enter the number of each address on which bgmaster_server "
        print "should listen for =clients=.  When you are done, type"
        print "Enter to stop adding new addresses.  Type \"h\" to get"
        print "help with this question."
                
        num = "127.0.0.1"
        usedaddrnums = []
        while len(num) != 0:
            addrnum = 0
            for addr in addrs:
                if str(addrnum) not in usedaddrnums:
                    print "(" + str(addrnum) + ") " + addr.Print()
                addrnum = addrnum + 1
                
            num = getInput(prompt)
            if num == 'h':
                print "bgmaster_server communicates with client command"
                print "processes.  These commands are used to start, stop"
                print "and get information about the BGQ servers."
                print "The client command processes connect to bgmaster server so"
                print "they must know how to find it on the network."
                print "Select all IP addresses connected to networks that"
                print "client commands will use to contact bgmaster_server."
                print "Consider how your BGQ nodes are connected carefully."
                print "NOTE: If only loopback addresses are configured, ::1,"
                print "127.0.0.1, and 'loopback', you will not be able to"
                print "run bgmaster commands like 'master_status' from "
                print "remote machines like FENs and LNs."
            if num != 'h' and len(num) != 0:
                if num.isdigit() == True:
                    self._clientaddrlist.append(addrs[int(num)])
                    usedaddrnums.append(num)
                else:
                    num + " is an invalid entry.  Try again."
        os.system('clear')
        if len(self._agentaddrlist) == 0:
            print "No client listen addresses configured!  Will use loopback."
            print
            self._clientaddrlist.append(defaultaddrm4)
            self._clientaddrlist.append(defaultaddrm6)

        agentportprompt = '-->[32041]'
        while True:
            print "Enter the port number for bgmaster_server to listen for agents"
            print "or enter nothing to accept the default."
            port = getInput(agentportprompt)
            if len(port) == 0:
                port = '32041'
            try:
                self._agentport = int(port)
                break
            except ValueError:
                print "Bad port number " + port + "Try again."
        os.system('clear')                    
        
        clientportprompt = '-->[32042]'
        while True:
            print "Enter the port number for bgmaster_server to listen for clients"
            print "or enter nothing to accept the default."
            port = getInput(clientportprompt)
            if len(port) == 0:
                port = '32042'
            try:
                self._clientport = int(port)
                break
            except ValueError:
                print "Bad port number " + port + "Try again."

        os.system('clear')
        
        print "Next, set the logging location for bgmaster_server.  This will"
        print "also be the default logging location for all managed binaries."
        logprompt = '-->[' + self._defaultlogdir + '] '
        loglocation = getInput(logprompt)
        if len(loglocation) != 0:
            self._defaultlogdir = loglocation
        return False

class Server:
    # Class to represent a specific server
    _name = ""
    _userid = "bgqadmin"
    _logdir = "/bgsys/logs/BGQ"
    _hostlist = []
    _policylist = []
    _args = ""
    _binary = ""
    _binname = ""

    def __init__(self):
        self._policylist = []
        self._hostlist = [ "127.0.0.1", "::1" ]

    def setupServer(self, name):
        os.system('clear')
        # Get the server settings.
        self._name = name
        print
        os.system('clear')
        print "Setting up server " + self._name

        # Set up a few that tend to have aliases that differ from
        # their actual binary executable names.
        tmp_binname = self._name
        if self._name == "mc_server":
            tmp_binname = "mc_server_64"
        if "Subnet" in self._name:
            tmp_binname = "SubnetMc_64"
        if "runjob_mux" in self._name:
            tmp_binname = "runjob_mux"
        if "realtime_server" in self._name:
            tmp_binname = "bg_realtime_server"
        if "bgws" in self._name:
            tmp_binname = "bgws_server"

        print "Using " + tmp_binname + " for the binary executable for this server."
        print "if this is not correct, enter the correct executable name."
        newbinname = getInput(prompt)
        if len(newbinname) != 0:
            self._binname = newbinname
        else:
            self._binname = tmp_binname

        serverprompt = "{" + self._name + "}" + prompt 

        shortpath = ""
        if self._name == "bgmaster_server" or self._name == "realtime_server" or "runjob_mux" in self._name or self._name == "runjob_mux" or self._name == "mmcs_server" or self._name == "runjob_server":
            shortpath = "hlcs/sbin/"
        elif self._name == "mc_server" or "Subnet" in self._name:
            shortpath = "control/sbin/"
        elif self._name == "bgws":
            shortpath = "bgws/sbin/"
        else:
            print "I can't guess where this binary might be. Please enter the full path:"
            self._binary = getInput(serverprompt)

        if len(shortpath) != 0:
            self._binary = driver_path + shortpath + self._binname

        print "Calculated full path to the binary for " + self._name + " is "
        print self._binary
        print "If this is not correct, enter the correct full path."
        newbinpath = getInput(serverprompt)
        if len(newbinpath) != 0:
            self._binary = newbinpath

        while os.path.exists(self._binary) == False and self._binary != "skip":
            print "Specified binary path is invalid.  Please enter a valid one"
            print "or 'skip' to give up."
            print "If you skip this step, you'll need to manually add a valid"
            print "path to the [master.binmap] section."
            self._binary = getInput(prompt)

        userprompt = serverprompt + "[" + self._userid + "] "
        print
        print "Enter the user id you wish to run this server "
        print "under or hit enter for the default."
        while True:
            user = getInput(userprompt)
            if len(user) != 0:
                try:
                    pwd.getpwnam(user)
                    self._userid = user
                    break
                except KeyError:
                    print "WARNING: User " + user + " not found in password database."
                    print "(T)ry again or (u)se it anyway?"
                    tuprompt = prompt + "[t]/u "
                    tu = getInput(tuprompt)
                    if tu == "u":
                        self._userid = user
                        break
        print
        print "Enter any command line parameters that this server will need."
        self._args = getInput(serverprompt)

        print 
        print "If you'd like to change this server's logging directory from"
        print "the default, enter it below."
        logdirprompt = serverprompt + "[" + self._logdir + "]"
        newlogdir = getInput(logdirprompt)
        if len(newlogdir) != 0:
            self._logdir = newlogdir

        # This variable will collect the host names required by all
        # policies that we define below.  Later, we'll add them to 
        # the allowed host list.
        defined_policy_hostnames = []
        tmp_pol_list = list(master_policy_list)
        print "Next, we need to assign any necessary policies to the server"
        yn = 'y'
        while len(tmp_pol_list) != 0 and yn == 'y':
            print "The defined policies not assigned to this server:"
            for pol in tmp_pol_list:
                print pol._name
            print
            print "Would you like to assign any of these policies to this server?"
            yn = getInput(serverprompt + "(y/[n])")
            while yn == "y" and len(tmp_pol_list) != 0:
                print "The defined policies not assigned to this server:"
                for pol in tmp_pol_list:
                    print pol._name
                print "Enter the name of a policy to assign to this server:"
                pol_name = getInput(serverprompt)
                
                # We'll see if the policy we've entered has been defined.
                foundname = False
                foundpol = Policy()
                # Loop through the list of policies we haven't used...
                for checkpol in tmp_pol_list:
                    if pol_name == checkpol._name:
                        foundname = True
                        foundpol = checkpol
                        for host_pair in checkpol._behavior._host_pairs:
                            defined_policy_hostnames.append(host_pair._host_pair[0])
                            defined_policy_hostnames.append(host_pair._host_pair[1])
                        break

                if foundname == False:
                    print "ERROR: Policy " + pol_name + " is an invalid policy name."
                    print "Skipping."
                elif pol_name in self._policylist:
                    print "ERROR: Policy " + pol_name + " already added to server " + self._name
                    print "Skipping."
                else:
                    self._policylist.append(pol_name)
                    try:
                        tmp_pol_list.remove(foundpol)
                    except ValueError:
                        print "."
                if len(tmp_pol_list) != 0:
                    print "Would you like to add another policy to this server?"
                    yn = getInput(serverprompt + "(y/[n])")

        os.system('clear')

        print "Next, add any host IP addresses or names on which you'd like "
        print "to allow this server to run.  If you enter nothing, you get "
        print "the loopback addresses only.  Note that using 'loopback' as"
        print "a host name may cause intermittent connection problems because"
        print "getaddrinfo() in the libraries that ship with Red Hat"
        print "sometimes does not return both the IPv4 and IPv6 addresses."
        print
        print "NOTE:  All hosts that are required by policies you associated"
        print "with this server earlier will AUTOMATICALLY be added to this list."
        # Add all host names required by assigned policies!
        defaultlist = True
        uniquenames = []
        if len(defined_policy_hostnames) != 0:
            # We've got at least one.  Don't use the defaults.
            del self._hostlist[:]
            defaultlist = False
        for hostname in defined_policy_hostnames:
            if hostname not in uniquenames:
                uniquenames.append(hostname)
        for name in uniquenames:
            self._hostlist.append(name)

        if defaultlist == False:
            print "The following hosts have been added automatically:"
            for host in self._hostlist:
                print host

        yn = "y"
        while yn == "y":
            print "Would you like to add a new host now?"
            yn = getInput(serverprompt + "y/[n]")
            if yn == "y":
                if defaultlist:
                    # Empty the list.
                    del self._hostlist[:]
                    defaultlist = False
                newhost = getInput(serverprompt + "[hostname] ")
                if newhost not in self._hostlist:
                    self._hostlist.append(newhost)
                else:
                    print "Host " + newhost + " already in host list."
                print "Current list of hosts for this server:"
                for h in self._hostlist:
                    print h

        for name in self._hostlist:
            print name
        del defined_policy_hostnames[:]

class Agent:
    _logdir = "/bgsys/logs/BGQ"
    _uid = "bgqadmin"
    def setupAgent(self):
        """Configure bgagentd control parameters."""
        os.system('clear')
        print "Setting up bgagentd parameters."
        print "Specify the location for bgagentd to log or enter to accept default"
        logprompt = '-->[' + self._logdir + '] '
        loglocation = getInput(logprompt)
        if len(loglocation) != 0:
            self._logdir = loglocation
        print "Specify the user ID under which bgagentd will run or enter to accept default"
        uidprompt = '-->[' + self._uid + '] '
        uidlocation = getInput(uidprompt)
        if len(uidlocation) != 0:
            self._uid = uidlocation
        return False
        
def doMasterDotClient():
    """Write output for bgmaster client configuration."""
    outfile.write("[master.client]\n")
    # Simple.  Just the host connection.
    hostconnect = "host = "
    first = True
    for addr in primarysn._clientaddrlist:
        addrstr = ""
        if first == False:
            addrstr = ","
        first = False
        if ":" in addr._ip:
            tmp = addrstr + "[" + addr._ip
            if addr._ip != "::1":
                tmp = tmp + "%" + addr._iface
            tmp = tmp + "]"
            addrstr = tmp
        else:
            addrstr = addrstr + addr._ip
        hostconnect = hostconnect + addrstr + ":" + str(primarysn._clientport)
    outfile.write(hostconnect + "\n")
    outfile.write("\n")

def doMasterDotServer():
    """Write output for bgmaster_server configuration."""
    global outfile
    outfile.write("[master.server]\n")
    # Ports:
    agent_listen_ports = "agent_listen_ports = "
    first = True
    for addr in primarysn._agentaddrlist:
        addrstr = ""
        if first == False:
            addrstr = ","
        first = False
        if ":" in addr._ip:
            tmp = addrstr + "[" + addr._ip
            if addr._ip != "::1":
                tmp = tmp + "%" + addr._iface
            tmp = tmp + "]"
            addrstr = tmp
        else:
            addrstr = addrstr + addr._ip
        agent_listen_ports = agent_listen_ports + addrstr + ":" + str(primarysn._agentport)
    client_listen_ports = "client_listen_ports = "
    first = True
    for addr in primarysn._clientaddrlist:
        addrstr = ""
        if first == False:
            addrstr = ","
        first = False
        if ":" in addr._ip:
            tmp = addrstr + "[" + addr._ip
            if addr._ip != "::1":
                tmp = tmp + "%" + addr._iface
            tmp = tmp + "]"
            addrstr = tmp
        else:
            addrstr = addrstr + addr._ip
        client_listen_ports = client_listen_ports + addrstr + ":" + str(primarysn._clientport)
                
    outfile.write(agent_listen_ports)
    outfile.write("\n")
    outfile.write(client_listen_ports)
    outfile.write("\n")
    outfile.write("logdir = " + primarysn._defaultlogdir + "\n")
    outfile.write("\n")

def doMasterDotAgent():
    """Write output for bgagentd configuration."""
    outfile.write("[master.agent]\n")
    hostconnect = "host = "
    first = True
    for addr in primarysn._agentaddrlist:
        addrstr = ""
        if first == False:
            addrstr = ","
        first = False
        if ":" in addr._ip:
            tmp = addrstr + "[" + addr._ip
            if addr._ip != "::1":
                tmp = tmp + "%" + addr._iface
            tmp = tmp + "]"
            addrstr = tmp
        else:
            addrstr = addrstr + addr._ip
        hostconnect = hostconnect + addrstr + ":" + str(primarysn._agentport)
    outfile.write(hostconnect + "\n")
    outfile.write("logdir = " + agent._logdir + "\n")
    outfile.write("agentuid = " + agent._uid + "\n")
    outfile.write("\n")

def doMasterServers():
    """Write configuration for each server"""
    binmap = "[master.binmap]\n"
    user = "[master.user]\n"
    args = "[master.binargs]\n"
    logdirs = "[master.logdirs]\n"
    hostlist = "[master.policy.host_list]\n"
    policymap = "[master.policy.map]\n"
    for server in server_obj_list:
        binmap = binmap + server._name + " = " + server._binary + "\n"
        user = user + server._name + " = " + server._userid + "\n"
        args = args + server._name + " = " + server._args
        for subnet in subnet_list:
            if server._name == subnet._subnet_id:
                # This is a subnetmc.  We need to add the ip addresses to the args.
                if subnet._primary_ip != "0":
                    args = args + "--ip " + subnet._primary_ip
                if subnet._backup_ip != "0":
                    args = args + "," + subnet._backup_ip
        args = args + "\n"
        logdirs = logdirs + server._name + " = " + server._logdir + "\n"
        hostlist = hostlist + server._name + " = "
        first = True
        for host in server._hostlist:
            if first == False:
                hostlist = hostlist + ","
            first = False
            hostlist = hostlist + host
        hostlist = hostlist + "\n"

        policymap = policymap + server._name + " = "
        first = True
        for policy in server._policylist:
            if first == False:
                policymap = policymap + ","
            first = False
            policymap = policymap + policy
        policymap = policymap + "\n"
    outfile.write(binmap + "\n")
    outfile.write(user + "\n")
    outfile.write(args + "\n")
    outfile.write(logdirs + "\n")
    outfile.write(hostlist + "\n")
    outfile.write(policymap + "\n")

def doPolicies():
    """Write configuration for each policy."""
    failpolicy = "[master.policy.failure]\n"
    for policy in master_policy_list:
        trigger = "bogus"
        if policy._trigger == Trigger.KILL_REQUESTED:
            trigger = "killed"
        elif policy._trigger == Trigger.BINARY_ABEND:
            trigger = "binary"
        elif policy._trigger == Trigger.AGENT_ABEND:
            trigger = "agent"
        baction = "bogus"
        if policy._behavior._action == Action.FAILOVER:
            baction = "failover"
        elif policy._behavior._action == Action.RESTART:
            baction = "restart"
        elif policy._behavior._action == Action.CLEANUP:
            baction = "cleanup"
        hostlist = ""
        first = True
        for hostpair in policy._behavior._host_pairs:
            if first == False:
                hostlist = hostlist + "|"
            first = False
            hostlist = hostlist + hostpair._host_pair[0] + ":" + hostpair._host_pair[1]
        failpolicy = failpolicy + policy._name + " = " + trigger + "," + baction + "," + str(policy._behavior._retries) + "," + hostlist + "\n"
    outfile.write(failpolicy + "\n")

def doSubnets():
    """Run the subnet configs."""
    for subnet in subnet_list:
        subnet.printSubnet()
    
def doEndProcessing():
    """All of the input has been gathered, call the methods that write it out."""
    doMasterDotServer()
    doMasterDotClient()
    doMasterDotAgent()
    doMasterServers()
    doPolicies()
    doSubnets()
    sys.exit(1)

def setDriverLoc():
    """Set the driver location."""
    os.system('clear')
    print "Where is the driver installed? (enter for the default location)"
    defaultdriverpath = "/bgsys/drivers/ppcfloor/"
    driverprompt = prompt + "[" + defaultdriverpath + "]"
    global driver_path
    while True:
        driver_path = getInput(driverprompt)
        if len(driver_path) == 0:
            driver_path = defaultdriverpath
        if os.path.exists(driver_path):
            break
        else:
            print "Driver path " + driver_path + " not found on this machine."
            print "Enter another path."
    return False

def setUpSubnets():
    """Configure the Subnet Service Nodes"""
    os.system('clear')
    # Get subnets
    while True:
        print "How many subnet service nodes do you have?"
        sc = getInput(prompt + "[0] ")
        if len(sc) == 0:
            subnet_count = 0
            break
        else:
            try:
                subnet_count = int(sc)
                break
            except ValueError:
                print sc + "is not a valid subnet count.  Try again."

    curr_num = 0
    while subnet_count != 0:
        newsubnet = SubnetSn()
        newsubnet._subnet_id = "Subnet" + str(curr_num)
        newsubnet._subnet_num = curr_num
        curr_num = curr_num + 1
        newsubnet.getSubnetInfo()
        subnet_list.append(newsubnet)
        subnet_count = subnet_count - 1
    return False

def setUpPolicies():
    """Configure BG/Q Failover and Restart Policies"""
    os.system('clear')
    # Set up policies
    print "Configuring Policies"
    print
    print "bgmaster_server can restart and fail over managed binaries."
    print "It does this using defined policies.  It's not a complete"
    print "High Availability solution (see Linux-HA for that), but"
    print "it's very useful for restarting servers on the primary"
    print "service node and necessary for failing over SubnetMc processes."
    print "The defaults in the bg.properties template have basic restart "
    print "policies defined and assigned to all of the servers."
    print "You should only need to define new policies here if you "
    print "are configuring Subnet (or some other) failover."
    print
    print "Some default restart policies have been created automatically."
    print "If you need failover policies or different restart policies,"
    print "you'll need to create those."
    print "Here are the current policies:"
    for policy in master_policy_list:
        policy.Print()
        
    print "Do you want to set up any more policies now? (y/n)"
    yn = getInput(promptynb)
    if yn == "y":
        os.system('clear')
        while True:
            current_policy = Policy()
            current_policy.setupPolicy()
            master_policy_list.append(current_policy)
            os.system('clear')

            print "Do you want to set up another policy?(y/n)"
            yn = getInput(promptyn)
            if yn != "y":
                break
            print "Here are the current policies:"
            for policy in master_policy_list:
                policy.Print()

    if yn == "back":
        return True
    return False

def setUpServers():
    """Configure managed BG/Q control system servers."""
    os.system('clear')
    # Set up servers.
    for subnet in subnet_list:
        server_list.append(subnet._subnet_id)
    print
    print "Next, we'll configure the servers with some aliases and associate some"
    print "configuration parameters.  The default server aliases are the following:"
    for server in server_list:
        print server

    print
    print "If you don't want to configure any of these servers, you can clear the"
    print "list.  You will have the opportunity later to either add or remove"
    print "individual servers from this list."
    print 
    print "Would you like to clear the list of servers?"
    yn = "n"
    yn = getInput(promptyn)
    if yn == "y":
        del server_list[:]

    yn = "y"
    while yn == "y":
        print "Do you wish to add to this list?  (Additional runjob_mux servers are a good candidate for this.)"
        yn = getInput(promptynb)
        if yn == "y":
            print "Enter the server name."
            newname = getInput(prompt)
            if newname in server_list:
                print "ERROR:"
                print newname + " is already in the server list.  If you wish to have more than one instance,"
                print " enter a different, unique, alias.  \"runjob_mux_fen2\", for example."
            else:
                server_list.append(newname)
            print 
            print "Current aliases:"
            for server in server_list:
                print server
        if yn == "back":
            return True

    print "This is the current list of servers to manage:"
    for server in server_list:
        print server

    yn = "y"
    while yn == "y":
        print
        print "Do you wish to REMOVE items from this list?"
        print "(These would be servers you don't plan on running."
        yn = getInput(promptynb)
        if yn == "y":
            print "Enter the server name."
            newname = getInput(prompt)
            if newname not in server_list:
                print "ERROR:"
                print newname + " is not in the server list."
            else:
                server_list.remove(newname)
            print 
            print "Current aliases:"
            for server in server_list:
                print server
        if yn == "back":
            return True

    # Now we've got a complete list of the servers to configure.
    for servername in server_list:
        newserver = Server()
        newserver.setupServer(servername)
        server_obj_list.append(newserver)
    return False

def help():
    print "BlueGene/Q BGMaster configuration assistant asks questions "
    print "and provides suggestions for important bg.properties "
    print "parameters in an output file."
    print
    print "You may type 'quit' to exit this program at any prompt."
    print "To skip any further processing and produce output for "
    print "what you've done so far, you can enter 'done' at any prompt."
    print
    print "At some prompts, you can type 'back' to return to an earlier"
    print "configuration step."

# Start here!
prompt = '--> '
promptyn = '-->(y/[n]) '
promptynh = '-->(y/[n]/h) '
promptynb = '-->(y/[n]/back) '
os.system('clear')
help()
print
print "Is this program running on your primary service node?"
yn = str(getInput(promptyn))
rightyn = 'y'
if yn == rightyn:
    # Do nothing
    print '\n'
else:
    print "You must run this program on the primary service node.  Ending."
    sys.exit(1)

os.system('clear')

# Initial global declarations
server_list = [ "mc_server", "realtime_server", "mmcs_server", "bgws_server", "runjob_server", "runjob_mux", "teal_server", "teal_bg" ]
server_obj_list = []
agent = Agent()
master_policy_list = [] 
subnet_list = []
primarysn = Sn()
driver_path = ""
index = 0

# Create default policies.  Restart for agent, killed and binary.
default_behavior = Behavior()
default_behavior._action = Action.RESTART
default_behavior._retries = 3

agent_policy = Policy()
agent_policy._name = "agent_restart"
agent_policy._trigger = Trigger.AGENT_ABEND
agent_policy._behavior = default_behavior

killed_policy = Policy()
killed_policy._name = "killed_restart"
killed_policy._trigger = Trigger.KILL_REQUESTED
killed_policy._behavior = default_behavior

binary_policy = Policy()
binary_policy._name = "binary_restart"
binary_policy._trigger = Trigger.BINARY_ABEND
binary_policy._behavior = default_behavior

master_policy_list.append(agent_policy)
master_policy_list.append(killed_policy)
master_policy_list.append(binary_policy)

print "Where do you want your output file?"
outfilename = "./bg.config"
outprompt = prompt + "[" + outfilename + "] "
inputoutfilename = getInput(outprompt)
if len(inputoutfilename) != 0:
    outfilename = inputoutfilename
outfile = open(outfilename, 'w')

print "You can choose to use a menu mode that allows you to pick"
print "specific sections to configure or a linear mode that asks"
print "every question before you can get output.  You can end linear"
print "mode by entering 'quit' or 'done' at any prompt.  Quit ends"
print "the program with no output.  Done stops execution and dumps"
print "output for completed work."
print
print "Would you prefer (m)enu or (l)inear mode?"
promptml = prompt + '[l]/m '
ml = getInput(promptml)

# This list is a bunch of "function pointers" to various independent methods
# that implement the program's question and answer logic.  Each method can
# return either True or False.  If it returns True, the user has requested
# that the code jump back to a previous method.  
method_list = [ setDriverLoc, agent.setupAgent, primarysn.getSnInfo, setUpSubnets, setUpPolicies, setUpServers ]
method_count = len(method_list)
if ml != 'm':
    while index < method_count:
        func = method_list[index]
        back = func()
        if back == True:
            index = index - 1
        else:
            index = index + 1
    print "Configuration complete.  Output is in " + outfilename

if ml == 'm':
    menu_selection = ""
    class MethodMap:
        _num = 0
        _method = setDriverLoc
    method_map_list = []
    while menu_selection != "done":
        os.system('clear')
        print "Select an item number or type 'done' or 'quit':"
        # Generate a menu:
        item_num = 0
        for method in method_list:
            mm = MethodMap()
            mm._num = item_num
            mm._method = method
            method_map_list.append(mm)
            print "(" + str(item_num) + ") " + method.__doc__
            item_num = item_num + 1
        print "(done)"
        print "(quit)"
        menu_selection = getInput(prompt)
        if menu_selection == "done":
            continue
        found = False
        for mm in method_map_list:
            if mm._num == int(menu_selection):
                found = True
                mm._method()
        if found == False:
            print "Invalid selection " + menu_selection + ".  Try again."
        del method_map_list[:]
doEndProcessing()
