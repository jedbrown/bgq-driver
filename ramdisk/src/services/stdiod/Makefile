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
SRCS    := stdiod.cc
SRCS    += StdioConfig.cc
SRCS    += StdioController.cc
SRCS    += SimStdioController.cc
SRCS    += HwStdioController.cc

# Object files
OBJS  = $(SRCS:%.cc=%.o_64)

ifeq ($(OS_ARCH),ppc64)
STDIOD = stdiod
endif

ifeq ($(BGQ_CONTROL_SYSTEM_SIMULATION),yes)
STDIOD = stdiod
endif

# Additional compiler flags
#CXXFLAGS +=

# Additional linker flags
LDFLAGS  += -L $(IOS_LIBRARY_DIR) -lbgcios
LDFLAGS  += -llog4cxx
LDFLAGS  += -libverbs
LDFLAGS  += -lrdmacm

all: $(STDIOD)

clean:
	$(RM) $(STDIOD) *.o *.o_64 *.d *.objdump

distclean: clean
	$(RM) $(IOS_PROGRAM_DIR)$(STDIOD)
    
install: all

objdump: $(STDIOD)
	$(OBJDUMP) $< > $<.objdump

$(STDIOD): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)
	$(MKDIR) -p $(IOS_PROGRAM_DIR)
	$(CP) $(STDIOD) $(IOS_PROGRAM_DIR)

#include our dependency rules
-include *.d

