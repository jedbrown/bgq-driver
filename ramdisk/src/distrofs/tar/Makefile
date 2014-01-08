#
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
# ramdisk/src/distrofs/tar Makefile
#
# Author: Jay S. Bryant <jsbryant@us.ibm.com>
#
include ../../../../Make.rules

DIRS=$(shell find * -type d  | grep -ve "\.svn")
INSTALL_DIRS=$(addprefix ${BGQ_DISTROFS_TAR_PATH}/, ${DIRS})
#BG_FILES=$(shell find * -type f \! -empty | grep -v Makefile | grep -ve "\.svn"; \
# 		 find * -type l \! -empty | grep -v Makefile | grep -ve "\.svn")
BG_FILES=$(shell find * -type f | grep -v Makefile | grep -ve "\.svn"; \
                 find * -type l \! -empty | grep -v Makefile | grep -ve "\.svn")
INSTALL_BG_FILES=$(addprefix ${BGQ_DISTROFS_TAR_PATH}/, ${BG_FILES})
OS_FILES=$(shell find * -type f -empty | grep -ve "\.svn" | grep -v Makefile)
#INSTALL_OS_FILES=$(addprefix ${BGQ_DISTROFS_PATH}/, ${OS_FILES})


install: ${INSTALL_DIRS} ${INSTALL_BG_FILES} INSTALL_COMMON_FILES

.PHONY : FORCE 

.NOTPARALLEL:


# Create directories in the install directory.
${INSTALL_DIRS}:  
	@${MKDIR} $@	
	@chmod 777 $@


# Copy any files we provide here to the install directory.
# If this rule is triggered it means there are changes we want to get into the
# distrofs.tar file.  We force a rebuild by removing the file.
${INSTALL_BG_FILES}: ${BGQ_DISTROFS_TAR_PATH}/% : % 
	@${CP} -d $< $@
	@if [ -e ../distrofs.tar ] ; then rm ../distrofs.tar ; fi

# Copy common files from initramfs.
# Note that files in this list need to be edited under initramfs.
INSTALL_COMMON_FILES:
	@${CP} ../../initramfs/etc/sysctl.conf ${BGQ_DISTROFS_TAR_PATH}/etc/
	@${CP} ../../initramfs/etc/init.d/functions ${BGQ_DISTROFS_TAR_PATH}/etc/init.d/
	@${CP} ../../initramfs/etc/init.d/network ${BGQ_DISTROFS_TAR_PATH}/etc/rc.d/init.d/
	@${CP} -d ../../initramfs/etc/rc.d/rc3.d/S05network ${BGQ_DISTROFS_TAR_PATH}/etc/rc.d/rc3.d/


# Create symlinks in the install directory to any files here that are empty.
${INSTALL_OS_FILES}:  ${BGQ_DISTROFS_TAR_PATH}/% : ${BGQ_LINUX_PATH}/OS/%
	@${LN} -sf $< $@


#Debug
print-%: ; @echo $* is $($*)

default: install

all: install

clean:
	@echo "Cleaning distrofs/tar"
	@rm -rf ${BGQ_DISTROFS_TAR_PATH}/*

distclean: clean
	@echo "Dist-cleaning distrofs/tar"
	@rm -f ./Make.depend


fresh:	clean all
