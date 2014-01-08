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
include ../../../../Make.xlc.rules
#include ../../../../Make.xlc.rules
include ../../../../Make.test.rules
include ../../../Make.test-common.vars

VPATH = ..
C_SRC   = $(wildcard ../*.c)
CXX_SRC = $(wildcard ../*.cc)
TEST_OBJS := $(notdir $(patsubst %.c,%.o,$(C_SRC)) $(patsubst %.cc,%.o,$(CXX_SRC)))
TESTNAME := $(notdir $(shell cd ..;pwd))_$(TESTNAME)

#export BGPM_DUMP_ON_ERR = 1
export OBJDUMP = 1
export TIMELIMIT = 300

#CFLAGS += -qsmp
#CPPFLAGS += -qsmp

CFLAGS += -qsmp
CXXFLAGS += -qsmp
LDFLAGS += -qsmp
LIBS += pthread stdc++


JOB_PPN = 1

include ../../../Make.test-common.rules
