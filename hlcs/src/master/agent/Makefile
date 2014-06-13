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

include ../../../Make.hlcs.rules
include ../../../../Make.rules


TOP_DIR=hlcs
TOP_INSTALL_DIR=$(BGQ_INSTALL_DIR)/$(TOP_DIR)
MODULE_INIT_DIR=$(TOP_INSTALL_DIR)/etc/init


CXXFLAGS += \
 -isystem $(BGQ_INSTALL_DIR) \
 -iquote .. \
 -W \
 -Wall \
 -Wextra \
 -Wundef \
 -Wconversion \
 -O2 \
 -MD -MP \
 -pthread


LDFLAGS += \
 -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility -lcxxsockets \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib \
 -L $(BGQ_INSTALL_DIR)/xml/lib -lXML \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/xml/lib \
 -lboost_date_time-mt \
 -lboost_filesystem-mt \
 -lboost_program_options-mt \
 -lboost_system-mt \
 -lboost_thread-mt \
 -llog4cxx


parts := \
 bgagentd \
 Agent \
 MasterConnection \
 SignalHandler

objs := ${parts:%=%.o}

common_parts := \
 AgentBase \
 BinaryController \
 Ids \
 Protocol

common_objs := ${common_parts:%=../common/%.o}

protocol_parts := \
 BGMasterAgentProtocolSpec
 
protocol_objs := ${protocol_parts:%=../protocol/%.o}


exe := bgagentd
bg_sbin_links = $(exe)




default: all

all: $(exe)

install: $(call INSTALL_SBIN_TARGET, $(exe)) $(bg_links_targets) $(MODULE_INIT_DIR)/bgagent $(MODULE_INIT_DIR)/bgagent.v1r1m0 $(MODULE_INIT_DIR)/bgagent.v1r1m1 $(MODULE_INIT_DIR)/bgagent.v1r1m2 $(MODULE_INIT_DIR)/bgagent.v1r2m0 $(MODULE_INIT_DIR)/bgagent.v1r2m1


$(exe): $(objs) $(common_objs) $(protocol_objs)
	$(CXX) -o $@ $^ $(LDFLAGS)


$(MODULE_INIT_DIR):
	$(MKDIR) -p $@

$(MODULE_INIT_DIR)/%: % | $(MODULE_INIT_DIR)
	$(CP) $^ $@


clean:
	$(RM) *.o *.d

distclean: clean


.PHONY: default all install clean distclean

-include *.d
