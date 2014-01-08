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
# (C) Copyright IBM Corp.  2011, 2012                              
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
include ../Make.rules

docs_install_dir=$(BGQ_INSTALL_DIR)/ramdisk/docs/services

default:

all:

docs: Doxyfile

clean:
	$(RM) -rf $(docs_install_dir)

distclean: clean

install: docs

docs: Doxyfile
	$(MKDIR) -p $(BGQ_INSTALL_DIR)/ramdisk/docs/services
	sed -e s^@OUTPUT_DIR@^$(docs_install_dir)^ Doxyfile | doxygen - &> doxygen.out

