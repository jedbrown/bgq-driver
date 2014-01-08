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
include ../../../../../Make.rules
include ../../../../Make.hlcs.rules
include ../../Make.rules

DIRS += $(subst /Makefile,,$(wildcard */Makefile))

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


.PHONY: install all default clean distclean test $(TARGETS) $(DIRS)

CXXFLAGS += -Wall
CXXFLAGS += -Wno-old-style-cast
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/utility/include
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/spi/include/kernel/cnk -isystem $(BGQ_INSTALL_DIR)/spi/include/mu/default
CXXFLAGS += -pthread
CXXFLAGS += -DBOOST_TEST_DYN_LINK
CXXFLAGS += -DBOOST_TEST_MAIN


LDFLAGS += ../../server/Options.o
LDFLAGS += ../../server/Reconnect.o
LDFLAGS += -L ../../common -lrunjob_common
LDFLAGS += -Wl,-rpath,$(BGQ_BUILD_DIR)/hlcs/src/runjob/common
LDFLAGS += -L $(BGQ_INSTALL_DIR)/ras/lib -lras
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/ras/lib
LDFLAGS += -lboost_filesystem-mt
LDFLAGS += -lboost_program_options-mt
LDFLAGS += -lboost_system-mt
LDFLAGS += -lboost_unit_test_framework-mt
LDFLAGS += -llog4cxx


TESTERS   = $(patsubst %.cc,%,$(wildcard *.cc))


all:            $(TARGETS_ALL) $(TESTERS:%=out_%)
install:        $(TARGETS_INSTALL) $(TESTERS:%=out_%)
distclean:      clean $(TARGETS_DISTCLEAN)
uninstall:      $(TARGETS_UNINSTALL)
clean:          $(TARGETS_CLEAN)
	$(RM) test_* out_*
	$(RM) *.o *.d


$(TARGETS):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))


out_%: test_%
	@strip $<
	@echo "execute $@"
	$(SILENT)BG_PROPERTIES_FILE=$(BGQ_INSTALL_DIR)/utility/etc/bg.properties.tpl ./$< 1> $(patsubst test_%, out_%, $<)


test_io_connection: io_connection.o ../../server/cios/ConnectionInterval.o
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $^ -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib $(LDFLAGS)

test_performance_counter_interval: performance_counter_interval.o ../../server/performance/Interval.o
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $^ $(LDFLAGS)

test_mapping_file: mapping_file.o ../../server/job/ValidateMappingFile.o
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $^ $(LDFLAGS)

test_copy_mapping_file: copy_mapping_file.o ../../server/job/CopyMappingFile.o
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $^ $(LDFLAGS)

test_max_user_processes: max_user_processes.o
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $^ $(LDFLAGS)

.DELETE_ON_ERROR:

-include *.d
