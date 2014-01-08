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

include ../../Make.hlcs.rules
include ../../../Make.rules

CXXFLAGS += -O2
CXXFLAGS += -Wall -fpic
CXXFLAGS += -fvisibility=default
CXXFLAGS += -MD -MP
CXXFLAGS += -isystem $(BGQ_BUILD_DIR)
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -pthread
LDFLAGS  += -shared
LDFLAGS  += -pthread
LDFLAGS  += -Wl,-soname,$(lib_vers)
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/bgq_util/lib -lbgq_util
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/bgq_util/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib

lib_base=security
lib_major_vers=1
lib_minor_vers=0
lib_minor_rev=0
lib_short=lib$(lib_base).so
lib_vers=$(lib_short).$(lib_major_vers)
lib_full=$(lib_vers).$(lib_minor_vers).$(lib_minor_rev)

INSTALL_LIST = $(call INSTALL_LIB_TARGET,$(lib_short) $(lib_vers) $(lib_full))

SRCS      = $(wildcard *.cc)
SRCS     += $(wildcard db/*.cc)


all: $(lib_short)
	$(MAKE) --directory test $@
	$(MAKE) --directory docs $@


$(lib_full): $(SRCS:%.cc=%.o) Makefile
	$(CXX) $(filter-out Makefile, $^) -o $@ $(LDFLAGS)


$(lib_vers): $(lib_full)
	ln -sf $< $@


$(lib_short): $(lib_vers)
	ln -sf $< $@


install: $(INSTALL_LIST) ras_scan
	$(MAKE) --directory test $@
	$(MAKE) --directory docs $@


uninstall:
	$(RM) $(INSTALL_LIST)


clean:
	$(RM) *.o db/*.o
	$(RM) $(lib_short) $(lib_vers) $(lib_full)
	$(RM) *.d db/*.d
	$(MAKE) --directory test $@
	$(MAKE) --directory docs $@
	$(RM) ras_scan

test: install
	$(MAKE) --directory test $@

distclean: clean

ras_scan: $(SRCS)
	$(BGQ_BUILD_DIR)/scripts/rasScan.py $(BGQ_BUILD_DIR)/hlcs/src/security $(BGQ_INSTALL_DIR)/hlcs/ras/metadata security_ras.xml $(BGQ_INSTALL_DIR)/ras/include/ras.xml
	@touch $@

-include *.d
-include db/*.d
