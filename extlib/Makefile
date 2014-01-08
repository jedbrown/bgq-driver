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
include ../Make.rules

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


.PHONY: $(DIRS) clean docs install distclean


default:        install
all:            install $(TARGETS_ALL)
install:		$(TARGETS_INSTALL) docs $(call TLD_INSTALL_PACKAGING_TARGET, extlib.specsrc)
clean:			$(TARGETS_CLEAN) docs.clean
distclean:	clean   $(TARGETS_DISTCLEAN)
uninstall:		$(TARGETS_UNINSTALL)


$(TARGETS)::
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))

docs.clean:
	$(RM) doxygen.out

doxygen.out:
	@echo "generating documentation"
	$(MKDIR) -p $(BGQ_INSTALL_DIR)/extlib/docs
	cat docs/Doxyfile | sed 's:!OUTPUT_DIR!:$(BGQ_INSTALL_DIR)/extlib/docs:' | doxygen - > $@

docs: doxygen.out
