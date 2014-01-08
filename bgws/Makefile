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
# (C) Copyright IBM Corp.  2010, 2011                              
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
include Make.bgws.rules


dirs :=\
 src


targets_all := $(addsuffix .all,$(dirs))
targets_install := $(addsuffix .install,$(dirs))
targets_clean := $(addsuffix .clean,$(dirs))
targets_distclean := $(addsuffix .distclean,$(dirs))
targets_uninstall := $(addsuffix .uninstall,$(dirs))

targets := \
 $(targets_all) \
 $(targets_install) \
 $(targets_clean) \
 $(targets_distclean) \
 $(targets_uninstall)

installed_bgws_conf := $(bgq_tld_install_dir)/etc/bgws.conf


# Only build if --with-db (BGQDB==yes)

ifeq ($(BGQDB),yes)
install_targets := $(targets_install) $(call TLD_INSTALL_PACKAGING_TARGET, bgws.specsrc) $(installed_bgws_conf)
else
install_targets :=
endif

default: all

all: install


$(targets):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))

$(installed_bgws_conf): bgws.conf
	$(MKDIR) -p $(dir $@)
	$(CP) $< $@


install: $(install_targets)

clean: $(targets_clean)

distclean: $(targets_distclean)

uninstall: $(targets_uninstall)


.PHONY: default all install clean distclean uninstall $(targets)

