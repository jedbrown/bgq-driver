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
 -isystem $(BGQ_INSTALL_DIR) \
 -iquote .. \
 -W \
 -Wall \
 -Wextra \
 -Wundef \
 -Wconversion \
 -O2 \
 -MD -MP \
 -pthread \
 -fPIC


parts := \
 AgentBase \
 ArgParse \
 BinaryController \
 Ids \
 Protocol \
 ThreadLog

objs := ${parts:%=%.o}

clean_cmd := $(RM) *.o *.d

default: all

all: $(objs)

install: all

clean:
	$(clean_cmd)

distclean:


.PHONY: default all install clean distclean

-include *.d
