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
include ../../Make.utility.rules


CXXFLAGS +=\
 -DBOOST_TEST_DYN_LINK -isystem $(BGQ_INSTALL_DIR)/extlib/include


LDFLAGS += \
 -L$(BGQ_INSTALL_DIR)/extlib/lib -lboost_unit_test_framework-mt -Wl,-rpath,$(BGQ_INSTALL_DIR)/extlib/lib \
 -L../../src -lbgutility -Wl,-rpath,../../src


exes := \
 test_ClientPortConfiguration \
 test_Connector \
 test_PortConfiguration \
 test_ServerPortConfiguration \
 test_Acceptor


run_targets = ${exes:%=%.run}


test_%: test_%.o
	$(CXX) -o $@ $^ $(LDFLAGS) $(LOADLIBES) $(LDLIBS)


test_%.run: test_%
	@echo "Running" $<
	@BG_PROPERTIES_FILE=$(etc_install_dir)/bg.properties.tpl ./$< &> $@.tmp ; RES=$$? ; if [ $$RES != 0 ]; then echo $< Failed ; cat $@.tmp; rm $@.tmp; exit 1; else mv $@.tmp $@; fi


default: all


all: $(run_targets)


${exes:%=%.o} $(exes):


test_Acceptor.o: test_Acceptor.cc ../../src/libbgutility.so.1.0.0

test_ClientPortConfiguration.o: test_PortConfiguration.cc ../../src/libbgutility.so.1.0.0

test_Connector.o: test_Connector.cc ../../src/libbgutility.so.1.0.0

test_PortConfiguration.o: test_PortConfiguration.cc ../../src/libbgutility.so.1.0.0

test_ServerPortConfiguration.o: test_PortConfiguration.cc ../../src/libbgutility.so.1.0.0



test: $(run_targets)


install: all


clean:
	$(RM) *.o *.d $(exes)


distclean: clean


.PHONY: default all test install clean distclean


-include *.d
