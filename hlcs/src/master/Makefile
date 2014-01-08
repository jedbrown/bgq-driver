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
# (C) Copyright IBM Corp.  2011, 2011                              
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

include ../../Make.hlcs.rules
include ../../../Make.rules

TOP_DIR=hlcs
TOP_INSTALL_DIR=$(BGQ_INSTALL_DIR)/$(TOP_DIR)
MODULE_INIT_DIR=$(TOP_INSTALL_DIR)/etc/init

EXECS = bgmaster_server
EXECS += bgagentd

DIRS += protocol
DIRS += docs

lib_base:=bgmasterclient
lib_dev:=lib$(lib_base).so
lib_vers:=$(lib_dev).1
lib_full:=$(lib_vers).0.0

DIR_TARGS_ALL		= $(addsuffix .all,$(DIRS))
DIR_TARGS_INSTALL       = $(addsuffix .install,$(DIRS))
DIR_TARGS_CLEAN		= $(addsuffix .clean,$(DIRS))
DIR_TARGS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))

DIR_TARGS  = $(DIR_TARGS_ALL)
DIR_TARGS += $(DIR_TARGS_CLEAN)
DIR_TARGS += $(DIR_TARGS_DISTCLEAN)
DIR_TARGS += $(DIR_TARGS_INSTALL)

#CXXFLAGS += -gstabs
CXXFLAGS += -O2
CXXFLAGS += -Wall
CXXFLAGS += -MD
CXXFLAGS += -I $(BGQ_BUILD_DIR)/hlcs/include/master
CXXFLAGS += -I $(BGQ_INSTALL_DIR)
CXXFLAGS += -I $(BGQ_INSTALL_DIR)/bgq_util/include
CXXFLAGS += -I $(BGQ_INSTALL_DIR)/utility/include
CXXFLAGS += -I $(BGQ_INSTALL_DIR)/utility/include/cxxsockets
CXXFLAGS += -I $(BGQ_INSTALL_DIR)/ras/include
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/hlcs/include
CXXFLAGS += -pthread
CXXFLAGS_NODB := $(CXXFLAGS)
CXXFLAGS_NODB += -UWITH_DB
ifeq ($(BGQDB), yes)
CXXFLAGS += -DWITH_DB
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/db/include
else
CXXFLAGS += -UWITH_DB
endif

#CXXFLAGS += -E

LDFLAGS  += -lboost_thread-mt
LDFLAGS  += -lboost_date_time-mt
LDFLAGS  += -lboost_program_options-mt
LDFLAGS  += -lboost_system-mt
LDFLAGS  += -lboost_filesystem-mt
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/bgq_util/lib -lbgq_util
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility
# This should come out and we should only link to the XML.o binary to avoid 
# sucking in all of the other xml interfaces.  Either that, or change the xml libs.
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/xml/lib -lXML
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/ras/lib -lras
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/bgq_util/lib
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/xml/lib
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/ras/lib
LDFLAGS_NODB := $(LDFLAGS)
ifeq ($(BGQDB), yes)
LDFLAGS    += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
LDFLAGS    += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib
endif

CLIENT_LDFLAGS = $(LDFLAGS)
CLIENT_LDFLAGS +=
CLIENT_LDFLAGS += -Wl,-rpath,$(BGQ_LIB_INSTALL_DIR)

PROTOCOL_OBJS  = protocol/BGMasterAgentProtocolSpec.o
PROTOCOL_OBJS += protocol/BGMasterClientProtocolSpec.o

COMMON_OBJS = BGMasterProtocol.o
COMMON_OBJS += BGAgentBase.o
COMMON_OBJS += Ids.o
COMMON_OBJS += Thread.o

BGM_OBJS += BGAgentRep.o
BGM_OBJS += ClientManager.o
BGM_OBJS += Registrar.o
BGM_OBJS += BGAgentManager.o
BGM_OBJS += Alias.o
BGM_OBJS += Policy.o
ifeq ($(BGQDB), yes)
BGM_DB_OBJS := DBUpdater.o
endif
BGM_NODB_OBJS := $(BGM_OBJS)
BGM_NODB_OBJS += BGMaster_nodb.o
BGM_NODB_OBJS += bgmaster_server_nodb.o
BGM_OBJS += BGMaster.o

