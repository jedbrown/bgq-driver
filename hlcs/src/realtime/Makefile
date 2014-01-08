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


include ../../../Make.rules
include ../../Make.hlcs.rules

dirs :=\
 server \
 utilities \
 docs


dirs_install := ${dirs:%=%.install}
dirs_uninstall := ${dirs:%=%.uninstall}
dirs_clean := ${dirs:%=%.clean}
dirs_distclean := ${dirs:%=%.distclean}

init_script_installed := $(BGQ_INSTALL_DIR)/hlcs/etc/init/realtime_server


%.install:
	$(MAKE) -C ${@:%.install=%} install

%.uninstall:
	$(MAKE) -C ${@:%.uninstall=%} uninstall

%.clean:
	$(MAKE) -C ${@:%.clean=%} clean

%.distclean:
	$(MAKE) -C ${@:%.distclean=%} distclean


default: all

all: $(dirs) $(init_script_installed)


server:
	$(MAKE) -C $@

utilities:
	$(MAKE) -C $@

docs:
	$(MAKE) -C $@


$(init_script_installed): realtime_server
	mkdir -p $(dir $@)
	$(CP) $< $@



install: $(dirs_install) $(init_script_installed)

uninstall: $(dirs_uninstall)

clean: $(dirs_clean)

distclean: $(dirs_distclean)


.PHONY: default all install clean distclean $(dirs)
	
