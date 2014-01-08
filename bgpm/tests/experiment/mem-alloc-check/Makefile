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
#include ../../../Make.xlc.rules
include ../../../Make.test.rules
include ../../Make.test-common.vars

#export BGPM_DUMP_ON_ERR = 1
export OBJDUMP = 1
export TIMELIMIT = 180 

#CFLAGS += -qsmp
#CPPFLAGS += -qsmp
#LDFLAGS += -qsmp
#LIBS += test_utils pthread stdc++

CFLAGS += -fopenmp
CPPFLAGS += -fopenmp
LIBS += test_utils gomp pthread stdc++

LIB_DIRS += ../../test_utils
LOCAL_LIBS += ../../test_utils/libtest_utils.a


PROG_ARGS = 1
JOB_PPN = 1
export GOMP_STACKSIZE = 25000
export BGPM_EVT_FEEDBACK = 3
export BG_SHAREDMEMSIZE=1
#export RUN=0


include ../../Make.test-common.rules
