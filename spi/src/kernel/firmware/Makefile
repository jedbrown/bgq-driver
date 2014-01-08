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

CFLAGS  = -O2 -g -Wall

# path for BGQ installation
CFLAGS += -I$(BGQ_INSTALL_DIR)/gnu/runtime -I$(BGQ_INSTALL_DIR)

# path for firmware implementation of SPIs
CFLAGS += -I$(BGQ_INSTALL_DIR)/spi/include/kernel/firmware

# Tells compiler to create dependency file
CFLAGS += -MMD -MF .dep.$<.d

# for additional command line args, ie "make ARGS=-Dxxx=yyy"
CFLAGS += $(ARGS)

CXXFLAGS = $(CFLAGS) -fno-exceptions

CXX     = $(BGQ_CROSS_CXX)
CC      = $(BGQ_CROSS_CC)
FC      = $(BGQ_CROSS_FC)
AR      = $(BGQ_CROSS_AR) crv

NAME=

TARGET_STATIC_LIB = libSPI_firmware.a
LIB_OBJS = MU.o
TARGET_DIR = $(BGQ_INSTALL_DIR)/spi/lib

install:	all

all:	$(TARGET_DIR)/$(TARGET_STATIC_LIB) $(TARGET_DIR)/firmware/libSPI.a

#	@echo "usage: make NAME=prog [ compile | dis | syms | read | run | clean ]"
#	@echo "   or: make check"

$(TARGET_DIR)/$(TARGET_STATIC_LIB):	$(TARGET_STATIC_LIB)
			$(MKDIR) -p $(TARGET_DIR)
			$(CP) $(TARGET_STATIC_LIB) $(TARGET_DIR)


$(TARGET_DIR)/firmware/libSPI.a:	$(TARGET_STATIC_LIB)
	$(MKDIR) -p $(TARGET_DIR)/firmware
	$(CP) $(TARGET_STATIC_LIB) $(TARGET_DIR)/firmware/libSPI.a

$(TARGET_STATIC_LIB):	$(LIB_OBJS)
			$(AR) $(TARGET_STATIC_LIB) $(LIB_OBJS)

$(NAME):


compile:$(NAME)

dis:	$(NAME)
	$(BGQ_CROSS_OBJDUMP) -d -S $(NAME)

syms:	$(NAME)
	$(BGQ_CROSS_OBJDUMP) -t $(NAME)

ssyms:	$(NAME)
	$(BGQ_CROSS_OBJDUMP) -t $(NAME) | sort | fgrep 0000

size:	$(NAME)
	$(BGQ_CROSS_SIZE) --format=SysV  $(NAME)

elf:	$(NAME)
	readelf -a $(NAME)

clean:
	$(RM) $(NAME) $(NAMES)
	$(RM) $(TARGET_STATIC_LIB) 
	$(RM) *.o *.listing *.nm *.map *.elf *.objdump *.readelf .dep.*.d *~

distclean:clean
	$(RM) $(TARGET_DIR)/$(TARGET_STATIC_LIB)



# Includes the dependency file from a previous make
ifeq ($(filter distclean clean,$(MAKECMDGOALS)),)
-include .dep.*.d
endif
