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
# (C) Copyright IBM Corp.  2004, 2011                              
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

include ../../Make.rules

$(eval $(call bgq_header_make, hlcs/include))

DIRS = $(subst /Makefile,,$(wildcard */Makefile))

TARGETS_ALL	    	= $(addsuffix .all,$(DIRS))
TARGETS_INSTALL		= $(addsuffix .install,$(DIRS))
TARGETS_CLEAN		= $(addsuffix .clean,$(DIRS))
TARGETS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))

TARGETS  = $(TARGETS_ALL)
TARGETS += $(TARGETS_INSTALL)
TARGETS += $(TARGETS_CLEAN)
TARGETS += $(TARGETS_DISTCLEAN)

.PHONY: default all install clean distclean $(TARGETS)

default: install

all: 		$(TARGETS_ALL) install
install:	$(TARGETS_INSTALL) $(TARGET_HDRS)
distclean:	$(TARGETS_DISTCLEAN)
clean:		$(TARGETS_CLEAN)


$(TARGETS):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))


