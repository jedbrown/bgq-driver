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
#
# Make rules to install include files....
#

include ../../Make.rules

$(eval $(call bgq_header_make, ras/include))

default:	install

all: install


$(BGQ_INSTALL_DIR)/ras/include/%: %
	$(call bgq_install_bin,$<,$@)

install: $(TARGET_HDRS) $(BGQ_INSTALL_DIR)/ras/include/ras.xml

clean:	
	$(cleancmd)

distclean:	clean
	$(RM) -r $(BGQ_INSTALL_DIR)/ras/include


none:



