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
# (C) Copyright IBM Corp.  2010, 2012                              
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


DIRS = \
 common \
 console \
 server \
 lite \
 docs


DIR_TARGETS_ALL		= $(addsuffix .all,$(DIRS))
DIR_TARGETS_INSTALL	= $(addsuffix .install,$(DIRS))
DIR_TARGETS_CLEAN	= $(addsuffix .clean,$(DIRS))
DIR_TARGETS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))
DIR_TARGETS_UNINSTALL	= $(addsuffix .uninstall,$(DIRS))


DIR_TARGETS  = $(DIR_TARGETS_ALL)
DIR_TARGETS += $(DIR_TARGETS_INSTALL)
DIR_TARGETS += $(DIR_TARGETS_CLEAN)
DIR_TARGETS += $(DIR_TARGETS_DISTCLEAN)
DIR_TARGETS += $(DIR_TARGETS_UNINSTALL)


all: default $(DIR_TARGETS_ALL)

default: $(TARGETS) $(INSTALL_LIST)


$(DIR_TARGETS):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))


ras_scan_files := $(shell find . -name "*.cc" -or -name "*.h")

ras_scan: $(ras_scan_files)
	$(BGQ_BUILD_DIR)/scripts/rasScan.py '$(BGQ_BUILD_DIR)/hlcs/src/mmcs' '$(BGQ_INSTALL_DIR)/hlcs/ras/metadata' 'mmcs_ras.xml' '$(BGQ_INSTALL_DIR)/ras/include/ras.xml'
	@touch $@


# SUBDIR DEPENDENCIES

server.all: common.all
server.install: common.install

console.all: common.all 
console.install: common.install

lite.all: common.all console.all server.all
lite.install: common.install console.install server.install


install: $(INSTALL_LIST) ras_scan $(DIR_TARGETS_INSTALL)


clean:  $(DIR_TARGETS_CLEAN)
	$(RM) ras_scan


distclean: $(DIR_TARGETS_DISTCLEAN)
	$(RM) ras_scan


uninstall: $(DIR_TARGETS_UNINSTALL)


.PHONY: default all install clean distclean uninstall
