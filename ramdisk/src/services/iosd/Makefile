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

.PHONY: clean tags

# Source files
IOSD_SRCS := iosd.cc
IOSD_SRCS += IosConfig.cc
IOSD_SRCS += IosController.cc
IOSD_SRCS += HwIosController.cc
IOSD_SRCS += SimIosController.cc
IOSD_SRCS += ServiceProcess.cc
IOSD_SRCS += ServiceManager.cc
IOSD_SRCS += ServiceMonitor.cc

# Object files
IOSD_OBJS = $(IOSD_SRCS:%.cc=%.o_64)
JOB_SIM = start_job_simulation

ifeq ($(OS_ARCH),ppc64)
IOSD = iosd
endif

ifeq ($(BGQ_CONTROL_SYSTEM_SIMULATION),yes)
IOSD = iosd
endif


# Additional linker flags
LDFLAGS  += -L $(IOS_LIBRARY_DIR) -lbgcios
LDFLAGS  += -llog4cxx
LDFLAGS  += -lcrypto
LDFLAGS  += -lssl

all: $(IOSD)

clean:
	$(RM) $(IOSD) *.o *.o_64 *.d *.objdump

distclean: clean
	$(RM) $(IOS_PROGRAM_DIR)/$(IOSD)
    
install: all $(BGQ_INSTALL_DIR)/ramdisk/bin/$(JOB_SIM)

objdump: $(IOSD)
	$(OBJDUMP) $< > $<.objdump

$(IOSD): $(IOSD_OBJS)
	$(CXX) -o $@ $(IOSD_OBJS) $(LDFLAGS)
	$(MKDIR) -p $(IOS_PROGRAM_DIR)
	$(CP) $(IOSD) $(IOS_PROGRAM_DIR)

$(BGQ_INSTALL_DIR)/ramdisk/bin/$(JOB_SIM): $(JOB_SIM).template
	$(MKDIR) -p $(dir $@)
	sed -e "s'DRIVER_INSTALL_DIR'$(BGQ_INSTALL_DIR)'g" $< > $@
	chmod +x $@
	

#include our dependency rules
-include *.d

