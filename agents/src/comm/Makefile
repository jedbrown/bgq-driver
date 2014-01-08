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

include ../../../Make.rules

#SILENT=@
SILENT=

GCC=1
XL=0

ifeq (${GCC},1)
CFLAGS  = -O3 -g -Wall
CFLAGS += -I$(BGQ_INSTALL_DIR)/gnu/runtime
# Tells compiler to create dependency file
CFLAGS += -MMD -MF .dep.$<.d
CXX     = $(BGQ_CROSS_CXX)
CC      = $(BGQ_CROSS_CC)
FC      = $(BGQ_CROSS_FC)
LD      = $(CC)
endif

ifeq (${XL},1)
CFLAGS  = -g -O4 -qflag=i:i -qsaveopt -D__XL__
CXX     = /opt/ibmcmp/vacpp/bg/12.1/bin/bgxlc
CC      = /opt/ibmcmp/vacpp/bg/12.1/bin/bgxlc
LD      = /opt/ibmcmp/vacpp/bg/12.1/bin/bgxlc
endif

# path for BGQ installation
CFLAGS += -I$(BGQ_INSTALL_DIR)

# path for CNK implementation of SPIs
CFLAGS += -I$(BGQ_INSTALL_DIR)/spi/include/kernel/cnk

# for additional command line args, ie "make ARGS=-Dxxx=yyy"
CFLAGS += $(ARGS)

CXXFLAGS = $(CFLAGS) -fno-exceptions

AGENT_LDFLAGS = -static

AGENT_LIBS = -L$(BGQ_INSTALL_DIR)/spi/lib -l SPI -l SPI_cnk -lpthread -lrt

AGENT_NAME = $(notdir $(shell pwd))

S_SRC =	

C_SRC = commagent.c rgetpacing.c fence.c

AGENT_OBJ = commagent.o rgetpacing.o fence.o

AGENT_TARGET = $(AGENT_NAME).elf

AGENT_ALL = $(AGENT_TARGET) 

AGENT_TARGET_DIR = $(BGQ_INSTALL_DIR)/agents/bin

default:	all

all:	$(AGENT_ALL)

install: all
	@echo "Installing $(AGENT_NAME).elf in $(AGENT_TARGET_DIR)"
	$(MKDIR) -p $(AGENT_TARGET_DIR)
	$(CP) $(AGENT_NAME).elf $(AGENT_TARGET_DIR)/.

$(AGENT_TARGET): $(AGENT_OBJ) 
	$(BGQ_CROSS_CC) $(AGENT_LDFLAGS) -Wl,--script=$(BGQ_INSTALL_DIR)/cnk/tools/AppAgent0.lds -o $(AGENT_TARGET) $(AGENT_OBJ) $(AGENT_LIBS)
#	$(BGQ_CROSS_CC) $(AGENT_LDFLAGS) -o rgetpacing.test.elf rgetpacing.test.o $(AGENT_LIBS)

clean:
	$(RM) $(AGENT_ALL)
	$(RM) *.o *.listing *.nm *.map *.elf *.objdump *.readelf .dep.*.d *~

distclean:clean
	$(RM) $(AGENT_TARGET_DIR)/$(AGENT_TARGET)
#
# script for local self testing
#

runmambo: install
	BG_FORCE_TLBGENERATE=1 \
	BG_APPAGENT1=/dev/cnkmemfs/$(AGENT_TARGET) \
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
		--ttype cnk \
		--numcores 17 \
		--parmstest test.parms \
		--program $(AGENT_NAME).test.elf \
		--svchost pers.svchost \
		--script runMambo \
	 $(parms)

runfpga: install
	BG_FORCE_TLBGENERATE=1 \
	BG_APPAGENT1=/dev/cnkmemfs/$(AGENT_TARGET) \
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
		--ttype cnk \
		--numcores 17 \
		--script runFpga \
		--program $(AGENT_NAME).test.elf \
		--svchost pers.svchost \
	$(parms)

runmmcslite: install
	BG_FORCE_TLBGENERATE=1 BG_PROCESSESPERNODE=4 \
	BG_SHAREDMEMSIZE=1 \
	BG_APPAGENT1=/dev/cnkmemfs/$(AGENT_TARGET) \
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
		--ttype cnk \
		--program $(AGENT_NAME).test.elf \
		--svchost pers.svchost \
		--script runMmcsLite \
		--timelimit 200 \
	$(parms)

runmesahelp:
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh --help


-include Make.depend
