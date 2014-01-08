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


include ../../Make.rules

#SILENT=@
SILENT=

BASENAME = firmware
RM_F = /bin/rm -f

BEAM_RESULTS += --beam::ipa=all --beam::data=$(BGQ_INSTALL_DIR)/firmware/beam

FW_KFLAGS    = -D__KERNEL__ -DFW_DUAL_DOMAIN=1
FW_AFLAGS    = -D__ASSEMBLY__
FW_INCLUDES  = -I$(BGQ_INSTALL_DIR) -I$(BGQ_INSTALL_DIR)/spi/include/kernel/firmware
FW_CPU_FLAGS = -m64 -g -Os

FW_LDMAP = -Map $(basename $@).map
#FW_LDMAP =

##FW_LDSCRIPT = BGQ_Firmware.lds
FW_LDSCRIPT = $(BGQ_INSTALL_DIR)/firmware/include/Firmware.lds
FW_LDFLAGS  = -Bstatic $(FW_LDMAP) --script $(FW_LDSCRIPT)
FW_CFLAGS = -Wall -Werror $(LISTING_OPTIONS) $(FW_INCLUDES) -c

FW_LIBS = -static -nostdlib

LOCAL_LIBS =

%.o: %.c
	@echo "gcc: $<"
	$(SILENT)$(BGQ_CROSS_CC) $(FW_CPU_FLAGS) $(FW_KFLAGS) $(FW_CFLAGS) $< -o $*.o

%.o: %.cc
	@echo "gcc: $<"
	$(SILENT)$(BGQ_CROSS_CC) $(FW_CPU_FLAGS) $(FW_KFLAGS) $(FW_CFLAGS) $< -o $*.o

%.o: %.S
	@echo "gcc: $<"
	$(SILENT)$(BGQ_CROSS_CC) $(FW_CPU_FLAGS) $(FW_KFLAGS) $(FW_CFLAGS) $(FW_AFLAGS) -Os $< -o $*.o

%.elf:
	@echo "Linking Executable $@ ..."
	$(SILENT)$(BGQ_CROSS_LD) $(FW_LDFLAGS) -o $@ $^ $(FW_LIBS) $(LOCAL_LIBS)

#%.prelink:
#	@echo "Pre-Linking $@ ..."
#	$(SILENT)$(BGQ_CROSS_LD) -Ur $(FW_LDFLAGS) -o $@ $^ $(FW_LIBS) $(LOCAL_LIBS)

%.objdump: %.elf
	@echo "Generating $@ ..."
	$(SILENT)$(BGQ_CROSS_OBJDUMP)  -dfhl $< > $*.objdump
#	$(SILENT)$(BGQ_CROSS_READELF)  $< > $*.readelf


S_SRC =  fw_start.S

C_SRC = fw_main.c \
	fw_bedram.c \
	fw_clockstop.c \
	fw_dcr_arbiter.c \
	fw_devbus.c \
	fw_envmon.c \
	fw_ms_genct.c \
	fw_testint.c \
	fw_pers.c \
	fw_l1p.c \
	fw_l2.c \
	fw_ddr.c \
	fw_fpu.c \
	fw_interrupts.c \
	fw_networks.c \
	fw_mbox.c \
	fw_mmu.c \
	fw_mu.c \
	fw_nd.c \
	fw_pcie.c \
	fw_serdes.c \
	fw_upc.c \
	fw_wu.c \
	fw_utils.c \
	fw_vsprintf.c \
	crc.c \
	fw_SimSupport.c \
	fw_reset.c \
	main.c

