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

$(eval $(call bgq_header_make, hlcs/include/runjob))

.PHONY: default all install clean distclean none

SUBDIRS = commands

default:	install

all: install


install: $(TARGET_HDRS)
	@for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done


clean:	
	$(cleancmd)
	@for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done


distclean:	clean

none:

