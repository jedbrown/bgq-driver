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
# (C) Copyright IBM Corp.  2009, 2011                              
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
include ../Make.teal.rules
include ../../Make.rules

all: install

install: 
	$(MKDIR) -p $(TEAL_OPT_INSTALL_DIR)/teal/data
	rsync -r -v --exclude="- .svn" ibm $(TEAL_OPT_INSTALL_DIR)/teal/data/.

clean:
	$(RM) -r $(TEAL_OPT_INSTALL_DIR)/teal/data/*

distclean:	clean
