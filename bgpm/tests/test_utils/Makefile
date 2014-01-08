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
include ../../../Make.rules
include ../../Make.gcc.rules

SILENT = 

NAME=

# find source in parent dir

TARGET_STATIC_LIB = libtest_utils.a

C_SRC 	= $(wildcard *.c)
CXX_SRC	= $(wildcard *.cc)
LIB_OBJS := $(notdir $(patsubst %.c,%.o,$(C_SRC)) $(patsubst %.cc,%.o,$(CXX_SRC)))

CFLAGS += 
CXXFLAGS += -Weffc++
CXXFLAGS += -fno-rtti 


.PHONY: default all install clean distclean subdirs $(SUBDIRS)


install:	all


all:	subdirs $(TARGET_STATIC_LIB)


$(TARGET_STATIC_LIB):	$(LIB_OBJS)
			@echo making library
			$(AR) $(TARGET_STATIC_LIB) $(LIB_OBJS)

$(NAME):


clean:
	-$(RM) *.o
	-$(RM) .dep*
	-$(RM) *.a

distclean:clean


subdirs:  $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) --directory $@


ifeq ($(filter distclean clean,$(MAKECMDGOALS)),)	
-include .dep.*.d
endif
	
