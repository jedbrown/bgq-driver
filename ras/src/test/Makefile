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
include ../../../Make.rules
include ../../Make.ras.rules

lib_test  = libRasTest
lib_short = $(lib_test).so
lib_vers  = $(lib_short).1
lib_full  = $(lib_vers).0.0

DECODER_SRCS = testDecoder.cc
DECODER_OBJS = ${DECODER_SRCS:%.cc=%.$(obj_ext)}

CXXFLAGS += -Wall
CXXFLAGS += -fPIC
CXXFLAGS += -MD
CXXFLAGS += -g
CXXFLAGS += -I$(BGQ_INSTALL_DIR)
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/ras/include
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/bgq_util/include
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/utility/include
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/extlib/include

OS=$(shell uname)
ifeq ($(OS),Linux)
LDFLAGS += -L $(BGQ_LIB_INSTALL_DIR) -lras
LDFLAGS += -Wl,-rpath,$(BGQ_LIB_INSTALL_DIR)
LDFLAGS += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib/
LDFLAGS += -L $(BGQ_INSTALL_DIR)/extlib/lib -llog4cxx -Wl,-rpath,$(BGQ_INSTALL_DIR)/extlib/lib/
LDFLAGS2 = $(LDFLAGS)
LDFLAGS2 += -shared
LDFLAGS2 += -Wl,-soname,$(SOLIB_VERS)
endif

ifeq ($(OS),AIX)
LDFLAGS  = -pthread
LDFLAGS += $(BGQ_INSTALL_DIR)/ras/lib/libras.a
LDFLAGS += $(BGQ_INSTALL_DIR)/xml/lib/libXML.a
LDFLAGS += $(BGQ_EXPAT_PATH)/lib/libexpat.a
endif

TESTERS   = tester
TESTERS  += testMailbox
TESTERS  += id_test
#TESTERS   = kernel_ras_decode
SRC       = $(addsuffix .cc,$(TESTERS))

all: install

install: $(TESTERS) $(lib_short) ras_scan
	$(MKDIR) -p $(decoder_install_dir)
	$(CP) $(wildcard *.xml) $(decoder_install_dir)
	$(CP) -P $(lib_short) $(lib_vers) $(lib_full) $(decoder_install_dir)

test: $(TESTERS)
	BG_PROPERTIES_FILE=$(BGQ_INSTALL_DIR)/utility/etc/bg.properties.tpl ./tester

$(TESTERS): $(SRC:%.cc=%.$(obj_ext))
	$(CXX) -o $@ $@.$(obj_ext) $(LDFLAGS)

$(lib_short): $(lib_vers)
	ln -sf $< $@

$(lib_vers): $(lib_full)
	ln -sf $< $@

$(lib_full): $(DECODER_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS2)

clean:
	$(RM) $(TESTER)
	$(RM) *.$(obj_ext)
	$(RM) *.d
	$(RM) $(lib_full) $(lib_vers) $(lib_short)

distclean: clean

-include *.d

ras_scan: 
	$(BGQ_BUILD_DIR)/scripts/rasScan.py '$(BGQ_BUILD_DIR)/ras' '$(BGQ_INSTALL_DIR)/ras/ras/metadata' 'test_ras.xml' '$(BGQ_INSTALL_DIR)/ras/include/ras.xml'
