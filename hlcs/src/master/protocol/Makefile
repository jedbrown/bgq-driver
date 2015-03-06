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

# Location of the python scripts to build headers
XMLSCRIPT_DIR=$(BGQ_INSTALL_DIR)/xml/scripts

CXXFLAGS += -Wall -Wno-format-y2k
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -O2
CXXFLAGS += -fpic
CXXFLAGS += -MD -MP

INSTALL_DIR=$(BGQ_INSTALL_DIR)/hlcs/include/c_api
BGMASTER_PROTOCOL_DIR=../../../include/master/protocol/
IDLC = $(XMLSCRIPT_DIR)/generateCode.py --dir .
XML_DEFS = $(wildcard *.xml)
C_API_H  = $(addsuffix .h, $(basename $(notdir $(XML_DEFS))))
C_API_CC = $(addsuffix .cc, $(basename $(notdir $(XML_DEFS))))

default: all install

SRCS=$(C_API_CC)
OBJS   = $(SRCS:%.cc=%.o)

xml:
	@echo $(XML_DEFS)

$(INSTALL_DIR):
	$(MKDIR) -p $(INSTALL_DIR)

install: all

all: $(C_API_CC) $(OBJS) 

$(C_API_CC): $(C_API_H) $(XML_DEFS)

%.h: %.xml
	$(IDLC) --cheader $<

%.cc: %.xml
	$(IDLC) --cimpl $<

clean:
	rm -rf *.o *.h *.cc *.d

-include *.d
