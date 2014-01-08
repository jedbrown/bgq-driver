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

BASENAME = fwtest
RM_F = /bin/rm -f


FWTST_LDMAP = -Map $(basename $@).map

FWTST_LDSCRIPT = fwtest.lds
FWTST_LDFLAGS  = -Bstatic $(FWTST_LDMAP) --script $(FWTST_LDSCRIPT)
#FWTST_CFLAGS = -D__FWTST__ -Wall -Werror $(LISTING_OPTIONS) $(FWTST_INCLUDES) -c

FWTST_LIBS = -static -nostdlib

FWLIBINSTALL_DIR=$(BGQ_INSTALL_DIR)/firmware/lib/
FWINCINSTALL_DIR=$(BGQ_INSTALL_DIR)/firmware/include/

LOCAL_LIBS =


S_SRC = start.S
C_SRC = test_main.c check_interface.c vsprintf.c

FWTST_OBJ     = start.o test_main.o check_interface.o vsprintf.o
FWTST_TARGET  = fwtest.elf
FWTST_OBJDUMP = fwtest.objdump

default:	all


install:	all $(TEST_INSTALL_DIR)/$(TEST).elf $(TEST_INSTALL_DIR)/$(TEST).svchost

objdump:	$(FWTST_OBJDUMP)


all:	$(FWTST_TARGET)

$(FWTST_TARGET):	$(FWTST_OBJ) $(FTST_LIBS) $(LOCAL_LIBS)
	@echo "Now linking executable $@ ..."
	$(SILENT)$(BGQ_CROSS_LD) $(FWTST_LDFLAGS) -o $@ $(FWTST_OBJ) $(FWTST_LIBS) $(LOCAL_LIBS)

$(TEST_INSTALL_DIR)/$(TEST).elf: $(FWTST_TARGET)
	$(call bgq_install_bin,$<,$@)

$(TEST_INSTALL_DIR)/$(TEST).svchost: firmware.svchost
	$(call bgq_install_bin,$<,$@)


%.objdump: %.elf
	@echo "Generating $@ ..."
	$(SILENT)$(BGQ_CROSS_OBJDUMP)  -dfhl $< > $*.objdump
#	$(SILENT)$(BGQ_CROSS_READELF)  $< > $*.readelf


$(FWINCINSTALL_DIR)/$(FWTST_LDSCRIPT): $(FWTST_LDSCRIPT)
	-$(call bgq_install_lib,$<,$@)

$(FWTST_TARGET):	$(FWTST_OBJ) $(FWTST_LDSCRIPT)

$(FWTST_OBJDUMP):	$(FWTST_TARGET)


clean::
	@echo "Cleaning $(BASENAME)"
	@-$(RM_F) *.o *.listing *.map *.objdump *.nm *.readelf *.elf *.beam .dep.*


distclean: clean
	@echo "Dist-cleaning $(BASENAME)"
	@-$(RM_F) ./Make.depend


depend::
	@echo "Making dependencies for $(BASENAME)"
	@-$(RM_F) ./Make.depend
	@$(BGQ_CROSS_CC) -M $(FWTST_CPU_FLAGS) $(FWTST_KFLAGS) $(FWTST_CFLAGS) $(FWTST_AFLAGS) $(S_SRC)    >  ./Make.depend
	@$(BGQ_CROSS_CC) -M $(FWTST_CPU_FLAGS) $(FWTST_KFLAGS) $(FWTST_CFLAGS)                 $(C_SRC)    >> ./Make.depend
#	@$(BGQ_CROSS_CC) -M $(FWTST_CPU_FLAGS) $(FWTST_KFLAGS) $(FWTST_CFLAGS)                 $(TST_SRC) >> ./Make.depend


fresh:	clean depend install

#
# script for local self testing
#
runmesa: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwddr \
	--parmstest test.parms \
	--program $(FWTST_TARGET) \
	--svchost firmware.svchost \
	 $(parms)

runmambo: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwddr \
	--parmstest test.parms \
	--program $(FWTST_TARGET) \
	--script runMambo \
	--svchost firmware.svchost \
	 $(parms)

runmmcslite: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwddr \
	--program $(FWTST_TARGET) \
	--script runMmcsLite \
	--svchost firmware.svchost \
	 $(parms)

runmesahelp:
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh --help


ifeq ($(filter distclean clean,$(MAKECMDGOALS)),)	
-include .dep.*.d
endif

