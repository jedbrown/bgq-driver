# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# Licensed Materials - Property of IBM                             
# Blue Gene/Q                                                      
# (C) Copyright IBM Corp. 2012 All Rights Reserved                 
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
#
# Top level makefile for ramdisk/src/initramfs
# 
# Author: Jay S. Bryant <jsbryant@us.ibm.com>
# 
include ../../../Make.rules

DIRS=$(shell find * -type d  | grep -ve "\.svn" | grep -v "ofed" )
INSTALL_DIRS=$(addprefix ${BGQ_INITRAMFS_PATH}/, ${DIRS})
BG_FILES=$(shell find * -type f \! -empty | grep -v Makefile | grep -ve "\.svn"; \
		 find * -type l \! -empty | grep -v Makefile | grep -ve "\.svn")
INSTALL_BG_FILES=$(addprefix ${BGQ_INITRAMFS_PATH}/, ${BG_FILES})
OS_FILES=$(shell find * -type f -empty | grep -ve "\.svn" | grep -v Makefile)
INSTALL_OS_FILES=$(addprefix ${BGQ_INITRAMFS_PATH}/, ${OS_FILES})
LIBS=ld libblkid libuuid libc libpthread librt libdl
EXTRACTED=$(shell ls ${BGQ_INITRAMFS_PATH}/bin/busybox &> /dev/null ; echo $$?)


install: ${INSTALL_DIRS} ${INSTALL_BG_FILES} ${LIBS} populateInitramfs
	$(shell cd ${BGQ_INITRAMFS_PATH}/lib64; find * -name 'ld*.so*' -type f -exec ln -sf '{}' ${BGQ_INITRAMFS_PATH}/lib64/ld64.so.1 \; )
#       Make sure that none of the binaries are prelinked to avoid problems with RPM install.
#       Forcing the rc to 0 as prelink returns 0 for non-prelinked files.
	@find ${BGQ_INITRAMFS_PATH}/* -type f | xargs /usr/sbin/prelink -u &> /dev/null ; exit 0

.PHONY : FORCE 

.NOTPARALLEL:


# Create directories in the install directory.
${INSTALL_DIRS}:  
	@${MKDIR} $@	
	@chmod 777 $@


# Copy any files we provide here to the install directory.
# Force rebuild of distrofs.tar by removing it if it exists and we get into this rule meaning
# that there are changes we want to pick up.
${INSTALL_BG_FILES}: ${BGQ_INITRAMFS_PATH}/% : % 
	@${CP} -d $< $@
	@if [ -e ../distrofs.tar ] ; then rm ../distrofs.tar ; fi

# Create symlinks in the install directory to any files here that are empty.
${INSTALL_OS_FILES}:  ${BGQ_INITRAMFS_PATH}/% : ${BGQ_LINUX_PATH}/OS/%
	@${LN} -sf $< $@


${BGQ_INITRAMFS_PATH}/usr/lib64/libibverbs.so.1.0.0:
	make --directory ../ofed install


${LIBS}:
#	@$(shell find ${BGQ_CROSS_TOOLS_DIR}/powerpc64-bgq-linux/lib -name $@*.so* -exec cp -d '{}' ${BGQ_INITRAMFS_PATH}/lib64/. \;)
#	@$(shell find ${BGQ_CROSS_TOOLS_DIR}/powerpc64-bgq-linux/lib64 -name $@*.so* -exec cp -d '{}' ${BGQ_INITRAMFS_PATH}/lib64/. \;)
#	@$(shell find ${BGQ_INSTALL_DIR}/utility/lib -name *.so* -exec cp -d '{}' ${BGQ_INITRAMFS_PATH}/lib/. \;)

# Copy the extlib libraries needed for cioservices to run into the ramdisk.
#	@strip -o ${BGQ_INITRAMFS_PATH}/lib/libbgutility.so.1.0.0 ${BGQ_INSTALL_DIR}/utility/lib/libbgutility.so.1.0.0
#	@strip -o ${BGQ_INITRAMFS_PATH}/lib/liblog4cxx.so.10 ${BGQ_INSTALL_DIR}/extlib/lib/liblog4cxx.so.10.0.0
#	@strip -o ${BGQ_INITRAMFS_PATH}/lib/libboost_program_options.so.1.39.0 ${BGQ_INSTALL_DIR}/extlib/lib/libboost_program_options.so.1.39.0	
#	@strip -o ${BGQ_INITRAMFS_PATH}/lib/libboost_regex.so.1.39.0 ${BGQ_INSTALL_DIR}/extlib/lib/libboost_regex.so.1.39.0
#	@strip -o ${BGQ_INITRAMFS_PATH}/lib/libboost_system.so.1.39.0 ${BGQ_INSTALL_DIR}/extlib/lib/libboost_system.so.1.39.0
#	@strip -o ${BGQ_INITRAMFS_PATH}/lib/libboost_serialization.so.1.39.0 ${BGQ_INSTALL_DIR}/extlib/lib/libboost_serialization.so.1.39.0
#	@strip -o ${BGQ_INITRAMFS_PATH}/lib/libboost_thread.so.1.39.0 ${BGQ_INSTALL_DIR}/extlib/lib/libboost_thread.so.1.39.0
#	@strip -o ${BGQ_INITRAMFS_PATH}/lib/libsqlite3.so.0 ${BGQ_INSTALL_DIR}/extlib/lib/libsqlite3.so.0

populateInitramfs: ${BGQ_RAMDISK_PATH}/tools/bgqPopulateInitramfs.sh
	@${BGQ_RAMDISK_PATH}/tools/bgqPopulateInitramfs.sh

#Debug
print-%: ; @echo $* is $($*)

default: install

all: install

clean:
	@echo "Cleaning initramfs"
	@rm -rf ${BGQ_INITRAMFS_PATH}/*

distclean: clean
	@echo "Dist-cleaning initramfs"
	@rm -f ./Make.depend


fresh:	clean all
