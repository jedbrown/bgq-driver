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
include ../../../Make.rules
include ../../Make.utility.rules

CXXFLAGS += -Wall
CXXFLAGS += -MD
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -pthread
CXXFLAGS += -DBOOST_TEST_DYN_LINK
CXXFLAGS += -DBOOST_TEST_MAIN


LDFLAGS += -L $(BGQ_BUILD_DIR)/utility/src -lbgutility
LDFLAGS += -ldl
LDFLAGS += -Wl,-rpath,$(BGQ_BUILD_DIR)/utility/src
LDFLAGS += -lboost_unit_test_framework-mt

TESTERS   = plugin

lib_base       = sample_plugin
lib_major_vers = 1
lib_minor_vers = 0
lib_minor_rev  = 0
lib_short      = lib$(lib_base).so
lib_vers       = $(lib_short).$(lib_major_vers)
lib_full       = $(lib_vers).$(lib_minor_vers).$(lib_minor_rev)


all: $(TESTERS:%=out_%)


out_%: test_%
	@strip $<
	BG_PROPERTIES_FILE=$(etc_install_dir)/bg.properties.tpl ./$< 1> $(patsubst test_%, out_%, $<)

test_plugin: plugin.o Makefile | $(lib_short)
	$(CXX) $(LDFLAGS) -o $@ $(filter-out Makefile, $^)


SamplePlugin.o: CXXFLAGS += -fpic

TEST_PLUGIN_SRC = SamplePlugin.cc

$(lib_full): LDFLAGS  += -shared
$(lib_full): LDFLAGS  += -Wl,-soname,$(lib_vers)

$(lib_full): $(TEST_PLUGIN_SRC:%.cc=%.o)
	@echo "link $@"
	$(SILENT)$(CXX) $(LDFLAGS) $^ -o $@

$(lib_vers): $(lib_full)
	ln -sf $< $@

$(lib_short): $(lib_vers)
	ln -sf $< $@


install: all 


clean:
	$(RM) test_* out_*
	$(RM) *.o
	$(RM) *.d
	$(RM) $(lib_short) $(lib_vers) $(lib_full)


distclean: clean

.DELETE_ON_ERROR:

-include *.d
