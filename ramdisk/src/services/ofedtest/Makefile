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
SRCS    := ofedtest.cc
SRCS    += HwStdioController.cc

# Object files
OBJS  = $(SRCS:%.cc=%.o_64)

ifeq ($(OS_ARCH),ppc64)
OFEDTEST = ofedtest
endif

# Additional compiler flags
#CXXFLAGS +=

# Additional linker flags
LDFLAGS  += -L $(IOS_LIBRARY_DIR) -lbgcios
LDFLAGS  += -lcrypto
LDFLAGS  += -lssl

all: $(OFEDTEST)

clean:
	$(RM) $(OFEDTEST) *.o *.o_64 *.d *.objdump

distclean: clean
	$(RM) $(IOS_PROGRAM_DIR)$(OFEDTEST)
    
install: all

objdump: $(OFEDTEST)
	$(OBJDUMP) $< > $<.objdump

$(OFEDTEST): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)
	$(MKDIR) -p $(IOS_PROGRAM_DIR)
	$(CP) $(OFEDTEST) $(IOS_PROGRAM_DIR)

#include our dependency rules
-include *.d

