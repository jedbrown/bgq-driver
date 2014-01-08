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

EXECS = properties_validate

CXXFLAGS += -O2
CXXFLAGS += -Wall
CXXFLAGS += -MD -MP
CXXFLAGS += -iquote $(BGQ_BUILD_DIR)/hlcs/src/master
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)

LDFLAGS  += -lboost_date_time-mt
LDFLAGS  += -lboost_thread-mt
LDFLAGS  += -lboost_program_options-mt
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility 
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/xml/lib -lXML
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/xml/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/ras/lib -lras
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/ras/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/control/lib -lmcserver
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/control/lib

BGM_OBJS =
BGM_OBJS += ../master/common/AgentBase.o
BGM_OBJS += ../master/common/BinaryController.o
BGM_OBJS += ../master/server/AgentManager.o
BGM_OBJS += ../master/server/AgentRep.o
BGM_OBJS += ../master/server/Alias.o
BGM_OBJS += ../master/server/AliasList.o
BGM_OBJS += ../master/server/Behavior.o
BGM_OBJS += ../master/server/ClientController.o
BGM_OBJS += ../master/server/ClientManager.o
BGM_OBJS += ../master/server/DBUpdater.o
BGM_OBJS += ../master/common/Ids.o
BGM_OBJS += ../master/server/MasterController.o
BGM_OBJS += ../master/common/Protocol.o
BGM_OBJS += ../master/server/Registrar.o

BGM_OBJS += ../master/protocol/BGMasterAgentProtocolSpec.o
BGM_OBJS += ../master/protocol/BGMasterClientProtocolSpec.o

MMCS_OBJS  = ../mmcs/common/Properties.o
MMCS_OBJS += ../mmcs/common/Subnet.o

INSTALL_LIST += $(call INSTALL_BIN_TARGET, $(EXECS))

bg_bin_links := $(EXECS)

.PHONY: clean docs install distclean all 

all: $(EXECS) test

install: $(INSTALL_LIST) $(bg_links_targets) test

clean:			
	$(RM) $(INSTALL_LIST)
	$(RM) *.o
	$(RM) *.d
	$(RM) $(EXECS)

distclean:	clean

properties_validate: properties_validate.o $(BGM_OBJS) $(MMCS_OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

test: properties_validate
	./$^ $(BGQ_INSTALL_DIR)/utility/etc/bg.properties.ship.tpl

-include *.d
