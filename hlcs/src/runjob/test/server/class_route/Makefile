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

include ../../../../../../Make.rules
include ../../../../../Make.hlcs.rules
include ../../../Make.rules

CXXFLAGS += -Wall
CXXFLAGS += -Wno-old-style-cast
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/utility/include
CXXFLAGS += -iquote $(BGQ_INSTALL_DIR)/spi/include/kernel/cnk
CXXFLAGS += -iquote $(BGQ_INSTALL_DIR)/spi/include/mu/default
CXXFLAGS += -pthread
CXXFLAGS += -DBOOST_TEST_DYN_LINK
CXXFLAGS += -DBOOST_TEST_MAIN

LDFLAGS += ../../../server/job/class_route/Coordinates.o
LDFLAGS += ../../../server/job/class_route/Mapping.o
LDFLAGS += ../../../server/job/class_route/Np.o
LDFLAGS += ../../../server/job/class_route/Rectangle.o
LDFLAGS += -L ../../../common -lrunjob_common 
LDFLAGS += -llog4cxx
LDFLAGS += -Wl,-rpath,$(BGQ_BUILD_DIR)/hlcs/src/runjob/common
LDFLAGS += -lboost_filesystem-mt
LDFLAGS += -lboost_program_options-mt
LDFLAGS += -lboost_system-mt
LDFLAGS += -lboost_unit_test_framework-mt
LDFLAGS += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib
LDFLAGS += -L $(BGQ_INSTALL_DIR)/ras/lib -lras
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/ras/lib

LD1FLAGS = ../../../server/job/class_route/Coordinates.o
LD1FLAGS += ../../../server/job/class_route/Rectangle.o
LD1FLAGS += -L -llog4cxx
LD1FLAGS += -Wl,-rpath,/bgsys/drivers/ppcfloor/extlib/lib
LD1FLAGS += -L ../../../common -lrunjob_common 
LD1FLAGS += -Wl,-rpath,/bgsys/drivers/ppcfloor/hlcs/lib 
LD1FLAGS += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility
LD1FLAGS += -Wl,-rpath,/bgsys/drivers/ppcfloor/utility/lib

TESTERS   = $(patsubst %.cc,%,$(wildcard *.cc))


all: $(TESTERS:%=out_%)


out_%: test_%
	@strip $<
	@echo "execute $@"
	$(SILENT)BG_PROPERTIES_FILE=$(BGQ_INSTALL_DIR)/utility/etc/bg.properties.tpl ./$< 1> $(patsubst test_%, out_%, $<)


test_coordinates: coordinates.o Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)


test_np: np.o Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)
	
test_mapping: mapping.o Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LD1FLAGS)
	
install: all 


clean:
	$(RM) test_* out_*
	$(RM) *.o
	$(RM) *.d


distclean: clean

.DELETE_ON_ERROR:

-include *.d
