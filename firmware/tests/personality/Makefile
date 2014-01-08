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

#
#	Definitions
#


C_SRC 	= $(patsubst %.c,%.o,$(wildcard *.c))
S_SRC	=

TEST	= $(notdir $(shell pwd))

FWTST_OBJ	= $(patsubst %.c,%.o,$(C_SRC)) $(patsubst %.S,%.o,$(S_SRC))

LOCAL_LIBS =

#SUBDIRS=../../libfwext

.PHONY: default all install clean dist_clean subdirs $(SUBDIRS)



#
# 	Standard targets
#

default:	install

all:	subdirs	$(TEST).elf


$(TEST_INSTALL_DIR)/$(TEST).elf: $(TEST).elf
	$(call bgq_install_bin,$<,$@)

$(TEST_INSTALL_DIR)/test.parms: test.parms
	$(call bgq_install_bin,$<,$@)

$(TEST_INSTALL_DIR)/dumpPersonality:	dumpPersonality
	$(call bgq_install_bin,$<,$@)

install: all $(TEST_INSTALL_DIR)/$(TEST).elf $(TEST_INSTALL_DIR)/test.parms $(TEST_INSTALL_DIR)/dumpPersonality


clean:
	$(RM_F) *.o *.listing *.nm *.map *.elf *.objdump *.readelf *.beam .dep.*


distclean:	clean

objdump:	$(TEST).objdump

fresh:	clean all objdump install

#
#	Dependencies
#


$(TEST).elf:	$(FWTST_OBJ) 

$(TEST).objdump:	$(TEST).elf


#
# library subdirectoreis
# 
subdirs:  $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) --directory $@




#
# script for local self testing
#
runmesa: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwddr \
	--parmstest test.parms \
	--program $(TEST).elf \
	 $(parms)

runmambo: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwddr \
	--parmstest test.parms \
	--program $(TEST).elf \
	--script runMambo \
	--svchost $(TEST).svchost \
	 $(parms)

runmmcslite: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwddr \
	--program $(TEST).elf \
	--script runMmcsLite \
	--svchost $(TEST).svchost \
	 $(parms)

dumpDefaults: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwddr \
	--program $(TEST).elf \
	--script runMmcsLite \
	--envvar DISABLE.VALIDATE.PERSONALIZED=1 \
	--envvar DISABLE.VALIDATE.DEFAULT=1 \
	--svchost empty.svchost \
	 $(parms)

runmesahelp:
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh --help


ifeq ($(filter distclean clean,$(MAKECMDGOALS)),)	
-include .dep.*.d
endif

