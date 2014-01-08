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

include ../../../Make.rules

INSTALL_DIR=$(BGQ_INSTALL_DIR)/hlcs/include/master
HEADERS = $(wildcard *.h)
INSTALL_TARGETS = $(addprefix $(BGQ_INSTALL_DIR)/hlcs/include/master/, $(HEADERS))

DIRS	                = protocol
DIR_TARGS_CLEAN		= $(addsuffix .clean,$(DIRS))
DIR_TARGS_ALL		= $(addsuffix .all,$(DIRS))
DIR_TARGS               = $(DIR_TARGS_ALL)

default: all install

$(INSTALL_DIR):
	$(MKDIR) -p $(INSTALL_DIR)

$(INSTALL_TARGETS): $(HEADERS) | $(INSTALL_DIR)
	$(CP) $(notdir $@) $@

$(DIR_TARGS):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))

$(DIR_TARGS_CLEAN):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))

install: $(HEADERS) $(DIR_TARGS) $(INSTALL_DIR) $(INSTALL_TARGETS) $(C_API_H) 

all: install

clean:	$(DIR_TARGS_CLEAN)
	rm -rf *o *o_64 *~ \#* $(INSTALL_TARGETS)

distclean: clean
