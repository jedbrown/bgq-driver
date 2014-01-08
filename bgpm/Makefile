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
# (C) Copyright IBM Corp.  2012, 2012                              
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


OS	= $(shell uname)

ALLDIRS = $(subst /Makefile,,$(wildcard */Makefile))
DIRS = $(filter-out examples,$(ALLDIRS))

TARGETS_ALL		= $(addsuffix .all,$(DIRS))
TARGETS_INSTALL		= $(addsuffix .install,$(DIRS))
TARGETS_CLEAN		= $(addsuffix .clean,$(DIRS))
TARGETS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))

TARGETS = $(TARGETS_ALL)       \
	  $(TARGETS_INSTALL)   \
	  $(TARGETS_CLEAN)     \
	  $(TARGETS_DISTCLEAN)


# The directory to install the Bgpm RAS metadata.
BGPM_RAS_INSTALL_DIR = $(BGQ_INSTALL_DIR)/bgpm/ras/metadata


# +++
#
# ---

.SUFFIXES:

.PHONY: default all install clean dist_clean $(TARGETS)

default:	install

# make the all also do an install so the top level default make works..
all: install

# +++
#
# ---

#all:		this.all	$(TARGETS_ALL)
install:	this.install	$(TARGETS_INSTALL) ras_scan $(call TLD_INSTALL_PACKAGING_TARGET, bgpm.specsrc)
clean:		this.clean	$(TARGETS_CLEAN)
distclean:	this.distclean	$(TARGETS_DISTCLEAN)

# dependency to make sure include directory happens first..
$(filter-out include.install,$(TARGETS_INSTALL)):: include.install
$(filter-out include.all,$(TARGETS_ALL)):: include.all

# dependency to make sure src directory happens second...
$(filter-out include.install src.install,$(TARGETS_INSTALL)):: src.install
$(filter-out include.all src.all,$(TARGETS_ALL)):: src.all

# +++
#	Each element in "$TARGETS" is of the form:
#		subdirectory.target
#	"$(basename $@)" picks off "subdirectory"
#	"$(subst .,,$(suffix $@))" picks off "target"
# ---

$(TARGETS)::
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))

docs:
	doxygen doxygen_fullsource.config

$(BGPM_RAS_INSTALL_DIR):
	$(MKDIR) -p $(BGPM_RAS_INSTALL_DIR)


ras_scan: $(BGPM_RAS_INSTALL_DIR)
	$(BGQ_BUILD_DIR)/scripts/rasScan.py $(BGQ_BUILD_DIR)/bgpm/src $(BGPM_RAS_INSTALL_DIR) bgpm_ras.xml $(BGQ_INSTALL_DIR)/ras/include/ras.xml



# +++
#	Targets to be executed in the current directory
# ---

this.all:
	@echo "making all"

this.install:
	@echo "making install"

this.clean:
	@echo "making clean"
	rm -f *~

this.distclean: this.clean
	@echo "making distclean"


