# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# (C) Copyright IBM Corp.  2011, 2011                              
# Eclipse Public License (EPL)                                     
#                                                                  
# ================================================================ 
#                                                                  
# end_generated_IBM_copyright_prolog                               
include ../../../Make.rules
include ../../Make.utility.rules

CXXFLAGS += -Wall
CXXFLAGS += -MD
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -g

LDFLAGS = -lboost_thread-mt

all: sample

install: all

# test_doq: test_doq.o
# 	$(CXX) $(LDFLAGS) -o $@ $(filter-out Makefile, $^)

sample: sample.o
	$(CXX) $(LDFLAGS) -o $@ $(filter-out Makefile, $^)

clean:
	$(RM) *o *d sample
