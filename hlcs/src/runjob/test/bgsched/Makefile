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

include ../../Make.rules
include ../../../../Make.hlcs.rules
include ../../../../../Make.rules

SRC          = $(wildcard *.cc)

BGSCHED_OBJS  = $(SRC:%.cc=%.o)
DEPENDENCY_FILES = $(SRC:%.cc=%.d)

BGSCHED = bgsched_runjob

LDFLAGS  += -L ../../client -lrunjob_client
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib
LDFLAGS  += -lboost_thread-mt

install: $(BGSCHED)

all: $(BGSCHED)

$(BGSCHED): $(BGSCHED_OBJS) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

clean:
	$(RM) $(BGSCHED)
	$(RM) $(BGSCHED_OBJS)
	$(RM) $(DEPENDENCY_FILES)

distclean: clean

-include $(DEPENDENCY_FILES)
