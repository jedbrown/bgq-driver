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
# (C) Copyright IBM Corp.  2010, 2012                              
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

#SILENT=@
SILENT=

BASENAME = cnk
RM_F = /bin/rm -f

K_KFLAGS    = -D__KERNEL__
K_AFLAGS    = -D__ASSEMBLY__
K_INCLUDES  = -I$(BGQ_INSTALL_DIR)
K_CPU_FLAGS = -m64 -O2

K_CFLAGS = -Wall $(LISTING_OPTIONS) $(K_INCLUDES)

HOST_CC=gcc

L_LIBS =

LOCAL_LIBS =

%.o: %.c
	@echo "gcc: "`$(BASENAME) $$PWD`"/"`$(BASENAME) $<`
	$(SILENT)$(HOST_CC) $(K_CPU_FLAGS) $(K_KFLAGS) $(K_CFLAGS) $< -o $*.o

%.o: %.cc
	@echo "gcc: "`$(BASENAME) $$PWD`"/"`$(BASENAME) $<`
	$(SILENT)$(HOST_CC) $(K_CPU_FLAGS) $(K_KFLAGS) $(K_CFLAGS) $< -o $*.o

%.o:	%.S
	@echo "gcc: "`$(BASENAME) $$PWD`"/"`$(BASENAME) $<`
	$(SILENT)$(HOST_CC) $(K_CPU_FLAGS) $(K_KFLAGS) $(K_CFLAGS) $(K_AFLAGS) -Os $< -o $*.o

%.elf:
	@echo "Linking Executable $@ ..."
	$(SILENT)$(BGQ_CROSS_LD) $(K_LDFLAGS) $(K_LDSCRIPT) -o $@ $^ $(K_LIBS) $(LOCAL_LIBS)
	$(SILENT)$(BGQ_CROSS_OBJDUMP)  -dfhl $@ > $*.objdump
#	$(SILENT)$(BGQ_CROSS_READELF)  $@ > $*.readelf


all:	bldmemfs

install: all
	$(INSTALLCANFAIL)mkdir -p $(BGQ_INSTALL_DIR)/$(BASENAME)/tools
	$(INSTALLCANFAIL)cp -pf ../tools/mkstandalone.pl $(BGQ_INSTALL_DIR)/$(BASENAME)/tools
	$(INSTALLCANFAIL)cp -pf ../tools/standaloneldrinfo.lds $(BGQ_INSTALL_DIR)/$(BASENAME)/tools
	$(INSTALLCANFAIL)cp -pf ../tools/standaloneldrinfo_l2.lds $(BGQ_INSTALL_DIR)/$(BASENAME)/tools
	$(INSTALLCANFAIL)cp -pf ../tools/bldmemfs $(BGQ_INSTALL_DIR)/$(BASENAME)/tools


bldmemfs:	bldmemfs.c
	$(SILENT)$(HOST_CC) $(K_CPU_FLAGS) $(K_CFLAGS) -DDEFAULT_LINKER=\"$(BGQ_CROSS_LD)\" -o bldmemfs bldmemfs.c 



clean::
	@echo "Cleaning `$(BASENAME) $$PWD`"
	@-$(RM_F) *.o bldmemfs

distclean: clean
	@echo "Dist-cleaning `$(BASENAME) $$PWD`"
