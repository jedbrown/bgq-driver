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
# (C) Copyright IBM Corp.  2007, 2011                              
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

TOP_DIR=control

# Location of the python scripts to build headers
XMLSCRIPT_DIR=$(BGQ_BUILD_DIR)/xml/src/scripts

CXXFLAGS += $(DEBUG_FLAGS)
CXXFLAGS += -Wall -g -Wno-format-y2k
CXXFLAGS += -I$(BGQ_INSTALL_DIR)
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/utility/include
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/bgq_util/include
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/utility/include/cxxsockets
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/xml/include/library
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/xml/include/c_api

CXXFLAGS_64 += -m64

compile-cmd = g++ $(CXXFLAGS) $1 -o $@ -c $<

# explicit 64
%.o_64: %.cc
	$(call compile-cmd, $(CXXFLAGS_64))

SRCS=StoreProtocolSpec.cc
OBJS64   = $(SRCS:%.cc=%.o_64)

all: test

test: generate $(OBJS64) store stored

TEST_LDFLAGS  = -L $(BGQ_INSTALL_DIR)/utility/lib/ -llog4cxx
TEST_LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib

generate: StoreProtocolSpec.xml
	$(XMLSCRIPT_DIR)/generateCode.py --cheader StoreProtocolSpec.xml ; $(XMLSCRIPT_DIR)/generateCode.py --cimpl StoreProtocolSpec.xml ; cp StoreProtocolSpec.h $(BGQ_INSTALL_DIR)/xml/include/c_api/

store: generate store.cc $(OBJS64)
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_64) -o store store.cc -lpthread $(LDFLAGS) $(TEST_LDFLAGS) $(BGQ_INSTALL_DIR)/bgq_util/lib/libbgq_util_s.a $(BGQ_INSTALL_DIR)/utility/lib/libcxxsockets_s.a $(BGQ_INSTALL_DIR)/xml/lib/libXML_s.a $(OBJS64) -lbgutility

stored: generate stored.cc $(OBJS64)
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_64) -o stored stored.cc -lpthread $(LDFLAGS) $(TEST_LDFLAGS) $(BGQ_INSTALL_DIR)/bgq_util/lib/libbgq_util_s.a $(BGQ_INSTALL_DIR)/utility/lib/libcxxsockets_s.a $(BGQ_INSTALL_DIR)/xml/lib/libXML_s.a $(OBJS64) -lbgutility

clean:
	rm -rf store stored *o *o_64 *~ Store*[cch] \#*

-include *.d
-include .dep.*.64.d
