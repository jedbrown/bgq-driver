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

DIRS = 
DIRS += include
DIRS += src

TARGETS_ALL		= $(addsuffix .all,$(DIRS))
TARGETS_INSTALL		= $(addsuffix .install,$(DIRS))
TARGETS_CLEAN		= $(addsuffix .clean,$(DIRS))
TARGETS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))

TARGETS = $(TARGETS_ALL)       \
	  $(TARGETS_INSTALL)   \
	  $(TARGETS_CLEAN)     \
	  $(TARGETS_DISTCLEAN)

BUILD_LIBS :=  include.install
BUILD_DIRS := $(filter-out $(BUILD_LIBS),$(TARGETS_INSTALL))


# +++
#
# ---

.SUFFIXES:

.PHONY: default all install clean dist_clean $(TARGETS)

default:	install

#
# dependency ordering...
# this must occure BEFORE the more general rule that provdides the commands
# to do the submake

$(BUILD_DIRS):: $(BUILD_LIBS)

# make the all also do an install so the top level default make works..
all: install

src.install:: include.install

# +++
#
# ---

#all:		this.all	$(TARGETS_ALL)
install:	this.install	$(TARGETS_INSTALL) ras_scan $(call TLD_INSTALL_PACKAGING_TARGET, ras.specsrc)
clean:		this.clean	$(TARGETS_CLEAN)
distclean:	this.distclean	$(TARGETS_DISTCLEAN)



# +++
#	Each element in "$TARGETS" is of the form:
#		subdirectory.target
#	"$(basename $@)" picks off "subdirectory"
#	"$(subst .,,$(suffix $@))" picks off "target"
# ---

$(TARGETS)::
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))



# +++
#	Targets to be executed in the current directory
# ---

this.all:
	@echo "making all"

this.install:
	@echo "making install"

this.clean:
	@echo "making clean"
	$(RM) -r *~

this.distclean: this.clean
	@echo "making distclean"
	$(RM) -r $(BGQ_INSTALL_DIR)/ras

ras_scan:
	mkdir -p $(BGQ_INSTALL_DIR)/ras/ras/metadata
	$(BGQ_BUILD_DIR)/scripts/rasScan.py '$(BGQ_BUILD_DIR)/ras/include' '$(BGQ_INSTALL_DIR)/ras/ras/metadata' 'test_ras.xml' '$(BGQ_INSTALL_DIR)/ras/include/ras.xml'

