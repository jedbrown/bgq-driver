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

include ../../../../../Make.rules
include ../../../../Make.hlcs.rules
include ../../Make.rules

.PHONY: clean tags

CXXFLAGS += -Wall -fpic
CXXFLAGS += -MD
CXXFLAGS += -pthread
CXXFLAGS += -Wno-effc++
LDFLAGS  += -pthread
LDFLAGS  += -shared
LDFLAGS  += -Wl,-soname,$(lib_vers)
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/hlcs/lib -lbgsched
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib

SRCS  = Configuration.cc
SRCS += logging.cc
SRCS += Node.cc
SRCS += Nodes.cc
SRCS += Plugin.cc
SRCS += QueryBlockStatus.cc


lib_base       = find_a_node
lib_major_vers = 1
lib_minor_vers = 0
lib_minor_rev  = 0
lib_short      = lib$(lib_base).so
lib_vers       = $(lib_short).$(lib_major_vers)
lib_full       = $(lib_vers).$(lib_minor_vers).$(lib_minor_rev)


INSTALL_LIST   = $(addprefix $(MODULE_LIB_DIR)/,$(lib_short))
INSTALL_LIST  += $(addprefix $(MODULE_LIB_DIR)/,$(lib_vers))
INSTALL_LIST  += $(addprefix $(MODULE_LIB_DIR)/,$(lib_full))


all: test


$(lib_full): $(SRCS:%.cc=%.o)
	@echo "link $@"
	$(SILENT)$(CXX) $^ -o $@ $(LDFLAGS)


$(lib_vers): $(lib_full)
	ln -sf $< $@


$(lib_short): $(lib_vers)
	ln -sf $< $@


$(MODULE_LIB_DIR)/$(lib_short): $(lib_short) | $(MODULE_LIB_DIR)
	cp -fP $^ $@


$(MODULE_LIB_DIR)/$(lib_vers): $(lib_vers) | $(MODULE_LIB_DIR)
	cp -fP $^ $@


$(MODULE_LIB_DIR)/$(lib_full): $(lib_full) | $(MODULE_LIB_DIR)
	cp -fP $^ $@


install: $(INSTALL_LIST)


uninstall:
	rm -f $(INSTALL_LIST)


clean:
	$(RM) *.o
	$(RM) $(lib_short) $(lib_vers) $(lib_full)
	$(RM) *.d
	$(RM) test.out test


test.out: test
	./$^ &> $@

test: test.o $(lib_short)
	$(CXX) -o $@ test.o -L. -l$(lib_base) -Wl,-rpath,. -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib -L$(BGQ_INSTALL_DIR)/db/lib -lbgdb -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib -lboost_unit_test_framework-mt -L $(BGQ_INSTALL_DIR)/hlcs/lib -lbgsched

distclean: clean

.DELETE_ON_ERROR:

-include *.d
