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


DIRS += protocol
DIRS += common
DIRS += agent
DIRS += server
DIRS += lib
DIRS += command
DIRS += docs



DIR_TARGS_ALL		= $(addsuffix .all,$(DIRS))
DIR_TARGS_INSTALL       = $(addsuffix .install,$(DIRS))
DIR_TARGS_CLEAN		= $(addsuffix .clean,$(DIRS))
DIR_TARGS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))

DIR_TARGS  = $(DIR_TARGS_ALL)
DIR_TARGS += $(DIR_TARGS_CLEAN)
DIR_TARGS += $(DIR_TARGS_DISTCLEAN)
DIR_TARGS += $(DIR_TARGS_INSTALL)

PROTOCOL_OBJS  = protocol/BGMasterAgentProtocolSpec.o
PROTOCOL_OBJS += protocol/BGMasterClientProtocolSpec.o

INSTALL_LIST += $(call INSTALL_SBIN_TARGET, startagents.py)


bg_sbin_links := \
 bgmaster_server \
 bgagentd \
 master_start \
 master_stop \
 fail_over \
 bgmaster_server_refresh_config \
 bgmaster_server_log_level\

bg_bin_links := \
 alias_wait \
 binary_status \
 binary_wait \
 list_agents \
 list_clients \
 master_status \
 monitor_master \
 get_errors \
 get_history

.PHONY: $(DIR_TARGS) clean docs install distclean all build

all: $(DIR_TARGS_ALL)

install: build ras_scan $(call INSTALL_SBIN_TARGET, startagents.py)

clean:			$(DIR_TARGS_CLEAN)

distclean:	clean

build: $(DIR_TARGS_ALL)


$(DIR_TARGS):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))


common.all: protocol.all ras_scan
agent.all: common.all
server.all: common.all
lib.all: common.all
command.all: lib.all
docs.all: protocol.all ras_scan


vars:
	echo $(DIR_TARGS_ALL) $(DIR_TARGS_CLEAN) dt $(DIR_TARGS)

ras_scan: MasterRasMetadata.h
	$(BGQ_BUILD_DIR)/scripts/rasScan.py '$(BGQ_BUILD_DIR)/hlcs/src/master' '$(BGQ_INSTALL_DIR)/hlcs/ras/metadata' 'bgmaster_ras.xml' '$(BGQ_INSTALL_DIR)/ras/include/ras.xml'
	@touch $@
