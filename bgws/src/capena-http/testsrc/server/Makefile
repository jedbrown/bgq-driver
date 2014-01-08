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


include ../../../../Make.bgws.rules
include ../../../../../Make.rules


CXXFLAGS += \
 -isystem $(BGQ_INSTALL_DIR)/extlib/include \
 -isystem $(BGQ_INSTALL_DIR)


LDFLAGS += \
 -Wl,--enable-new-dtags \
 -L$(BGQ_INSTALL_DIR)/extlib/lib \
 -lboost_unit_test_framework-mt \
 -lboost_system-mt \
 -lboost_thread-mt \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility \
 -Wl,-rpath-link,$(BGQ_INSTALL_DIR)/extlib/lib:$(BGQ_INSTALL_DIR)/utility/lib \
 -lcurl \
 -llog4cxx


test_%.o: CXXFLAGS+=-DBOOST_TEST_DYN_LINK


test_%.run: test_%
	LD_LIBRARY_PATH=$(BGQ_INSTALL_DIR)/extlib/lib:$(BGQ_INSTALL_DIR)/utility/lib:../.. BG_PROPERTIES_FILE=$(BGQ_INSTALL_DIR)/utility/etc/bg.properties.tpl ./$< > $@.tmp
	mv $@.tmp $@


tests := \
 test_basics \
 test_Connection \
 test_Request \
 test_Responder

output_files := ${tests:%=%.run}


clean_cmds := $(RM) *.o *.d *.run $(tests)


default: all


all: $(tests)

test: $(output_files)


test_basics: test_basics.o CurlHandle.o GlobalFixture.o ../../libcapena-http.so
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

test_Connection: test_Connection.o ConnectSendGetResponseLine.o CurlHandle.o EchoResponder.o GlobalFixture.o ../../libcapena-http.so
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

test_Request: test_Request.o ConnectSendGetResponseLine.o CurlHandle.o GlobalFixture.o ../../libcapena-http.so
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

test_Responder: test_Responder.o CurlHandle.o GlobalFixture.o ../../libcapena-http.so
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


clean:
	$(clean_cmds)


distclean:
	$(clean_cmds)


.PHONY: default all clean distclean test


-include *.d
