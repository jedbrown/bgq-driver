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
include ../../../../../Make.rules
include ../../../../Make.gcc.rules
include ../../../../Make.test.rules

SILENT = 

NAME=


TARGET_STATIC_LIB = libtestlib.a

C_SRC 	= $(wildcard *.c)
CXX_SRC	= $(wildcard *.cc)
LIB_OBJS := $(notdir $(patsubst %.c,%.o,$(C_SRC)) $(patsubst %.cc,%.o,$(CXX_SRC)))

CFLAGS += -fno-exceptions -fno-rtti
           
TARGET_DIR = $(BGQ_INSTALL_DIR)/bgpm/lib


.PHONY: default all install clean distclean subdirs $(SUBDIRS)


install:	all


all:	subdirs $(TARGET_STATIC_LIB)


$(TARGET_STATIC_LIB):	$(LIB_OBJS)
			@echo making library
			$(AR) $(TARGET_STATIC_LIB) $(LIB_OBJS)
			$(MKDIR) -p $(TARGET_DIR)
			$(CP) $(TARGET_STATIC_LIB) $(TARGET_DIR)

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
	
