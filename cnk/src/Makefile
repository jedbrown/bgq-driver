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
# (C) Copyright IBM Corp.  2010, 2012                              
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

include ../../Make.rules

SILENT=@
#SILENT=

.PHONY : network/network.a fs/fs.a syscalls/syscalls.a libgpfs.a

BASENAME = cnk
RM_F = /bin/rm -f

BEAM_RESULTS += --beam::ipa=all --beam::data=$(BGQ_INSTALL_DIR)/cnk/beam

K_KFLAGS    = -D__KERNEL__
K_AFLAGS    = -D__ASSEMBLY__
K_INCLUDES  = -I$(BGQ_INSTALL_DIR) \
              -iquote $(BGQ_BUILD_DIR)/cnk/src \
	          -I$(BGQ_INSTALL_DIR)/spi/include/kernel/firmware
K_CPU_FLAGS = -m64 -g -O2 -Werror -MMD -MF .dep.$<.d

K_LDMAP     = -Map $(basename $@).map

K_LDS_NAME  = Kernel_DDR.lds

K_LDFLAGS  = -Bstatic $(K_LDMAP)
K_LDSCRIPT = --script $(K_LDS_NAME)

K_CFLAGS = -Wall $(LISTING_OPTIONS) $(K_INCLUDES) -fno-strict-aliasing -c

K_LIBS =
LOCAL_LIBS = network/network.a syscalls/syscalls.a fs/fs.a \
			 $(BGQ_INSTALL_DIR)/mudm/lib/cnk/mudm.a        \
			 $(BGQ_INSTALL_DIR)/spi/lib/libSPI_upci_fwcnk.a \
			 $(BGQ_INSTALL_DIR)/spi/lib/libSPI.a

ifeq ($(BGQ_HOST_OS),x86_64)
K_INCLUDES += -I/bgsys/bgq/drivers/x86_64.mambo/include
else
MAMBOFLAGS=
endif

ifeq ($(BEAM), 1)
OBJSUFFIX=beam
else
OBJSUFFIX=o
endif

%.$(OBJSUFFIX): %.c
	@echo "gcc: $<"
	$(SILENT)$(BGQ_CROSS_CC) $(K_CPU_FLAGS) $(K_KFLAGS) $(K_CFLAGS) $<

%.$(OBJSUFFIX): %.cc
	@echo "gcc: $<"
	$(SILENT)$(BGQ_CROSS_CXX) $(K_CPU_FLAGS) $(K_KFLAGS) $(K_CFLAGS) $<

%.$(OBJSUFFIX):	%.S
	@echo "gcc: $<"
	$(SILENT)$(BGQ_CROSS_CC) $(K_CPU_FLAGS) $(K_KFLAGS) $(K_CFLAGS) $(K_AFLAGS) -Os $<

%.elf:
	@echo "Linking Executable $@ ..."
	$(SILENT)$(BGQ_CROSS_LD) $(K_LDFLAGS) $(K_LDSCRIPT) -o $@ $^ -static -nostdlib $(LOCAL_LIBS)

%.objdump: %.elf
	@echo "Generating $@ ..."
	$(SILENT)$(BGQ_CROSS_OBJDUMP)  -dfhl $< > $*.objdump
#	$(SILENT)$(BGQ_CROSS_READELF)  $< > $*.readelf


S_SRC =	start.S

C_SRC =	entry.c \
		main.c \
		vmm.c \
		sched.c \
		kthread.c \
		core2core.c \
		timers.c \
		debug.c \
		syscalls.c \
		util.c \
		mmap.c \
		file.c \
		string.c \
		futex.c \
		ctype.c \
		lock.c \
		vsprintf.c \
		flih.c \
		rollback.c \
		speculation.c \
		signal.c \
		align.c \
		powermgmt.c \
		ndinthandler.c \
		flightlog.c \
		upc_kbridge.c \
		CrcExchange.c

CC_SRC = statictlb.cc elfimage.cc NodeController.cc AppSetup.cc AppCleanup.cc rank.cc ProcessSetup.cc guard.cc ToolControl.cc CoreDump.cc JobControl.cc mailbox.cc bgcios_parse.cc dumpthreadstate.cc 

K_OBJ  = $(patsubst %.S,%.$(OBJSUFFIX),$(S_SRC)) $(patsubst %.c,%.$(OBJSUFFIX),$(C_SRC))  $(patsubst %.cc,%.$(OBJSUFFIX),$(CC_SRC)) 

K_TARGET  = bgq_kernel.elf
K_OBJDUMP = bgq_kernel.objdump

default:	all


