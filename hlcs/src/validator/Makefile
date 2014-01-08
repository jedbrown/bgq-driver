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

TOP_DIR=hlcs
TOP_INSTALL_DIR=$(BGQ_INSTALL_DIR)/$(TOP_DIR)
MODULE_INIT_DIR=$(TOP_INSTALL_DIR)/etc/init

EXECS = properties_validate

DIR_TARGS_ALL		= $(addsuffix .all,$(DIRS))
DIR_TARGS_INSTALL       = $(addsuffix .install,$(DIRS))
DIR_TARGS_CLEAN		= $(addsuffix .clean,$(DIRS))
DIR_TARGS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))

DIR_TARGS  = $(DIR_TARGS_ALL)
DIR_TARGS += $(DIR_TARGS_CLEAN)
DIR_TARGS += $(DIR_TARGS_DISTCLEAN)
DIR_TARGS += $(DIR_TARGS_INSTALL)

#CXXFLAGS += -gstabs
CXXFLAGS += -O2
CXXFLAGS += -Wall
CXXFLAGS += -MD
CXXFLAGS += -I $(BGQ_BUILD_DIR)/hlcs/src/master
CXXFLAGS += -I $(BGQ_INSTALL_DIR)
CXXFLAGS += -I $(BGQ_INSTALL_DIR)/bgq_util/include
CXXFLAGS += -I $(BGQ_INSTALL_DIR)/utility/include
CXXFLAGS += -I $(BGQ_INSTALL_DIR)/utility/include/cxxsockets
CXXFLAGS += -I $(BGQ_INSTALL_DIR)/ras/include
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/hlcs/include
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/db/include

LDFLAGS  += -lboost_date_time-mt
LDFLAGS  += -lboost_program_options-mt
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility
# # This should come out and we should only link to the XML.o binary to avoid 
# # sucking in all of the other xml interfaces.  Either that, or change the xml libs.
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/xml/lib -lXML
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/xml/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/ras/lib -lras
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/ras/lib
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib
LDFLAGS  += -Wl,-rpath,$(BGQ_LIB_INSTALL_DIR)

BGM_OBJS =
BGM_OBJS += ../master/common/AgentBase.o
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
BGM_OBJS += ../master/common/ThreadLog.o

BGM_OBJS += ../master/protocol/BGMasterAgentProtocolSpec.o
BGM_OBJS += ../master/protocol/BGMasterClientProtocolSpec.o

MMCS_OBJS  = ../mmcs/MMCSProperties.o
MMCS_OBJS += ../mmcs/MMCSSubnet.o

INSTALL_LIST += $(call INSTALL_BIN_TARGET, properties_validate)

bg_bin_links := properties_validate

.PHONY: $(DIR_TARGS) clean docs install distclean all build

all: $(DIR_TARGS_ALL) $(INSTALL_LIST) install build

$(MODULE_INIT_DIR):
	$(MKDIR) -p $@

$(MODULE_INIT_DIR)/%: % | $(MODULE_INIT_DIR)
	$(CP) $^ $@

install: build $(INSTALL_LIST) $(lib_dev) $(MODULE_INIT_DIR)/bgagent $(bg_links_targets)

clean:			$(DIR_TARGS_CLEAN)
			$(RM) $(INSTALL_LIST)
			$(RM) *.o *.o_64 *~ core* \#*
			$(RM) *.d
			$(RM) properties_validate

distclean:	clean

build: $(DIR_TARGS_ALL) properties_validate

$(DIR_TARGS):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))

properties_validate: properties_validate.o $(BGM_OBJS) $(MMCS_OBJS) | $(DIR_TARGS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) -lpthread -lcxxsockets	
