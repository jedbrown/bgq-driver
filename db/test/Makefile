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


# How to populate the database:
# ./schema/dbPopulate.pl --properties $(PROPERTIES_FILE) --input ../schema/BGQ_config.txt --size 4x4

# Set BG_PROPERTIES_FILE=$(PROPERTIES_FILE) to set the properties file to use.

# Unit tests are always run on make.
# test target runs regression tests, too.


include ../../Make.rules
include ../Make.db.rules


libbgqdb_dir=../src/api
libbgqdb_so=$(libbgqdb_dir)/libbgdb.so.1.0.0


CXXFLAGS += -Wall
CXXFLAGS += -MD
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -iquote  $(BGQ_BUILD_DIR)/db
CXXFLAGS += -iquote  $(BGQ_BUILD_DIR)/db/include
CXXFLAGS += -iquote  $(BGQ_BUILD_DIR)/db/include/api
CXXFLAGS += -DBOOST_TEST_DYN_LINK

LDFLAGS += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS += -L $(BGQ_INSTALL_DIR)/bgq_util/lib -lbgq_util
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/bgq_util/lib
LDFLAGS += -L $(BGQ_INSTALL_DIR)/control/lib -lmcserver
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/control/lib
LDFLAGS += -Wl,-rpath,$(libbgqdb_dir)
LDFLAGS += -L $(BGQ_INSTALL_DIR)/ras/lib -lras
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/ras/lib
LDFLAGS += -lboost_program_options-mt

UNIT_TEST_LDFLAGS = $(LDFLAGS) -lboost_unit_test_framework-mt


PROPERTIES_FILE = $(BGQ_INSTALL_DIR)/utility/etc/bg.properties.tpl

DRIVER = dblibdriver

# unit tests are run with every make
UNIT_TESTS = \
 insert_colons \
 test_GenBlockParams \
 test_getJobs \
 test_TimeInterval \
 test_hexConversion

UNIT_TESTS_RUN = $(addsuffix .run,$(UNIT_TESTS))

# regression tests are only run if requested (with make test).
# these tests require a populated database.
REGRESSION_TESTS += test_genblock
REGRESSION_TESTS += test_performance_counters
REGRESSION_TESTS += test_pool_connections
REGRESSION_TESTS += test_job
REGRESSION_TESTS += test_putras
REGRESSION_TESTS_RUN = $(addsuffix .run,$(REGRESSION_TESTS))


%.run: %
	BG_PROPERTIES_FILE=$(PROPERTIES_FILE) ./$<
	@touch $@


%.properties: %.properties.template
	sed -e "s/%%BGQDB_DBNAME%%/$(BGQDB_DBNAME)/" $< | \
	sed -e "s/%%BGQDB_USER%%/$(BGQDB_USER)/" | \
	sed -e "s/%%BGQDB_PWD%%/$(BGQDB_PWD)/" | \
	sed -e "s/%%BGQDB_SCHEMA%%/$(BGQDB_SCHEMA)/" > $@


default: all

all: install

test: $(UNIT_TESTS_RUN) $(REGRESSION_TESTS_RUN)


$(DRIVER): dblibdriver.o $(libbgqdb_so)
	$(CXX) -o $@ $^ $(LDFLAGS)

insert_colons: insert_colons.o $(libbgqdb_so)
	$(CXX) -o $@ $^ $(UNIT_TEST_LDFLAGS)

test_genblock: test_genblock.o $(libbgqdb_so)
	$(CXX) -o $@ $^ $(UNIT_TEST_LDFLAGS) -lbgutility

test_putras: test_putras.o $(libbgqdb_so)
	$(CXX) -o $@ $^ $(UNIT_TEST_LDFLAGS)

test_putras.run: test_putras
	./$<
	@touch $@

test_genblock.run: test_genblock
	./$<
	@touch $@


test_GenBlockParams: test_GenBlockParams.o $(libbgqdb_so)
	$(CXX) -o $@ $^ $(UNIT_TEST_LDFLAGS) -lbgutility

test_pool_connections: test_pool_connections.o $(libbgqdb_so) good.properties garbage.properties
	$(CXX) -o $@ test_pool_connections.o $(libbgqdb_so) $(UNIT_TEST_LDFLAGS) -lbgutility

test_performance_counters: test_performance_counters.o $(libbgqdb_so)
	$(CXX) -o $@ $^ $(UNIT_TEST_LDFLAGS) -lbgutility -llog4cxx

test_getJobs: test_getJobs.o $(libbgqdb_so)
	$(CXX) -o $@ $^ $(UNIT_TEST_LDFLAGS) -lbgutility -lboost_date_time-mt

test_job: test_job.o $(libbgqdb_so)
	$(CXX) -o $@ $^ $(UNIT_TEST_LDFLAGS) -lbgutility

test_TimeInterval: test_TimeInterval.o $(libbgqdb_so)
	$(CXX) -o $@ $^ $(UNIT_TEST_LDFLAGS) -lboost_date_time-mt

test_hexConversion: test_hexConversion.o $(libbgqdb_so)
	$(CXX) -o $@ $^ $(UNIT_TEST_LDFLAGS) -lbgutility

install: dblibdriver $(UNIT_TESTS_RUN) $(REGRESSION_TESTS)


clean:
	$(RM) $(DRIVER) $(UNIT_TESTS) $(REGRESSION_TESTS) *.o *.d *.properties *.run


distclean: clean


.PHONY: all install clean distclean


-include *.d
