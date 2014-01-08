# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# Licensed Materials - Property of IBM                             
# Blue Gene/Q                                                      
# (C) Copyright IBM Corp. 2012 All Rights Reserved                 
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
# ----------------------------------------------------------------
# Main makefile for bgsethostid.
#
# Author:  Jay S. Bryant (jsbryant@us.ibm.com)
#
# Date:  6/23/10
# ----------------------------------------------------------------

include ../../../Make.rules

ifeq ($(RHEL6),yes)
	CC=gcc
	LINUX_CFLAGS = -O2 -c
endif
ifeq ($(OS_ARCH),x86_64)
	CC=$(BGQ_CROSS_CC)
	LINUX_CFLAGS = -O2 -c -dynamic
endif
ifeq ($(RHEL6),no)
	CC=$(BGQ_CROSS_CC)
	LINUX_CFLAGS = -O2 -c -dynamic
endif

TARGET=bgsethostid
SILENT=

SRC_C	= bgsethostid.c
OBJ	+= $(SRC_C:%.c=%.o)


.PHONY : all clean depend install distclean

.SUFFIXES:

%.o: %.c
	@echo "Compiling $<"
	$(CC) $(LINUX_CFLAGS) $< -o $*.o

default: all install

all: $(TARGET)

install: all ${BGQ_INITRAMFS_PATH}/usr/bin ${BGQ_DISTROFS_PATH}/rpm/usr/bin
	@${CP} $(TARGET) ${BGQ_INITRAMFS_PATH}/usr/bin/.
	@${CP} $(TARGET) ${BGQ_DISTROFS_PATH}/rpm/usr/bin/.

${BGQ_INITRAMFS_PATH}/usr/bin:
	-@${MKDIR} -p ${BGQ_INITRAMFS_PATH}/usr/bin

${BGQ_DISTROFS_PATH}/rpm/usr/bin:
	-@${MKDIR} -p ${BGQ_DISTROFS_PATH}/rpm/usr/bin

$(TARGET): $(OBJ)
	$(CC) -o $@ $<

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
