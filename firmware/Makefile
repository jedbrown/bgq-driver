# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
#  --------------------------------------------------------------- 
#                                                                  
# Licensed Materials - Property of IBM                             
# Blue Gene/Q                                                      
# (C) Copyright IBM Corp.  2010, 2012                              
# US Government Users Restricted Rights - Use, duplication or      
#   disclosure restricted by GSA ADP Schedule Contract with IBM    
#   Corp.                                                          
#                                                                  
# This software is available to you under the Eclipse Public       
# License (EPL).                                                   
#                                                                  
#  --------------------------------------------------------------- 
#                                                                  
# end_generated_IBM_copyright_prolog                               
#

include ../Make.rules

OS := $(shell uname)
OS_ARCH := $(shell uname -m)


DIRS = 
DIRS += include

# for aix, we ONLY care about the includes
ifneq ($(OS),AIX)
DIRS += src
DIRS += tools

# ------------------------------------------------------------------------------------
# NOTE: skip over the ras subcomponent if we are doing this special tool-chain-only
#       build for Toronto (which is only done on grotius).
# ------------------------------------------------------------------------------------

ifneq ($(HOSTNAME),grotius)
DIRS += ras
endif

DIRS += tests
endif


ifeq ($(BGQ_CONTROL_SYSTEM_SIMULATION),yes)
DIRS =
DIRS += include
DIRS += tools
endif


BEAM_RESULTS += --beam::ipa=all --beam::data=$(BGQ_INSTALL_DIR)/firmware/beam

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

# dependencies
src.install:: include.install
tests.install:: src.install

#
# dependency ordering...
# this must occure BEFORE the more general rule that provdides the commands
# to do the submake

$(BUILD_DIRS):: $(BUILD_LIBS)

# make the all also do an install so the top level default make works..
all: install

# +++
#
# ---

#all:		this.all	$(TARGETS_ALL)

# ras_scan

ifeq ($(OS),AIX)
install:	this.install	$(TARGETS_INSTALL)
else
install:	this.install	$(TARGETS_INSTALL) ras_scan $(call TLD_INSTALL_PACKAGING_TARGET, firmware.specsrc) 
	-@${LN} -sf ../firmware/bin/bgq_firmware.elf $(BGQ_INSTALL_DIR)/boot/firmware
endif

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

test:
	$(MAKE) --directory tests test


# +++
#	Targets to be executed in the current directory
# ---

this.all:
	@echo "making all"

this.install:
	@echo "Firmware: making install on host "$(HOSTNAME)

this.clean:
	@echo "making clean"
	rm -f *~

this.distclean: this.clean
	@echo "making distclean"

ras_scan:
	$(MKDIR) -p $(BGQ_INSTALL_DIR)/firmware/ras/metadata/
	$(BGQ_BUILD_DIR)/scripts/rasScan.py $(BGQ_BUILD_DIR)/firmware/src $(BGQ_INSTALL_DIR)/firmware/ras/metadata firmware_ras.xml $(BGQ_INSTALL_DIR)/ras/include/ras.xml

print:
	@echo TARGETS_ALL=${TARGETS_ALL}
	@echo TARGETS_INSTALL=${TARGETS_INSTALL}
	@echo BUILD_DIRS=${BUILD_DIRS}
	@echo TARGETS=$(TARGETS)
	@echo DIRS=$(DIRS)

