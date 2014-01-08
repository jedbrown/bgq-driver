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
                           
include ../../../../Make.rules
include ../../../Make.hlcs.rules

TOP_DIR=hlcs

# Location of the python scripts to build headers
XMLSCRIPT_DIR=$(BGQ_INSTALL_DIR)/xml/scripts

CXXFLAGS += $(DEBUG_FLAGS)
CXXFLAGS += -Wall -g -Wno-format-y2k
CXXFLAGS += -I$(BGQ_INSTALL_DIR)
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/utility/include
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/bgq_util/include
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/utility/include/cxxsockets
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/xml/include/library
CXXFLAGS += -I$(BGQ_INSTALL_DIR)/hlcs/include/c_api
CXXFLAGS += -fPIC

CXXFLAGS_64 += -m64

INSTALL_DIR=$(BGQ_INSTALL_DIR)/hlcs/include/c_api
BGMASTER_PROTOCOL_DIR=../../../include/master/protocol/
IDLC = $(XMLSCRIPT_DIR)/generateCode.py --dir $(INSTALL_DIR)
XML_DEFS = $(wildcard $(BGMASTER_PROTOCOL_DIR)*.xml)
C_API_H  = $(wildcard $(BGQ_INSTALL_DIR)/xml/include/c_api/*.h)
C_API_CC = $(addsuffix .cc, $(basename $(notdir $(XML_DEFS))))
LOCAL_H  = $(wildcard *h)
INSTALL_TARGETS = $(addprefix $(BGQ_INSTALL_DIR)/xml/include/c_api/, $(LOCAL_H))

compile-cmd = g++ $(CXXFLAGS) $1 -o $@ -c $<

default: all install

# explicit 64
ifneq ($(BITS),32)
%.o: %.cc
	$(call compile-cmd, $(CXXFLAGS_64))
else
%.o: %.cc
	$(call compile-cmd, $(CXXFLAGS))
endif

SRCS=$(C_API_CC)
OBJS64   = $(SRCS:%.cc=%.o)

api:
	@echo $(INSTALL_TARGETS)

xml:
	@echo $(XML_DEFS)

$(INSTALL_DIR):
	$(MKDIR) -p $(INSTALL_DIR)

$(INSTALL_TARGETS): $(LOCAL_H)
	$(CP) $(notdir $@) $@

install: $(INSTALL_DIR) $(INSTALL_TARGETS)

all: $(C_API_CC) $(OBJS64) 

TEST_LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib

$(C_API_CC): $(C_API_H) $(XML_DEFS)

%.cc: $(BGMASTER_PROTOCOL_DIR)/%.xml
	$(IDLC) --cimpl $(BGMASTER_PROTOCOL_DIR)/$(basename $@).xml

clean:
	rm -rf *o *o_64 *.d *~ BGMaster*[cch] \#* $(INSTALL_TARGETS)

#-include *.d
#-include .dep.*.64.d
