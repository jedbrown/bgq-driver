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
                             
include ../../../../Make.rules

TOP_DIR=control

# Location of the python scripts to build headers
XMLSCRIPT_DIR=$(BGQ_INSTALL_DIR)/xml/scripts

INSTALL_DIR=$(BGQ_INSTALL_DIR)/hlcs/include/c_api
IDLC = $(XMLSCRIPT_DIR)/generateCode.py --dir $(INSTALL_DIR)
XML_DEFS = $(wildcard *.xml)
C_API_H = $(addsuffix .h, $(basename $(notdir $(XML_DEFS))))
INSTALL_TARGETS = $(addprefix $(BGQ_INSTALL_DIR)/hlcs/include/c_api/, $(C_API_H))

default: all install

$(INSTALL_DIR):
	$(MKDIR) -p $(INSTALL_DIR)

$(INSTALL_TARGETS): $(C_API_H)
	$(CP) $(notdir $@) $@

install: $(INSTALL_DIR) $(INSTALL_TARGETS) $(C_API_H)

all: $(C_API_H) install

TEST_LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib

$(C_API_H): $(XML_DEFS)

%.h: %.xml
	$(IDLC) --cheader $(basename $@).xml


clean:
	rm -rf *o *o_64 *~ BGMaster*[cch] \#* $(INSTALL_TARGETS)
