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
# (C) Copyright IBM Corp.  2010, 2011                              
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

DIRS = docs
DIR_TARGETS_ALL		= $(addsuffix .all,$(DIRS))
DIR_TARGETS_INSTALL	= $(addsuffix .install,$(DIRS))
DIR_TARGETS_CLEAN	= $(addsuffix .clean,$(DIRS))
DIR_TARGETS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))
DIR_TARGETS_UNINSTALL	= $(addsuffix .uninstall,$(DIRS))


DIR_TARGETS  = $(DIR_TARGETS_ALL)
DIR_TARGETS += $(DIR_TARGETS_INSTALL)
DIR_TARGETS += $(DIR_TARGETS_CLEAN)
DIR_TARGETS += $(DIR_TARGETS_DISTCLEAN)
DIR_TARGETS += $(DIR_TARGETS_UNINSTALL)

CXXFLAGS += -Wall
CXXFLAGS += -iquote .
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/utility/include
CXXFLAGS += -I../master
CXXFLAGS += -MD
CXXFLAGS += -MP
CXXFLAGS += -O2

PYTHON_INC=$(shell ls -d /usr/include/python[23].[0-9])
EXT_LIBBOOST_PYTHON=$(shell ls $(BGQ_INSTALL_DIR)/extlib/lib/libboost_python.so 2>/dev/null)


ifneq ($(EXT_LIBBOOST_PYTHON),)
  PYTHON_LDFLAGS  += -L $(BGQ_INSTALL_DIR)/extlib/lib
  PYTHON_LDFLAGS  += -lboost_python
  PYTHON_LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/extlib/lib
else
  PYTHON_LDFLAGS  += -lboost_python
endif

# linker flags for all binaries
LDFLAGS    += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility
LDFLAGS    += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS    += -L $(BGQ_INSTALL_DIR)/bgq_util/lib -lbgq_util
LDFLAGS    += -Wl,-rpath,$(BGQ_INSTALL_DIR)/bgq_util/lib
LDFLAGS    += -L $(BGQ_INSTALL_DIR)/control/lib -lmcserver
LDFLAGS    += -Wl,-rpath $(BGQ_INSTALL_DIR)/control/lib
LDFLAGS    += -L $(BGQ_INSTALL_DIR)/ras/lib -lras
LDFLAGS    += -Wl,-rpath,$(BGQ_INSTALL_DIR)/ras/lib
LDFLAGS    += -L $(BGQ_INSTALL_DIR)/xml/lib -lXML
LDFLAGS    += -Wl,-rpath,$(BGQ_INSTALL_DIR)/xml/lib
LDFLAGS    += -L $(BGQ_INSTALL_DIR)/control/lib -lbgqconfig
LDFLAGS    += -Wl,-rpath,$(BGQ_INSTALL_DIR)/control/lib
LDFLAGS    += -pthread
LDFLAGS    += -ledit
LDFLAGS    += -lboost_date_time-mt
LDFLAGS    += -lboost_system-mt
LDFLAGS    += -lboost_filesystem-mt
LDFLAGS    += -lboost_program_options-mt
LDFLAGS    += -lboost_regex-mt

# mmcs_lite
LITE_SRC        =
LITE_SRC       += IOBlockController.cc
LITE_SRC       += CNBlockController.cc
LITE_SRC       += BlockHelper.cc
LITE_SRC       += BlockControllerBase.cc
LITE_SRC       += BlockControllerNodeInfo.cc
LITE_SRC       += BlockControllerTarget.cc
LITE_SRC       += ConsoleController.cc
LITE_SRC       += MMCSCommandReply.cc
LITE_SRC       += MMCSConsolePort.cc
LITE_SRC       += MMCSProperties.cc
LITE_SRC       += MMCSThread.cc
LITE_SRC       += MailboxMonitor.cc
LITE_SRC       += SecureMailboxMonitor.cc
LITE_SRC       += LiteControlEventListener.cc
LITE_SRC       += MMCSCommand_lite.cc
LITE_SRC       += fakeDb.cc
LITE_SRC       += MMCSExternalCommand.cc
LITE_SRC       += HardwareBlockList.cc
LITE_SRC       += RackBitset.cc
LITE_SRC       += MMCSSubnet.cc
LITE_SRC       += lite/ClassRoute.cc
LITE_SRC       += lite/Database.cc
LITE_SRC       += lite/Job.cc
LITE_SRC       += lite/MMCSCommand_killjob.cc
LITE_SRC       += lite/MMCSCommand_waitjob.cc
LITE_SRC       += lite/MMCSCommand_runjob.cc
LITE_SRC       += lite/Np.cc
LITE_SRC       += lite/Options.cc
LITE_SRC       += lite/SecurityKey.cc
LITE_OBJ        = $(LITE_SRC:%.cc=%.o)

