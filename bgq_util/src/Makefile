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
# (C) Copyright IBM Corp.  2011, 2011                              
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

MODULE_DIR=bgq_util
MODULE_INSTALL_DIR=$(BGQ_INSTALL_DIR)/$(MODULE_DIR)
MODULE_LIB_DIR=$(MODULE_INSTALL_DIR)/lib

TARGET_LIB           = libbgq_util.a
TARGET_LIB_32        = libbgq_util32.a
TARGET_LIB_64        = libbgq_util_s.a
TARGET_LIB_BQC       = libbgq_util_bqc.a
TARGET_LIB_XLC       = libbgq_utilxlc.a

SHARED_LIB_64_BASE 	 = bgq_util
SHARED_LIB_64_MAJOR_VERS = 1
SHARED_LIB_64_MINOR_VERS = 0
SHARED_LIB_64_MINOR_REV  = 0
SHARED_LIB_64_SHORT 	= lib$(SHARED_LIB_64_BASE).so
SHARED_LIB_64_VERS 	= $(SHARED_LIB_64_SHORT).$(SHARED_LIB_64_MAJOR_VERS)
SHARED_LIB_64_FULL 	= $(SHARED_LIB_64_VERS).$(SHARED_LIB_64_MINOR_VERS).$(SHARED_LIB_64_MINOR_REV)

TARGET_LIST = $(TARGET_LIB)



 DIRS =

OS=$(shell uname)
OS_ARCH := $(shell uname -m)


#
# call definitions to move to make .rules
.PHONY: (DIRS) install all default


#
# make these utilits accessable for use by fusion on awan.
#
#ifeq ($(OS),AIX)
#  CXX = xlC
#  CXXFLAGS += -q64 -qrtti -qinfo=cmp:dcl:ini:por:trd:uni -D__IOC_ANSI_STREAM=1
#else
#  CXXFLAGS += -g -fPIC
#endif

# on 64 bit x86 platforms, compile defaults to 64 bits, we need a 32 bit library
# for fusion...
ifeq  ($(OS_ARCH),x86_64)
  TARGET_LIST+=$(TARGET_LIB_32)
  TARGET_LIST+=$(TARGET_LIB_64)
  TARGET_LIST+=$(SHARED_LIB_64_SHORT)
  TARGET_LIST+=$(SHARED_LIB_64_VERS)
  TARGET_LIST+=$(SHARED_LIB_64_FULL)
  TARGET_LIST+=$(TARGET_LIB_BQC)
  CXXFLAGS_64 += -m64
  CXXFLAGS_32 += -m32
endif

ifeq ($(OS_ARCH),ppc64)
  CXXFLAGS += -m64
  LDFLAGS += -m64
  TARGET_LIST+=$(SHARED_LIB_64_SHORT)
  TARGET_LIST+=$(SHARED_LIB_64_VERS)
  TARGET_LIST+=$(SHARED_LIB_64_FULL)
  TARGET_LIST+=$(TARGET_LIB_BQC)
endif

ifeq ($(OS_ARCH),i686)
  TARGET_LIST+=$(SHARED_LIB_64_SHORT)
  TARGET_LIST+=$(SHARED_LIB_64_VERS)
  TARGET_LIST+=$(SHARED_LIB_64_FULL)
endif

ifeq ($(OS),AIX)
  TARGET_LIST+=$(TARGET_LIB_XLC)
  CXX_XLC = xlC
  CXXFLAGS_XLC = -q64
  CXXFLAGS_XLC += -qrtti -qinfo=cmp:dcl:ini:por:trd:uni -D__IOC_ANSI_STREAM=1
  CXXFLAGS_XLC += -I$(BGQ_EXPAT_PATH)/include
  CXXFLAGS_XLC += -I$(BGQ_INSTALL_DIR)/bgq_util/include
  CXXFLAGS_XLC += -I$(BGQ_INSTALL_DIR)

  AR := $(AR)
endif

