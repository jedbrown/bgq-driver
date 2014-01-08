# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# Licensed Materials - Property of IBM                             
# Blue Gene/Q                                                      
# (C) Copyright IBM Corp. 2012  All Rights Reserved                 
# US Government Users Restricted Rights - Use,                     
# duplication or disclosure restricted by GSA ADP                  
# Schedule contract with IBM Corp.                                 
#                                                                  
# This software is available to you under the                      
# GNU General Public License (GPL).                                
#                                                                  
# ================================================================ 
#                                                                  
# end_generated_IBM_copyright_prolog                               
#+++
# Makefile for Blue Gene/Q Halt Program 
#
# Author: Jay S. Bryant
#
#---

include ../../../Make.rules

ifeq ($(RHEL6),yes)
        CC=gcc
endif
ifeq ($(OS_ARCH),x86_64)
        CC=$(BGQ_CROSS_CC)
endif
ifeq ($(RHEL6),no)
        CC=$(BGQ_CROSS_CC)
endif



TARGET=bghalt
SILENT=

SRC_C	= bghalt.c
OBJ	+= $(SRC_C:%.c=%.o)


LINUX_CFLAGS = -O2 -c -I${BGQ_INSTALL_DIR}/linux/include

.PHONY : all clean depend install distclean

.SUFFIXES:

%.o: %.c
	@echo "Compiling $<"
	$(CC) $(LINUX_CFLAGS) $< -o $*.o

default: all install

all: $(TARGET)

install: all ${BGQ_INITRAMFS_PATH}/sbin 
	@${CP} $(TARGET) ${BGQ_INITRAMFS_PATH}/sbin/.

${BGQ_INITRAMFS_PATH}/sbin:
	-@${MKDIR} -p ${BGQ_INITRAMFS_PATH}/sbin

$(TARGET): $(OBJ)
	$(CC) -dynamic -o $@ $<

clean:
	@echo "Cleaning $(TARGET)"
	@${RM} -f $(OBJ) $(TARGET)

distclean: clean
	@${RM} -f Make.depend

depend::
	@echo "Making dependencies"
	@$(CC) -M $(LINUX_CFLAGS) $(SRC_CC) $(SRC_C) > ./Make.depend


fresh:  clean depend all

-include Make.depend

