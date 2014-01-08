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

CXXFLAGS += -Wno-old-style-cast
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/utility/include
CXXFLAGS += -pthread
CXXFLAGS += -DBOOST_TEST_DYN_LINK
CXXFLAGS += -DBOOST_TEST_MAIN

LDFLAGS += -lboost_unit_test_framework-mt
LDFLAGS += -llog4cxx
LDFLAGS += -lboost_date_time-mt
LDFLAGS += -lboost_serialization-mt
LDFLAGS += -lboost_system-mt
LDFLAGS +=  -L ../../common/ -lrunjob_common -Wl,-rpath,../../common

TESTERS   = $(patsubst %.cc,%,$(wildcard *.cc))


all: $(TESTERS:%=out_%)


out_%: test_%
	@strip $<
	@echo "execute $@"
	$(SILENT)BG_PROPERTIES_FILE=$(BGQ_INSTALL_DIR)/utility/etc/bg.properties.tpl ./$< 1> $(patsubst test_%, out_%, $<)


test_convert: convert.o Makefile 
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)


test_cwd: cwd.o Makefile 
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)


test_shape: shape.o Makefile ../../common/librunjob_common.so
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile ../../common/librunjob_common.so, $^) $(LDFLAGS)


test_corner: corner.o Makefile ../../common/librunjob_common.so
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile ../../common/librunjob_common.so, $^) $(LDFLAGS)


test_tool_subset: tool_subset.o Makefile ../../common/librunjob_common.so
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile ../../common/librunjob_common.so, $^) $(LDFLAGS)



install: all 


clean:
	$(RM) test_* out_*
	$(RM) *.o
	$(RM) *.d

distclean: clean

.DELETE_ON_ERROR:

-include *.d

