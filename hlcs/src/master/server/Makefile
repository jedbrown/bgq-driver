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
 -L$(BGQ_INSTALL_DIR)/db/lib -lbgdb \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib \
 -L$(BGQ_INSTALL_DIR)/ras/lib -lras \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/ras/lib \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility -lcxxsockets \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib \
 -L$(BGQ_INSTALL_DIR)/xml/lib -lXML \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/xml/lib


parts := \
 AgentManager \
 AgentRep \
 Alias \
 AliasList \
 Behavior \
 bgmaster_server \
 ClientController \
 ClientManager \
 DBUpdater \
 MasterController \
 Registrar \

objs := ${parts:%=%.o}

common_parts := \
 AgentBase \
 Ids \
 Protocol \
 ThreadLog

common_objs := ${common_parts:%=../common/%.o}

protocol_parts := \
 BGMasterAgentProtocolSpec \
 BGMasterClientProtocolSpec

protocol_objs := ${protocol_parts:%=../protocol/%.o}


exe := bgmaster_server
bg_sbin_links = $(exe)


clean_cmds := $(RM) *.o *.d $(exe)


default: all

all: $(exe) $(call INSTALL_SBIN_TARGET, $(exe)) $(bg_links_targets)

$(exe): $(objs) $(common_objs) $(protocol_objs)
	$(CXX) $(LDFLAGS) -o $@ $^

install: all

clean:
	$(clean_cmds)

distclean:
	$(clean_cmds)


.PHONY: default all install clean distclean

-include *.d