# commands
LITE_CMD_SRC    =
LITE_CMD_SRC   += MMCSCommand_block.cc
LITE_CMD_SRC   += MMCSCommand_block_status.cc
LITE_CMD_SRC   += MMCSCommand_dump.cc
LITE_CMD_SRC   += MMCSCommand_help.cc
LITE_CMD_SRC   += MMCSCommand_ionodes.cc
LITE_CMD_SRC   += MMCSCommand_locate.cc
LITE_CMD_SRC   += MMCSCommand_log_level.cc
LITE_CMD_SRC   += MMCSCommand_mcserver.cc
LITE_CMD_SRC   += MMCSCommand_replyformat.cc
LITE_CMD_SRC   += MMCSCommand_shell.cc
LITE_CMD_SRC   += MMCSCommand_version.cc
LITE_CMD_SRC   += MMCSCommand_diags.cc
LITE_CMD_SRC   += MMCSCommand_barrier.cc
LITE_CMD_OBJ    = $(LITE_CMD_SRC:%.cc=%.o)

LITE_LDFLAGS    = $(LDFLAGS)
LITE_LDFLAGS   += -lsqlite3

TARGETS        += mmcs_lite
INSTALL_LIST   += $(call INSTALL_SBIN_TARGET, mmcs_lite)
INSTALL_LIST   += $(call INSTALL_LIB_TARGET,$(console_lib_full) $(console_lib_vers) $(console_lib_dev))

console_lib_base:=consolecmds
console_lib_dev:=lib$(console_lib_base).so
console_lib_vers:=$(console_lib_dev).1
console_lib_full:=$(console_lib_vers).0.0

# check if we have configured it for DB2
CONSOLE_SRC      += BlockHelper.cc
CONSOLE_SRC      += BlockControllerBase.cc
CONSOLE_SRC      += BlockControllerNodeInfo.cc
CONSOLE_SRC      += BlockControllerTarget.cc
CONSOLE_SRC      += ConsoleController.cc
CONSOLE_SRC      += CNBlockController.cc
CONSOLE_SRC      += IOBlockController.cc
CONSOLE_SRC      += MMCSProperties.cc
CONSOLE_SRC      += MMCSConsolePort.cc
CONSOLE_SRC      += MMCSCommandReply.cc
CONSOLE_SRC      += MMCSThread.cc
CONSOLE_SRC      += MMCSExternalCommand.cc
CONSOLE_SRC      += MMCSSubnet.cc

# Need to share these with the server because
# other shared components, Block Controllers,
# depend on them.
CONSOLE_SRC      += HardwareBlockList.cc
CONSOLE_SRC      += MailboxMonitor.cc
CONSOLE_SRC      += SecureMailboxMonitor.cc
CONSOLE_SRC      += RackBitset.cc

SERVER_SRC       = $(CONSOLE_SRC)
SERVER_SRC      += DBBlockController.cc
SERVER_SRC      += DBConsoleController.cc
SERVER_SRC      += DefaultControlEventListener.cc
SERVER_SRC      += MMCSConsoleMonitor.cc
SERVER_SRC      += MMCSDBMonitor.cc
SERVER_SRC      += MMCSEnvMonitor.cc
SERVER_SRC      += MMCSExcludeList.cc
SERVER_SRC      += MMCSServerParms.cc
SERVER_SRC      += MMCSSocketList.cc
SERVER_SRC      += mcServerInit.cc
SERVER_SRC      += ReconnectBlocks.cc
SERVER_SRC      += RunJobConnection.cc
SERVER_SRC      += CheckBootCompleteThread.cc
SERVER_SRC      += PerformanceCounters.cc
SERVER_SRC      += DBStatics.cc
SERVER_SRC      += NeighborInfo.cc
SERVER_SRC 	+= master/AliasWaiter.cc
SERVER_SRC 	+= master/Monitor.cc
SERVER_SRC 	+= master/SubnetWaiter.cc
SERVER_SRC 	+= master/WaiterList.cc

