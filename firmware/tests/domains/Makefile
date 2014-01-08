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

BASENAME = domains
RM_F = /bin/rm -f

DOMAINS_KFLAGS    = -D__KERNEL__
DOMAINS_AFLAGS    = -D__ASSEMBLY__
DOMAINS_INCLUDES  = -I$(BGQ_INSTALL_DIR)
DOMAINS_CPU_FLAGS = -m64 -g -O2

DOMAINS_LDMAP = -Map $(basename $@).map

DOMAINS_LDSCRIPT   = domains.lds
DOMAINS_LDFLAGS    = -Bstatic $(DOMAINS_LDMAP) --script $(DOMAINS_LDSCRIPT)
DOMAINS_CFLAGS     = -D__DOMAINS__ -Wall -Werror $(LISTING_OPTIONS) $(DOMAINS_INCLUDES) -c

DOMAINS_LIBS = -static -nostdlib

FWLIBINSTALL_DIR=$(BGQ_INSTALL_DIR)/firmware/lib/
FWINCINSTALL_DIR=$(BGQ_INSTALL_DIR)/firmware/include/

LOCAL_LIBS =

%.o: %.c
	@echo "gcc: $<"
	$(SILENT)$(BGQ_CROSS_CC) $(DOMAINS_CPU_FLAGS) $(DOMAINS_KFLAGS) $(DOMAINS_CFLAGS) $< -o $*.o

%.o: %.cc
	@echo "gcc: $<"
	$(SILENT)$(BGQ_CROSS_CC) $(DOMAINS_CPU_FLAGS) $(DOMAINS_KFLAGS) $(DOMAINS_CFLAGS) $< -o $*.o

%.o: %.S
	@echo "gcc: $<"
	$(SILENT)$(BGQ_CROSS_CC) $(DOMAINS_CPU_FLAGS) $(DOMAINS_KFLAGS) $(DOMAINS_CFLAGS) $(DOMAINS_AFLAGS) -Os $< -o $*.o

domains.elf:
	@echo "Linking Executable $@ ..."
	$(SILENT)$(BGQ_CROSS_LD) $(DOMAINS_LDFLAGS) -o $@ $(filter %.o,$^) $(DOMAINS_LIBS) $(LOCAL_LIBS)


%.objdump: %.elf
	@echo "Generating $@ ..."
	$(SILENT)$(BGQ_CROSS_OBJDUMP)  -dfhl $< > $*.objdump
#	$(SILENT)$(BGQ_CROSS_READELF)  $< > $*.readelf

S_SRC = start.S
C_SRC = test_main.c vsprintf.c entry.c

DOMAINS_OBJ      = test_main.o vsprintf.o start_domain_0.o entry_domain_0.o start_domain_1.o entry_domain_1.o
DOMAINS_TARGETS  = domains.elf 
DOMAINS_OBJDUMPS = domains.objdump 

default:	all


install:	all

objdump:	$(DOMAINS_OBJDUMPS)


all:	$(DOMAINS_TARGETS)


start_domain_0.S:	start.S.template
	sed 's/XDOMAINX/domain_0/g' start.S.template | sed 's/NDOMAINN/0/g' > start_domain_0.S

entry_domain_0.c:	entry.c.template
	sed 's/XDOMAINX/domain_0/g' entry.c.template | sed 's/NDOMAINN/0/g' > entry_domain_0.c

start_domain_1.S:	start.S.template
	sed 's/XDOMAINX/domain_1/g' start.S.template | sed 's/NDOMAINN/1/g' > start_domain_1.S

entry_domain_1.c:	entry.c.template
	sed 's/XDOMAINX/domain_1/g' entry.c.template | sed 's/NDOMAINN/1/g' > entry_domain_1.c

$(FWINCINSTALL_DIR)/$(DOMAINS_LDSCRIPT): $(DOMAINS_LDSCRIPT)
	-$(call bgq_install_lib,$<,$@)

$(DOMAINS_TARGETS):	$(DOMAINS_OBJ) $(DOMAINS_LDSCRIPT)

$(DOMAINS_OBJDUMP):	$(DOMAINS_TARGET)


clean::
	@echo "Cleaning $(BASENAME)"
	@-$(RM_F) *.o *.listing *.map *.objdump *.nm *.readelf *.elf *.beam start_domain_*.S entry_domain_*.c


distclean: clean
	@echo "Dist-cleaning $(BASENAME)"
	@-$(RM_F) ./Make.depend


depend::
	@echo "Making dependencies for $(BASENAME)"
	@-$(RM_F) ./Make.depend
	@$(BGQ_CROSS_CC) -M $(DOMAINS_CPU_FLAGS) $(DOMAINS_KFLAGS) $(DOMAINS_CFLAGS) $(DOMAINS_AFLAGS) $(S_SRC)    >  ./Make.depend
	@$(BGQ_CROSS_CC) -M $(DOMAINS_CPU_FLAGS) $(DOMAINS_KFLAGS) $(DOMAINS_CFLAGS)                 $(C_SRC)    >> ./Make.depend
#	@$(BGQ_CROSS_CC) -M $(DOMAINS_CPU_FLAGS) $(DOMAINS_KFLAGS) $(DOMAINS_CFLAGS)                 $(TEST_SRC) >> ./Make.depend


fresh:	clean depend install

#
# script for local self testing
#
runmesa: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwddr \
	--parmstest test.parms \
	--program domains.elf \
	--svchost domains.svchost \
	--numcores 17 \
	 $(parms)

runmmcslite: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwddr \
	--program domains.elf \
	--script runMmcsLite \
	--svchost domains.svchost \
	--numcores 17 \
	 $(parms)

runmambo: install
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh \
        --ttype fwddr \
	--parmstest test.parms \
	--program domains.elf \
	--script runMambo \
	--svchost domains.svchost \
	--numcores 17 \
	 $(parms)

runmesahelp:
	$(BGQ_INSTALL_DIR)/scripts/runfctest.sh --help


ifeq ($(filter distclean clean,$(MAKECMDGOALS)),)	
-include .dep.*.d
endif

