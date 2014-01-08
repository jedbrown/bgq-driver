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
include ../../../Make.rules

default:

all:

docs: html

clean:
	$(RM) -rf $(BGQ_INSTALL_DIR)/bgpm/docs/html

distclean: clean

install: docs

html: Doxyfile
	$(MKDIR) -p $(BGQ_INSTALL_DIR)/bgpm/docs/html
	cd ../../../ && cat bgpm/docs/html/$^ | sed 's:BGQ_INSTALL_DIR:$(BGQ_INSTALL_DIR):' | doxygen - &> $(BGQ_INSTALL_DIR)/bgpm/docs/html/doxygen.html.out
