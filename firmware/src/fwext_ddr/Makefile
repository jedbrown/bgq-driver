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

include ../../../Make.rules

#SILENT=@
SILENT=

BASENAME = fwext_ddr
RM_F = /bin/rm -f

BEAM_RESULTS += --beam::ipa=all --beam::data=$(BGQ_INSTALL_DIR)/firmware/extbeam

FWEXT_KFLAGS    = -D__KERNEL__
FWEXT_AFLAGS    = -D__ASSEMBLY__
FWEXT_INCLUDES  = -I$(BGQ_INSTALL_DIR) \
		  -I$(BGQ_INSTALL_DIR)/spi/include/kernel/firmware -I..
FWEXT_CPU_FLAGS = -m64 -g -O2

FWEXT_LDMAP = -Map $(basename $@).map

FWEXT_LDSCRIPT = FWExtDDR.lds
##FWEXT_LDSCRIPT = $(BGQ_INSTALL_DIR)/firmware/include/FWExtDDR.lds
FWEXT_LDFLAGS  = -Bstatic $(FWEXT_LDMAP) --script $(FWEXT_LDSCRIPT)
FWEXT_CFLAGS = -D__FWEXT__ -Wall -Werror $(LISTING_OPTIONS) $(FWEXT_INCLUDES) -c

FWPRELINK_LDSCRIPT = FWExtPrelink.lds
FWPRELINK_LDFLAGS  = -Bstatic $(FWEXT_LDMAP) --script $(FWPRELINK_LDSCRIPT)

FWEXT_LIBS = -static -nostdlib

FWLIBINSTALL_DIR=$(BGQ_INSTALL_DIR)/firmware/lib/
FWINCINSTALL_DIR=$(BGQ_INSTALL_DIR)/firmware/include/

LOCAL_LIBS =
#LOCAL_LIBS = -L$(BGQ_INSTALL_DIR)/spi/lib -lSPI 

%.o: %.c
	@echo "gcc: $<"
	$(SILENT)$(BGQ_CROSS_CC) $(FWEXT_CPU_FLAGS) $(FWEXT_KFLAGS) $(FWEXT_CFLAGS) $< -o $*.o

%.o: %.cc
	@echo "gcc: $<"
	$(SILENT)$(BGQ_CROSS_CC) $(FWEXT_CPU_FLAGS) $(FWEXT_KFLAGS) $(FWEXT_CFLAGS) $< -o $*.o

%.o: %.S
	@echo "gcc: $<"
	$(SILENT)$(BGQ_CROSS_CC) $(FWEXT_CPU_FLAGS) $(FWEXT_KFLAGS) $(FWEXT_CFLAGS) $(FWEXT_AFLAGS) -Os $< -o $*.o

%.elf:
	@echo "Linking Executable $@ ..."
	#$(SILENT)$(BGQ_CROSS_LD) $(FWEXT_LDFLAGS) -o $@ $(FWEXT_PRELINK) $(filter %.o,$^) $(FWEXT_LIBS) $(LOCAL_LIBS)
	$(SILENT)$(BGQ_CROSS_LD) $(FWEXT_LDFLAGS) -o $@ $(filter %.o,$^) $(FWEXT_LIBS) $(LOCAL_LIBS)

%.prelink:
	@echo "Pre-Linking $@ ..."
	$(SILENT)$(BGQ_CROSS_LD) -Ur $(FWPRELINK_LDFLAGS) -o $@ $(filter %.o,$^) $(LOCAL_LIBS)

%.objdump: %.elf
	@echo "Generating $@ ..."
	$(SILENT)$(BGQ_CROSS_OBJDUMP)  -dfhl $< > $*.objdump
#	$(SILENT)$(BGQ_CROSS_READELF)  $< > $*.readelf


$(FWINCINSTALL_DIR)/%.h: %.h
	$(call bgq_install_hdr,$<,$@)


S_SRC =  fwext_start.S

C_SRC = fwext_main.c \
	fwext_util.c \
	fwext_vprintf.c \
	fwext_interrupts.c \
	fwext_lib.c \
	fwext_mu.c \
	fwext_nd.c \
	fwext_crcx.c \
	test_main.c \
	serdes_init.c \
	serdes_pcie_init.c \
	sd_helper.c \
	sd_edi.c \
	sd_rcb.c \
	core_pll.c \
	core_bit_tools.c \
	core_serdes_left_dcr.c \
	core_serdes_right_dcr.c \
	core_goals.c \
	core_checks.c \
	core_lanes.c \
	core_train.c \
	core_dcr_addons.c \
	core_sd_pcs.c \
	hss_init.c \
	core_assert_reset.c \
	fwext_stubs.c

