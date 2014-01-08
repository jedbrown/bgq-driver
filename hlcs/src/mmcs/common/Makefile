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


include ../../../Make.hlcs.rules
include ../../../../Make.rules


CXXFLAGS += \
 -O2 \
 -MD -MP \
 -Wall -Wextra \
 -DWITH_DB \
 -pthread \
 -iquote ../.. \
 -iquote ../../../include \
 -isystem $(BGQ_INSTALL_DIR) \
 -fpic


LDFLAGS += \
 -L../../libmmcs_client -lmmcs_client \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility -lcxxsockets \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib \
 -L $(BGQ_INSTALL_DIR)/control/lib -lmcserver \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/control/lib \
 -ledit


command_parts := \
 Help


parts := \
 ConsoleController \
 Properties \
 Subnet \
 Thread \
 ${command_parts:%=command/%}


objs := ${parts:%=%.o}


clean_cmds = \
 $(RM) *.o *.d \
  command/*.o command/*.d \
  libmmcs_common.so.1.0.0 libmmcs_common.so.1 libmmcs_common.so


install_filenames := \
 libmmcs_common.so.1.0.0 \
 libmmcs_common.so.1 \
 libmmcs_common.so

install_targets = ${install_filenames:%=$(BGQ_INSTALL_DIR)/hlcs/lib/%}


default: all


all: libmmcs_common.so


libmmcs_common.so.1.0.0: $(objs)
	$(CXX) -o $@ $(CXXFLAGS) $^ -shared -Wl,-soname,libmmcs_common.so.1 $(LDFLAGS)

libmmcs_common.so.1: libmmcs_common.so.1.0.0
	$(RM) $@
	$(LN) -s $< $@

libmmcs_common.so: libmmcs_common.so.1
	$(RM) $@
	$(LN) -s $< $@


install: $(install_targets)


clean:
	$(clean_cmds)


distclean:
	$(clean_cmds)


uninstall:


.PHONY: default all install clean distclean uninstall

-include *.d
-include command/*.d
