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
# (C) Copyright IBM Corp.  2012, 2012                              
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
include ../../../../Make.rules
include ../../../Make.gcc.rules
include ../../../Make.test.rules
include ../../Make.test-common.vars

#export BGPM_DUMP_ON_ERR = 1
#export OBJDUMP = 1

CFLAGS += -I$(BGQ_BUILD_DIR)/bgpm/src -fopenmp
LIBS += gomp pthread stdc++

PROG_ARGS = 4
JOB_PPN = 4
export TIMELIMIT = 180

include ../../Make.test-common.rules
