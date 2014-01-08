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

dirs :=\
 server \
 utilities \
 docs


dirs_install := ${dirs:%=%.install}
dirs_uninstall := ${dirs:%=%.uninstall}
dirs_clean := ${dirs:%=%.clean}
dirs_distclean := ${dirs:%=%.distclean}


%.install:
	$(MAKE) -C ${@:%.install=%} install

%.uninstall:
	$(MAKE) -C ${@:%.uninstall=%} uninstall

%.clean:
	$(MAKE) -C ${@:%.clean=%} clean

%.distclean:
	$(MAKE) -C ${@:%.distclean=%} distclean


default: all

all: $(dirs)


server:
	$(MAKE) -C $@

utilities:
	$(MAKE) -C $@

docs:
	$(MAKE) -C $@


install: $(dirs_install)

uninstall: $(dirs_uninstall)

clean: $(dirs_clean)

distclean: $(dirs_distclean)


.PHONY: default all install clean distclean $(dirs)
	
