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

RUNJOB_OBJS  = $(SRC:%.cc=%.o)
DEPENDENCY_FILES = $(SRC:%.cc=%.d)

RUNJOB = runjob

LDFLAGS  += -L $(BGQ_INSTALL_DIR)/hlcs/lib -lrunjob_client
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib
LDFLAGS  += -lboost_thread-mt

all: $(RUNJOB) 

install: $(RUNJOB) 

$(RUNJOB): $(RUNJOB_OBJS) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

clean:
	$(RM) $(RUNJOB)
	$(RM) $(RUNJOB_OBJS)
	$(RM) $(DEPENDENCY_FILES)

distclean: clean

-include $(DEPENDENCY_FILES)
