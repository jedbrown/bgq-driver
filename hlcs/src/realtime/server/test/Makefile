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

include ../../../../Make.rules

ifeq ($(BITS),)

UNAME_PROC=$(shell uname -i)

ifeq ($(UNAME_PROC),ppc64)
BITS=64
else
BITS=32
endif
endif


CXXFLAGS+=-MMD
CXXFLAGS+=-g
CXXFLAGS+=-I..
ifneq ($(BITS),32)
CXXFLAGS+=-m64
CXXFLAGS+=-I$(DB2PATH)/include64
else
CXXFLAGS+=-I$(DB2PATH)/include
endif

CXXFLAGS+=-pthread

CXXFLAGS+=-iquote $(BGPSYS_DIR)/control/bridge
CXXFLAGS+=-iquote ../../ext 
CXXFLAGS+=-iquote ../../common
CXXFLAGS+=-iquote $(BGPSYS_DIR)/control/util

LDFLAGS =\
 -ldl

fake_submit_server_objs =\
 fake_submit_server.o \
 compute_node_fail_info.o \
 io_node_fail_info.o \
 rt_ext_msgs.o \
 rt_protocol_message.o

test_timed_event_objs =\
 test_timed_event.o \
 timed_events.o \
 Log.o \
 util.o


vpath %.cc .. ../../ext ../../../util # find some ccs elsewhere.


exes = fake_submit_server test_timed_event

all: $(exes)

fake_submit_server: $(fake_submit_server_objs)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $^

test_timed_event: $(test_timed_event_objs)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $^

test_bg_realtime: rtdbview.o test_bg_realtime.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $^

.PHONY: clean

clean:
	$(RM) $(exes) *.o *.d

-include *.d
