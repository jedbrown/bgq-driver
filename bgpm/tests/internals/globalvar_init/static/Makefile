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
TEST_OBJS := test_main.o
TESTNAME := $(notdir $(shell cd ..;pwd))_$(TESTNAME)

CFLAGS := -I. -I.. $(CFLAGS) -fopenmp
LIBS += gomp pthread 
LIBS := $(subst bgpm,,$(LIBS))
LIBS := $(subst SPI_upci_cnk,,$(LIBS))
LIBS := $(subst SPI_upci_cnk,,$(LIBS))
LIBS := $(subst SPI_cnk,,$(LIBS))
LIBS := $(subst SPI,,$(LIBS))
LIBS += Vars

LOCAL_LIBS := 


#LDFLAGS := $(subst static,dynamic,$(LDFLAGS))
LIB_DIRS += .

ifeq ($(strip $(RUNPATH)),)
export RUNPATH=/bgusr/$(USER)
endif

PROG_ARGS = 16    # num threads per proc
JOB_PPN = 4       # num processes


include ../../../Make.test-common.rules


libVars.a : vars.o vars.cc
	$(AR) libVars.a vars.o  

$(TARGET) : libVars.a $(TEST_OBJS)


