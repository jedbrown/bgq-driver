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

include ../../../Make.rules

MODULE_DIR=xml
MODULE_INSTALL_DIR=$(BGQ_INSTALL_DIR)/$(MODULE_DIR)
MODULE_LIB_DIR=$(MODULE_INSTALL_DIR)/lib

OS := $(shell uname)
OS_ARCH = $(shell uname -m)

#IDLC            = ../xidl/xidlMain.py
IDLC            = ../../include/scripts/generateCode.py
XML_DEFS_DIR    = ../defs
XML_DEFS        = $(wildcard $(XML_DEFS_DIR)/*.xml)
XMLLIB_INST_INC = $(BGQ_INSTALL_DIR)/xml/include/c_api

KEEP            = XML.cc base64.cc
C_API_CC        = $(addsuffix .cc, $(basename $(notdir $(XML_DEFS))))
C_FILES         = $(KEEP) $(C_API_CC)
OBJS            = $(patsubst %.cc, %.o, $(C_FILES))
OBJS32          = $(patsubst %.cc, %.o_32, $(C_FILES))
OBJS64          = $(patsubst %.cc, %.o_64, $(C_FILES))
OBJSXLC         = $(patsubst %.cc, %.o_xlc, $(C_FILES))

LIB     = libXML.a
LIB32   = libXML32.a
LIB64   = libXML_s.a
LIBXLC  = libXML_xlc.a

SOLIB_DEV = libXML.so
SOLIB_VERS = $(SOLIB_DEV).1
SOLIB_FULL = $(SOLIB_VERS).0.1

ifeq ($(OS_ARCH),ppc64)
SOLIB_OBJS = $(OBJS64)
else
SOLIB_OBJS = $(OBJS)
endif

INC_PATH=-I$(BGQ_INSTALL_DIR)


ifeq ($(OS),AIX)
  CXX_XLC = xlC
  CXXFLAGS_XLC = -q64 -qrtti -qinfo=cmp:dcl:ini:por:trd:uni -D__IOC_ANSI_STREAM=1
  CXXFLAGS_XLC += -I$(BGQ_EXPAT_PATH)/include
  CXXFLAGS_XLC += $(INC_PATH)
  CXXFLAGS     += -I$(BGQ_EXPAT_PATH)/include
else
  CXXFLAGS += -O2
  CXXFLAGS += -g -fPIC
  CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/extlib/include
  SOLIB_LDFLAGS += -Wl,--enable-new-dtags # use DT_RUNPATH
endif


SOLIB_LDFLAGS +=\
 -shared \
 -Wl,-soname,$(SOLIB_VERS)

# on ppc64, only build 64-bit library
ifeq ($(OS_ARCH),ppc64)
SOLIB_LDFLAGS += -m64
LIB_LDFLAGS += -m64
CXXFLAGS += -m64
endif

LIB_LDFLAGS  += -lexpat -shared -Wl,-soname,


CXXFLAGS += $(INC_PATH)
CXXFLAGS += $(DEBUG_FLAGS) 

# on 64 bit x86 platforms, compile defaults to 64 bits, we need a 32 bit static library
# for fusion...
ifneq ($(OS),AIX)
  TARGET_LIST   += $(SOLIB_DEV)
  TARGET_LIST   += $(SOLIB_VERS)
  TARGET_LIST   += $(SOLIB_FULL)
endif
ifeq ($(OS_ARCH),x86_64)
  TARGET_LIST += $(LIB)
  TARGET_LIST += $(LIB32)
  TARGET_LIST += $(LIB64)		# build an explicit 64 bit library
endif
ifeq ($(OS_ARCH),ppc64) 
  TARGET_LIST += $(LIB64)
endif
ifeq ($(OS),AIX)
  TARGET_LIST += $(LIB)
  TARGET_LIST += $(LIBXLC)
endif

INSTALL_LIST = $(addprefix $(MODULE_LIB_DIR)/,$(TARGET_LIST))

echo OS_ARCH=$(OS_ARCH)

default: install 

install: $(INSTALL_LIST) 

$(MODULE_LIB_DIR)/%.a: %.a
	$(call bgq_install_lib,$<,$@)

$(MODULE_LIB_DIR)/%.so: %.so
	$(call bgq_install_lib,$<,$@)

$(MODULE_LIB_DIR)/%.so.1.0.1: %.so.1.0.1
	$(call bgq_install_lib,$<,$@)

$(MODULE_LIB_DIR)/$(SOLIB_DEV): $(SOLIB_DEV)
	$(RM) $@
	$(MKDIR) -p $(dir $@)
	ln -s $(SOLIB_VERS) $@

$(MODULE_LIB_DIR)/$(SOLIB_VERS): $(SOLIB_VERS)
	$(RM) $@
	$(MKDIR) -p $(dir $@)
	ln -s $(SOLIB_FULL) $@

$(LIB): $(OBJS)
	$(SILENT)$(AR) rcs $(LIB) $(OBJS)

$(LIB32): $(OBJS32)
	 $(SILENT)$(AR) rcs $(LIB32) $(OBJS32)

$(LIBXLC): $(OBJSXLC)
	 $(SILENT)$(AR) -X64 rcs $(LIBXLC) $(OBJSXLC)

$(LIB64): $(OBJS64)
	 $(SILENT)$(AR) rcs $(LIB64) $(OBJS64)

$(SOLIB_DEV): $(SOLIB_VERS)
	$(RM) $@
	$(LN) -s $< $@

$(SOLIB_VERS): $(SOLIB_FULL)
	$(RM) $@
	$(LN) -s $< $@

$(SOLIB_FULL): $(SOLIB_OBJS)
	$(CXX) -o $@ $^ $(SOLIB_LDFLAGS)

$(C_API_CC): 

%.cc: $(XML_DEFS_DIR)/%.xml
#	$(IDLC) --cimpl ../defs/$(basename $@).xml > $@
	$(IDLC) --cimpl ../defs/$(basename $@).xml

%.o: %.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

%.o_32: %.cc
	$(CXX) $(CXXFLAGS) -m32 -o $@ -c $<

%.o_64: %.cc
	$(CXX) $(CXXFLAGS) -m64 -o $@ -c $<

%.o_xlc: %.cc
	$(CXX_XLC) $(CXXFLAGS_XLC) -o $@ -c $<

clean: 
	$(RM) $(filter-out $(KEEP), $(wildcard *.cc))
	$(RM) *.o *.o_32 *.o_64 *.o_xlc *.a *.h $(SOLIB_DEV) $(SOLIB_VERS) $(SOLIB_FULL) dep.files

distclean: clean
	-rm -f *.vpwhistu
	-rm -f *.vtg 

ifeq ($(filter distclean clean,$(MAKECMDGOALS)),)	
-include .dep.*.d
endif

print: 
	@echo INSTALL_LIST=$(INSTALL_LIST)
	@echo TARGET_LIST=$(TARGET_LIST)
	@echo OBJS=$(OBJS)
	@echo OBJS64=$(OBJS64)
	@echo INSTALL_LIST=$(INSTALL_LIST)
