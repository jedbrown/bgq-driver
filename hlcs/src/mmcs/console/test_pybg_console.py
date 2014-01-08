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
# (C) Copyright IBM Corp.  2010, 2011                              
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

import pybg_console;
import string;
import sys;



# NOTE: Passing --help will cause this entire python exe to exit.
#bg=pybg_console.BgConsole("--help");
bg=pybg_console.BgConsole("");
try:

    #
    # simple demonstration showing printing
    # the results all at once.
    rt=bg.cmd("help");
    print rt
    
    
    #
    #  demonstration showing parsing the data one line at a time.
    #
    rt=bg.cmd('list_io');
    for l in string.split(rt, '\n'):
        print l;
    
    
    rt=bg.cmd('list_users');
    for l in string.split(rt, '\n'):
        print l;

except Exception as err:
        print "Oops! Boot script error: ", sys.exc_info()[0]
        print "====================================================================="
        print err
        print "====================================================================="
        sys.exit()

