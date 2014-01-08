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

DIRS  = $(subst /Makefile,,$(wildcard */Makefile))

TARGETS_ALL		= $(addsuffix .all,$(DIRS))
TARGETS_INSTALL		= $(addsuffix .install,$(DIRS))
TARGETS_CLEAN		= $(addsuffix .clean,$(DIRS))
TARGETS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))
TARGETS_UNINSTALL	= $(addsuffix .uninstall,$(DIRS))

TARGETS  = $(TARGETS_ALL)
TARGETS += $(TARGETS_INSTALL)
TARGETS += $(TARGETS_CLEAN)
TARGETS += $(TARGETS_DISTCLEAN)
TARGETS += $(TARGETS_UNINSTALL)

SRC             = $(filter-out commands/%.cc,$(wildcard */*.cc) $(wildcard *.cc))
OBJS            = $(SRC:%.cc=%.o)
DEPENDENCIES    = $(SRC:%.cc=%.d)

MUX = runjob_mux
bg_sbin_links := $(MUX)

LDFLAGS  += -L ../common -lrunjob_common
LDFLAGS  += -L ../../bgsched -lbgsched
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib
LDFLAGS  += -lboost_filesystem-mt
LDFLAGS  += -lboost_program_options-mt
LDFLAGS  += -lboost_serialization-mt
LDFLAGS  += -lboost_system-mt 
LDFLAGS  += -lboost_thread-mt 
LDFLAGS  += -lboost_date_time-mt
LDFLAGS  += -llog4cxx
LDFLAGS  += -rdynamic


INSTALL_LIST          += $(call INSTALL_SBIN_TARGET, $(MUX))


all: $(MUX) $(TARGETS_ALL)


clean: $(TARGETS_CLEAN)
	$(RM) $(OBJS)
	$(RM) $(MUX)
	$(RM) $(DEPENDENCIES)


distclean: clean $(TARGETS_DISTCLEAN)


install: $(INSTALL_LIST) $(TARGETS_INSTALL) $(bg_links_targets)


uninstall: $(TARGETS_UNINSTALL)
	$(RM) $(INSTALL_LIST)


$(MUX): $(OBJS) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)


$(TARGETS):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))

-include $(DEPENDENCIES)

