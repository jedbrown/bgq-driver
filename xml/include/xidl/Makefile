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
INSTALL_DIR=$(BGQ_INSTALL_DIR)/xml/xidl
PYSCRIPTS = $(wildcard *.py)
INSTALL_TARGETS = $(addprefix $(BGQ_INSTALL_DIR)/xml/xidl/, $(PYSCRIPTS))

$(INSTALL_DIR):
	$(MKDIR) -p $(INSTALL_DIR)

$(INSTALL_TARGETS): $(PYSCRIPTS) | $(INSTALL_DIR)
	$(CP) $(notdir $@) $@

install: $(INSTALL_DIR) $(INSTALL_TARGETS)

clean:
	$(RM) -r $(INSTALL_DIR)

distclean: clean