FWEXT_PRELINK_OBJ = fwext_start.o \
	fwext_main.o \
	fwext_vprintf.o \
	fwext_interrupts.o \
	fwext_util.o \
	fwext_mu.o \
	fwext_nd.o \
	fwext_lib.o \
	fw_mu.o \
	fw_nd.o \
	crc.o \
	serdes_init.o \
	serdes_pcie_init.o \
	sd_ei.o \
	sd_helper.o \
	sd_rcb.o \
	core_pll.o \
	core_bit_tools.o \
	core_serdes_left_dcr.o \
	core_serdes_right_dcr.o \
	core_goals.o \
	core_checks.o \
	core_lanes.o \
	core_train.o \
	core_dcr_addons.o \
	core_sd_pcs.o \
	hss_init.o \
	core_assert_reset.o

FWEXT_LIB_OBJ = \
	fwext_lib.o \
	fwext_stubs.o

#
# so far, only  static linking of the object work, linking the prelink does not...
FWEXT_OBJ = $(FWEXT_PRELINK_OBJ) test_main.o
#FWEXT_OBJ = test_main.o


FWEXT_TARGET  = fwext.elf
FWEXT_PRELINK = fwext.prelink
FWEXT_OBJDUMP = fwext.objdump
FWEXT_LIB     = libfwext.a

default:	all


install:	all \
		$(FWLIBINSTALL_DIR)/$(FWEXT_PRELINK) \
		$(FWLIBINSTALL_DIR)/$(FWEXT_LIB) \
		$(FWINCINSTALL_DIR)/$(FWEXT_LDSCRIPT)



objdump:	$(FWEXT_OBJDUMP)


FW_PARTS = fw_mu.c fw_nd.c crc.c  _fw_mu.h _fw_nd.h

all:	$(FW_PARTS) $(FWEXT_TARGET) $(FWEXT_PRELINK) $(FWEXT_LIB)

$(FWEXT_LIB): Makefile $(FWEXT_LIB_OBJ)
	$(BGQ_CROSS_AR) crv  $(FWEXT_LIB) $(FWEXT_LIB_OBJ)

$(FWLIBINSTALL_DIR)/$(FWEXT_LIB): $(FWEXT_LIB)
	-$(call bgq_install_lib,$<,$@)

$(FWLIBINSTALL_DIR)/$(FWEXT_PRELINK): $(FWEXT_PRELINK)
	-$(call bgq_install_lib,$<,$@)


$(FWINCINSTALL_DIR)/$(FWEXT_LDSCRIPT): $(FWEXT_LDSCRIPT)
	-$(call bgq_install_lib,$<,$@)

$(FWEXT_TARGET):	$(FWEXT_OBJ) $(FWEXT_LDSCRIPT) $(FWEXT_PRELINK)

$(FWEXT_PRELINK):	$(FWEXT_PRELINK_OBJ) $(FWEXT_LDSCRIPT)

$(FWEXT_OBJDUMP):	$(FWEXT_TARGET)


fw_nd.o:	fw_nd.c

fw_nd.c:	../fw_nd.c
	cp ../fw_nd.c fw_nd.c

fw_mu.o:	fw_mu.c

fw_mu.c:	../fw_mu.c
	cp ../fw_mu.c fw_mu.c

crc.o:	crc.c

crc.c:	../crc.c
	cp ../crc.c crc.c

Firmware_RAS.h:	../Firmware_RAS.h
	cp ../Firmware_RAS.h .

_fw_mu.h:	../_fw_mu.h
	cp ../_fw_mu.h .

_fw_nd.h:	../_fw_nd.h
	cp ../_fw_nd.h .

clean::
	@echo "Cleaning $(BASENAME)"
	@-$(RM_F) *.o *.listing *.map *.objdump *.nm *.readelf *.elf *.prelink *.beam $(FW_PARTS)


distclean: clean
	@echo "Dist-cleaning $(BASENAME)"
	@-$(RM_F) ./Make.depend


depend::
	@echo "Making dependencies for $(BASENAME)"
	@-$(RM_F) ./Make.depend
	@$(BGQ_CROSS_CC) -M $(FWEXT_CPU_FLAGS) $(FWEXT_KFLAGS) $(FWEXT_CFLAGS) $(FWEXT_AFLAGS) $(S_SRC)    >  ./Make.depend
	@$(BGQ_CROSS_CC) -M $(FWEXT_CPU_FLAGS) $(FWEXT_KFLAGS) $(FWEXT_CFLAGS)                 $(C_SRC)    >> ./Make.depend
#	@$(BGQ_CROSS_CC) -M $(FWEXT_CPU_FLAGS) $(FWEXT_KFLAGS) $(FWEXT_CFLAGS)                 $(TEST_SRC) >> ./Make.depend


fresh:	clean depend install


-include Make.depend

