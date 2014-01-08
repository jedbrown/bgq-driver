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

MODULE_DIR=bgq_util


boost_libs_dir=$(BGQ_INSTALL_DIR)/utility/lib

my_install_lib_dir=$(BGQ_INSTALL_DIR)/$(MODULE_DIR)/lib


CXXFLAGS += -iquote ../../include
CXXFLAGS += -O0 -g


LDFLAGS += -Wl,-rpath,$(my_install_lib_dir)



.PHONY: default all clean distclean test



default: all


all: ArbitraryLengthNumberTest testBgqCableBadWires

ArbitraryLengthNumberTest.o: CXXFLAGS += -isystem$(BGQ_BUILD_DIR)

ArbitraryLengthNumberTest: LDFLAGS += ../libbgq_util.a

ArbitraryLengthNumberTest: ArbitraryLengthNumberTest.o ../libbgq_util.a
	$(CXX) $^ -o $@

testBgqCableBadWires.o: CXXFLAGS += -isystem$(BGQ_BUILD_DIR)

testBgqCableBadWires: LDFLAGS += $(my_install_lib_dir)/libbgq_util.so.1.0.0

testBgqCableBadWires: testBgqCableBadWires.o $(my_install_lib_dir)/libbgq_util.so.1.0.0
	$(CXX) $^ -o $@ -Wl,-rpath,$(boost_libs_dir) -Wl,-rpath,$(my_install_lib_dir)
	
test_location.o: CXXFLAGS += -DBOOST_TEST_DYN_LINK -isystem$(BGQ_BUILD_DIR) -isystem$(BGQ_INSTALL_DIR)
test_location: test_location.o $(my_install_lib_dir)/libbgq_util.so.1.0.0
	$(CXX) -o $@ $^ -L$(boost_libs_dir) -lboost_unit_test_framework -Wl,-rpath,$(boost_libs_dir) -Wl,-rpath,$(my_install_lib_dir)

test: test_location
	./$<


clean:
	$(RM) *.o *.o_64 test_location


distclean: clean
