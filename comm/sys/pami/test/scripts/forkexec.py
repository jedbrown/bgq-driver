#!/usr/bin/python
# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
#  --------------------------------------------------------------- 
# Licensed Materials - Property of IBM                             
# Blue Gene/Q 5765-PER 5765-PRP                                    
#                                                                  
# (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           
# US Government Users Restricted Rights -                          
# Use, duplication, or disclosure restricted                       
# by GSA ADP Schedule Contract with IBM Corp.                      
#                                                                  
#  --------------------------------------------------------------- 
#                                                                  
# end_generated_IBM_copyright_prolog                               
import os
import sys

if ( len( sys.argv ) < 4 ):
   print "forkexec.py config_file np test_to_run"
   sys.exit(2)

config = sys.argv[1]
np = sys.argv[2]
test = sys.argv[3]
tmprev = test.split("/")
testname = tmprev[len(tmprev)-1]

for i in xrange(int(np)):
    rc = os.fork()
    if (rc > 0):
        os.environ['PAMI_SOCK_TASK'] = str(i)
        os.environ['PAMI_SOCK_SIZE'] = np
        os.environ['PAMI_UDP_CONFIG'] = config
#       sys.stdout = open(str(i) + ".stdout", 'w');
#       sys.stderr = open(str(i) + ".stderr", 'w');
        execname = testname # + " 2>" + str(i) + ".err"
        print ("fork() .. " + execname)
        os.execlp(test,execname)
    elif (rc == 0):
        print "Started rank " + str(i)
    else:
        print "Failed to start rank " + str(i)


