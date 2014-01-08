# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
#  --------------------------------------------------------------- 
#                                                                  
# Licensed Materials - Property of IBM                             
# Blue Gene/Q                                                      
# (C) Copyright IBM Corp.  2010, 2012                              
# US Government Users Restricted Rights - Use, duplication or      
#   disclosure restricted by GSA ADP Schedule Contract with IBM    
#   Corp.                                                          
#                                                                  
# This software is available to you under the Eclipse Public       
# License (EPL).                                                   
#                                                                  
#  --------------------------------------------------------------- 
#                                                                  
# end_generated_IBM_copyright_prolog                               

include ../../../Make.rules

SILENT=@
#SILENT=

BASENAME = fwaddrs

TARGET = fwaddrs
SRC = fwaddrs.c
PROG_OBJ = fwaddrs.o

CFLAGS = -g -c -m64 -Wall -D__KERNEL__ -I$(BGQ_INSTALL_DIR) -I../../.. -MMD -MF .dep.$<.d
LDFLAGS = -m64

.PHONY : all clean depend install distclean

.SUFFIXES:

%.o: %.c
	$(BGQ_CROSS_CC) $(CFLAGS) $< -o $*.o

%.o: %.cc
	$(BGQ_CROSS_CC) $(CFLAGS) $< -o $*.o

$(BGQ_INSTALL_DIR)/firmware/tools/%: %
	$(SILENT)echo "Installing $(BASENAME)"
	$(call bgq_install_bin,$<,$@)

all: default

default: $(TARGET)

$(TARGET): $(PROG_OBJ)
	$(BGQ_CROSS_CC) $(LDFLAGS) -o $@ $^

install: all $(BGQ_INSTALL_DIR)/firmware/tools/$(TARGET)

#	$(SILENT)echo "Installing $(BASENAME)"
#	$(SILENT)$(MKDIR) -p $(BGQ_INSTALL_DIR)/firmware/tools
#	$(CP) $(TARGET) $(BGQ_INSTALL_DIR)/firmware/tools

clean::
	$(RM) $(PROG_OBJ) $(TARGET) *.beam .dep.*

distclean: clean
	$(SILENT)$(RM) ./Make.depend

depend::
	$(SILENT)$(RM) ./Make.depend
	$(SILENT)$(BGQ_CROSS_CC) -M $(CFLAGS) $(SRC) > ./Make.depend

fresh: clean depend all

-include Make.depend
-include .dep.*.d
-include .dep.*.64.d
