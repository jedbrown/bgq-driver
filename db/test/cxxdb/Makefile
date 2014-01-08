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


include ../../Make.db.rules
include ../../../Make.rules

CXXFLAGS +=\
 -pthread \
 -Wall \
 -MMD \
 -iquote ../../include/api \
 -iquote ../../include/api/cxxdb \
 -isystem $(BGQ_INSTALL_DIR) \
 -DBOOST_TEST_DYN_LINK -isystem $(BGQ_INSTALL_DIR)/extlib/include


path_to_cxxdb_lib=../../src/api


LDFLAGS += \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib \
 -L$(BGQ_INSTALL_DIR)/extlib/lib -lboost_unit_test_framework-mt -Wl,-rpath,$(BGQ_INSTALL_DIR)/extlib/lib \
 -Wl,-rpath,$(path_to_cxxdb_lib)

cxxdb_lib=$(path_to_cxxdb_lib)/libbgdb.so.1.0.0


exes=testcxxdb testConnection testStatement testColumns testParameters testConnectionPool

run_targets=${exes:%=%.run}


%.run: %
	BG_PROPERTIES_FILE=$(BGQ_INSTALL_DIR)/utility/etc/bg.properties.tpl ./$<

%: %.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


default: all


testcxxdb: testcxxdb.o $(cxxdb_lib)

testColumns: testColumns.o $(cxxdb_lib)

testConnection: testConnection.o $(cxxdb_lib)

testConnectionPool: testConnectionPool.o $(cxxdb_lib)

testParameters: testParameters.o $(cxxdb_lib)

testStatement: testStatement.o $(cxxdb_lib)


all: $(exes)


test: $(run_targets)


install:

uninstall:

clean:
	$(RM) $(exes) *.o *.d


distclean: clean


.PHONY: default all install uninstall clean distclean test


-include *.d
