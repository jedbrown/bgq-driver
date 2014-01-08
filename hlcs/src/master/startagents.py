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
import getopt

def usage():
    print "startagents.py --file=< machine file > --agent=< agent path > < --sudo > --start|--stop"

try:
        opts, args = getopt.getopt(sys.argv[1:], "hfau", ["help", "start", "stop", "file=", "agent=", "sudo"])
except getopt.GetoptError, err:
    print str(err)
    usage()
    sys.exit(2)

machine_exists = False
sudo = False
agent_loc = ""
startstop = "";

for o, a in opts:
    if o in ("-h", "--help"):
        usage()
        print "\nstartagents.py starts or stops remote agent init scripts using ssh."
        print "You'll need to have the proper authorities on the remote machine"
        print "and have ssh keys properly configured.\n"
        print "The --file parameter is a filename with a list of remote machines"
        print "on which you intend to start remote agents.\n"
        print "The --agent parameter is the full path to the init script. By default,"
        print "it runs /etc/init.d/bgagent.\n"
        print "The --sudo parameter determines whether or not to run the init script using 'sudo'"
        print "to acquire root authority."
        print "--start or --stop specify whether to start or stop the init script.\n"
        print "Typically, startagents.py will be run from a service node and the"
        print "machine file will have the names of all of the front end nodes and"
        print "subnet service nodes."
        sys.exit()
    elif o in ("-f", "--file"):
        machine_file=open(a)
        machine_exists = True
    elif o in ("-a", "--agent"):
        agent_loc=a
    elif o in ("--start"):
        startstop = "start"
    elif o in ("--stop"):
        startstop = "stop"
    elif o in ("-u", "--sudo"):
        sudo=True

if startstop == "":
    print "Must specify --start or --stop to start or stop remote agents"
    usage()
    sys.exit(2)

if machine_exists == False:
    print "Must specify a file containing a list of remote host names"
    usage()
    sys.exit(2)

if len(agent_loc) == 0:
    # Take default
    agent_loc = "/etc/init.d/bgagent"

for machine in machine_file:
    machine = machine.rstrip()
    s = ""
    if startstop == "start":
        s = "Starting agent on " + machine
    elif startstop == "stop":
        s = "Stopping agent on " + machine
    print s
    if sudo == True:
        exestr = "ssh -t " + machine + " sudo " + agent_loc + " " + startstop
    else:
        exestr = "ssh " + machine + " " + agent_loc + " " + startstop

    os.system(exestr)
