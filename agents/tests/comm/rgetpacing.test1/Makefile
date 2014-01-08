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

include ../../../../Make.rules

#SILENT=@
SILENT=

CNK = 1
ifeq (${CNK},1)
include ../cnk.muspi.rules
else
include ../firmware.muspi.rules
endif

S_SRC =	

C_SRC = $(wildcard *.c)

K_OBJ = $(addsuffix .o,$(notdir $(basename $(C_SRC))))

K_TARGET = $(TESTNAME).elf

TESTNAME = $(notdir $(shell pwd))

default:	all $(TEST_INSTALL_DIR)/$(TESTNAME).elf

$(TEST_INSTALL_DIR)/$(TESTNAME).elf: $(TESTNAME).elf
	$(call bgq_install_bin,$<,$@)

install:	all $(TEST_INSTALL_DIR)/$(TESTNAME).elf
	@echo "Installing `$(BASENAME) $$PWD`"


all:	subdirs $(K_TARGET)

$(K_TARGET): $(K_LIBS) $(K_OBJ) $(K_LDS_NAME) $(LOCAL_LIBS)

#
# library subdirectoreis
# 
subdirs:  $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) --directory $@

clean::
	@echo "Cleaning `$(BASENAME) $$PWD`"
	@-$(RM_F) *.o *.listing *.map *.objdump *.nm *.readelf *.elf *.bin *~


distclean: clean
	@echo "Dist-cleaning `$(BASENAME) $$PWD`"
	@-rm -rf $(TEST_INSTALL_DIR)
	@-$(RM_F) ./Make.depend


depend::
	@echo "Making dependencies for `$(BASENAME) $$PWD`"
	@-$(RM_F) ./Make.depend
	@$(BGQ_CROSS_CC) -MM $(K_CPU_FLAGS) $(K_KFLAGS) $(K_CFLAGS) $(K_AFLAGS) $(S_SRC)    >  ./Make.depend
	@$(BGQ_CROSS_CC) -MM $(K_CPU_FLAGS) $(K_KFLAGS) $(K_CFLAGS)             $(C_SRC)    >> ./Make.depend


fresh:	clean depend install

#
# script for local self testing
# Run a firmware extension on mesa
runmesa: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwddr \
	--parmstest ../test.parms \
	--program $(TESTNAME).elf \
	--svchost $(TESTNAME).svchost \
	 $(parms)

runmmcslite: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwddr \
	--parmstest ../test.parms \
	--program $(TESTNAME).elf \
	--svchost $(TESTNAME).svchost \
	--script runMmcsLite \
	 $(parms)

# Run a firmware extension on mambo
runmambo: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
	--ttype fwddr \
	--parmstest ../test.parms \
	--program $(TESTNAME).elf \
	--svchost $(TESTNAME).svchost \
	--script runMambo \
	--timelimit 15 \
	$(parms)

# Run a CNK app on mambo
runmambocnk: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
	--ttype cnk \
	--program $(TESTNAME).elf \
	--svchost $(TESTNAME).svchost \
	--script runMambo \
	--timelimit 15 \
	--svchost pers.svchost \
	$(parms)

runmesahelp:
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh --help



-include Make.depend

