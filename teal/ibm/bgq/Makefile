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
# (C) Copyright IBM Corp.  2009, 2011                              
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
include ../../Make.teal.rules
include ../../../Make.rules

DIRS=analyzer init.d

TARGETS_ALL		= $(addsuffix .all,$(DIRS))
TARGETS_INSTALL		= $(addsuffix .install,$(DIRS))
TARGETS_CLEAN		= $(addsuffix .clean,$(DIRS))
TARGETS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))

TARGETS = $(TARGETS_ALL)       \
	  $(TARGETS_INSTALL)   \
	  $(TARGETS_CLEAN)     \
	  $(TARGETS_DISTCLEAN)

# bg realtime client requires 64 bit build
# To pyrealtime you need the following parameters set:
PYINC = $(shell ls -d /usr/include/python[23].[0-9])

CXXFLAGS += -w -Wall -g -m64 -pthread
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/hlcs/include
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/utility/include
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/extlib/include
CXXFLAGS += -I$(PYINC) -shared 

LDFLAGS += -L$(BGQ_INSTALL_DIR)/hlcs/lib -lbgsched -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib
LDFLAGS += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib/
LDFLAGS += -L $(BGQ_INSTALL_DIR)/extlib/lib -llog4cxx -Wl,-rpath,$(BGQ_INSTALL_DIR)/extlib/lib/
LDFLAGS += -lpthread


all: default

default: build install 
#default: install 

install: $(TARGETS_INSTALL) build
	$(MKDIR) -p $(TEAL_OPT_INSTALL_DIR)/teal/ibm/bgq/
	$(CP) *.py   $(TEAL_OPT_INSTALL_DIR)/teal/ibm/bgq/.
	$(CP) *.so   $(TEAL_OPT_INSTALL_DIR)/teal/ibm/bgq/.

build: pyrealtime.so

pyrealtime.so: pyrealtime.cc
	gcc -fPIC pyrealtime.cc $(CXXFLAGS) $(LDFLAGS) -o pyrealtime.so

clean: $(TARGETS_CLEAN)
	$(RM) pyrealtime.so
	$(RM) *.pyc
	$(RM) $(TEAL_OPT_INSTALL_DIR)/teal/ibm/bgq/*.py
	$(RM) $(TEAL_OPT_INSTALL_DIR)/teal/ibm/bgq/*.so

$(TARGETS)::
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))

.PHONY: all clean default distclean $(TARGETS)

