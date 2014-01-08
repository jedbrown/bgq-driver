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


install_dir := $(BGQ_INSTALL_DIR)/$(tld)/doc/capena-http

generated_file := $(install_dir)/html/index.html


default: all


$(generated_file): Doxyfile ../http/*.cc ../http/*.hpp ../http/uri/*.cc ../http/uri/*.hpp ../server/*.cc ../server/*.hpp
	$(MKDIR) -p $(install_dir)
	cat $< | sed -e "s^@INSTALL_DIR@^$(install_dir)^" | doxygen - > /dev/null 2>&1


all install: $(generated_file)


clean distclean:


uninstall:
	$(RM) -r $(install_dir)


.PHONY: default all install clean distclean uninstall
