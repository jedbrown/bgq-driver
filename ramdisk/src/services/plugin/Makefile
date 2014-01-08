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
include ../Make.rules

.PHONY: clean tags

# Set compiler flags
CXXFLAGS += -fPIC

do_build=no
ifeq ($(OS_ARCH),ppc64)
do_build=yes
endif


# Source files
ifeq ($(do_build),yes)
SRCS  := Plugx.cc

endif

# Object files
OBJS  = $(SRCS:%.cc=%.o_64)

LDFLAGS  += -L $(IOS_LIBRARY_DIR)
ifeq ($(do_build),yes)
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/ramdisk/initramfs/usr/lib64
LDFLAGS  += -libverbs -lrdmacm
endif

lib_base       = Plugx
lib_major_vers = 1
lib_minor_vers = 0
lib_minor_rev  = 0
lib_short      = lib$(lib_base).so
lib_vers       = $(lib_short).$(lib_major_vers)
lib_full       = $(lib_vers).$(lib_minor_vers).$(lib_minor_rev)

all: install

default: install

lib_objs=$(OBJS)

$(lib_full): $(lib_objs)
	$(CXX) -shared $(CXXFLAGS) $(LDFLAGS) $^ -o $@

$(lib_vers): $(lib_full)
	ln -sf $< $@

$(lib_short): $(lib_vers)
	ln -sf $< $@

objdump: $(lib_full)
	$(OBJDUMP) $< > $<.objdump

ifeq ($(do_build),yes)
install: $(lib_short)
	$(MKDIR) -p $(IOS_LIBRARY_DIR)
	$(CP) -P $(lib_short) $(lib_vers) $(lib_full) $(IOS_LIBRARY_DIR)
	$(MKDIR) -p $(BGQ_INITRAMFS_PATH)/lib64
	$(CP) -P $(lib_short) $(lib_vers) $(lib_full) $(BGQ_INITRAMFS_PATH)/lib64
else
install: $(lib_short)
endif

clean:
	$(RM) $(OBJS)
	$(RM) $(lib_short) $(lib_vers) $(lib_full)
	$(RM) *.d

distclean: clean
	$(RM) $(IOS_LIBRARY_DIR)/$(lib_full)
	$(RM) $(IOS_LIBRARY_DIR)/$(lib_vers)
	$(RM) $(IOS_LIBRARY_DIR)/$(lib_short)

-include *.d

