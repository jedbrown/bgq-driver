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
include ../../../Make.rules
include ../../../../../Make.hlcs.rules
include ../../../../../../Make.rules

.PHONY: clean


COMMAND              = $(shell basename `pwd`)
COMMAND_SRC          = main.cc
COMMAND_SRC         += Options.cc

LDFLAGS += -lrunjob_common
LDFLAGS += -lboost_program_options-mt
LDFLAGS += -lboost_date_time-mt
LDFLAGS += -llog4cxx

INSTALL_LIST          += $(call INSTALL_BIN_TARGET, $(COMMAND))
bg_bin_links          := $(COMMAND)


$(COMMAND): $(COMMAND_SRC:%.cc=%.o) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)


all: $(COMMAND) 


install: $(INSTALL_LIST) $(bg_links_targets)


clean:
	$(RM) $(COMMAND)
	$(RM) *.o
	$(RM) *.d


distclean: clean


uninstall:
	$(RM) $(INSTALL_LIST)

-include *.d
