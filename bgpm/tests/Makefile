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
OS	= $(shell uname)


ALLDIRS = $(subst /Makefile,,$(wildcard */Makefile))

# Exclude directories in filter-out list
# Don't know proper way to find XLC compiler yet - so let's leave these tests of 
#  the default build.
DIRS = $(filter-out test_utils experiment se-tm,$(ALLDIRS))


TARGETS_ALL			= $(addsuffix .all,$(DIRS))
TARGETS_INSTALL		= $(addsuffix .install,$(DIRS))
TARGETS_CLEAN		= $(addsuffix .clean,$(DIRS))
TARGETS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))

TARGETS = $(TARGETS_ALL)       \
	  $(TARGETS_INSTALL)   \
	  $(TARGETS_CLEAN)     \
	  $(TARGETS_DISTCLEAN)
	  
	  
LOCAL_LIBS = test_utils
LIB_ALL			= $(addsuffix .all,$(LOCAL_LIBS))
LIB_INSTALL		= $(addsuffix .install,$(LOCAL_LIBS))
LIB_CLEAN		= $(addsuffix .clean,$(LOCAL_LIBS))
LIB_DISTCLEAN	= $(addsuffix .distclean,$(LOCAL_LIBS))
LIB_TARGETS = $(LIB_ALL)       \
	$(LIB_INSTALL)   \
	$(LIB_CLEAN)     \
	$(LIB_DISTCLEAN)



# +++
#
# ---

.SUFFIXES:

.PHONY: default all install realinstall clean dist_clean $(TARGETS) $(LIB_TARGETS)

default:	install

# make the all also do an install so the top level default make works..
all: install

# +++
#
# ---

#all:		this.all		$(TARGETS_ALL)
install:	this.install
realinstall:	this.realinstall	$(TARGETS_INSTALL)
clean:		this.clean		$(TARGETS_CLEAN)
distclean:	this.distclean	$(TARGETS_DISTCLEAN)

$(TARGETS_ALL):			$(LIB_ALL)
$(TARGETS_INSTALL): 	$(LIB_INSTALL)
$(TARGETS_CLEAN):		$(LIB_CLEAN)
$(TARGETS_DISTCLEAN): 	$(LIB_DISTCLEAN)


# +++
#	Each element in "$TARGETS" is of the form:
#		subdirectory.target
#	"$(basename $@)" picks off "subdirectory"
#	"$(subst .,,$(suffix $@))" picks off "target"
# ---

$(TARGETS) $(LIB_TARGETS):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))


docs:
	doxygen doxygen_fullsource.config


# +++
#	Targets to be executed in the current directory
# ---

this.all:
	@echo "making all"

this.install:
	@echo "making install"

this.realinstall:
	@echo "making realinstall"

this.clean:
	@echo "making clean"
	rm -f *~

this.distclean: this.clean
	@echo "making distclean"


