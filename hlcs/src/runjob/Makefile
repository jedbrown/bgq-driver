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

DIRS  = $(subst /Makefile,,$(wildcard */Makefile))

TARGETS_ALL		= $(addsuffix .all,$(DIRS))
TARGETS_INSTALL		= $(addsuffix .install,$(DIRS))
TARGETS_CLEAN		= $(addsuffix .clean,$(DIRS))
TARGETS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))
TARGETS_UNINSTALL	= $(addsuffix .uninstall,$(DIRS))


TARGETS  = $(TARGETS_ALL)
TARGETS += $(TARGETS_INSTALL)
TARGETS += $(TARGETS_CLEAN)
TARGETS += $(TARGETS_DISTCLEAN)
TARGETS += $(TARGETS_UNINSTALL)


.PHONY: install all default clean distclean test $(TARGETS) $(DIRS)


all:            $(TARGETS_ALL)
install:        $(TARGETS_INSTALL) 
clean:          $(TARGETS_CLEAN)
distclean:      $(TARGETS_DISTCLEAN)
uninstall:      $(TARGETS_UNINSTALL)


# dependencies
samples.all: common.all client.all
samples.install: common.install client.install
client.all: common.all
client.install: common.install
server.all: common.all
server.install: common.install
mux.all: common.all
mux.install: common.install
test.all: client.all mux.all server.all
test.install: client.install mux.install server.install
docs.install: common.install
docs.all: common.all

$(TARGETS):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))