install:	all
	@echo "Installing $(BASENAME)"
	$(INSTALLCANFAIL)mkdir -p $(BGQ_INSTALL_DIR)/$(BASENAME)/bin
	$(INSTALLCANFAIL)mkdir -p $(BGQ_INSTALL_DIR)/$(BASENAME)/tools
	$(INSTALLCANFAIL)mkdir -p $(BGQ_INSTALL_DIR)/$(BASENAME)/lib
	$(INSTALLCANFAIL)cp -pf $(K_TARGET)   $(BGQ_INSTALL_DIR)/$(BASENAME)/bin
	$(INSTALLCANFAIL)cp -pf AppAgent0.lds $(BGQ_INSTALL_DIR)/$(BASENAME)/tools
	$(INSTALLCANFAIL)cp -pf AppAgent1.lds $(BGQ_INSTALL_DIR)/$(BASENAME)/tools
	$(INSTALLCANFAIL)cp -pf AppAgent2.lds $(BGQ_INSTALL_DIR)/$(BASENAME)/tools
	$(INSTALLCANFAIL)cp -pf AppAgent3.lds $(BGQ_INSTALL_DIR)/$(BASENAME)/tools
	$(INSTALLCANFAIL)cp -pf staticmapper  $(BGQ_INSTALL_DIR)/$(BASENAME)/tools
	$(INSTALLCANFAIL)cp -pf libgpfs.a     $(BGQ_INSTALL_DIR)/$(BASENAME)/lib
	$(INSTALLCANFAIL)cp -pf libgpfs.so    $(BGQ_INSTALL_DIR)/$(BASENAME)/lib

all:	$(K_TARGET) staticmapper libgpfs

libgpfs: libgpfs.c
	@echo "Creating libgpfs.a and libgpfs.so"
	$(SILENT)$(BGQ_CROSS_CC) $(K_CPU_FLAGS) $(K_CFLAGS) libgpfs.c -o libgpfs.o
	$(SILENT)$(BGQ_CROSS_AR) cr libgpfs.a libgpfs.o
	$(SILENT)$(BGQ_CROSS_CC) $(K_CPU_FLAGS) $(K_CFLAGS) -shared -fPIC libgpfs.c -o libgpfs.so

objdump:	$(K_OBJDUMP)

dosyscalls:
	@$(MAKE) -C ./syscalls

syscalls/syscalls.a:
	@$(MAKE) -C ./syscalls

dofs:
	@$(MAKE) -C ./fs

fs/fs.a:
	@$(MAKE) -C ./fs

donetwork:
	@$(MAKE) -C ./network

network/network.a:
	@$(MAKE) -C ./network

$(K_TARGET):	$(K_LIBS) $(K_OBJ) $(K_LDS_NAME) $(LOCAL_LIBS)

$(K_OBJDUMP):	$(K_TARGET)

staticmapper: Makefile statictlb.cc
	g++ statictlb.cc -o staticmapper -g -O2 $(K_KFLAGS) -Wall $(K_INCLUDES)  -DSTANDALONE -lpthread -m64

asmConstants.h: genAsmConstants.c
	@echo "Generating asmConstants.h"
	$(SILENT)$(BGQ_CROSS_CC) $(K_CPU_FLAGS) $(K_KFLAGS) $(K_INCLUDES) -S $<
	$(SILENT)awk ' \
	    BEGIN { \
		printf "// THIS FILE IS AUTOMATICALLY GENERATED\n"; \
		printf "// Modify genAsmConstants.c to make changes.\n"; \
		printf "\n"; \
		name = NULL; \
	    } \
	    /^ASM_CONSTANT_[a-zA-Z0-9_]*:$$/ \
		{name = substr($$1, 14, length($$1)-14)} \
	    /^	.quad/ \
	    	{if (name != NULL) { printf "#define %s %s\n", name, $$2; name = NULL}} \
	    /^	.zero/ \
	    	{if (name != NULL) { printf "#define %s 0\n", name; name = NULL}} \
	    ' genAsmConstants.s > asmConstants.h
	$(SILENT)sed -e '1s/genAsmConstants\.o/asmConstants.h/' \
	    .dep.genAsmConstants.c.d > tmp.d
	$(SILENT)mv tmp.d .dep.genAsmConstants.c.d

start.o: asmConstants.h

clean::
	@echo "Cleaning $(BASENAME)"
	@$(MAKE) -C ./fs clean
	@$(MAKE) -C ./syscalls clean
	@$(MAKE) -C ./network clean
	@-$(RM_F) *.o genAsmConstants.s asmConstants.h *.beam *.listing *.map *.objdump *.nm *.readelf *.elf staticmapper .dep.*.d

distclean: clean
	@echo "Dist-cleaning $(BASENAME)"
	@-$(RM_F) ./Make.depend

fresh:	clean depend install

-include .dep.*.d
