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

include ../../../Make.hlcs.rules
include ../../../../Make.rules

CXXFLAGS += -Wall -fpic
CXXFLAGS += -MD -MP
CXXFLAGS += -isystem $(BGQ_BUILD_DIR)
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -iquote ../
CXXFLAGS += -pthread
LDFLAGS  += -shared
LDFLAGS  += -pthread
LDFLAGS  += -Wl,-soname,$(lib_vers)
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib

lib_base=security_database
lib_major_vers=1
lib_minor_vers=0
lib_minor_rev=0
lib_short=lib$(lib_base).so
lib_vers=$(lib_short).$(lib_major_vers)
lib_full=$(lib_vers).$(lib_minor_vers).$(lib_minor_rev)

INSTALL_LIST = $(call INSTALL_LIB_TARGET,$(lib_short) $(lib_vers) $(lib_full))

SRCS      = create.cc
SRCS     += Database.cc
SRCS     += destroy.cc
SRCS     += Filter.cc
SRCS     += owner.cc
SRCS     += user.cc
SRCS     += utility.cc

all: $(lib_short)


$(lib_full): $(SRCS:%.cc=%.o) Makefile
	$(CXX) $(filter-out Makefile, $^) -o $@ $(LDFLAGS)


$(lib_vers): $(lib_full)
	ln -sf $< $@


$(lib_short): $(lib_vers)
	ln -sf $< $@


install: $(INSTALL_LIST)


uninstall:
	$(RM) $(INSTALL_LIST)


clean:
	$(RM) *.o
	$(RM) $(lib_short) $(lib_vers) $(lib_full)
	$(RM) *.d


distclean: clean

-include *.d
