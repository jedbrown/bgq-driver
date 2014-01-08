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

#

include ../../../Make.rules
include ../test.rules

#SILENT=@
SILENT=


S_SRC =	

C_SRC = $(wildcard *.c)

K_OBJ = $(addsuffix .o,$(notdir $(basename $(C_SRC))))

K_TARGET = $(TESTNAME).elf

TESTNAME = $(notdir $(shell pwd))
SVCHOSTNAME = ${TESTNAME}.svchost

default:	all

install:	all   $(TEST_INSTALL_DIR)/$(TEST).elf $(TEST_INSTALL_DIR)/test.parms $(TEST_INSTALL_DIR)/$(TEST).svchost $(TEST_INSTALL_DIR)/calibrate $(TEST_INSTALL_DIR)/calibrate.cn.script $(TEST_INSTALL_DIR)/calibrate.ion.script
	@echo "Installing `$(BASENAME) $$PWD`"

all:	$(K_TARGET)

$(K_TARGET): $(K_LIBS) $(K_OBJ) $(K_LDS_NAME) $(LOCAL_LIBS)

$(TEST_INSTALL_DIR)/$(TEST).elf: $(TEST).elf
	$(call bgq_install_bin,$<,$@)

$(TEST_INSTALL_DIR)/$(TEST).svchost: $(TEST).svchost
	$(call bgq_install_bin,$<,$@)

$(TEST_INSTALL_DIR)/test.parms: test.parms
	$(call bgq_install_bin,$<,$@)

$(TEST_INSTALL_DIR)/calibrate:	calibrate
	$(call bgq_install_bin,$<,$@)

 $(TEST_INSTALL_DIR)/calibrate.cn.script:	calibrate.cn.script
	$(call bgq_install_bin,$<,$@)

 $(TEST_INSTALL_DIR)/calibrate.ion.script:	calibrate.ion.script
	$(call bgq_install_bin,$<,$@)

$(SUBDIRS):
	$(MAKE) --directory $@

clean::
	@echo "Cleaning `$(BASENAME) $$PWD`"
	@-$(RM_F) *.o *.listing *.map *.objdump *.nm *.readelf *.elf *.bin *~


distclean: clean
	@echo "Dist-cleaning `$(BASENAME) $$PWD`"
	@-$(RM_F) ./Make.depend


depend::
	@echo "Making dependencies for `$(BASENAME) $$PWD`"
	@-$(RM_F) ./Make.depend
	@$(BGQ_CROSS_CC) -MM $(K_CPU_FLAGS) $(K_KFLAGS) $(K_CFLAGS) $(K_AFLAGS) $(S_SRC)    >  ./Make.depend
	@$(BGQ_CROSS_CC) -MM $(K_CPU_FLAGS) $(K_KFLAGS) $(K_CFLAGS)             $(C_SRC)    >> ./Make.depend


fresh:	clean depend install

runmmcslite: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwddr \
	--parmstest test.parms \
	--svchost $(TESTNAME).svchost \
	--program $(TESTNAME).elf \
	--script runMmcsLite \
	--timelimit 200 \
	 $(parms)

# Run a firmware extension on mambo
runmambo: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
	--ttype fwddr \
	--parmstest ../test.parms \
	--program $(TESTNAME).elf \
	--svchost $(SVCHOSTNAME) \
	--script runMambo \
	--timelimit 60 \
	--envvar MUSPI_IOLINK_BW_MODE=$(MODE) \
        --envvar MUSPI_IOLINK_BW_MESSAGE_SIZE=$(MSGSIZE) \
        --envvar MUSPI_IOLINK_BW_CHECK_MESSAGES=$(CHECK) \
        --envvar MUSPI_IOLINK_BW_NETWORK=$(NETWORK) \
	--envvar MUSPI_IOLINK_BW_ITERATIONS=${ITERATIONS} \
	$(parms)

runmesahelp:
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh --help



-include Make.depend

