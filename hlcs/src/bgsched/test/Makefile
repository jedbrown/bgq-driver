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
# (C) Copyright IBM Corp.  2004, 2011                              
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

# Set BG_PROPERTIES_FILE=$(PROPERTIES_FILE) to set the properties file to use.

# Unit tests are always run on make.
# test target runs regression tests, too.
include ../../../../Make.rules
include ../../../Make.hlcs.rules
include ../Make.bgsched.rules

CXXFLAGS += -DBOOST_TEST_DYN_LINK

LDFLAGS += -Wl,-soname,$(lib_vers)
LDFLAGS += -L $(BGQ_LIB_INSTALL_DIR) -lbgsched
LDFLAGS += -Wl,-rpath,$(BGQ_LIB_INSTALL_DIR)
LDFLAGS += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib

LDFLAGSPLUS += -Wl,-soname,$(lib_vers)
LDFLAGSPLUS += -L $(BGQ_LIB_INSTALL_DIR) -lbgsched
LDFLAGSPLUS += -Wl,-rpath,$(BGQ_LIB_INSTALL_DIR)
LDFLAGSPLUS += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility
LDFLAGSPLUS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGSPLUS += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
LDFLAGSPLUS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib
LDFLAGSPLUS += -L $(BGQ_INSTALL_DIR)/extlib/lib -llog4cxx
LDFLAGSPLUS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/extlib/lib

# test_allocator sources
TEST_ALLOCATOR_SRCS  =  TestAllocatorEventListener.cc 
TEST_ALLOCATOR_SRCS +=  test_allocator.cc 

TESTALLOCATOR = test_allocator

# test_block sources
TEST_BLOCK_SRCS =  test_block.cc 

TESTBLOCK = test_block

# test_block2 sources
TEST_BLOCK2_SRCS =  test_block2.cc 

TESTBLOCK2 = test_block2

# test_I/O block sources
TEST_IOBLOCK_SRCS =  test_ioblock.cc 

TESTIOBLOCK = test_ioblock

# test_realtime_client sources
TEST_REALTIME_CLIENT_SRCS =  test_realtime_client.cc 

TESTREALTIMECLIENT = test_realtime_client

# test_resourcespec sources
TEST_RESOURCESPEC_SRCS =  test_resourcespec.cc 

TESTRESOURCESPEC = test_resourcespec

# test_schedutil sources
TEST_SCHEDUTIL_SRCS =  test_schedutil.cc 

TESTSCHEDUTIL = test_util

# test_dimension sources
TEST_DIMENSION_SRCS =  test_dimension.cc 

TESTDIMENSION = test_dimension

# create_jobs sources
CREATEJOBS_SRCS =  create_jobs.cc 

CREATEJOBS = create_jobs

PROPERTIES_FILE = $(BGQ_INSTALL_DIR)/utility/etc/bg.properties.tpl

#DRIVER          = bgschedlibdriver
DRIVER          = 
DRIVER_SRC      = $(addsuffix .cc,$(DRIVER))

# Run with every Make
#UNIT_TESTS      =  test_schedutil test_resourcespec test_block
UNIT_TESTS      = 
UNIT_TESTS_SRC  = $(addsuffix .cc,$(UNIT_TESTS))

#REGRESSION_TESTS        = test_schedutil test_resourcespec test_block
REGRESSION_TESTS        = 
REGRESSION_TESTS_SRC    = $(addsuffix .cc,$(REGRESSION_TESTS))

all: install asyncRealtime disconnectRealtime

test: $(REGRESSION_TESTS)

$(UNIT_TESTS): $(UNIT_TESTS_SRC:%.cc=%.o)
	$(CXX) -o $@ $@.o $(LDFLAGS) -lboost_unit_test_framework-mt
	BG_PROPERTIES_FILE=$(PROPERTIES_FILE) ./$@


$(DRIVER): $(DRIVER_SRC:%.cc=%.o)
	$(CXX) -o $@ $^ $(LDFLAGS)
	
$(DRIVER): $(DRIVER_SRC:%.cc=%.o)
	$(CXX) -o $@ $^ $(LDFLAGS)

asyncRealtime: asyncRealtime.o
	$(CXX) -o $@ $^ $(LDFLAGS)

disconnectRealtime: disconnectRealtime.o
	$(CXX) -o $@ $^ $(LDFLAGS)

install: $(DRIVER) $(UNIT_TESTS)

$(TESTALLOCATOR): $(TEST_ALLOCATOR_SRCS:%.cc=%.o) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

$(TESTBLOCK): $(TEST_BLOCK_SRCS:%.cc=%.o) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)
	
$(TESTBLOCK2): $(TEST_BLOCK2_SRCS:%.cc=%.o) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

$(TESTIOBLOCK): $(TEST_IOBLOCK_SRCS:%.cc=%.o) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)	

$(TESTREALTIMECLIENT): $(TEST_REALTIME_CLIENT_SRCS:%.cc=%.o) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)
	
$(TESTRESOURCESPEC): $(TEST_RESOURCESPEC_SRCS:%.cc=%.o) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

$(TESTSCHEDUTIL): $(TEST_SCHEDUTIL_SRCS:%.cc=%.o) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

$(TESTDIMENSION): $(TEST_DIMENSION_SRCS:%.cc=%.o) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)
	
$(CREATEJOBS): $(CREATEJOBS_SRCS:%.cc=%.o) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGSPLUS)

$(REGRESSION_TESTS): $(REGRESSION_TESTS_SRC:%.cc=%.o)
	$(CXX) -o $@ $@.o $(LDFLAGS) -lboost_unit_test_framework-mt
	./$@

clean:
	$(RM) $(DRIVER) $(UNIT_TESTS) $(REGRESSION_TESTS)
	$(RM) *.o
	$(RM) *.d
	$(RM) test_allocator test_schedutil test_resourcespec test_block test_ioblock test_realtime_client test_dimension asyncRealtime disconnectRealtime create_jobs

distclean: clean

-include *.d
