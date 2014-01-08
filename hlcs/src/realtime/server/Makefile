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
# (C) Copyright IBM Corp.  2004, 2011                              
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
include ../Make.realtime.rules


db2_table_parts := \
 Block \
 Cable \
 EventLog \
 IoDrawer \
 IoNode \
 Job \
 Midplane \
 Node \
 NodeCard \
 Switch


db2_parts :=\
 AbstractTable \
 DbChangesMonitor \
 TableInfos \
 Transaction \
 Transactions \
 ${db2_table_parts:%=table/%}


parts :=\
 bg_realtime_server \
 BgRealtimeServer \
 Client \
 ClientFilter \
 ClientFilterVisitor \
 CommandClient \
 Configuration \
 DbChangesMonitor \
 Status \
 TcpServer \
 ${db2_parts:%=db2/%}


objs = ${parts:%=%.o}

exe = bg_realtime_server


install_files = $(call INSTALL_SBIN_TARGET, $(exe))

bg_sbin_links := $(exe)


CXXFLAGS += \
 -iquote ../../bgsched/realtime \
 -iquote .. \
 -iquote ../.. \
 -iquote ../../../include \
 -isystem ../../../include \
 -isystem $(DB2PATH)/include


system_uname := $(shell uname -m)

ifeq ($(system_uname),x86_64)

CXXFLAGS += \
 -DUSE_ALTERNATIVE_SQLA

endif


LDFLAGS += \
 -L../../bgsched -lbgsched \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility \
 -L$(BGQ_INSTALL_DIR)/extlib/lib -lboost_program_options-mt -lboost_system-mt \
 -L$(DB2PATH)/lib -ldb2 \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib:$(BGQ_INSTALL_DIR)/utility/lib:$(BGQ_INSTALL_DIR)/extlib/lib:$(DB2PATH)/lib


.PHONY: default all install clean distclean docs uninstall


default: all


all: $(exe) test_db2_mem_leak_1


$(exe): $(objs)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

test_db2_mem_leak_1: test_db2_mem_leak_1.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^


install: $(install_files) $(bg_links_targets)


clean:
	$(RM) $(exe) *.o *.d db2/*.o db2/*.d db2/table/*.o db2/table/*.d

distclean: clean


uninstall:
	$(RM) -r $(install_files) $(BGQ_INSTALL_DIR)/hlcs/docs/realtime


-include *.d db2/*.d db2/table/*.d
