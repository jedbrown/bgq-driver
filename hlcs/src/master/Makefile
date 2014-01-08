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


DIRS = $(subst /Makefile,,$(wildcard */Makefile))



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

.PHONY: $(DIR_TARGS) clean docs install distclean all 

all: $(DIR_TARGS_ALL)

install: $(DIR_TARGS_INSTALL)

clean: $(DIR_TARGS_CLEAN)

distclean: clean

$(DIR_TARGS):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))


common.all: protocol.all 
common.install: protocol.install 
agent.all: common.all
agent.install: common.install
server.all: common.all
server.install: common.install
lib.all: common.all
lib.install: common.install
command.all: lib.all
command.install: lib.install
docs.all: protocol.all 
docs.install: protocol.install
test.all: common.all
test.install: common.install

vars:
	echo $(DIR_TARGS_ALL) $(DIR_TARGS_CLEAN) dt $(DIR_TARGS)

