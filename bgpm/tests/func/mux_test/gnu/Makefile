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
include ../../../../../Make.rules
include ../../../../Make.gcc.rules
include ../../../../Make.test.rules
include ../../../Make.test-common.vars

VPATH = ..
C_SRC   = $(wildcard ../*.c)
CXX_SRC = $(wildcard ../*.cc)
TEST_OBJS := $(notdir $(patsubst %.c,%.o,$(C_SRC)) $(patsubst %.cc,%.o,$(CXX_SRC)))
TESTNAME := $(notdir $(shell cd ..;pwd))_$(TESTNAME)

#export BGPM_DUMP_ON_ERR = 1
#export OBJDUMP = 1
export TIMELIMIT = 120

CFLAGS += -fopenmp
CPPFLAGS += -fopenmp
LIBS += test_utils gomp pthread stdc++
LIB_DIRS += ../../../test_utils
LOCAL_LIBS += ../../../test_utils/libtest_utils.a

#PROG_ARGS = BGPM_SW_DISTRIB
PROG_ARGS = 16 
JOB_PPN = 1
export BG_COREDUMP_ONEXIT=1
export EXPORTENV=BG_COREDUMP_ONEXIT


#LIBS := $(subst bgpm,bgpm_debug,$(LIBS))

include ../../../Make.test-common.rules