FW_LIB_OBJ = fw_start.o \
	fw_main.o \
	fw_clockstop.o \
	fw_dcr_arbiter.o \
	fw_devbus.o \
	fw_envmon.o \
	fw_ms_genct.o \
	fw_testint.o \
	fw_upc.o \
	fw_wu.o \
	fw_pers.o \
	fw_l1p.o \
	fw_l2.o \
	fw_ddr.o \
	fw_bedram.o \
	fw_fpu.o \
	fw_interrupts.o \
	fw_networks.o \
	fw_mbox.o \
	fw_mmu.o \
	fw_mu.o \
	fw_nd.o \
	fw_pcie.o \
	fw_serdes.o \
	fw_timebase.o \
	fw_utils.o \
	fw_vsprintf.o \
	fw_reset.o \
	crc.o \
	fw_SimSupport.o



FW_OBJ = $(FW_LIB_OBJ) main.o


FW_TARGET  = bgq_firmware.elf
FW_OBJDUMP = bgq_firmware.objdump
FWEXT_DDR_MAKE = fwext_ddr.make
FWEXT_DDR_INSTALL = fwext_ddr.install
FWEXT_DDR_CLEAN = fwext_ddr.clean
FW_LIB     = libfw.a

default:	all



$(BGQ_INSTALL_DIR)/$(BASENAME)/bin/$(FW_TARGET):	$(FW_TARGET)
	$(call bgq_install_bin,$<,$@)

$(BGQ_INSTALL_DIR)/$(BASENAME)/lib/$(FW_LIB):	$(FW_LIB)
	$(call bgq_install_bin,$<,$@)

install:	all $(BGQ_INSTALL_DIR)/$(BASENAME)/bin/$(FW_TARGET)  $(BGQ_INSTALL_DIR)/$(BASENAME)/lib/$(FW_LIB) $(FWEXT_DDR_INSTALL)
	@echo "Installed $(BASENAME) into $(BGQ_INSTALL_DIR)/$(BASENAME)"	

objdump:	$(FW_OBJDUMP)


all:	$(FW_TARGET) $(FWEXT_DDR_MAKE) $(FW_LIB)


$(FWEXT_DDR_MAKE):
	$(MAKE) --directory fwext_ddr all

$(FWEXT_DDR_INSTALL): $(FWEXT_DDR_MAKE)
	$(MAKE) --directory fwext_ddr install

$(FWEXT_DDR_CLEAN): 
	$(MAKE) --directory fwext_ddr clean

$(FW_TARGET):	$(FW_OBJ) $(FW_LDSCRIPT)

$(FW_OBJDUMP):	$(FW_TARGET)

$(FW_LIB): Makefile $(FW_LIB_OBJ)
	$(BGQ_CROSS_AR) crv  $(FW_LIB) $(FW_LIB_OBJ)

personalize:	$(FW_TARGET)
	../tools/svchost/svc_host $(parms) -BootLoader=$(FW_TARGET) -PatchedBootLoader=$(FW_TARGET).patched
	$(MV) $(FW_TARGET).patched $(FW_TARGET)

persinst:	personalize install

clean:	$(FWEXT_DDR_CLEAN)
	@echo "Cleaning $(BASENAME)"
	@-$(RM_F) *.o *.listing *.map *.objdump *.nm *.readelf *.elf *.prelink *.beam


distclean: clean
	@echo "Dist-cleaning $(BASENAME)"
	@-$(RM_F) ./Make.depend


depend::
	@echo "Making dependencies for $(BASENAME)"
	@-$(RM_F) ./Make.depend
	@$(BGQ_CROSS_CC) -M $(FW_CPU_FLAGS) $(FW_KFLAGS) $(FW_CFLAGS) $(FW_AFLAGS) $(S_SRC)    >  ./Make.depend
	@$(BGQ_CROSS_CC) -M $(FW_CPU_FLAGS) $(FW_KFLAGS) $(FW_CFLAGS)              $(C_SRC)    >> ./Make.depend
#	@$(BGQ_CROSS_CC) -M $(FW_CPU_FLAGS) $(FW_KFLAGS) $(FW_CFLAGS)              $(TEST_SRC) >> ./Make.depend


fresh:	clean depend install


-include Make.depend

