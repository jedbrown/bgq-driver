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


objs97 = ${parts:%=%97.o}
objs101 = ${parts:%=%101.o}

exe = bg_realtime_server
exe97 = bg_realtime_server97
exe101 = bg_realtime_server101


CXXFLAGS += \
 -iquote ../../bgsched/realtime \
 -iquote .. \
 -iquote ../.. \
 -iquote ../../../include \
 -isystem ../../../include \

system_uname := $(shell uname -m)

ifeq ($(system_uname),x86_64)

CXXFLAGS += \
 -DUSE_ALTERNATIVE_SQLA

endif


LDFLAGS += \
 -L../../bgsched -lbgsched \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility \
 -L$(BGQ_INSTALL_DIR)/extlib/lib -lboost_program_options-mt -lboost_system-mt \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib:$(BGQ_INSTALL_DIR)/utility/lib:$(BGQ_INSTALL_DIR)/extlib/lib

ifneq ($(wildcard /opt/ibm/db2/V10.1),)
	FOUND_DB2_10 = yes
endif
ifneq ($(wildcard /opt/ibm/db2/V9.7),)
	FOUND_DB2_9 = yes
endif

ifeq ($(FOUND_DB2_10), yes)
CXXFLAGS101    = -DDB2VERSION=\"10.1\"
CXXFLAGS101   += -isystem /opt/ibm/db2/V10.1/include
LDFLAGS101    += -L/opt/ibm/db2/V10.1/lib64 -ldb2 -Wl,-rpath,/opt/ibm/db2/V10.1/lib64
server        += $(exe)
server        += $(exe101)
server        += realtime_version_check
install_files += $(call INSTALL_SBIN_TARGET, $(exe101))
install_files += $(call INSTALL_SBIN_TARGET, $(exe))
bg_sbin_links += $(exe101)
bg_sbin_links += $(exe)
bg_sbin_links += realtime_version_check
install_files += $(call INSTALL_SBIN_TARGET, realtime_version_check)
endif

ifeq ($(FOUND_DB2_9), yes)
CXXFLAGS97     = -DDB2VERSION=\"9.7\"
CXXFLAGS97    += -isystem /opt/ibm/db2/V9.7/include
LDFLAGS97     += -L/opt/ibm/db2/V9.7/lib64 -ldb2 -Wl,-rpath,/opt/ibm/db2/V9.7/lib64
server        += $(exe)
server        += $(exe97)
server        += realtime_version_check
install_files += $(call INSTALL_SBIN_TARGET, $(exe97))
install_files += $(call INSTALL_SBIN_TARGET, $(exe))
bg_sbin_links += $(exe97)
bg_sbin_links += $(exe)
bg_sbin_links += realtime_version_check
install_files += $(call INSTALL_SBIN_TARGET, realtime_version_check)
endif

ifneq ($(FOUND_DB2_9), yes)
ifneq ($(FOUND_DB2_10), yes)
# both not found, use provided path
CXXFLAGS97     = -DDB2VERSION=\"UNKNOWN\"
CXXFLAGS97    += -isystem $(DB2PATH)/include
LDFLAGS97     += -L$(DB2PATH)/lib64 -ldb2 -Wl,-rpath,$(DB2PATH)/lib64
server        += $(exe97)
server        += realtime_version_check
install_files += $(call INSTALL_SBIN_TARGET, $(exe97))
bg_sbin_links += $(exe97)
bg_sbin_links += $(exe)
bg_sbin_links += realtime_version_check
install_files += $(call INSTALL_SBIN_TARGET, realtime_version_check)
endif
endif

.PHONY: default all install clean distclean docs uninstall


default: all


all: $(exe) $(server) 


%97.o: %.cc
	$(CXX) -o $@ $(CXXFLAGS) $(CXXFLAGS97) -c $<

%101.o: %.cc
	$(CXX) -o $@ $(CXXFLAGS) $(CXXFLAGS101) -c $<

$(exe): realtime_version_check
	$(LN) -sf $^ $@

$(exe97): $(objs97) 
	$(CXX) $(LDFLAGS) $(LDFLAGS97) -o $@ $^

$(exe101): $(objs101)
	$(CXX) $(LDFLAGS) $(LDFLAGS101) -o $@ $^

test_db2_mem_leak_1.o: test_db2_mem_leak_1.cc
	$(CXX) -o $@ $(CXXFLAGS97) -isystem /opt/ibm/db2/V9.7/include -c $<
test_db2_mem_leak_1: test_db2_mem_leak_1.o
	$(CXX) $(LDFLAGS) $(LDFLAGS97) -o $@ $^

realtime_version_check.o: CXXFLAGS += -DDB2PATH=\"$(DB2PATH)/bin\"
realtime_version_check.o: CXXFLAGS += -DREALTIME_PATH=\"$(BGQ_INSTALL_DIR)/hlcs/sbin\"
realtime_version_check: realtime_version_check.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -lboost_regex-mt -o $@ $^


install: $(install_files) $(bg_links_targets)


clean:
	$(RM) $(exe) $(exe97) $(exe101) test_db2_mem_leak_1 realtime_version_check 
	$(RM) realtime_version_check.o $(objs97) $(objs101) *.d db2/*.o db2/*.d db2/table/*.o db2/table/*.d

distclean: clean


uninstall:
	$(RM) -r $(install_files) $(BGQ_INSTALL_DIR)/hlcs/docs/realtime


-include *.d db2/*.d db2/table/*.d
