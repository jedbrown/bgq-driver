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
.NOTPARALLEL: 

include ../Make.paths.rules
include ../Make.mpi-xlc.rules
BGQ_INSTALL_DIR = /bgsys/drivers/ppcfloor

TESTNAME = pingpong-nw


C_SRC 	= $(wildcard *.c)
CXX_SRC	= $(wildcard *.cc)
TEST_OBJS    := $(notdir $(patsubst %.c,%.o,$(C_SRC)) $(patsubst %.cc,%.o,$(CXX_SRC)))

TARGET   = $(TESTNAME).elf
TEST_INSTALL_DIR := $(TEST_INSTALL_DIR)/$(TESTNAME)

LIB_DIRS  += $(BGQ_INSTALL_DIR)/bgpm/lib
LIBS      += bgpm #  stdc++ rt  don't explicitly link stdc++ and rt when using MPI, creates multiply defined symbol errors. 


.PHONY: default install clean

default: 	install 

install:	$(TARGET)
	mkdir -p $(TEST_INSTALL_DIR)
	cp $(TARGET) $(TEST_INSTALL_DIR)/$(TARGET)
	
clean::
	-rm *.o *.listing *.map *.objdump *.nm *.readelf *.elf *.bin


$(TARGET): $(TEST_OBJS)





include ../Make.test.rules

-include Make.depend

