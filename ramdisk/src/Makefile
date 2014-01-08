# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# Licensed Materials - Property of IBM                             
# Blue Gene/Q                                                      
# (C) Copyright IBM Corp. 2012  All Rights Reserved                 
# US Government Users Restricted Rights - Use,                     
# duplication or disclosure restricted by GSA ADP                  
# Schedule contract with IBM Corp.                                 
#                                                                  
# This software is available to you under the                      
# GNU General Public License (GPL).                                
#                                                                  
# ================================================================ 
#                                                                  
# end_generated_IBM_copyright_prolog                               
#
# Minimal ramdisk for Linux
#
# Generation of the ramdisk occurs during install using the installed
# files as input (assuring they match).2
#

include ../../Make.rules

# Note: set RDOPTS="--addtree t" to add directory t to the ramdisk.
# Use --nocompress when using mambo to save a significant amount of boot time.
RDOPTS=

#Cross compiler eventually needs to be fixed
CROSS_COMPILER=${BGQ_CROSS_LD}

TOOLSDIR=$(BGQ_LINUX_PATH)/tools
SUBDIRS=$(shell find * -type d | grep -ve "\.svn" | grep -v rpms | grep -v '/' | grep -v ofed | grep -v bgfirmwaredev | grep -v bglogdev | grep -v bgmudev | grep -v bgpersdev | grep -v bgrasdev ) 
INSTALL_RAMDISK=${BGQ_RAMDISK_PATH}/bin
INSTALL_BUILD_RAMDISK=${INSTALL_RAMDISK}/build-ramdisk

.PHONY: ${SUBDIRS} FORCE

.NOTPARALLEL: 

default: install

all: default	 

# First check to make sure we are on RHEL6 before we start to build.  If so, make sure that it
# is a ppc64 based system.  If either case is false we configure the install rule to
# print the user a warning.  We cannot make this a fatal error as it would break the
# RHEL5 build which works around this by copying over a RHEL6 ramdisk.
ifeq ($(RHEL6),yes)
ifeq ($(OS_ARCH),ppc64)
install: Make.rules ${BGQ_RAMDISK_PATH} ${BGQ_DISTROFS_PATH} ${BGQ_INITRAMFS_PATH} MAKE_BIN_PATH ${INSTALL_RAMDISK} ${SUBDIRS} build-ramdisk ${INSTALL_BUILD_RAMDISK} mlxFwUpdate distrofs.tar build-rpms
else
install:
	@echo -en "\n\n#############################################################"
	@echo -en "\n# WARNING:"
	@echo -en "\n#\tRamdisk build is only supported on PPC64 based systems."
	@echo -en "\n#\tSkipping the ramdisk build."
	@echo -en "\n###############################################################\n\n"
endif
else
install:
	@echo -en "\n\n#############################################################"
	@echo -en "\n# WARNING:"
	@echo -en "\n#\tRamdisk build is only supported on RHEL6 based systems."
	@echo -en "\n#\tSkipping the ramdisk build."
	@echo -en "\n###############################################################\n\n"
endif

${SUBDIRS}: FORCE
	@${MAKE} -C $@

${BGQ_RAMDISK_PATH}/include/infiniband:
	@${MAKE} -C ofed

${BGQ_RAMDISK_PATH}:
	@${MKDIR} -p ${BGQ_RAMDISK_PATH}

${BGQ_DISTROFS_PATH}:
	@${MKDIR} -p ${BGQ_DISTROFS_PATH}

${BGQ_INITRAMFS_PATH}:
	@${MKDIR} -p ${BGQ_INITRAMFS_PATH}

Make.rules:	../../Make.rules
	@${CP} ../../Make.rules .

MAKE_BIN_PATH:
	@${MKDIR} -p ${INSTALL_RAMDISK}

# The zipped ramdisk is now the default.
${INSTALL_RAMDISK}: ramdisk.gz
	@${CP} $< $@

${INSTALL_BUILD_RAMDISK}: build-ramdisk
	@${CP} $< $@

