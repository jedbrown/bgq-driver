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

include ../../Make.db.rules
include ../../../Make.rules

SRC          = $(wildcard *.cc)

REAPER_OBJS  = $(SRC:%.cc=%.o)
DEPENDENCY_FILES = $(SRC:%.cc=%.d)

REAPER = reaper

CXXFLAGS += -isystem $(BGQ_BUILD_DIR)
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib

install: all

all: $(REAPER)

$(REAPER): $(REAPER_OBJS) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

clean:
	$(RM) $(REAPER)
	$(RM) $(REAPER_OBJS)
	$(RM) $(DEPENDENCY_FILES)

distclean: clean

-include $(DEPENDENCY_FILES)
