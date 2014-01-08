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

include ../Make.db.rules
include ../../Make.rules


default: all

all: install


$(BGQ_INSTALL_DIR)/$(tld)/bin/generateSqlStrings.py: generateSqlStrings.py
	$(MKDIR) -p $(dir $@)
	$(CP) $< $@


install: $(BGQ_INSTALL_DIR)/db/bin/generateSqlStrings.py
	$(MAKE) -C api $@
	$(MAKE) --ignore-errors -C docs $@

clean distclean:
	$(MAKE) -C api $@
	$(MAKE) --ignore-errors -C docs $@


.PHONY: default all install clean distclean
