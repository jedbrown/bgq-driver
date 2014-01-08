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
include ../../Make.utility.rules
include ../../../Make.rules

# for optimization and debugging
CXXFLAGS += -Wall
CXXFLAGS += -O0
CXXFLAGS += -iquote $(BGQ_BUILD_DIR)/utility/include
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/bgq_util/include
CXXFLAGS += -isystem $(include_install_dir)/cxxsockets
CXXFLAGS += -MD
CXXFLAGS += -fPIC

SHARED_LDFLAGS   = $(LDFLAGS)
SHARED_LDFLAGS  += -L ../ -lbgutility
SHARED_LDFLAGS  += -lssl
SHARED_LDFLAGS  += -lboost_system-mt
SHARED_LDFLAGS  += -shared
SHARED_LDFLAGS  += -Wl,-soname,$(lib_vers)
SHARED_LDFLAGS  += -Wl,-rpath,$(BGQ_LIB_INSTALL_DIR)
SHARED_LDFLAGS  += -Wl,--enable-new-dtags # set DT_RUNPATH
EXE_LDFLAGS     = $(LDFLAGS)
EXE_LDFLAGS     += -L $(BGQ_LIB_INSTALL_DIR) -lcxxsockets
EXE_LDFLAGS     += -Wl,-rpath,$(BGQ_LIB_INSTALL_DIR)

lib_base:=cxxsockets
lib_dev:=lib$(lib_base).so
lib_vers:=$(lib_dev).1
lib_full:=$(lib_vers).0.0
STATIC_LIB = libcxxsockets.a

install_list=$(call INSTALL_LIB_TARGET,$(lib_dev) $(lib_vers) $(lib_full) $(STATIC_LIB))

SRCS  = Functors.cc
SRCS += CxxSocket.cc
SRCS += TCPSocket.cc
SRCS += ListeningSocket.cc
SRCS += SecureTCPSocket.cc
SRCS += File.cc
SRCS += LocalSocket.cc
SRCS += SocketAddrs.cc
SRCS += FileSet.cc
SRCS += SocketSet.cc
SRCS += ListenerSets.cc
SRCS += PollSocketSet.cc
SRCS += PollFileSet.cc
SRCS += EpollingFileSet.cc
SRCS += EpollingSocketSet.cc

OBJS     = $(SRCS:%.cc=%.o)

default: $(STATIC_LIB) $(lib_short)

$(STATIC_LIB): Makefile $(OBJS)
	$(AR) crv  $(STATIC_LIB) $(OBJS)

$(lib_dev): $(lib_vers)
	ln -sf $< $@

$(lib_vers): $(lib_full)
	ln -sf $< $@

$(lib_full): $(OBJS)
	$(CXX) $(SHARED_LDFLAGS) -o $@ $^

install: $(install_list)

uninstall:
	$(RM) $(install_list)

test: $(SHARED_LIB) test_client test_server hello hellod

test_client: test_client.cc | install
	$(CXX) $(CXXFLAGS) -o $@ $< $(EXE_LDFLAGS)

test_server: test_server.cc | install
	$(CXX) $(CXXFLAGS) -o $@ $< $(EXE_LDFLAGS)

hello: $(SRC) hello.cc | install
	$(CXX) $(CXXFLAGS) -o $@ $< $(EXE_LDFLAGS)

hellod: $(SRC) hellod.cc | install
	$(CXX) $(CXXFLAGS) -o $@ $< $(EXE_LDFLAGS)

hello_nouid: $(SRC) hello_nouid.cc | install
	$(CXX) $(CXXFLAGS) -o $@ $< $(EXE_LDFLAGS)

hello_nouidd: $(SRC) hello_nouidd.cc | install
	$(CXX) $(CXXFLAGS) -o $@ $< $(EXE_LDFLAGS)

hello_insecure: hello_insecure.cc | install
	$(CXX) $(CXXFLAGS) -o $@ $< $(EXE_LDFLAGS)

hello_insecured: hello_insecured.cc | install
	$(CXX) $(CXXFLAGS) -o $@ $< $(EXE_LDFLAGS)

all: default

clean: 
	rm -rf test_client test_server hello hellod *o *o_64 $(STATIC_LIB) $(SHARED_LIB) *.d *~ \#*\# core

distclean: clean

-include *.d
