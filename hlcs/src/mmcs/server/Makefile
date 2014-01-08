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
 -O2 \
 -DWITH_DB \
 -MD -MP \
 -Wall \
 -pthread \
 -iquote .. \
 -iquote ../.. \
 -iquote ../../../include \
 -isystem $(BGQ_INSTALL_DIR)


LDFLAGS += \
 -rdynamic \
 -L../common -lmmcs_common \
 -L../../libmmcs_client -lmmcs_client \
 -L../../master/lib -lbgmasterclient \
 -L../../security -lsecurity \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib \
 -L$(BGQ_INSTALL_DIR)/bgq_util/lib -lbgq_util \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/bgq_util/lib \
 -L$(BGQ_INSTALL_DIR)/control/lib -lmcserver -lbgqconfig \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/control/lib \
 -L$(BGQ_INSTALL_DIR)/db/lib -lbgdb \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib \
 -L$(BGQ_INSTALL_DIR)/ras/lib -lras \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/ras/lib \
 -L$(BGQ_INSTALL_DIR)/xml/lib -lXML \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/xml/lib \
 -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib \
 -lcxxsockets \
 -lboost_date_time-mt \
 -llog4cxx \
 -lboost_serialization-mt \
 -lboost_system-mt \
 -lboost_thread-mt \
 -lboost_filesystem-mt


command_parts := \
 AddBootOption \
 Allocate \
 AllocateBlock \
 BlockStatus \
 BootBlock \
 CompleteIoService \
 CopyBlock \
 Connect \
 CreateNodeConfig \
 Delete \
 DeselectBlock \
 DiagWait \
 Disconnect \
 DumpBlock \
 DumpMachine \
 DumpPersonalities \
 Free \
 FreeAll \
 FreeBlock \
 GenBlock \
 GenBlocks \
 GenFullBlock \
 GenIoBlock \
 GenMidplaneBlock \
 GenSmallBlock \
 GetBlockInfo \
 GetBlockSize \
 GrantBlockAuthority \
 KillMidplaneJobs \
 List \
 ListBlockAuthority \
 ListBlocks \
 ListHwPolling \
 ListIo \
 ListIoLinks \
 ListMidplanes \
 ListSelectedBlock \
 ListUsers \
 Locate \
 LogLevel \
 McserverClients \
 McserverStatus \
 RebootNodes \
 Redirect \
 RedirectBlock \
 RefreshConfig \
 RevokeBlockAuthority \
 SelectBlock \
 SetBlockInfo \
 SetBootOptions \
 ShowBarrier \
 Sql \
 StartHwPolling \
 Status \
 StopHwPolling \
 Sysrq \
 Version \
 WaitBoot \
 Wc \
 WriteCon


env_parts := \
 BulkPower \
 Coolant \
 HealthCheck \
 IoDrawer \
 Location \
 LocationList \
 McServerConnection \
 Monitor \
 NodeBoard \
 Optical \
 PerfData \
 Polling \
 ServiceCard \
 Token \
 utility \


master_parts := \
 AliasWaiter \
 Monitor \
 SubnetWaiter \
 WaiterList \


parts := \
 areBlocksActive \
 mmcs_server \
 BCNodeInfo \
 BlockControllerBase \
 BlockControllerTarget \
 BlockDBPollingGovernor \
 BlockHelper \
 BootFailure \
 CheckBootComplete \
 CNBlockController \
 CommandProcessor \
 ConsoleConnectionThread \
 ConsoleListener \
 DatabaseBlockCommandThread \
 DatabaseMonitorThread \
 DBBlockController \
 DBConsoleController \
 DBPollingGovernor \
 DBStatics \
 DefaultListener \
 ExcludeList \
 HardwareBlockList \
 IOBlockController \
 mcServerInit \
 MMCSCommandProcessor_db \
 Options \
 PerformanceCounters \
 RackBitset \
 ReconnectBlocks \
 RunJobConnection \
 SecureConnect \
 MailboxMonitor \
 neighbor/add \
 neighbor/ComputeNode \
 neighbor/Direction \
 neighbor/Info \
 neighbor/IoLink \
 neighbor/IoNode \
 neighbor/LinkChip \
 ${command_parts:%=command/%} \
 ${env_parts:%=env/%} \
 ${master_parts:%=master/%}

objs := ${parts:%=%.o}


mmcs_server_installed := $(BGQ_INSTALL_DIR)/hlcs/sbin/mmcs_server
mmcs_server_sbin_link := $(BGQ_INSTALL_DIR)/sbin/mmcs_server

install_targets := \
 $(mmcs_server_installed) \
 $(mmcs_server_sbin_link)


clean_cmds := \
 $(RM) *.o *.d \
  command/*.o command/*.d \
  env/*.o env/*.d \
  master/*.o master/*.d \
  neighbor/*.o neighbor/*.d \
  mmcs_server


default: all

all: mmcs_server


MMCSCommandProcessor_db.o: ../MMCSCommandProcessor.cc
	$(CXX) -o $@ $(CXXFLAGS) -DMMCS_SERVER -c $<

CommandProcessor.o: CommandProcessor.cc
	$(CXX) -o $@ $(CXXFLAGS) -DMMCS_SERVER -c $<


mmcs_server: $(objs)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


$(mmcs_server_installed): mmcs_server
	$(CP) $< $@

$(mmcs_server_sbin_link):
	$(RM) $@
	$(LN) -s ../hlcs/sbin/mmcs_server $@


install: $(install_targets)

clean:
	$(clean_cmds)

distclean:
	$(clean_cmds)


uninstall:


.PHONY: default all install clean distclean uninstall

-include *.d
-include command/*.d
-include env/*.d
-include master/*.d
-include neighbor/*.d
