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
include ../../../../Make.hlcs.rules
include ../../../../../Make.rules

EXE         = exe
EXE_SRC     = exe.cc
EXE_OBJS    = $(EXE_SRC:%.cc=%.o)

BLOCK       = block
BLOCK_SRC   = block.cc
BLOCK_OBJS  = $(BLOCK_SRC:%.cc=%.o)

MAPPING     = mapping
MAPPING_SRC = mapping.cc
MAPPING_OBJS= $(MAPPING_SRC:%.cc=%.o)

AUTHID      = job_security_authid
AUTHID_SRC  = job_security_authid.cc
AUTHID_OBJS = $(AUTHID_SRC:%.cc=%.o)

LDFLAGS  += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib

install: all

all: $(EXE) $(BLOCK) $(MAPPING) $(AUTHID)

$(AUTHID): $(AUTHID_OBJS) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

$(EXE): $(EXE_OBJS) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

$(BLOCK): $(BLOCK_OBJS) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

$(MAPPING): $(MAPPING_OBJS) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

clean:
	$(RM) *.o
	$(RM) $(EXE) $(BLOCK) $(MAPPING)
	$(RM) *.d

distclean: clean

-include *.d
