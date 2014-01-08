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

include ../../../Make.rules

#IDLC = ../../src/xidl/xidlMain.py
IDLC = ../../include/scripts/generateCode.py

INSTALL_DIR=$(BGQ_INSTALL_DIR)/xml/include/c_api
XML_DEFS_DIR=../../src/defs
XML_DEFS = $(wildcard $(XML_DEFS_DIR)/*.xml)
C_API_H = $(addsuffix .h, $(basename $(notdir $(XML_DEFS))))
INSTALL_TARGETS = $(addprefix $(BGQ_INSTALL_DIR)/xml/include/c_api/, $(C_API_H))

CXXFLAGS += -I$(BGQ_INSTALL_DIR)
CXXFLAGS += $(DEBUG_FLAGS)

default: install

install: $(INSTALL_DIR) $(INSTALL_TARGETS)

$(INSTALL_DIR):
	$(MKDIR) -p $(INSTALL_DIR)

$(INSTALL_TARGETS): $(C_API_H) | $(INSTALL_DIR)
	$(CP) $(notdir $@) $@

$(C_API_H): 

%.h: $(XML_DEFS_DIR)/%.xml
#	$(IDLC) --cheader ../../src/defs/$(basename $@).xml > $@
	$(IDLC) --cheader ../../src/defs/$(basename $@).xml

clean:
	$(RM) *.h

distclean: clean
