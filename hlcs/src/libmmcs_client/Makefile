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


include ../../Make.hlcs.rules
include ../../../Make.rules


CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -fPIC
CXXFLAGS += -MD -MP
CXXFLAGS += -Wall -Wextra

LDFLAGS += -shared
LDFLAGS += -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility -lcxxsockets -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib


lib_name := mmcs_client

dev_name := lib$(lib_name).so
short_name := $(dev_name).1
full_name := $(short_name).0.0


parts := \
 CommandReply \
 ConsolePort \
 utility

objs := ${parts:%=%.o}


all_targets := libmmcs_client.so.1.0.0 $(short_name) $(dev_name)

install_targets := ${all_targets:%=$(BGQ_INSTALL_DIR)/hlcs/lib/%}

install_dir = $(BGQ_INSTALL_DIR)/hlcs/lib


$(install_dir)/%: %
	$(MKDIR) $(install_dir)
	$(CP) $< $(install_dir)/


default: all


all: $(all_targets)


$(full_name): $(objs)
	$(CXX) $(CXXFLAGS) -Wl,-soname,$(short_name) -o $@ $^ $(LDFLAGS)

$(short_name): $(full_name)
	$(LN) -sf $(full_name) $@

$(dev_name): $(short_name)
	$(LN) -sf $(short_name) $@


install: $(install_targets)


clean:
	$(RM) *.o *.d $(all_targets)


distclean:
	$(RM) *.o *.d $(all_targets)


.PHONY: default all install clean distclean

-include *.d
