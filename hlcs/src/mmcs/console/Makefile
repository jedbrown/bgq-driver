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
 -pthread \
 -MD -MP \
 -Wall \
 -iquote .. \
 -iquote ../.. \
 -iquote ../../../include \
 -isystem $(BGQ_INSTALL_DIR)


LDFLAGS += \
 -rdynamic \
 -L../common -lmmcs_common \
 -L../../libmmcs_client -lmmcs_client \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib \
 -L$(BGQ_INSTALL_DIR)/extlib/lib -llog4cxx \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/extlib/lib \
 -L$(BGQ_INSTALL_DIR)/utility/lib  -lbgutility -lcxxsockets \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib \
 -lboost_filesystem-mt \
 -lboost_system-mt \
 -lboost_program_options-mt


command_parts := \
 Comment \
 DumpBlock \
 DumpMachine \
 MmcsServerCmd \
 MmcsServerConnect \
 Quit \
 Redirect \
 RedirectBlock \
 RedirectInput \
 RefreshConfig \
 Sleep \
 SubShell \
 Username


parts := \
 CommandProcessor \
 ConnectionMonitor \
 ExternalCommand \
 MMCSCommandProcessor_console \
 Options \
 RedirectMonitorThread \
 ${command_parts:%=command/%} \

objs := ${parts:%=%.o}

exes := \
 bg_console \
 bg_consolepy


installed_bg_console = $(BGQ_INSTALL_DIR)/hlcs/bin/bg_console
installed_bg_consolepy = $(BGQ_INSTALL_DIR)/hlcs/bin/bg_consolepy
installed_symlink = $(BGQ_INSTALL_DIR)/bin/bg_console

install_targets = \
 $(installed_bg_console) \
 $(installed_bg_consolepy) \
 $(installed_symlink)


clean_cmds = $(RM) *.o *.d command/*.o command/*.d $(exes)


PYTHON_INC=$(shell ls -d /usr/include/python[23].[0-9])
EXT_LIBBOOST_PYTHON=$(shell ls $(BGQ_INSTALL_DIR)/extlib/lib/libboost_python.so 2>/dev/null)


PYTHON_LDFLAGS    += -lboost_regex-mt
ifneq ($(EXT_LIBBOOST_PYTHON),)
  PYTHON_LDFLAGS  += -L $(BGQ_INSTALL_DIR)/extlib/lib
  PYTHON_LDFLAGS  += -lboost_python
  PYTHON_LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/extlib/lib
else
  PYTHON_LDFLAGS  += -lboost_python
endif


default: all

all: $(exes)


MMCSCommandProcessor_console.o: ../MMCSCommandProcessor.cc
	$(CXX) -o $@ $(CXXFLAGS) -UWITH_DB -DBG_CONSOLE -c $<

CommandProcessor.o: CommandProcessor.cc
	$(CXX) -o $@ $(CXXFLAGS) -UWITH_DB -DBG_CONSOLE -c $<


bg_console: bg_console.o $(objs)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


bg_consolepy.o: CXXFLAGS += -I$(PYTHON_INC) -fPIC

bg_consolepy: bg_consolepy.o $(objs)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(PYTHON_LDFLAGS)


$(installed_bg_console): bg_console

$(installed_bg_consolepy): bg_consolepy

$(installed_symlink):
	$(RM) $@
	$(LN) -s ../hlcs/bin/bg_console $@


install: $(install_targets)


clean:
	$(clean_cmds)


distclean:
	$(clean_cmds)


uninstall:


.PHONY: default all install clean distclean uninstall

-include *.d
-include command/*.d
