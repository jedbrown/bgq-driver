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


exes =\
 realtime_server_log_level \
 realtime_server_status

bg_sbin_links := $(exes)


CXXFLAGS += \
 -iquote ..


LDFLAGS += \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib \
 -llog4cxx \
 -lboost_system-mt \
 -lboost_program_options-mt


install_files = $(call INSTALL_SBIN_TARGET, $(exes)) $(bg_links_targets)


default: all

all: $(exes)


realtime_server_log_level: realtime_server_log_level.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

realtime_server_status: realtime_server_status.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


install: $(install_files)

uninstall:
	$(RM) $(install_files)


clean:
	$(RM) $(exes) *.d *.o


distclean: clean


.PHONY: default all install uninstall clean distclean


-include *.d