SERVER_OBJ       = $(SERVER_SRC:%.cc=%.o)

CONSOLE_OBJ       = $(CONSOLE_SRC:%.cc=%.o)

SERVER_LDFLAGS     = $(LDFLAGS)
SERVER_LDFLAGS    += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
SERVER_LDFLAGS    += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib
SERVER_LDFLAGS    += -L $(BGQ_INSTALL_DIR)/hlcs/lib -lbgmasterclient
SERVER_LDFLAGS 	  += -Wl,-rpath $(BGQ_INSTALL_DIR)/hlcs/lib
SERVER_LDFLAGS    += -L$(BGQ_INSTALL_DIR)/hlcs/lib -lsecurity -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib

CONSOLE_LDFLAGS    = $(LDFLAGS)
CONSOLE_LDFLAGS   += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib
CONSOLE_LDFLAGS   += -Wl,-rpath $(BGQ_INSTALL_DIR)/hlcs/lib
CONSOLE_LDFLAGS   += -L$(BGQ_INSTALL_DIR)/hlcs/lib -lsecurity -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib

CONSOLE_CMD_SRC    += MMCSConsoleCommand_xml.cc
CONSOLE_CMD_SRC    += MMCSConsoleCommand_redirect.cc
CONSOLE_CMD_SRC    += MMCSConsoleCommand_refresh_config.cc
CONSOLE_CMD_SRC    += MMCSConsoleCommand_replyformat.cc
CONSOLE_CMD_SRC    += MMCSConsoleCommand_username.cc
CONSOLE_CMD_SRC    += MMCSCommand_mmcs_server_cmd.cc
CONSOLE_CMD_SRC    += MMCSCommand_mmcs_server_connect.cc
CONSOLE_CMD_SRC    += MMCSCommand_shell.cc
CONSOLE_CMD_SRC    += MMCSCommand_help.cc

SERVER_CMD_SRC    += MMCSCommand_allocate.cc
SERVER_CMD_SRC    += MMCSCommand_authority.cc
SERVER_CMD_SRC    += MMCSCommand_envs.cc
SERVER_CMD_SRC    += MMCSCommand_free.cc
SERVER_CMD_SRC    += MMCSCommand_list.cc
SERVER_CMD_SRC    += MMCSCommand_refresh_config.cc
SERVER_CMD_SRC    += MMCSCommand_select_block.cc
SERVER_CMD_SRC    += MMCSCommand_username.cc
SERVER_CMD_SRC    += MMCSCommand_reboot.cc
#SERVER_CMD_SRC    += MMCSCommand_shutdown_hardware.cc
SERVER_CMD_SRC    += MMCSCommand_db.cc
SERVER_CMD_SRC    += MMCSCommand_redirect.cc
SERVER_CMD_SRC    += MMCSCommand_status.cc
SERVER_CMD_SRC    += MMCSServerCommand_block.cc
SERVER_CMD_SRC    += MMCSServerCommand_xml.cc
SERVER_CMD_SRC    += MMCSServerCommand_disconnect.cc


ifeq ($(MMCS_PROHIBITED), yes)
SERVER_CMD_SRC    += MMCSCommand_inject_ras.cc
LITE_CMD_SRC      += MMCSCommand_inject_ras.cc
CXXFLAGS          += -D MMCS_PROHIBITED
endif
SERVER_CMD_OBJ     = $(SERVER_CMD_SRC:%.cc=%.o)
CONSOLE_CMD_OBJ    = $(CONSOLE_CMD_SRC:%.cc=%.o)

CXXFLAGS      += -DWITH_DB

TARGETS       += mmcs_server
INSTALL_LIST  += $(call INSTALL_SBIN_TARGET, mmcs_server)
TARGETS	      += bg_console
INSTALL_LIST  += $(call INSTALL_BIN_TARGET, bg_console)

TARGETS	      += bg_consolepy
INSTALL_LIST  += $(call INSTALL_BIN_TARGET, bg_consolepy)

ifeq ($(SECURITY), yes)
CXXFLAGS += -DWITH_SECURITY
endif

$(LITE_OBJ): CXXFLAGS += -iquote $(BGQ_INSTALL_DIR)/spi/include/kernel/cnk -iquote $(BGQ_INSTALL_DIR)/spi/include/mu/default

all: default $(DIR_TARGETS_ALL)

