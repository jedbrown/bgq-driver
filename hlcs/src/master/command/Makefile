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
 -Wall \
 -Wextra \
 -W \
 -Wundef \
 -Wconversion \
 -O2 \
 -MD -MP \
 -pthread

LDFLAGS += \
 -L../lib -lbgmasterclient \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility -lcxxsockets \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib \
 -llog4cxx


common_parts := \
 ArgParse

common_objs := ${common_parts:%=../common/%.o}

bin_exes := \
 alias_wait \
 binary_status \
 binary_wait \
 get_errors \
 get_history \
 list_agents \
 list_clients \
 master_status \
 monitor_master

sbin_exes := \
 master_start \
 master_stop \
 bgmaster_server_log_level \
 bgmaster_server_refresh_config \
 fail_over

exes := $(bin_exes) $(sbin_exes)


build_cmd = $(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


installs := \
 $(call INSTALL_SBIN_TARGET, $(sbin_exes)) \
 $(call INSTALL_BIN_TARGET, $(bin_exes))

bg_sbin_links = $(sbin_exes)
bg_bin_links = $(bin_exes)


default: all

all: $(exes)


alias_wait: alias_wait.o $(common_objs)
	$(build_cmd)

bgmaster_server_log_level: bgmaster_server_log_level.o $(common_objs)
	$(build_cmd)

binary_status: bin_status.o $(common_objs)
	$(build_cmd)

binary_wait: bin_wait.o $(common_objs)
	$(build_cmd)

fail_over: fail_over.o $(common_objs)
	$(build_cmd)

get_errors: get_errors.o $(common_objs)
	$(build_cmd)

get_history: get_history.o $(common_objs)
	$(build_cmd)

list_agents: list_agents.o $(common_objs)
	$(build_cmd)

list_clients: list_clients.o $(common_objs)
	$(build_cmd)

master_start: master_start.o $(common_objs)
	$(build_cmd)

master_status: master_status.o $(common_objs)
	$(build_cmd)

master_stop: master_stop.o $(common_objs)
	$(build_cmd)

monitor_master: monitor_master.o $(common_objs)
	$(build_cmd)

bgmaster_server_refresh_config: reload_config.o $(common_objs)
	$(build_cmd)

install: $(installs) $(bg_links_targets)

clean:
	$(RM) *.o *.d $(exes)

distclean: clean

.PHONY: default all install clean distclean

-include *.d
