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

include ../../../../../Make.rules
include ../../../../Make.hlcs.rules
include ../../Make.rules

# calculate number of cpu's for test cases
MAKECPUS=$(shell grep -c '^processor' /proc/cpuinfo)

CXXFLAGS += -Wall
CXXFLAGS += -Wno-old-style-cast
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/utility/include
CXXFLAGS += -pthread
CXXFLAGS += -DBOOST_TEST_DYN_LINK
CXXFLAGS += -DBOOST_TEST_MAIN
CXXFLAGS += -DBOOST_TEST_MAIN

# thread pool tester needs to know how many processors we have
thread_pool_size.o: CXXFLAGS += -DNUM_PROCS=$(MAKECPUS)

LDFLAGS += ../../mux/Options.o
LDFLAGS += -L ../../common -lrunjob_common
LDFLAGS += -Wl,-rpath,$(BGQ_BUILD_DIR)/hlcs/src/runjob/common
LDFLAGS += -L ../../../bgsched -lbgsched
LDFLAGS += -Wl,-rpath,$(BGQ_BUILD_DIR)/hlcs/src/bgsched
LDFLAGS += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS += -lboost_filesystem-mt
LDFLAGS += -lboost_serialization-mt
LDFLAGS += -lboost_program_options-mt
LDFLAGS += -lboost_date_time-mt
LDFLAGS += -lboost_system-mt
LDFLAGS += -lboost_program_options-mt
LDFLAGS += -lboost_unit_test_framework-mt
LDFLAGS += -llog4cxx

TESTERS   = $(patsubst %.cc,%,$(wildcard *.cc))

all: $(TESTERS:%=out_%)

out_%: test_%
	@strip $<
	@echo "execute $@"
	$(SILENT)BG_PROPERTIES_FILE=$(BGQ_INSTALL_DIR)/utility/etc/bg.properties.tpl ./$< 1> $(patsubst test_%, out_%, $<)

test_message_queue: message_queue.o ../../mux/client/MessageQueue.o ../../mux/client/Message.o Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_performance_counter_interval: performance_counter_interval.o ../../mux/performance/Interval.o Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_plugin: plugin.o ../../mux/client/PluginUpdateResources.o Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_server_connection_interval: server_connection_interval.o ../../mux/server/Timer.o Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_socket: socket.o Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)


test_start_tool: start_tool.o ../../mux/commands/start_tool/Options.o ../../mux/commands/start_tool/Debugger.o Makefile
	@echo "link $@"
	$(SILENT)$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)


test_thread_pool_size: thread_pool_size.o Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_version: version.o Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)


install: all 


clean:
	$(RM) test_* out_*
	$(RM) *.o
	$(RM) *.d


distclean: clean

.DELETE_ON_ERROR:

-include *.d