default: $(TARGETS) $(INSTALL_LIST)

$(DIR_TARGETS)::
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))

$(console_lib_dev): $(console_lib_vers)
	ln -sf $< $@


$(console_lib_vers): $(console_lib_full)
	ln -sf $< $@

$(CONSOLE_CMD_OBJ): CXXFLAGS += -fPIC

$(console_lib_full): LDFLAGS += -Wl,-soname,$(console_lib_vers)

$(console_lib_full): $(CONSOLE_CMD_OBJ)
	$(CXX) $(LDFLAGS) -shared -o $@ $^

MMCSCommandProcessor_db.o: MMCSCommandProcessor.cc
	$(CXX) -o $@ $(CXXFLAGS) -DWITH_DB -DMMCS_SERVER -c $<

MMCSCommandProcessor_no_db.o: MMCSCommandProcessor.cc
	$(CXX) -o $@ $(CXXFLAGS) -UWITH_DB -UWITH_SECURITY -DMMCS_SERVER -c $<

MMCSCommandProcessor_console.o: MMCSCommandProcessor.cc
	$(CXX) -o $@ $(CXXFLAGS) -DBG_CONSOLE -c $<

mmcs_lite: mmcs_lite.o  MMCSCommandProcessor_no_db.o $(LITE_CMD_OBJ) $(LITE_OBJ) Makefile
	$(CXX) -o $@ $(LITE_LDFLAGS) $(filter-out Makefile, $^)

mmcs_server: mmcs_server.o MMCSCommandProcessor_db.o $(LITE_CMD_OBJ) $(SERVER_CMD_OBJ) $(SERVER_OBJ) Makefile
	$(CXX) -o $@ $(SERVER_LDFLAGS) $(filter-out Makefile, $^)

bg_console: bg_console.o MMCSCommandProcessor_console.o $(console_lib_full) $(CONSOLE_OBJ) ConsoleOptions.o MMCSConnectionMonitor.o Makefile
	$(CXX) -o $@ $(CONSOLE_LDFLAGS) $(filter-out Makefile, $^)

bg_consolepy.o: CXXFLAGS += -I$(PYTHON_INC) -fPIC
bg_consolepy: bg_consolepy.o MMCSCommandProcessor_console.o $(console_lib_full) $(CONSOLE_OBJ) ConsoleOptions.o MMCSConnectionMonitor.o Makefile
	$(CXX) -o $@ $(CONSOLE_LDFLAGS) $(PYTHON_LDFLAGS) $(filter-out Makefile, $^)

test_bitset: test_bitset.o RackBitset.o
	$(CXX) -o $@ $^

test_bitset_io: test_bitset_io.o RackBitset.o
	$(CXX) -o $@ $^

bg_sbin_links := mmcs_lite mmcs_server

bg_bin_links := bg_console bg_consolepy


ras_scan: $(wildcard *.cc) $(wildcard *.h) $(wildcard */*.h) $(wildcard */*.cc)
	$(BGQ_BUILD_DIR)/scripts/rasScan.py '$(BGQ_BUILD_DIR)/hlcs/src/mmcs' '$(BGQ_INSTALL_DIR)/hlcs/ras/metadata' 'mmcs_ras.xml' '$(BGQ_INSTALL_DIR)/ras/include/ras.xml'
	@touch $@


install: $(INSTALL_LIST) $(bg_links_targets) ras_scan $(DIR_TARGETS_INSTALL)


clean:  $(DIR_TARGETS_CLEAN)
	$(RM) $(TARGETS) $(LIBRARY)
	$(RM) *.o 
	$(RM) *.d
	$(RM) master/*.o 
	$(RM) master/*.d
	$(RM) lite/*.d
	$(RM) lite/*.o
	$(RM) ras_scan
	$(RM) $(console_lib_vers)
	$(RM) $(console_lib_full)
	$(RM) $(console_lib_dev)
	$(RM) $(BGQ_LIB_INSTALL_DIR)/$(console_lib_vers)
	$(RM) $(BGQ_LIB_INSTALL_DIR)/$(console_lib_full)
	$(RM) $(BGQ_LIB_INSTALL_DIR)/$(console_lib_dev)

distclean: clean
	$(RM) $(TARGETS)

uninstall:
	$(RM) $(INSTALL_LIST)

.PHONY: install clean

-include *.d
-include lite/*.d
-include master/*.d

