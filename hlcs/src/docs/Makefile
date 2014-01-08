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
include ../../Make.hlcs.rules

all:

docs: doxygen.out


clean:
	rm -f doxygen.out


distclean: clean


install: docs


uninstall:
	rm -rf $(BGQ_INSTALL_DIR)/hlcs/docs/overview


docs: doxygen.out


doxygen.out: Doxyfile
	$(MKDIR) -p $(BGQ_INSTALL_DIR)/hlcs/docs/overview
	cd ../../ && cat src/docs/$^ | sed 's:BGQ_INSTALL_DIR:$(BGQ_INSTALL_DIR):' | doxygen - &> src/docs/doxygen.out
