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

.NOTPARALLEL: 

include ../../../../Make.rules
include ../../../Make.gcc.rules
include ../../../Make.test.rules

#SILENT=@
SILENT=

C_SRC 	= $(wildcard *.c)
CXX_SRC	= $(wildcard *.cc)
TEST_OBJS := $(notdir $(patsubst %.c,%.o,$(C_SRC)) $(patsubst %.cc,%.o,$(CXX_SRC)))

TESTNAME = $(notdir $(shell pwd))

TARGET = $(TESTNAME).elf

CURDIRNAME := $(shell $(BASENAME) `pwd`)

CFLAGS += -fopenmp
CPPFLAGS += -fopenmp
LIBS += stdc++ gomp pthread



.PHONY: default all install clean distclean subdirs $(SUBDIRS)

default:	all $(TARGET)

all:	subdirs $(TARGET)


$(TARGET): $(TEST_OBJS) $(LOCAL_LIBS)

#
# library subdirectoreis
# 
subdirs:  $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) --directory $@

clean::
	@echo "Cleaning $(CURDIRNAME)/"
	@-$(RM_F) *.o *.listing *.map *.objdump *.nm *.readelf *.elf *.bin


distclean: clean
	@echo "Dist-cleaning $(CURDIRNAME)"
	@-rm -rf $(TEST_INSTALL_DIR)
	@-$(RM_F) ./Make.depend


depend::
	@echo "Making dependencies for $(CURDIRNAME)"
	@-$(RM_F) ./Make.depend
	@$(BGQ_CROSS_CC) -MM  $(K_CPU_FLAGS) $(K_KFLAGS) $(K_CPPFLAGS) $(C_SRC)    > ./Make.depend


fresh:	clean depend install


-include Make.depend

