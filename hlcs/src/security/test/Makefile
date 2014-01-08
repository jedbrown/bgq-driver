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

include ../../../../Make.rules
include ../../../Make.hlcs.rules

CXXFLAGS += -Wall
CXXFLAGS += -MD -MP
CXXFLAGS += -isystem $(BGQ_BUILD_DIR)
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -isystem $(BGQ_BUILD_DIR)/hlcs/include
CXXFLAGS += -DBOOST_TEST_DYN_LINK
CXXFLAGS += -DBOOST_TEST_MAIN
CXXFLAGS += -DBOOST_TEST_MAIN
CXXFLAGS += -g

LDFLAGS += -L .. -lsecurity
LDFLAGS += -Wl,-rpath,$(BGQ_BUILD_DIR)/hlcs/src/security
LDFLAGS += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS += -lboost_unit_test_framework-mt
LDFLAGS += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib

TESTERS   = actions
TESTERS  += db_permission
TESTERS  += list
TESTERS  += persistent


all: $(addprefix test_, $(TESTERS))

install: $(addprefix test_, $(TESTERS))

test: $(TESTERS:%=out_%)

out_%: export BG_PROPERTIES_FILE = $(BGQ_INSTALL_DIR)/utility/etc/bg.properties.tpl

out_%: test_%
	./$<

test_actions: actions.o Makefile
	$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_db_permission: db_permission.o Makefile
	$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_list: list.o Makefile
	$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_persistent: persistent.o Makefile
	$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

install: all

uninstall:


clean:
	$(RM) test_* out_*
	$(RM) *.o
	$(RM) *.d


distclean: clean

.DELETE_ON_ERROR:

-include *.d
