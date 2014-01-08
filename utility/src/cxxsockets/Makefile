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

CXXFLAGS += -Wall
CXXFLAGS += -Wextra
CXXFLAGS += -Wconversion
CXXFLAGS += -Wsign-conversion
CXXFLAGS += -Wlogical-op
CXXFLAGS += -Wvla
CXXFLAGS += -Wmissing-include-dirs
CXXFLAGS += -O2
CXXFLAGS += -MD -MP
CXXFLAGS += -fpic

ifeq ($(BGQ_HOST_OS),x86_64)
# tolerate warnings in rh5 x86 builds.
CFLAGS    := $(subst -Werror,,$(CFLAGS))
CXXFLAGS  := $(subst -Werror,,$(CXXFLAGS))
endif

SHARED_LDFLAGS   = $(LDFLAGS)
SHARED_LDFLAGS  += -L ../ -lbgutility
SHARED_LDFLAGS  += -lssl
SHARED_LDFLAGS  += -lboost_system-mt
SHARED_LDFLAGS  += -shared
SHARED_LDFLAGS  += -Wl,-soname,$(lib_vers)
SHARED_LDFLAGS  += -Wl,-rpath,$(BGQ_LIB_INSTALL_DIR)
SHARED_LDFLAGS  += -Wl,--enable-new-dtags # set DT_RUNPATH

lib_base:=cxxsockets
lib_dev:=lib$(lib_base).so
lib_vers:=$(lib_dev).1
lib_full:=$(lib_vers).0.0
STATIC_LIB = libcxxsockets.a

install_list=$(call INSTALL_LIB_TARGET,$(lib_dev) $(lib_vers) $(lib_full) $(STATIC_LIB))

SRCS += Socket.cc
SRCS += TCPSocket.cc
SRCS += ListeningSocket.cc
SRCS += SecureTCPSocket.cc
SRCS += File.cc
SRCS += SockAddr.cc
SRCS += SockAddrList.cc
SRCS += FileSet.cc
SRCS += ListenerSet.cc
SRCS += Host.cc

OBJS     = $(SRCS:%.cc=%.o)

default: $(STATIC_LIB) $(lib_short)

$(STATIC_LIB): Makefile $(OBJS)
	$(AR) crv  $(STATIC_LIB) $(OBJS)

$(lib_dev): $(lib_vers)
	ln -sf $< $@

$(lib_vers): $(lib_full)
	ln -sf $< $@

$(lib_full): $(OBJS)
	$(CXX) -o $@ $^ $(SHARED_LDFLAGS)

install: $(install_list)

uninstall:
	$(RM) $(install_list)

all: default

clean: 
	rm -rf *.o $(STATIC_LIB) $(lib_vers) $(lib_full) $(lib_dev) *.d *~ \#*\# core

distclean: clean

-include *.d
