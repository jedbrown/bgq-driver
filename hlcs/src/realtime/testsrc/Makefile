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
# (C) Copyright IBM Corp.  2012, 2012                              
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


include ../../../../Make.rules


CXXFLAGS +=\
 -Wall \
 -isystem $(BGQ_INSTALL_DIR)/hlcs/include \
 -isystem $(BGQ_INSTALL_DIR) \
 -DBOOST_TEST_DYN_LINK -isystem $(BGQ_INSTALL_DIR)/extlib/include


LDFLAGS += \
 -L$(BGQ_INSTALL_DIR)/hlcs/lib -lbgsched \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib \
 -L$(BGQ_INSTALL_DIR)/db/lib -lbgdb \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib \
 -L$(BGQ_INSTALL_DIR)/extlib/lib -lboost_unit_test_framework-mt


default: all

all: test_realtime


test: test_realtime
	./$<


test_realtime: test_realtime.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


install:

clean:
	$(RM) *.o *.d test_realtime

distclean:


.PHONY: default all install clean distclean test
