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
#	$(FW_OBJ):
#		object files needed to make the test
#
# ---

include ../../../Make.rules
include ../test.rules

#SILENT=@
SILENT=

BASENAME = takeCPU
RM_F = /bin/rm -f

TAKECPU_KFLAGS    = -D__KERNEL__
TAKECPU_AFLAGS    = -D__ASSEMBLY__
TAKECPU_INCLUDES  = -I$(BGQ_INSTALL_DIR)
TAKECPU_CPU_FLAGS = -m64 -g -O2

TAKECPU_LDMAP = -Map $(basename $@).map

TAKECPU_LDSCRIPT = takeCPU.lds
TAKECPU_LDFLAGS  = -Bstatic $(TAKECPU_LDMAP) --script $(TAKECPU_LDSCRIPT)
TAKECPU_CFLAGS = -D__TAKECPU__ -Wall -Werror $(LISTING_OPTIONS) $(TAKECPU_INCLUDES) -c

TAKECPU_LIBS = -static -nostdlib

FWLIBINSTALL_DIR=$(BGQ_INSTALL_DIR)/firmware/lib/
FWINCINSTALL_DIR=$(BGQ_INSTALL_DIR)/firmware/include/

LOCAL_LIBS =

%.o: %.c
	@echo "gcc: $<"
	$(SILENT)$(BGQ_CROSS_CC) $(TAKECPU_CPU_FLAGS) $(TAKECPU_KFLAGS) $(TAKECPU_CFLAGS) $< -o $*.o

%.o: %.cc
	@echo "gcc: $<"
	$(SILENT)$(BGQ_CROSS_CC) $(TAKECPU_CPU_FLAGS) $(TAKECPU_KFLAGS) $(TAKECPU_CFLAGS) $< -o $*.o

%.o: %.S
	@echo "gcc: $<"
	$(SILENT)$(BGQ_CROSS_CC) $(TAKECPU_CPU_FLAGS) $(TAKECPU_KFLAGS) $(TAKECPU_CFLAGS) $(TAKECPU_AFLAGS) -Os $< -o $*.o

$(TEST).elf:
	@echo "Linking Executable $@ ..."
	$(SILENT)$(BGQ_CROSS_LD) $(TAKECPU_LDFLAGS) -o $@ $(filter %.o,$^) $(TAKECPU_LIBS) $(LOCAL_LIBS)


%.objdump: %.elf
	@echo "Generating $@ ..."
	$(SILENT)$(BGQ_CROSS_OBJDUMP)  -dfhl $< > $*.objdump
#	$(SILENT)$(BGQ_CROSS_READELF)  $< > $*.readelf

S_SRC = start.S
C_SRC = test_main.c vsprintf.c

TAKECPU_OBJ     = start.o test_main.o vsprintf.o
TAKECPU_TARGET  = takeCPU.elf
TAKECPU_OBJDUMP = takeCPU.objdump

default:	all


install:	all

objdump:	$(TAKECPU_OBJDUMP)


all:	$(TAKECPU_TARGET)


$(FWINCINSTALL_DIR)/$(TAKECPU_LDSCRIPT): $(TAKECPU_LDSCRIPT)
	-$(call bgq_install_lib,$<,$@)

$(TAKECPU_TARGET):	$(TAKECPU_OBJ) $(TAKECPU_LDSCRIPT)

$(TAKECPU_OBJDUMP):	$(TAKECPU_TARGET)


clean::
	@echo "Cleaning $(BASENAME)"
	@-$(RM_F) *.o *.listing *.map *.objdump *.nm *.readelf *.elf *.beam


distclean: clean
	@echo "Dist-cleaning $(BASENAME)"
	@-$(RM_F) ./Make.depend


depend::
	@echo "Making dependencies for $(BASENAME)"
	@-$(RM_F) ./Make.depend
	@$(BGQ_CROSS_CC) -M $(TAKECPU_CPU_FLAGS) $(TAKECPU_KFLAGS) $(TAKECPU_CFLAGS) $(TAKECPU_AFLAGS) $(S_SRC)    >  ./Make.depend
	@$(BGQ_CROSS_CC) -M $(TAKECPU_CPU_FLAGS) $(TAKECPU_KFLAGS) $(TAKECPU_CFLAGS)                 $(C_SRC)    >> ./Make.depend
#	@$(BGQ_CROSS_CC) -M $(TAKECPU_CPU_FLAGS) $(TAKECPU_KFLAGS) $(TAKECPU_CFLAGS)                 $(TEST_SRC) >> ./Make.depend


fresh:	clean depend install

#
# script for local self testing
#
runmesa: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwddr \
	--parmstest test.parms \
	--program $(TAKECPU_TARGET) \
	--svchost takeCPU.svchost \
	 $(parms)

runmambo: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwddr \
	--parmstest test.parms \
	--program $(TAKECPU_TARGET) \
	--script runMambo \
	--svchost takeCPU.svchost \
	 $(parms)

runmesahelp:
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh --help


runmmcslite: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwddr \
	--program $(TEST).elf \
	--svchost $(TEST).svchost \
	--script runMmcsLite \
	 $(parms)

ifeq ($(filter distclean clean,$(MAKECMDGOALS)),)	
-include .dep.*.d
endif

