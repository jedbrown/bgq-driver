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
LDFLAGS += -Wl,-rpath,$(BGQ_BUILD_DIR)/utility/src
LDFLAGS += -lboost_system-mt
LDFLAGS += -lboost_thread-mt
LDFLAGS += -lboost_unit_test_framework-mt


TESTERS   = $(patsubst %.cc,%,$(wildcard *.cc))


all: $(TESTERS:%=out_%)


out_%: test_%
	@strip $<
	BG_PROPERTIES_FILE=$(etc_install_dir)/bg.properties.tpl ./$< 1> $(patsubst test_%, out_%, $<)

test_ctor: ctor.o Makefile
	$(CXX) $(LDFLAGS) -o $@ $(filter-out Makefile, $^)

test_diags: diags.o Makefile
	$(CXX) $(LDFLAGS) -o $@ $(filter-out Makefile, $^)

install: all 


clean:
	$(RM) test_* out_*
	$(RM) *.o
	$(RM) *.d


distclean: clean

.DELETE_ON_ERROR:

-include *.d
