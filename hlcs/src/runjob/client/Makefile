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

include ../Make.rules
include ../../../Make.hlcs.rules
include ../../../../Make.rules

.PHONY: clean tags

CLIENT = runjob

client_libbase       = runjob_client
client_libmajor_vers = 1
client_libminor_vers = 0
client_libminor_rev  = 0
client_libshort      = lib$(client_libbase).so
client_libvers       = $(client_libshort).$(client_libmajor_vers)
client_libfull       = $(client_libvers).$(client_libminor_vers).$(client_libminor_rev)


# runjob sources
CLIENT_SRC    = main.cc SignalHandler.cc
LIBRARY_SRC   = $(filter-out $(CLIENT_SRC), $(wildcard */*.cc) $(wildcard *.cc))

bg_bin_links := $(CLIENT)

# linker flags
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib
LDFLAGS  += -L ../common -lrunjob_common
LDFLAGS  += -lboost_filesystem-mt
LDFLAGS  += -lboost_program_options-mt
LDFLAGS  += -lboost_serialization-mt
LDFLAGS  += -lboost_system-mt
LDFLAGS  += -llog4cxx -lboost_date_time-mt

# compiler flags
$(LIBRARY_SRC:%.cc=%.o): CXXFLAGS += -fpic -fvisibility=default
bgsched/Client.o: CXXFLAGS += -DMAJOR_VERSION=$(client_libmajor_vers)
bgsched/Client.o: CXXFLAGS += -DMINOR_VERSION=$(client_libminor_vers)
bgsched/Client.o: CXXFLAGS += -DMOD_VERSION=$(client_libminor_rev)

INSTALL_LIST          += $(call INSTALL_BIN_TARGET, $(CLIENT))
INSTALL_LIST          += $(call INSTALL_LIB_TARGET, $(client_libshort) $(client_libvers) $(client_libfull))


all: $(CLIENT)


clean:
	$(RM) *.o */*.o
	$(RM) $(CLIENT) 
	$(RM) $(client_libshort) $(client_libvers) $(client_libfull)
	$(RM) *.d */*.d


distclean: clean


install: $(INSTALL_LIST) $(bg_links_targets)


uninstall:
	$(RM) $(INSTALL_LIST)


$(client_libfull): $(LIBRARY_SRC:%.cc=%.o) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) $(filter-out Makefile, $^) -o $@ $(LDFLAGS) -shared -Wl,-soname,$(client_libvers)


$(client_libvers): $(client_libfull)
	ln -sf $< $@


$(client_libshort): $(client_libvers)
	ln -sf $< $@




$(CLIENT): $(CLIENT_SRC:%.cc=%.o) Makefile $(client_libshort)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) -L. -l$(client_libbase) $(LDFLAGS)


#include our dependency rules
-include *.d */*.d

