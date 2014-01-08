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
include ../../../Make.bgws.rules


default: all


all: doxygen.out


doxygen.out: Doxyfile ${wildcard ../*.cc ../*.hpp}
	doxygen > $@


install: doxygen.out
	$(MKDIR) -p $(BGQ_INSTALL_DIR)/$(tld)/doc/chiron-json
	$(CP) -r html chiron-json.tagfile $(BGQ_INSTALL_DIR)/$(tld)/doc/chiron-json/


clean:
	$(RM) -r html doxygen.out


distclean: clean


uninstall:
	$(RM) -r $(BGQ_INSTALL_DIR)/$(tld)/doc/chiron-json/


.PHONY: default all install clean distclean uninstall
