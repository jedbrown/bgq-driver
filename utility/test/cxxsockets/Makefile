# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# (C) Copyright IBM Corp.  2011, 2011                              
# Eclipse Public License (EPL)                                     
#                                                                  
# ================================================================ 
#                                                                  
# end_generated_IBM_copyright_prolog                               
include ../../../Make.rules
include ../../Make.utility.rules

CXXFLAGS += -Wall
CXXFLAGS += -MD
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -g

LDFLAGS += -L $(BGQ_BUILD_DIR)/utility/src-lbgutility
LDFLAGS += -Wl,-rpath,$(BGQ_BUILD_DIR)/utility/src
LDFLAGS += -L $(BGQ_BUILD_DIR)/utility/src/cxxsockets -lcxxsockets
LDFLAGS += -Wl,-rpath,$(BGQ_BUILD_DIR)/utility/src/cxxsockets

SAMPLES   = asio listen_both46
TESTERS   = host

all: $(TESTERS:%=out_%) $(SAMPLES)

install: all

listen_both46: listen_both46.o
	$(CXX) -lboost_thread-mt $(LDFLAGS) -o $@ $(filter-out Makefile, $^)

asio: asio.o
	$(CXX) -lboost_thread-mt -lboost_system $(LDFLAGS) -o $@ $(filter-out Makefile, $^)

out_%: test_% $(etc_install_dir)/bg.properties.tpl
	@strip $<
	BG_PROPERTIES_FILE=$(etc_install_dir)/bg.properties.tpl ./$< 1> $(patsubst test_%, out_%, $<)

test_host: LDFLAGS += -lboost_unit_test_framework-mt
test_host: host.o Makefile
	$(CXX) $(LDFLAGS) -o $@ $(filter-out Makefile, $^)

host.o: CXXFLAGS += -DBOOST_TEST_DYN_LINK -DBOOST_TEST_MAIN

clean:
	$(RM) *o *d asio listen_both46
