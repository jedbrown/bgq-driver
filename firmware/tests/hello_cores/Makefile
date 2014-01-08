# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
#  --------------------------------------------------------------- 
#                                                                  
# Licensed Materials - Property of IBM                             
# Blue Gene/Q                                                      
# (C) Copyright IBM Corp.  2010, 2012                              
# US Government Users Restricted Rights - Use, duplication or      
#   disclosure restricted by GSA ADP Schedule Contract with IBM    
#   Corp.                                                          
#                                                                  
# This software is available to you under the Eclipse Public       
# License (EPL).                                                   
#                                                                  
#  --------------------------------------------------------------- 
#                                                                  
# end_generated_IBM_copyright_prolog                               
# +++
#	Makefile for Firmware-Extension Testcases
#
#	$(SRC):	
#		list of files to be linked with kernel to make test
#	$(TEST):
#		name of the test
#                 = name of current directory
#	$(GROUP):
#		name of the test group
#                 = name of directory containing current directory
#	$(FW_OBJ):
#		object files needed to make the test
#
# ---

include ../../../Make.rules
include ../test.rules

#SILENT=@
SILENT=

BASENAME = /bin/basename
RM_F = /bin/rm -f

FW_KFLAGS    = -D__KERNEL__
FW_AFLAGS    = -D__ASSEMBLY__
FW_INCLUDES  = -I$(BGQ_INSTALL_DIR)
FW_CPU_FLAGS = -m64 -g -O2

FW_LDMAP = -Map $(basename $@).map
#FW_LDMAP =

FW_LDSCRIPT = $(BGQ_INSTALL_DIR)/firmware/include/Firmware.lds

FW_LDFLAGS  = -Bstatic $(FW_LDMAP) --script $(FW_LDSCRIPT)
FW_CFLAGS = -Wall -Werror $(LISTING_OPTIONS) $(FW_INCLUDES) -c

FW_LIBS = -L $(BGQ_INSTALL_DIR)/firmware/lib -lfw -static -nostdlib



#
#	Definitions
#

C_SRC 	= $(wildcard *.c) $(wildcard *.cc) 
S_SRC	= $(wildcard *.S)

TEST	= $(notdir $(shell pwd))
GROUP   = $(notdir $(patsubst %/$(TEST),%,$(shell pwd)))

FW_OBJ	= $(patsubst %.c,%.o,$(C_SRC)) $(patsubst %.S,%.o,$(S_SRC))

LOCAL_LIBS =

#
# 	Standard targets
#

default:	install

all:		$(TEST).elf

install:	all
	@echo "Installing `$(BASENAME) $$PWD` into some T.B.D. location"
	$(MKDIR) -p $(BGQ_INSTALL_DIR)/firmware/tests/$(TEST)
	$(CP) $(TEST).elf $(BGQ_INSTALL_DIR)/firmware/tests/$(TEST)

clean:
	$(RM_F) *.o *.listing *.nm *.map *.elf *.objdump *.readelf *.beam


distclean:	clean

objdump:	$(TEST).objdump

fresh:	clean all objdump install


%.o: %.c
	@echo "gcc: "`$(BASENAME) $$PWD`"/"`$(BASENAME) $<`
	$(SILENT)$(BGQ_CROSS_CC) $(FW_CPU_FLAGS) $(FW_KFLAGS) $(FW_CFLAGS) $< -o $*.o

%.o: %.cc
	@echo "gcc: "`$(BASENAME) $$PWD`"/"`$(BASENAME) $<`
	$(SILENT)$(BGQ_CROSS_CC) $(FW_CPU_FLAGS) $(FW_KFLAGS) $(FW_CFLAGS) $< -o $*.o

%.o: %.S
	@echo "gcc: "`$(BASENAME) $$PWD`"/"`$(BASENAME) $<`
	$(SILENT)$(BGQ_CROSS_CC) $(FW_CPU_FLAGS) $(FW_KFLAGS) $(FW_CFLAGS) $(FW_AFLAGS) -Os $< -o $*.o

$(TEST).elf:
	@echo "Now linking executable $@ ..."
	$(SILENT)$(BGQ_CROSS_LD) $(FW_LDFLAGS) -o $@ $^ $(FW_LIBS) $(LOCAL_LIBS)

%.objdump: %.elf
	@echo "Generating $@ ..."
	$(SILENT)$(BGQ_CROSS_OBJDUMP)  -dfhl $< > $*.objdump
#	$(SILENT)$(BGQ_CROSS_READELF)  $< > $*.readelf


#
#	Dependencies
#

$(TEST).elf:	$(FW_OBJ)

$(TEST).objdump:	$(TEST).elf


runmambo: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwext \
	--parmstest test.parms \
	--svchost test.svchost \
	--script runMambo \
	--program $(TEST).elf
	 $(parms)
