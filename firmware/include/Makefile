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
# Make rules to install include files....
#

include ../../Make.rules

$(eval $(call bgq_header_make, firmware/include))

DIRS = fwext

TARGETS_ALL		= $(addsuffix .all,$(DIRS))
TARGETS_INSTALL		= $(addsuffix .install,$(DIRS))
TARGETS_CLEAN		= $(addsuffix .clean,$(DIRS))
TARGETS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))

TARGETS = $(TARGETS_ALL)       \
	  $(TARGETS_INSTALL)   \
	  $(TARGETS_CLEAN)     \
	  $(TARGETS_DISTCLEAN)

default:	install

all:	this.all $(TARGETS_ALL)

this.all: install


install: this.install $(TARGETS_INSTALL) checksum

this.install:	$(TARGET_HDRS)

clean:	this.clean $(TARGETS_CLEAN)

this.clean:	
	$(cleancmd)

distclean:	this.distclean $(TARGETS_DISTCLEAN)

this.distclean:	clean


# +------------------------------------------------------------------------------------+
# | NOTE: The control system is sensitive to changes to some firmware header files     |
# |       (e.g. personality.h).  This is due largely to serializatoin/deserialization  |
# |       issues.  In the absence of them generating their ser/deser code, we will     |
# |       monitor the sensitive files here via a MD5 checksum.  When making changes,   |
# |       you will need to regenerate the checksum (use the genchecksum target below). |
# +------------------------------------------------------------------------------------+

checksum:	
	md5sum -c firmware.md5

genchecksum:
	md5sum personality.h > firmware.md5

none:

# +++
#	Each element in "$TARGETS" is of the form:
#		subdirectory.target
#	"$(basename $@)" picks off "subdirectory"
#	"$(subst .,,$(suffix $@))" picks off "target"
# ---

$(TARGETS)::
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))


