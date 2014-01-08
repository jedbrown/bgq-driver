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

DIRS  = $(subst /Makefile,,$(wildcard */Makefile))

ifeq ($(BGQ_HOST_OS),x86_64)
# test directory has a dependency on boost not installed on x86 processors
DIRS    := $(filter-out test,$(DIRS))
endif


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


doxygen_version := $(subst 1.7,1.6,$(findstring 1.6, $(shell doxygen --version))$(findstring 1.7, $(shell doxygen --version)))
required_doxygen_version := 1.6


.PHONY: default all install clean distclean uninstall docs $(TARGETS)


# make the all also do an install so the top level default make works
default:    install
all:        install


install:		$(TARGETS_INSTALL) docs $(call TLD_INSTALL_PACKAGING_TARGET, utility.specsrc)
clean:			$(TARGETS_CLEAN) docs.clean
distclean:	clean   $(TARGETS_DISTCLEAN)
uninstall:              $(TARGETS_UNINSTALL)


# dependencies
src.install::  include.install
test.install:: etc.install src.install


$(TARGETS)::
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))

docs.clean:
	$(RM) doxygen.out


doxygen.out:
ifeq ($(doxygen_version),$(required_doxygen_version))
	@echo "generating documentation"
	$(MKDIR) -p $(BGQ_INSTALL_DIR)/utility/docs
	$(CP) docs/verbose.html $(BGQ_INSTALL_DIR)/utility/docs
	$(CP) docs/verbose.man $(BGQ_INSTALL_DIR)/utility/docs
	cat docs/Doxyfile | sed 's:!OUTPUT_DIR!:$(BGQ_INSTALL_DIR)/utility/docs:' | doxygen - > $@
	sed -e 's^@DRIVER@^$(BGQ_INSTALL_DIR)^' < docs/main_index.html > $(BGQ_INSTALL_DIR)/utility/docs/main_index.html
else
	@echo "Will not generate docs because doxygen version is" $(shell doxygen --version) "and required version is" $(required_doxygen_version)
	@touch $@
endif


docs: doxygen.out