distrofs.tar: distrofs initramfs ramdisk.gz build-ramdisk
	@cd ${BGQ_DISTROFS_PATH}/tar ; echo -en "Building distrofs.tar\n" ; sleep 5 ; \
		until tar --exclude=.svn -cvf ${BGQ_BUILD_DIR}/ramdisk/src/$@ ./* &> /dev/null ; do sleep 5 ; done
	${CP} $@ ${BGQ_RAMDISK_PATH}/bin

build-rpms:
	@${MAKE} -C rpms 

# Optional uncompressed ramdisk build
ramdisk: ${SUBDIRS} build-ramdisk $(TOOLSDIR) FORCE
	./build-ramdisk --nocompress --saveimg --verbose --rootfs ${BGQ_INITRAMFS_PATH} ${RDOPTS} -o $@
	@${CP} $@ ${INSTALL_RAMDISK}
	@${LN} -sf ../ramdisk/bin/ramdisk $(BGQ_INSTALL_DIR)/boot


# Default compressed ramdisk build
ramdisk.gz: ${SUBDIRS} build-ramdisk $(TOOLSDIR) FORCE
	if [ -e $@ ] ; then rm -f $@ ; fi
	if [ -e $@.cpio.gz ] ; then rm -f $@.cpio.gz ; fi
	if [ -e ${INSTALL_RAMDISK}/$@ ] ; then rm -f ${INSTALL_RAMDISK}/$@ ; fi
	if [ -e ${BGQ_INITRAMFS_PATH}/lib64/libbgcios.so.1.0.0 ] ; then mv ${BGQ_INITRAMFS_PATH}/lib64/libbgcios.so* \
		${BGQ_INSTALL_DIR}/ramdisk/ ; fi
	./build-ramdisk --saveimg --verbose --rootfs ${BGQ_INITRAMFS_PATH} ${RDOPTS} -o $@
	if [ -e ${BGQ_INSTALL_DIR}/ramdisk/libbgcios.so.1.0.0 ] ; then mv ${BGQ_INSTALL_DIR}/ramdisk/libbgcios.so* \
		${BGQ_INITRAMFS_PATH}/lib64/ ; fi
	@${CP} $@ ${INSTALL_RAMDISK}
	chmod a+rx ${INSTALL_RAMDISK}/$@
	@${LN} -sf ${INSTALL_RAMDISK}/ramdisk.gz ${INSTALL_RAMDISK}/ramdisk
	@${LN} -sf ../ramdisk/bin/ramdisk.gz $(BGQ_INSTALL_DIR)/boot/ramdisk

# Build a ramdisk to update the mlx firmwarei
# For development we copy the firmware images from baremetal before making the mlxFwUpdate ramdisk.
mlxFwUpdate: ${SUBDIRS} build-ramdisk $(TOOLSDIR) ../tools/mlxFwUpdate $(BGQ_INSTALL_DIR)/baremetal/firmware/mlxFwUpdate FORCE
        # Need to copy a couple of files into the mlxFwUpdate path.
	cp /lib64/libm-2.12.so ${BGQ_BUILD_DIR}/ramdisk/tools/mlxFwUpdate/lib64/
	cp -a /lib64/libm.so.6 ${BGQ_BUILD_DIR}/ramdisk/tools/mlxFwUpdate/lib64/
	cp /lib64/libz.so.1.2.3 ${BGQ_BUILD_DIR}/ramdisk/tools/mlxFwUpdate/lib64/
	cp -a /lib64/libz.so.1 ${BGQ_BUILD_DIR}/ramdisk/tools/mlxFwUpdate/lib64/
	cp -a /usr/lib64/libstdc++.so.6* ${BGQ_BUILD_DIR}/ramdisk/tools/mlxFwUpdate/usr/lib64
	cp /usr/bin/mstflint ${BGQ_BUILD_DIR}/ramdisk/tools/mlxFwUpdate/usr/bin
	find ${BGQ_BUILD_DIR}/ramdisk/tools/mlxFwUpdate/* -type f | xargs /usr/sbin/prelink -u &> /dev/null ; exit 0
	if [ ! -e ${INSTALL_RAMDISK}/$@ ] ; then cp -a $(BGQ_INSTALL_DIR)/baremetal/firmware/mlxFwUpdate/* $(BGQ_BUILD_DIR)/ramdisk/tools/mlxFwUpdate/bin/ ; ./build-ramdisk --saveimg --verbose --rootfs ${BGQ_INITRAMFS_PATH} --addtree ${BGQ_BUILD_DIR}/ramdisk/tools/mlxFwUpdate ${RDOPTS} -o $@ ; cp $@ ${INSTALL_RAMDISK} ; fi
#	@${CP} $@ ${INSTALL_RAMDISK}
#	@${LN} -sf ${INSTALL_RAMDISK}/$@ ${INSTALL_RAMDISK}/ramdisk
#	@${LN} -sf ../ramdisk/bin/$@ $(BGQ_INSTALL_DIR)/boot/ramdisk

build-ramdisk: build-ramdisk.pl
	sed 's,BG_ROOTFS_PATH,${BGQ_INITRAMFS_PATH},;s,BG_TOOLS_PATH,${TOOLSDIR},;s,BG_RUNOS_PATH,${BGQ_LINUX_OS_PATH},' < build-ramdisk.pl > build-ramdisk
	chmod +x build-ramdisk


clean: Make.rules
	@echo "Cleaning `basename $$PWD`"
	@make -C bgras clean
	@make -C bgrasdev clean
	@make -C bgvrnic clean
	@make -C bgmudev clean
	@make -C bgpersdev clean
	@make -C bgfirmwaredev clean
	@make -C bgsethostid clean
	@make -C rpms clean
	@make -C ofed clean
	@make -C services clean
	@rm -f *.o *~ *.elf *.objdump ramdisk* build-ramdisk distrofs.tar

distclean:  Make.rules 
	@echo "Dist-cleaning `basename $$PWD`"
	@make -C initramfs distclean
	@make -C distrofs distclean
	@make -C bgras distclean
	@make -C bgrasdev distclean
	@make -C bgvrnic distclean
	@make -C bgmudev distclean
	@make -C bgpersdev distclean
	@make -C bgfirmwaredev distclean
	@make -C bgsethostid distclean
	@make -C rpms distclean
	@make -C ofed distclean
	@make -C services distclean
	@rm -f *.o *~ *.elf *.objdump ramdisk* build-ramdisk Make.rules distrofs.tar
	@rm -rf ${BGQ_RAMDISK_PATH}/*

depend::
	@echo "Making dependencies for `basename $$PWD`"

fresh: clean depend all

.S.cnk.o:;    ${CC_CNK}  ${CNK_CFLAGS} -D__ASSEMBLY__ -c $< -o $@

# This script will read ras metadata from this top level src tree. 
# A subset of the metadata is written to an extract file in the
# $(BGPINSTALL_DIR)/ras/metadata directory.  The list of enum files
# is the third parameter passed to the script. 
ras_scan: 
#	$(BGPSYS_DIR)/scripts/ras/rasScan.py '$(BGPSYS_DIR)/ramdisk' '$(BGPINSTALL_DIR)/ras/metadata' '$(BGP_ARCH_PATH)/include/common/bgp_ras.h;$(BGP_ARCH_PATH)/include/cio/ciod_ras.h'

#Debug
print-%: ; @echo $* is $($*)
