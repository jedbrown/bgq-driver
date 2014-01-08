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
include ../../firmware.native.rules

#SILENT=@
SILENT=

BASENAME = svchost
RM_F = /bin/rm -f

TARGET=svc_host
TARGET2=fw_ddr_update
TARGET_LIB=libsvchost.a

# shared library
LIB_BASE       = svchost
LIB_MAJOR_VERS = 1
LIB_MINOR_VERS = 0
LIB_MINOR_REV  = 0
LIB_SHORT      = lib$(LIB_BASE).so
LIB_VERS       = $(LIB_SHORT).$(LIB_MAJOR_VERS)
LIB_FULL       = $(LIB_VERS).$(LIB_MINOR_VERS).$(LIB_MINOR_REV)

SRC = \
    svc_args.c \
    svc_crc16n.c \
    svc_elf_debug.c \
    svc_host.c \
    svc_elf.c \
    svc_main.c \
    svc_pers.c

LIB_OBJ = \
    svc_args.o \
    svc_crc16n.o \
    svc_elf_debug.o \
    svc_host.o \
    svc_elf.o \
    svc_pers.o

PROG_OBJ = svc_main.o
PROG_OBJ2 = fw_ddr_update.o

ifeq ($(HOST_OS),Linux)
EXPAT=-lexpat
endif

HOST_CFLAGS += -D__SVCHOST__ -D__KERNEL__ -I../../.. -MMD -MF .dep.$<.d
HOST_CFLAGS += -fPIC

.PHONY : all clean depend install distclean

.SUFFIXES:

%.o: %.c
	$(SILENT)$(HOST_GCC) $(HOST_CFLAGS) $< -o $*.o

%.o: %.cc
	$(SILENT)$(HOST_GCC) $(HOST_CFLAGS) $< -o $*.o


all: default

default: $(TARGET_LIB) $(TARGET) $(TARGET2) $(LIB_SHORT)

$(TARGET_LIB):  $(LIB_OBJ)
	$(AR) rcs $(TARGET_LIB) $(LIB_OBJ)

$(LIB_SHORT): $(LIB_VERS)
	ln -sf $< $@

$(LIB_VERS): $(LIB_FULL)
	ln -sf $< $@

$(LIB_FULL): $(LIB_OBJ)
	$(HOST_GCC) $(HOST_LDFLAGS) $(EXPAT) -fPIC -shared -o $@ $^

$(TARGET): $(PROG_OBJ) $(TARGET_LIB)
	$(SILENT) gcc $(HOST_LDFLAGS) $(EXPAT) -o $@ $^

$(TARGET2): $(PROG_OBJ2) $(TARGET_LIB)
	$(SILENT)gcc $(HOST_LDFLAGS) $(EXPAT) -o $@ $^


$(BGQ_INSTALL_DIR)/firmware/tools/%: %
	$(call bgq_install_bin,$<,$@)

$(BGQ_INSTALL_DIR)/firmware/lib/%: %
	$(call bgq_install_bin,$<,$@)


$(BGQ_INSTALL_DIR)/firmware/tools/README.please: $(TARGET)
	( ./$(TARGET) -help > $(BGQ_INSTALL_DIR)/firmware/tools/README.please ; test 1 )

$(BGQ_INSTALL_DIR)/firmware/tools/example.svchost: $(TARGET)
	( ./$(TARGET) -example=$(BGQ_INSTALL_DIR)/firmware/tools/example.svchost ; test 1 )


install:	all \
	$(BGQ_INSTALL_DIR)/firmware/lib/$(TARGET_LIB) \
	$(BGQ_INSTALL_DIR)/firmware/tools/$(TARGET) \
	$(BGQ_INSTALL_DIR)/firmware/tools/$(TARGET2) \
	$(BGQ_INSTALL_DIR)/firmware/lib/$(LIB_SHORT) \
	$(BGQ_INSTALL_DIR)/firmware/lib/$(LIB_VERS) \
	$(BGQ_INSTALL_DIR)/firmware/lib/$(LIB_FULL) \
	$(BGQ_INSTALL_DIR)/firmware/tools/README.please \
	$(BGQ_INSTALL_DIR)/firmware/tools/example.svchost



#	# @echo "Installing `$(BASENAME) $$PWD`"
#	$(MKDIR) -p $(BGQ_INSTALL_DIR)/firmware/tools $(BGQ_INSTALL_DIR)/firmware/lib
#	$(CP) $(TARGET_LIB) $(BGQ_INSTALL_DIR)/firmware/lib
#	$(CP) $(TARGET) $(BGQ_INSTALL_DIR)/firmware/tools
#	$(CP) $(TARGET2) $(BGQ_INSTALL_DIR)/firmware/tools
#	$(CP) -P $(LIB_SHORT) $(LIB_VERS) $(LIB_FULL) $(BGQ_INSTALL_DIR)/firmware/lib
#	( ./$(TARGET) -help > $(BGQ_INSTALL_DIR)/firmware/tools/README.please ; test 1 )
#	( ./$(TARGET) -example=$(BGQ_INSTALL_DIR)/firmware/tools/example.svchost ; test 1 )

clean::
	$(RM_F) $(PROG_OBJ) $(PROG_OBJ2) $(OBJ) $(LIB_OBJ) $(LIB_OBJ_64) $(TARGET) $(TARGET_LIB) $(LIB_SHORT) $(LIB_VERS) $(LIB_FULL) *.beam .dep.*


distclean: clean
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

