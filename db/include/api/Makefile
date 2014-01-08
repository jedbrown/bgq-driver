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
# (C) Copyright IBM Corp.  2004, 2011                              
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
include ../../Make.db.rules


$(eval $(call bgq_header_make, $(tld)/include/api))


default: install

all: install


cxxdb.install:
	$(MAKE) -C cxxdb install
	
filtering.install:
	$(MAKE) -C filtering install

job.install:
	$(MAKE) -C job install

tableapi.install:
	$(MAKE) -C tableapi install


install: $(TARGET_HDRS) tableapi.install cxxdb.install filtering.install job.install


clean:
	$(MAKE) -C cxxdb $@
	$(MAKE) -C filtering $@
	$(MAKE) -C job $@
	$(MAKE) -C tableapi $@

distclean:
	$(MAKE) -C cxxdb $@
	$(MAKE) -C filtering $@
	$(MAKE) -C job $@
	$(MAKE) -C tableapi $@


.PHONY: default all install clean distclean tableapi.install cxxdb.install filtering.install job.install