BGA_OBJS  = bgagentd.o
BGA_OBJS += BinaryController.o
BGA_OBJS += BGAgent.o
BGA_OBJS += ArgParse.o

LIB_OBJS  = $(COMMON_OBJS)
LIB_OBJS += ListAgents.o
LIB_OBJS += BGMasterClientApi.o
LIB_OBJS += ArgParse.o

INSTALL_LIST += $(call INSTALL_SBIN_TARGET, bgmaster_server)
INSTALL_LIST += $(call INSTALL_SBIN_TARGET, bgmaster_server_nodb)
INSTALL_LIST += $(call INSTALL_SBIN_TARGET, bgagentd)
INSTALL_LIST += $(call INSTALL_SBIN_TARGET, master_start)
INSTALL_LIST += $(call INSTALL_SBIN_TARGET, bgmaster_server_log_level)
INSTALL_LIST += $(call INSTALL_BIN_TARGET, master_status)
INSTALL_LIST += $(call INSTALL_SBIN_TARGET, master_stop) 
INSTALL_LIST += $(call INSTALL_BIN_TARGET, get_errors)
INSTALL_LIST += $(call INSTALL_BIN_TARGET, get_history)
INSTALL_LIST += $(call INSTALL_BIN_TARGET, binary_status)
INSTALL_LIST += $(call INSTALL_BIN_TARGET, list_agents)
INSTALL_LIST += $(call INSTALL_BIN_TARGET, list_clients)
INSTALL_LIST += $(call INSTALL_BIN_TARGET, binary_wait)
INSTALL_LIST += $(call INSTALL_BIN_TARGET, alias_wait)
INSTALL_LIST += $(call INSTALL_BIN_TARGET, monitor_master)
INSTALL_LIST += $(call INSTALL_SBIN_TARGET, fail_over)
INSTALL_LIST += $(call INSTALL_SBIN_TARGET, bgmaster_server_refresh_config)
INSTALL_LIST += $(call INSTALL_SBIN_TARGET, startagents.py)
INSTALL_LIST += $(call INSTALL_LIB_TARGET,$(lib_full) $(lib_vers) $(lib_dev))


bg_sbin_links := \
 bgmaster_server \
 bgagentd \
 master_start \
 master_stop \
 fail_over \
 bgmaster_server_refresh_config \
 bgmaster_server_log_level\

bg_bin_links := \
 alias_wait \
 binary_status \
 binary_wait \
 list_agents \
 list_clients \
 master_status \
 monitor_master \
 get_errors \
 get_history

.PHONY: $(DIR_TARGS) clean docs install distclean all build

all: $(DIR_TARGS_ALL) $(INSTALL_LIST) install build

$(MODULE_INIT_DIR):
	$(MKDIR) -p $@

$(MODULE_INIT_DIR)/%: % | $(MODULE_INIT_DIR)
	$(CP) $^ $@

install: build $(INSTALL_LIST) $(lib_dev) $(MODULE_INIT_DIR)/bgagent $(MODULE_INIT_DIR)/bgagent.v1r1m0 $(bg_links_targets) ras_scan

clean:			$(DIR_TARGS_CLEAN)
			$(RM) $(INSTALL_LIST)
			$(RM) *.o *.o_64 *~ core* \#*
			$(RM) *.d
			$(RM) lib*so*
			$(RM) $(BGQ_LIB_INSTALL_DIR)/$(lib_vers)
			$(RM) $(BGQ_LIB_INSTALL_DIR)/$(lib_full)
			$(RM) $(BGQ_LIB_INSTALL_DIR)/$(lib_dev)
			$(RM) bgmaster_server_nodb

distclean:	clean

build: $(DIR_TARGS_ALL) bgmaster_server bgagentd

$(lib_dev): $(lib_vers)
	ln -sf $< $@


$(lib_vers): $(lib_full)
	ln -sf $< $@


$(LIB_OBJS): CXXFLAGS += -fPIC
$(lib_full): LDFLAGS += -Wl,-soname,$(lib_vers)
$(lib_full): $(LIB_OBJS) | $(DIR_TARGS_ALL)
	$(CXX) $(LDFLAGS) -O1 -shared -o $@ $^ $(PROTOCOL_OBJS)


$(DIR_TARGS):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))

bgmaster_server_nodb.o: bgmaster_server.cc
	$(CXX) $(CXXFLAGS_NODB) -UWITH_DB -c -o $@ $<

BGMaster_nodb.o: BGMaster.cc
	$(CXX) $(CXXFLAGS_NODB) -UWITH_DB -c -o $@ $<

flags:
	echo $(CXXFLAGS_NODB)
	echo $(LDFLAGS_NODB)

bgmaster_server_nodb: $(COMMON_OBJS) $(BGM_NODB_OBJS) | $(DIR_TARGS_ALL) bgmaster_server_nodb.o BGMaster_nodb.o
	$(CXX) $(CXXFLAGS_NODB) -o $@ $^ $(PROTOCOL_OBJS) $(LDFLAGS_NODB) -lpthread -lcxxsockets

bgmaster_server: $(COMMON_OBJS) bgmaster_server.o $(BGM_OBJS) $(BGM_DB_OBJS) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(PROTOCOL_OBJS) $(LDFLAGS) -lpthread -lcxxsockets

alias_wait: alias_wait.o $(lib_full) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(CLIENT_LDFLAGS) -lpthread -lcxxsockets	

master_start: master_start.o $(lib_full) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(CLIENT_LDFLAGS) -lpthread -lcxxsockets	

master_stop: master_stop.o $(lib_full) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(CLIENT_LDFLAGS) -lpthread -lcxxsockets	

monitor_master: monitor_master.o $(lib_full) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(CLIENT_LDFLAGS) -lpthread -lcxxsockets	

get_errors: get_errors.o $(lib_full) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(CLIENT_LDFLAGS) -lpthread -lcxxsockets	

get_history: get_history.o $(lib_full) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(CLIENT_LDFLAGS) -lpthread -lcxxsockets	

master_status: master_status.o $(lib_full) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(CLIENT_LDFLAGS) -lpthread -lcxxsockets 

list_agents: list_agents.o $(lib_full) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(CLIENT_LDFLAGS) -lpthread -lcxxsockets

list_clients: list_clients.o $(lib_full) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(CLIENT_LDFLAGS) -lpthread -lcxxsockets	

binary_wait: bin_wait.o $(lib_full) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(CLIENT_LDFLAGS) -lpthread -lcxxsockets	

binary_status: bin_status.o $(lib_full) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(CLIENT_LDFLAGS) -lpthread -lcxxsockets	

fail_over: fail_over.o $(lib_full) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(CLIENT_LDFLAGS) -lpthread -lcxxsockets	

bgmaster_server_refresh_config: reload_config.o $(lib_full) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(CLIENT_LDFLAGS) -lpthread -lcxxsockets	

bgmaster_server_log_level: bgmaster_server_log_level.o $(lib_full) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(CLIENT_LDFLAGS) -lpthread -lcxxsockets	

bgagentd: $(COMMON_OBJS) $(BGA_OBJS) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(PROTOCOL_OBJS) $(LDFLAGS) -lpthread -lcxxsockets

vars:
	echo $(DIR_TARGS_ALL) $(DIR_TARGS_CLEAN) dt $(DIR_TARGS)

ras_scan: MasterRasMetadata.h
	$(BGQ_BUILD_DIR)/scripts/rasScan.py '$(BGQ_BUILD_DIR)/hlcs/src/master' '$(BGQ_INSTALL_DIR)/hlcs/ras/metadata' 'bgmaster_ras.xml' '$(BGQ_INSTALL_DIR)/ras/include/ras.xml'
	@touch $@

