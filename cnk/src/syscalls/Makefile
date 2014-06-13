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
include ../../../Make.rules

SILENT=@
#SILENT=

BEAM_RESULTS += --beam::ipa=all --beam::data=$(BGQ_INSTALL_DIR)/cnk/beam

BASENAME = /bin/basename
RM_F = /bin/rm -f

MODULE_DIR=bgq_util
MODULE_INSTALL_DIR=$(BGQ_INSTALL_DIR)/$(MODULE_DIR)
MODULE_LIB_DIR=$(MODULE_INSTALL_DIR)/lib

K_KFLAGS    = -D__KERNEL__
K_AFLAGS    = -D__ASSEMBLY__
K_INCLUDES  = -iquote $(BGQ_BUILD_DIR)/cnk/src \
              -I$(BGQ_INSTALL_DIR) \
	      -I$(BGQ_INSTALL_DIR)/spi/include/kernel/firmware
K_CPU_FLAGS = -m64 -g -O2

K_CFLAGS = -Wall -Werror -MMD -MF .dep.$<.d $(LISTING_OPTIONS) $(K_INCLUDES) -c

ifeq ($(notdir $(BGQ_CROSS_CC)),bgclang)
K_CFLAGS += -fno-builtin
endif

ifeq ($(USE_CLANG), 1)
BGQ_CROSS_CC      = /bgsys/drivers/toolchain/llvm/bin/bgclang
BGQ_CROSS_CXX     = /bgsys/drivers/toolchain/llvm/bin/bgclang++
endif

ifeq ($(CLANG_ANALYZER), 1)
K_CFLAGS += --analyze -Qunused-arguments
INSTALLCANFAIL = -
endif

CCSUFFIX=$(notdir $(BGQ_CROSS_CC))
CXXSUFFIX=$(notdir $(BGQ_CROSS_CXX))

ifeq ($(BEAM), 1)
OBJSUFFIX=beam
else
OBJSUFFIX = o
endif

ifeq ($(BGQ_HOST_OS),x86_64)
MAMBOFLAGS=-fno-rtti -I/bgsys/bgq/drivers/x86_64.mambo/include -DENABLE_MAMBOIO
else
MAMBOFLAGS=
endif

%.$(OBJSUFFIX): %.c
	@echo "$(CCSUFFIX): "`$(BASENAME) $$PWD`"/"`$(BASENAME) $<`
	$(SILENT)$(BGQ_CROSS_CC) $(K_CPU_FLAGS) $(K_KFLAGS) $(K_CFLAGS) $<

%.$(OBJSUFFIX): %.cc
	@echo "$(CXXSUFFIX): "`$(BASENAME) $$PWD`"/"`$(BASENAME) $<`
	$(SILENT)$(BGQ_CROSS_CXX) $(K_CPU_FLAGS) $(K_KFLAGS) $(K_CFLAGS) $(MAMBOFLAGS) $<

%.$(OBJSUFFIX):	%.S
	@echo "$(CCSUFFIX): "`$(BASENAME) $$PWD`"/"`$(BASENAME) $<`
	$(SILENT)$(BGQ_CROSS_CC) $(K_CPU_FLAGS) $(K_KFLAGS) $(K_CFLAGS) $(K_AFLAGS) -Os $< 

S_SRC =	

C_SRC =	$(wildcard *.cc) $(wildcard *.c)
K_OBJ =	$(addsuffix .$(OBJSUFFIX), $(notdir $(basename $(C_SRC))))

K_TARGET = syscalls.a

default:	all


install:	all
	@echo "Installing `$(BASENAME) $$PWD`"


all:	$(K_TARGET)


$(K_TARGET): $(K_OBJ) 
	$(INSTALLCANFAIL)$(BGQ_CROSS_AR) crv syscalls.a $(K_OBJ)

clean::
	@echo "Cleaning `$(BASENAME) $$PWD`"
	@-$(RM_F) *.o *.beam *.listing *.map *.objdump *.nm *.readelf *.elf *.a .dep.*.d


distclean: clean
	@echo "Dist-cleaning `$(BASENAME) $$PWD`"
	@-$(RM_F) ./Make.depend

fresh:	clean depend install

-include .dep.*.d