# If building control system simulation (on x86_64),
# only build the native libraries (not bg).
ifeq ($(BGQ_CONTROL_SYSTEM_SIMULATION),yes)
  TARGET_LIST=$(TARGET_LIB)
  TARGET_LIST+=$(SHARED_LIB_64_SHORT)
  TARGET_LIST+=$(SHARED_LIB_64_VERS)
  TARGET_LIST+=$(SHARED_LIB_64_FULL)
endif


CXXFLAGS += -Wall
CXXFLAGS += -g
CXXFLAGS += -O3
CXXFLAGS += -MMD
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/bgq_util/include
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/extlib/include
CXXFLAGS += -g -fPIC

INSTALL_LIST=$(addprefix ${MODULE_LIB_DIR}/, ${TARGET_LIST})


# for optimization and debugging
 CXXFLAGS += -I$(BGQ_BUILD_DIR)/bgq_util/include
 CXXFLAGS += -I$(BGQ_INSTALL_DIR)
# default for location of boot images
 CXXFLAGS += -DBOOT_IMAGE_DIRECTORY="\"$(BGQ_BIN_INSTALL_DIR)\""




compile-cmd = $(CXX) $(CXXFLAGS) $1 -o $@ -c $<

# cross-compile command for bqc objects
cross-compile-cmd = $(BGQ_CROSS_CXX) $(CXXFLAGS) $1 -o $@ -c $<

# xlc comile for awan..
xlccompile-cmd = $(CXX_XLC) $(CXXFLAGS_XLC) $1 -o $@ -c $<


# do native compile
%.o: %.cc
	$(call compile-cmd,)

%.o: %.c
	$(call compile-cmd, -fno-exceptions)

# explicit 64
%.o_64: %.cc
	$(call compile-cmd, $(CXXFLAGS_64))

%.o_64: %.c
	$(call compile-cmd, $(CXXFLAGS_64) -fno-exceptions)

# explicit 64
%.o_32: %.cc
	$(call compile-cmd, $(CXXFLAGS_32))

%.o_32: %.c
	$(call compile-cmd, $(CXXFLAGS_32) -fno-exceptions)


# explicit 64
%.o_xlc: %.cc
	$(call xlccompile-cmd, )

%.o_xlc: %.c
	$(call xlccompile-cmd, )

%.o_bqc: %.cc
	$(call cross-compile-cmd, $(CXXFLAGS_64))

%.o_bqc: %.c
	$(call cross-compile-cmd, $(CXXFLAGS_64) -fno-exceptions)


#
# library copy rule
#
$(MODULE_LIB_DIR)/% : ./%
	$(call bgq_install_lib,$<,$@)


SRCS = $(wildcard *.cc)
SRCSC = $(wildcard *.c)


#SRCS  = ArbitraryLengthNumber.cc
#SRCS += BgEcc.cc
#SRCS += BgL2Slice.cc
#SRCS += BgNDReduce.cc
#SRCS += BgParm.cc
#SRCS += Elf32File.cc
#SRCS += Elf64File.cc
#SRCS += Elf64SymbolTableMap.cc
#SRCS += JtagTapDecoder.cc
#SRCS += Location.cc
#SRCS += Log.cc
#SRCS += string_tokenizer.cc

OBJS     = $(SRCS:%.cc=%.o)     $(SRCSC:%.c=%.o)
OBJS32   = $(SRCS:%.cc=%.o_32)  $(SRCSC:%.c=%.o_32)
OBJS64   = $(SRCS:%.cc=%.o_64)  $(SRCSC:%.c=%.o_64)
OBJSBQC  = $(SRCS:%.cc=%.o_bqc) $(SRCSC:%.c=%.o_bqc)
OBJSXLC  = $(SRCS:%.cc=%.o_xlc) $(SRCSC:%.c=%.o_xlc)

# Exclude boost-dependent objects from bqc list
OBJSBQC := $(filter-out Location.o%,$(OBJSBQC))

