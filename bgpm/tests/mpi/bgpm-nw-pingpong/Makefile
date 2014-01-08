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
include ../../../Make.mpi-xlc.rules
include ../../../Make.test.rules
include ../../Make.test-common.vars

VPATH = ../common/
CFLAGS += -I../common
CMN_SRC = ../common/TestBuffer.cc ../common/Timer.cc ../common/TestArgs.cc
TEST_OBJS := $(notdir $(patsubst %.c,%.o,$(C_SRC)) $(patsubst %.cc,%.o,$(CXX_SRC)) $(patsubst %.cc,%.o,$(CMN_SRC)))
LIBS += m test_utils #pthread stdc++
#LIBS := $(subst bgpm,bgpm_debug,$(LIBS))
LIB_DIRS += ../../test_utils
LOCAL_LIBS += ../../test_utils/libtest_utils.a


export BGPM_DUMP_ON_ERR = 1
#export OBJDUMP = 1
export TIMELIMIT =180
#PROG_ARGS = "-l 100 -s 32768 -w 4"
export BGPM_TRACESTART=1

JOB_PPN = 1
JOB_NODES = 2


include ../../Make.test-common.rules
