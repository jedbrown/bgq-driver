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


include ../../Make.rules
include ../firmware.native.rules

#SILENT=@
SILENT=

RM_F = /bin/rm -f

TARGET=

LIB_ROOT   = libfwras
LIB_SO     = $(LIB_ROOT).so
LIB_VERS   = $(LIB_SO).1
TARGET_LIB = $(LIB_VERS).0.0

CXXFLAGS  =
CXXFLAGS += -g
CXXFLAGS += -m64

CXXFLAGS += -Wall -Werror
CXXFLAGS += -fPIC
CXXFLAGS += -I $(BGQ_INSTALL_DIR)

SOFLAGS  =
SOFLAGS += -shared
SOFLAGS += -Wl,-soname,$(SOLIB_VERS)


SRC = \
	utils.cc \
	fw_A2_machineCheckDecoder.cc \
	fw_BeDRAM_machineCheckDecoder.cc \
	fw_ClockStop_machineCheckDecoder.cc \
	fw_DcrArbiter_machineCheckDecoder.cc \
	fw_DdrArbiter_drilldown.cc \
	fw_DdrArbiter_machineCheckDecoder.cc \
	fw_DdrInit_Decoder.cc \
	fw_Devbus_machineCheckDecoder.cc \
	fw_EnvMon_machineCheckDecoder.cc \
	fw_GEA_machineCheckDecoder.cc \
	fw_L1P_machineCheckDecoder.cc \
	fw_L2_machineCheckDecoder.cc \
	fw_L2C_machineCheckDecoder.cc \
	fw_L2Ctr_machineCheckDecoder.cc \
	fw_MSGC_machineCheckDecoder.cc \
	fw_MU_machineCheckDecoder.cc \
	fw_ND_machineCheckDecoder.cc \
	fw_PCIe_machineCheckDecoder.cc \
	fw_Serdes_machineCheckDecoder.cc \
	fw_TestInt_machineCheckDecoder.cc \
	fw_UnexpectedInterrupt_Decoder.cc \
	fw_UPC_machineCheckDecoder.cc \
	fw_WU_machineCheckDecoder.cc \
	_bedram_interrupt_status.cc

OBJ = \
	utils.o \
	fw_A2_machineCheckDecoder.o \
	fw_BeDRAM_machineCheckDecoder.o \
	fw_ClockStop_machineCheckDecoder.o \
	fw_DcrArbiter_machineCheckDecoder.o \
	fw_DdrArbiter_drilldown.o \
	fw_DdrArbiter_machineCheckDecoder.o \
	fw_DdrInit_Decoder.o \
	fw_Devbus_machineCheckDecoder.o \
	fw_EnvMon_machineCheckDecoder.o \
	fw_GEA_machineCheckDecoder.o \
	fw_L1P_machineCheckDecoder.o \
	fw_L2_machineCheckDecoder.o \
	fw_L2C_machineCheckDecoder.o \
	fw_L2Ctr_machineCheckDecoder.o \
	fw_MSGC_machineCheckDecoder.o \
	fw_MU_machineCheckDecoder.o \
	fw_ND_machineCheckDecoder.o \
	fw_PCIe_machineCheckDecoder.o \
	fw_Serdes_machineCheckDecoder.o \
	fw_TestInt_machineCheckDecoder.o \
	fw_UnexpectedInterrupt_Decoder.o \
	fw_UPC_machineCheckDecoder.o \
	fw_WU_machineCheckDecoder.o




ifeq ($(HOST_OS),Linux)
EXPAT=-lexpat
endif

#HOST_CFLAGS += -D__SVCHOST__ -D__KERNEL__ -I../../.. -MMD -MF .dep.$<.d

.PHONY : all clean depend install distclean

.SUFFIXES:


%.o: %.cc
	$(SILENT)$(CXX) $(CXXFLAGS) -c $< -o $*.o


all: default

default: $(TARGET_LIB) $(TARGET) $(TARGET2) $(LIB_VERS) $(LIB_SO)

$(TARGET_LIB):  $(OBJ)
	#$(AR) rcs $(TARGET_LIB) $(OBJ)
	$(CXX) $(CXXFLAGS) $(SOFLAGS) -o $@ $^


$(LIB_VERS):	$(TARGET_LIB)
	ln -sf $< $@

$(LIB_SO):	$(LIB_VERS)
	ln -sf $< $@

tester:	tester.cc utils.cc
	$(CXX) -Wall  -I $(BGQ_INSTALL_DIR) -g -o tester tester.cc utils.cc

unit_test:	tester
	./tester


$(BGQ_INSTALL_DIR)/ras/lib:
	$(MKDIR) $(BGQ_INSTALL_DIR)/ras/lib/

$(BGQ_INSTALL_DIR)/firmware/ras/decoder/%: %
	$(call bgq_install_bin,$<,$@)

install:	all \
		$(BGQ_INSTALL_DIR)/firmware/ras/decoder/$(TARGET_LIB) \
		$(BGQ_INSTALL_DIR)/firmware/ras/decoder/$(LIB_VERS) \
		$(BGQ_INSTALL_DIR)/firmware/ras/decoder/$(LIB_SO) \
		$(BGQ_INSTALL_DIR)/firmware/ras/decoder/fw_ras_decoder_lib.xml



#        $(MKDIR) -p $(BGQ_INSTALL_DIR)/firmware/ras/                                         
#        $(MKDIR) -p $(BGQ_INSTALL_DIR)/firmware/ras/decoder/                                 
#        $(CP) -P $(TARGET_LIB) $(LIB_VERS) $(LIB_SO) $(BGQ_INSTALL_DIR)/firmware/ras/decoder/
#        $(CP) fw_ras_decoder_lib.xml $(BGQ_INSTALL_DIR)/firmware/ras/decoder/                


clean:
	$(RM_F) $(OBJ) $(TARGET_LIB) $(LIB_VERS) $(LIB_SO) *.beam 


distclean:	clean
	@-$(RM_F) ./Make.depend
	@-$(RM_F) -r sim
	@-$(RM_F) *.patched


depend::
	@-$(RM) ./Make.depend
	@$(HOST_GCC) -M $(HOST_CFLAGS) $(SRC) > ./Make.depend


fresh:	clean depend all


-include Make.depend
-include .dep.*.d
-include .dep.*.64.d