ifeq ($(OS),AIX)
OBJS := $(filter-out Location.o Log.o BgNDReduce.o BgNDReduce_C.o BgNDReduce_C_verify.o,$(OBJS))
OBJSXLC := $(filter-out Location.o% Log.o% BgNDReduce.o% BgNDReduce_C.o% BgNDReduce_C_verify.o%,$(OBJSXLC))
endif

#
# exclude these objects for aix build,
# they are not referenced in fusion
# and maybe not in the diag library..
# so maybe we can avoid the boost library in aix
#

default: all

all:  install

$(SHARED_LIB_64_SHORT): $(SHARED_LIB_64_VERS)
	ln -sf $< $@

$(SHARED_LIB_64_VERS): $(SHARED_LIB_64_FULL)
	ln -sf $< $@

$(SHARED_LIB_64_FULL): Makefile $(OBJS64)
	$(CXX) $(CXXFLAGS_64) -shared $(OBJS64) -pthread -Wl,-soname,$(SHARED_LIB_64_VERS) -o $@ $(LDFLAGS)

$(TARGET_LIB_64): Makefile $(OBJS64)
	$(AR) crv  $(TARGET_LIB_64) $(OBJS64)

$(TARGET_LIB_XLC): Makefile $(OBJSXLC)
	$(AR) -X64 crv  $(TARGET_LIB_XLC) $(OBJSXLC)

$(TARGET_LIB_32): Makefile $(OBJS32)
	$(AR) crv  $(TARGET_LIB_32) $(OBJS32)

$(TARGET_LIB): Makefile $(OBJS)
	$(AR) crv  $(TARGET_LIB) $(OBJS)

$(TARGET_LIB_BQC): Makefile $(OBJSBQC)
	$(RM) -f s_nextafter.o s_nextafter.o_bqc
	$(AR) xv $(BGQ_CROSS_TOOLS_DIR)/$(BGQ_CROSS_TARGET_ARCH)/lib/libm.a s_nextafter.o
	$(MV) s_nextafter.o s_nextafter.o_bqc
	$(AR) crv  $(TARGET_LIB_BQC) $(OBJSBQC) s_nextafter.o_bqc

${MODULE_LIB_DIR}/${TARGET_LIB_64}: ${TARGET_LIB_64}

${MODULE_LIB_DIR}/${TARGET_LIB_32}: ${TARGET_LIB_32}

${MODULE_LIB_DIR}/${TARGET_LIB}: ${TARGET_LIB}


install: $(INSTALL_LIST)

#ifeq ($(OS_ARCH),ppc64)
#	/sbin/ldconfig -n $(MODULE_LIB_DIR)
#	rm -f $(MODULE_LIB_DIR)/libbgq_util.so
#	ln -s $(MODULE_LIB_DIR)/libbgq_util.so.1 $(MODULE_LIB_DIR)/libbgq_util.so
#endif
#ifeq ($(OS_ARCH),i686)
#	/sbin/ldconfig -n $(MODULE_LIB_DIR)
#	rm -f $(MODULE_LIB_DIR)/libbgq_util.so
#	ln -s $(MODULE_LIB_DIR)/libbgq_util.so.1 $(MODULE_LIB_DIR)/libbgq_util.so
#endif


clean:
	-rm -rf *.o *.o_64 *.a *.o_32 *.o_bqc *.o_xlc *.d  ArbitraryLengthNumberTest BGResourceContainerTest $(TARGET_LIST)

distclean: clean
	-rm -f *.vpwhistu
	-rm *.vtg

test: install
	$(MAKE) -C test $@

print:
	@echo TARGET_LIST=${TARGET_LIST}
	@echo INSTALL_LIST=${INSTALL_LIST}
	@echo MODULE_LIB_DIR=${MODULE_LIB_DIR}
	@echo OBJS=$(OBJS)
	@echo OBJSXLC=$(OBJSXLC)

#include our dependency rules
-include *.d
