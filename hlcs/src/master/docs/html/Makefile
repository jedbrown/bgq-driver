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

include ../../../../../Make.rules
include ../../../../Make.hlcs.rules

all:

docs: Doxyfile

clean:
	rm -f doxygen.out


distclean: clean

INSTALL_DIR = $(BGQ_INSTALL_DIR)/hlcs/docs/master

IMAGE = Alias_etc.png

$(INSTALL_DIR):
	$(MKDIR) -p $@

$(INSTALL_DIR)/%: % | $(INSTALL_DIR)
	$(CP) $^ $@

install: docs $(INSTALL_DIR)/$(IMAGE)

uninstall:
	rm -rf $(BGQ_INSTALL_DIR)/hlcs/docs/master

doxygen_version := $(subst 1.7,1.6,$(findstring 1.6, $(shell doxygen --version))$(findstring 1.7, $(shell doxygen --version)))
required_doxygen_version := 1.6

docs: doxygen.out

doxygen.out: Doxyfile | $(INSTALL_DIR)
ifeq ($(doxygen_version),$(required_doxygen_version))
	@echo "generating documentation"
	cd ../../../../ && cat src/master/docs/html/$^ | sed 's:BGQ_INSTALL_DIR:$(BGQ_INSTALL_DIR):' | doxygen - > src/master/docs/html/doxygen.out
else
	@echo "Will not generate docs because doxygen version is" $(shell doxygen --version) "and required version is" $(required_doxygen_version)
	@touch $@
endif


