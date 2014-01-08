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

OS	= $(shell uname)


DIRS = $(subst /Makefile,,$(wildcard */Makefile))

TARGETS_ALL		= $(addsuffix .all,$(DIRS))
TARGETS_INSTALL		= $(addsuffix .install,$(DIRS))
TARGETS_CLEAN		= $(addsuffix .clean,$(DIRS))
TARGETS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))

TARGETS = $(TARGETS_ALL)       \
	  $(TARGETS_INSTALL)   \
	  $(TARGETS_CLEAN)     \
	  $(TARGETS_DISTCLEAN)



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
install:	this.install	$(TARGETS_INSTALL) this.examples docs-external ras_scan $(call TLD_INSTALL_PACKAGING_TARGET, spi.specsrc)
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

docs-external:
	$(RM) -rf doc/html
	$(RM) -rf $(BGQ_INSTALL_DIR)/spi/doc
	$(MKDIR) -p $(BGQ_INSTALL_DIR)/spi/doc
	$(CP)  doc/README $(BGQ_INSTALL_DIR)/spi/doc
	doxygen doxygen_external.config
	$(CP) -r doc/html $(BGQ_INSTALL_DIR)/spi/doc/.


# +++
#	Targets to be executed in the current directory
# ---

this.all:
	@echo "making all"

this.install:
	@echo "making install"

this.clean:
	@echo "making clean"
	$(RM) -f *~
	$(RM) -rf doc/html

this.distclean: this.clean
	@echo "making distclean"
	$(RM) -rf $(BGQ_INSTALL_DIR)/spi/examples
	$(RM) -rf $(BGQ_INSTALL_DIR)/spi/doc

# The entire "examples" directory tree is installed as is.
# The example directories are meant to be copied to user directories so
# "make" can be run there.
# The Makefiles in the examples directory tree are not processed here; they are
# just there for the user to use to build the examples.
this.examples:
	@echo "making examples"
	$(RM)    -rf $(BGQ_INSTALL_DIR)/spi/examples
	$(MKDIR) -p $(BGQ_INSTALL_DIR)/spi/examples/mu
	$(CP)    $(BGQ_BUILD_DIR)/spi/examples/mu/* $(BGQ_INSTALL_DIR)/spi/examples/mu/.

ifneq ($(HOSTNAME),grotius)

ras_scan:
	$(MKDIR) -p $(BGQ_INSTALL_DIR)/spi/ras/metadata/
	$(BGQ_BUILD_DIR)/scripts/rasScan.py $(BGQ_BUILD_DIR)/spi/ $(BGQ_INSTALL_DIR)/spi/ras/metadata spi_ras.xml $(BGQ_INSTALL_DIR)/ras/include/ras.xml
else

ras_scan:
	echo "(W) ras_scan skipped on this build."

endif
