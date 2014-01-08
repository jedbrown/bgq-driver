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

include ../../Make.bgws.rules
include ../../../Make.rules


CXXFLAGS += \
 -isystem $(BGQ_INSTALL_DIR) \
 -isystem $(BGQ_INSTALL_DIR)/extlib/include

LDFLAGS += \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib \
 -lboost_program_options-mt \
 -lboost_system-mt


default all: checkUserAdmin


checkUserAdmin: checkUserAdmin.o
	${cxxlink}

$(BGQ_LIBEXEC_INSTALL_DIR)/checkUserAdmin: checkUserAdmin
	$(MKDIR) -p $(BGQ_LIBEXEC_INSTALL_DIR)
	$(CP) $< $@
	@echo "sudo chown root:bgqadmin $@"
	@echo "sudo chmod 4755 $@"

clean distclean:
	$(RM) *.o *.d checkUserAdmin

install: $(BGQ_LIBEXEC_INSTALL_DIR)/checkUserAdmin

uninstall:
	$(RM) $(BGQ_LIBEXEC_INSTALL_DIR)/checkUserAdmin

test:


.PHONY: default all test install clean distclean uninstall


-include *.d
