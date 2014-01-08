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
# (C) Copyright IBM Corp.  2010, 2011                              
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

include ../Make.ras.rules
include ../../Make.rules

OS=$(shell uname)

lib_base       = ras
lib_major_vers = 1
lib_minor_vers = 0
lib_minor_rev  = 0
lib_short      = lib$(lib_base).so
lib_vers       = $(lib_short).$(lib_major_vers)
lib_full       = $(lib_vers).$(lib_minor_vers).$(lib_minor_rev)
lib_static     = lib$(lib_base).a

ifeq ($(OS),AIX)
INSTALL_LIST   = $(call INSTALL_LIB_TARGET,$(lib_static))
else
INSTALL_LIST   = $(call INSTALL_LIB_TARGET,$(lib_short) $(lib_vers) $(lib_full) $(lib_static))
endif

SRCS  = $(wildcard metadata/*.cc) 
SRCS += $(wildcard handler/*.cc)
SRCS += $(wildcard event/*.cc)

OBJS =${SRCS:%.cc=%.$(obj_ext)}

LDFLAGS  += -shared
LDFLAGS  += -Wl,-soname,$(lib_vers)
LDFLAGS  += -L$(BGQ_INSTALL_DIR)/xml/lib -lXML
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/xml/lib
LDFLAGS  += -lexpat -ldl

CXXFLAGS += -iquote $(include_install_dir)
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/xml/include
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/bgq_util/include
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -iquote $(BGQ_BUILD_DIR)/ras/src
CXXFLAGS += -fPIC
CXXFLAGS += -MD
CXXFLAGS += -g
CXXFLAGS += -O2
CXXFLAGS += -Wall
CXXFLAGS += -Wconversion
CXXFLAGS += -Wextra
CXXFLAGS += -Wformat
CXXFLAGS += -Woverloaded-virtual
CXXFLAGS += -Wundef

LIBS = $(lib_static)

ifeq ($(OS),Linux)
LDFLAGS  += -Wl,--enable-new-dtags # sets DT_RUNPATH
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS  += -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/utility/include
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/extlib/include
LIBS += $(lib_short)
EXTLIBS  = $(lib_vers)
EXTLIBS += $(lib_full)
endif

ifeq ($(OS),AIX)
CXXFLAGS += -I$(BGQ_EXPAT_PATH)/include
endif

.PHONY: event handler metadata test xml install all 

all: install
ifneq ($(OS),AIX)
	$(MAKE) --directory test $@
endif


$(lib_short): $(lib_vers)
	ln -sf $< $@

$(lib_vers): $(lib_full)
	ln -sf $< $@

$(lib_full): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(lib_static): Makefile $(OBJS)
	$(AR) crv  $@ $(OBJS)

install: $(INSTALL_LIST)
	@$(MKDIR) -p $(etc_install_dir)
	@$(CP) $(wildcard xml/*.xml) $(etc_install_dir)

clean:
	$(RM) metadata/*.d
	$(RM) handler/*.d
	$(RM) event/*.d
	$(RM) metadata/*.$(obj_ext)
	$(RM) handler/*.$(obj_ext)
	$(RM) event/*.$(obj_ext)
	$(RM) $(lib_full) $(lib_vers) $(lib_short)
	$(MAKE) --directory test $@

distclean: clean
	$(RM) -r $(MODULE_LIB_DIR)
	$(MAKE) --directory test $@

print:
	@echo INSTALL_LIST=${INSTALL_LIST}

-include */*.d
