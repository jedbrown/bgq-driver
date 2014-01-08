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
# (C) Copyright IBM Corp.  2010, 2012                              
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
include ../../../Make.firmware.rules

SILENT = 

NAME=

# find source in parent dir
VPATH = ..

# dependency on other directories when local build
SUBDIRS = ../../../include/kernel ../../../include/upci

TARGET_STATIC_LIB = libSPI_upci_fw.a


C_SRC 	= $(wildcard ../*.c)
CXX_SRC	= $(wildcard ../*.cc)
LIB_OBJS := $(notdir $(patsubst %.c,%.o,$(C_SRC)) $(patsubst %.cc,%.o,$(CXX_SRC)))

LIB_OBJS := $(filter-out upci_syscall.o, us_debug.o $(LIB_OBJS)) # fwext_personality.o

CFLAGS += -DENABLE_UPC_DD1_WORKAROUNDS 
#CFLAGS += -DNDEBUG

#CXXFLAGS += -DNDEBUG


#LIB_OBJS = upc_a2.o upc_atomic.o upc_c.o upc_p.o upc_l2.o
TARGET_DIR = $(BGQ_INSTALL_DIR)/spi/lib


.PHONY: default all install clean distclean subdirs $(SUBDIRS)


install:	all


all:	subdirs $(TARGET_STATIC_LIB) $(TARGET_STATIC_LIB_FWCNK)

			
$(TARGET_STATIC_LIB):	$(LIB_OBJS)
			@echo making library $(TARGET_STATIC_LIB)
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
	
