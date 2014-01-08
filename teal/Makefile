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
include ../Make.rules

SUBDIRS=data
SUBDIRS+=etc
SUBDIRS+=ibm/bgq

define subdir_cmd
${MAKE} -C $(basename $@) $(subst .,,$(suffix $@))
endef

%.default:
	${MAKE} -C $(basename $@)

%.all %.install %.uninstall %.clean %.distclean:
	+$(subdir_cmd)

default: install

all: $(addsuffix .all,$(SUBDIRS))

install: $(addsuffix .install,$(SUBDIRS)) $(call TLD_INSTALL_PACKAGING_TARGET, teal.specsrc)

uninstall: $(addsuffix .uninstall,$(SUBDIRS))

clean: $(addsuffix .clean,$(SUBDIRS))

distclean: $(addsuffix .distclean,$(SUBDIRS))


.PHONY: default all install uninstall clean distclean
