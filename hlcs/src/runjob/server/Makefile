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

SERVER_SRC    = $(filter-out commands/%.cc,$(wildcard */*/*.cc) $(wildcard */*.cc) $(wildcard *.cc))

SERVER_OBJS  = $(SERVER_SRC:%.cc=%.o)
DEPENDENCY_FILES = $(SERVER_SRC:%.cc=%.d)

SERVER = runjob_server
bg_sbin_links := $(SERVER)

CXXFLAGS += -iquote $(BGQ_INSTALL_DIR)/spi/include/kernel/cnk -iquote $(BGQ_INSTALL_DIR)/spi/include/mu/default
CXXFLAGS += -D BGQ_INSTALL_DIR=\"$(BGQ_INSTALL_DIR)\"

LDFLAGS  += -L ../common -lrunjob_common
LDFLAGS  += -L ../../security -lsecurity
LDFLAGS  += -L ../../bgsched -lbgsched
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/bgq_util/lib -lbgq_util
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/bgq_util/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/control/lib -lbgqconfig
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/control/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/ras/lib -lras
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/ras/lib
LDFLAGS  += -lboost_filesystem-mt
LDFLAGS  += -lboost_program_options-mt
LDFLAGS  += -lboost_serialization-mt
LDFLAGS  += -lboost_system-mt
LDFLAGS  += -lboost_thread-mt
LDFLAGS  += -lboost_date_time-mt
LDFLAGS  += -rdynamic
LDFLAGS  += -llog4cxx 


INSTALL_LIST          += $(call INSTALL_SBIN_TARGET, $(SERVER))


all: $(SERVER) $(TARGETS_ALL)


clean: $(TARGETS_CLEAN)
	$(RM) $(SERVER)
	$(RM) $(SERVER_OBJS)
	$(RM) $(DEPENDENCY_FILES)
	$(RM) ras_scan


distclean: clean $(TARGETS_DISTCLEAN)



install: $(INSTALL_LIST) $(TARGETS_INSTALL) ras_scan $(bg_links_targets)


uninstall: $(TARGETS_UNINSTALL)
	$(RM) $(INSTALL_LIST)


ras_scan: $(SERVER_SRC)
	$(BGQ_BUILD_DIR)/scripts/rasScan.py $(BGQ_BUILD_DIR)/hlcs/src/runjob/server $(BGQ_INSTALL_DIR)/hlcs/ras/metadata runjob_ras.xml $(BGQ_INSTALL_DIR)/ras/include/ras.xml
	@touch $@

$(SERVER): $(SERVER_SRC:%.cc=%.o) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)


$(TARGETS):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))


-include $(DEPENDENCY_FILES)

