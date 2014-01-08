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

include ../../../Make.rules

ifneq ($(BITS),32)
CXXFLAGS+= -m64
endif
CXXFLAGS+= -Wall
CXXFLAGS+= -MMD
CXXFLAGS+= -pthread
CXXFLAGS+= -I$(DB2PATH)/include
CXXFLAGS+= -I$(BGP_CONTROL_PATH)/../include
CXXFLAGS+= -I../server/test -I../server

ifneq ($(BITS),32)
LDFLAGS+= -L$(DB2PATH)/lib64  # DB2 client
LDFLAGS+= -L$(BGP_CONTROL_PATH)/../lib64
else
LDFLAGS+= -L$(DB2PATH)/lib  # DB2 client
LDFLAGS+= -L$(BGP_CONTROL_PATH)/../lib
endif
LDFLAGS+= -lbgrealtime
LDFLAGS+= -ldb2  # DB2 client
LDFLAGS+= -lrt  # clock_gettime

exes = DbPropertiesTest
exes+= nowait_update
exes+= response_time
exes+= response_time2
exes+= response_time3
exes+= rt_clients
exes+= test_block_update
exes+= test_disconnect_problem
exes+= test_disconnect_problem_serv
exes+= test_disconnect_problem_client


.PHONY: all clean distclean install


all: $(exes)

DbPropertiesTest: DbPropertiesTest.o DbProperties.o
	$(CXX) $(CXXFLAGS) -o $@ $^

nowait_update: DbProperties.o sql_util.o

response_time: response_time.cc DbProperties.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

response_time2: response_time2.cc DbProperties.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

response_time3: response_time3.o DbProperties.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

rt_clients: rt_clients.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

test_block_update: test_block_update.o DbProperties.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

test_disconnect_problem: test_disconnect_problem.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ ../server/test/rtdbview.o ../server/util.o

test_disconnect_problem_serv: test_disconnect_problem_serv.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ ../server/test/rtdbview.o ../server/util.o

test_disconnect_problem_client: test_disconnect_problem_client.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

install: all

clean:
	$(RM) $(exes) *.o *.d

distclean: clean


-include *.d
