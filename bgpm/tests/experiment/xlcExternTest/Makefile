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
include ../../../Make.xlc.rules
include ../../../Make.test.rules
include ../../Make.test-common.vars


VPATH = ..
C_SRC   = $(wildcard *.c)
CXX_SRC = $(wildcard *.cc)
TEST_OBJS := $(notdir $(patsubst %.c,%.o,$(C_SRC)) $(patsubst %.cc,%.o,$(CXX_SRC)))
TESTNAME := $(notdir $(shell cd ..;pwd))_$(TESTNAME)

#export BGPM_DUMP_ON_ERR = 1
export OBJDUMP = 1
export TIMELIMIT = 180 

CFLAGS += -qsmp
CXXFLAGS += -qsmp
LDFLAGS += -qsmp
LIBS += pthread stdc++

#CFLAGS += -fopenmp
#CXXFLAGS += -fopenmp
#LIBS += test_utils pthread stdc++

# use my BTMALLOC trace
#INCL_BTMALLOC=1
ifeq ($(INCL_BTMALLOC),)
else
BTMALLOC_PATH = ../../../../../../bgq_test/tools/BackTraceMalloc
CFLAGS   += -DINCL_BTMALLOC -I$(BTMALLOC_PATH)
CXXFLAGS += -DINCL_BTMALLOC -I$(BTMALLOC_PATH)
CFLAGS   := $(subst -O2,,$(CFLAGS))
CXXFLAGS := $(subst -O2,,$(CFLAGS))
LIBS := $(subst bgpm,bgpm_nonopt,$(LIBS))
LIBS += btmalloc pthread
LDFLAGS  += -Wl,--wrap,malloc -Wl,--wrap,free -Wl,--wrap,realloc -Wl,--wrap,abort
LIB_DIRS += $(BTMALLOC_PATH)
LOCAL_LIBS += $(BTMALLOC_PATH)/libbtmalloc.a
export BTMALLOC_LOG=$(TESTNAME).bttrace
export BTMALLOC_LEVELS=9
export EXPORTENV=BTMALLOC_LOG BTMALLOC_LEVELS
endif


PROG_ARGS = 8
JOB_PPN = 8
#export GOMP_STACKSIZE = 25000
export BGPM_EVT_FEEDBACK = 3
#export BGPM_EVT_FEEDBACK = 0
export BG_SHAREDMEMSIZE=1
#export RUN=0


include ../../Make.test-common.rules
