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
 -MD -MP \
 -pthread \
 -iquote .. \
 -iquote ../.. \
 -iquote ../../../include \
 -isystem $(BGQ_INSTALL_DIR) \
 -iquote $(BGQ_INSTALL_DIR)/spi/include/kernel/cnk \
 -iquote $(BGQ_INSTALL_DIR)/spi/include/mu/default


LDFLAGS += \
 -rdynamic \
 -L../../libmmcs_client -lmmcs_client \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib \
 -L$(BGQ_INSTALL_DIR)/bgq_util/lib -lbgq_util \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/bgq_util/lib \
 -L$(BGQ_INSTALL_DIR)/control/lib -lmcserver -lbgqconfig \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/control/lib \
 -L$(BGQ_INSTALL_DIR)/extlib/lib -llog4cxx \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/extlib/lib \
 -L$(BGQ_INSTALL_DIR)/ras/lib -lras \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/ras/lib \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility -lcxxsockets \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib \
 -L$(BGQ_INSTALL_DIR)/xml/lib -lXML \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/xml/lib \
 -lboost_filesystem-mt \
 -ledit \
 -lsqlite3 \
 -lboost_system-mt \
 -lboost_thread-mt \
 -lboost_program_options-mt


clean_cmds := \
 $(RM) *.o *.d command/*.o command/*.d mmcs_lite


common_parts := \
 ConsoleController \
 Properties \
 Subnet \
 Thread \
 command/Help


command_parts := \
 CreateBlock \
 DumpBlock \
 DumpMachine \
 Quit

console_command_parts := \
 RedirectInput \
 Sleep \
 SubShell

server_command_parts := \
 BlockStatus \
 Connect \
 DiagWait \
 Disconnect \
 DumpPersonalities \
 GetBlockSize \
 Locate \
 LogLevel \
 McserverClients \
 McserverStatus \
 ShowBarrier \
 Sysrq \
 Version \
 Wc \
 WriteCon

server_parts := \
 BCNodeInfo \
 BlockControllerBase \
 BlockControllerTarget \
 BlockHelper \
 CNBlockController \
 HardwareBlockList \
 IOBlockController \
 RackBitset \
 SecureConnect \
 MailboxMonitor \
 ${server_command_parts:%=command/%}


parts := \
 mmcs_lite \
 BlockController \
 ClassRoute \
 ConsoleController \
 Database \
 fakeDb \
 Job \
 LiteControlEventListener \
 MMCSCommand_killjob \
 MMCSCommand_lite \
 MMCSCommand_runjob \
 MMCSCommand_waitjob \
 MMCSCommandProcessor_no_db \
 Np \
 Options \
 SecurityKey \
 ../console/ExternalCommand \
 ${common_parts:%=../common/%} \
 ${command_parts:%=command/%} \
 ${console_command_parts:%=../console/command/%} \
 ${server_parts:%=../server/%}


objs := ${parts:%=%.o}


default: all


all: mmcs_lite


MMCSCommandProcessor_no_db.o: ../MMCSCommandProcessor.cc
	$(CXX) -o $@ $(CXXFLAGS) -UWITH_DB -DMMCS_SERVER -c $<


mmcs_lite: $(objs)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


$(BGQ_INSTALL_DIR)/hlcs/sbin/mmcs_lite: mmcs_lite

install: $(BGQ_INSTALL_DIR)/hlcs/sbin/mmcs_lite


clean:
	$(clean_cmds)

distclean:
	$(clean_cmds)


uninstall:


.PHONY: default all install clean distclean uninstall

-include *.d
-include command/*.d
