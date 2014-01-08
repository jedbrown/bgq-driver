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
# (C) Copyright IBM Corp.  2012, 2012                              
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
include ../../Make.gcc.rules

SILENT = 


# find source in parent dir
VPATH = ..

TARGET_STATIC_LIB = libbgpm_nonopt.a 

C_SRC 	= $(wildcard ../*.c)
CXX_SRC	= $(wildcard ../*.cc)
LIB_OBJS := $(notdir $(patsubst %.c,%.o,$(C_SRC)) $(patsubst %.cc,%.o,$(CXX_SRC)))

#BTMALLOC_PATH = /bgusr/krisd/bgq_test/tools/BackTraceMalloc
#CFLAGS   += -DINCL_BTMALLOC -I$(BTMALLOC_PATH)
#CXXFLAGS += -DINCL_BTMALLOC -I$(BTMALLOC_PATH)

CFLAGS   += -DENABLE_UPC_DD1_WORKAROUNDS 
CXXFLAGS += -DENABLE_UPC_DD1_WORKAROUNDS 
CXXFLAGS += -fno-exceptions -fno-rtti # -Weffc++
CFLAGS    := $(subst -O2,,$(CFLAGS))
CXXFLAGS  := $(subst -O2,,$(CXXFLAGS))
CFLAGS    := $(subst -Werror,,$(CFLAGS))
CXXFLAGS  := $(subst -Werror,,$(CXXFLAGS))


SUBDIRS += $(BGQ_BUILD_DIR)/bgpm/include
       
SUBLIB = $(BGQ_INSTALL_DIR)/spi/lib/libSPI_upci_cnk.a    
TARGET_DIR = $(BGQ_INSTALL_DIR)/bgpm/lib


.PHONY: default all install clean distclean subdirs $(SUBDIRS)


install:	all


all:	subdirs $(TARGET_STATIC_LIB)


$(TARGET_DIR) : 
	$(MKDIR) -p $(TARGET_DIR)

$(TARGET_STATIC_LIB):	$(LIB_OBJS) $(TARGET_DIR) $(BGQ_INSTALL_DIR)/bgpm/lib/$(DETAIL_TARGET) $(SUBLIB)
	@echo making library
	$(BGQ_CROSS_AR) xv $(SUBLIB)
	$(AR) $(TARGET_STATIC_LIB) *.o
	$(CP) $(TARGET_STATIC_LIB) $(TARGET_DIR)


clean:
	-$(RM) *.o
	-$(RM) .dep*
	-$(RM) *.a

distclean:clean


subdirs:  $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) --directory $@


ifeq ($(filter distclean clean,$(MAKECMDGOALS)),)	
-include .dep.*.d
endif
	
