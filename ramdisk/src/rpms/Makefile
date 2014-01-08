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
#+++
# Makefile for Blue Gene/Q runtime RPMs.
#
# Author: Jay S. Bryant
#
#---

include ../../../Make.rules

.PHONY : install clean distclean

.NOTPARALLEL:

#Get the SVN revision.
SVN_REVISION=$(shell REVISION_ARR=(`cat ${BGQ_INSTALL_DIR}/svn_revision | grep Revision: | tr " " "\n"`) ; echo $${REVISION_ARR[1]})

RPMBUILD_SUBDIRS := rpmbuild/BUILD rpmbuild/SPECS rpmbuild/SOURCES rpmbuild/RPMS rpmbuild/SRPMS
RPMBUILD_DIR = ${BGQ_BUILD_DIR}/ramdisk/src/rpms/rpmbuild

default: install


install: ${RPMBUILD_SUBDIRS} rpmbuild/.rpmmacros ${BGQ_RAMDISK_PATH}/rpms BUILD_RPMS INSTALL_RPMS

${RPMBUILD_SUBDIRS}:
	${MKDIR} -p $@

${BGQ_RAMDISK_PATH}/rpms:
	${MKDIR} -p $@

INSTALL_RPMS: ./rpmbuild/RPMS/ppc64/bgq-distrofs_runtime-${BGQ_RPMVERSION}.${BGQ_RPMRELEASE}.${BGQ_RPMMODIFICATION}-${SVN_REVISION}.ppc64.rpm ./rpmbuild/RPMS/ppc64/bgq-cio_services_runtime-${BGQ_RPMVERSION}.${BGQ_RPMRELEASE}.${BGQ_RPMMODIFICATION}-${SVN_REVISION}.ppc64.rpm
	${CP} -a ./rpmbuild/RPMS/ppc64/bgq-distrofs_runtime-${BGQ_RPMVERSION}.${BGQ_RPMRELEASE}.${BGQ_RPMMODIFICATION}-${SVN_REVISION}.ppc64.rpm ${BGQ_RAMDISK_PATH}/rpms/ 
	${CP} -a ./rpmbuild/RPMS/ppc64/bgq-cio_services_runtime-${BGQ_RPMVERSION}.${BGQ_RPMRELEASE}.${BGQ_RPMMODIFICATION}-${SVN_REVISION}.ppc64.rpm ${BGQ_RAMDISK_PATH}/rpms/


BUILD_RPMS: distrofs_runtime.spec cio_services_runtime.spec
ifneq ($(PACKAGING_GPG_NAME),no)
	HOME=${RPMBUILD_DIR} expect -c "spawn rpmbuild --ba --define \"buildid .distrofs_runtime\" --target ppc64-ibm-linux --sign distrofs_runtime.spec ; expect phrase: { send \n ; exp_continue; } timeout { exp_continue; } ;"
	HOME=${RPMBUILD_DIR} expect -c "spawn rpmbuild --ba --define \"buildid .cio_services_runtime\" --target ppc64-ibm-linux --sign cio_services_runtime.spec ; expect phrase: { send \n ; exp_continue; } timeout { exp_continue; } ;"
else
	export HOME=${RPMBUILD_DIR} ; rpmbuild --ba --define "buildid .distrofs_runtime" --target ppc64-ibm-linux distrofs_runtime.spec
	export HOME=${RPMBUILD_DIR} ; rpmbuild --ba --define "buildid .cio_services_runtime" --target ppc64-ibm-linux cio_services_runtime.spec

endif

# Need t oset up the .rpmmacros file with variables that we will need.
# I borrowed this from build/packaging/Makefile so that we should end up with 
# rpms named in the same way as the rest of the driver.
rpmbuild/.rpmmacros:
	$(MKDIR) -p $(dir $@)
	$(RM) $@
	@echo "Creating $@"
	@echo "%_topdir  `pwd`/rpmbuild" >> $@
	@echo "%_base_dir  $(BGQ_INSTALL_DIR)" >> $@
	@echo "%_parent_dir  $(patsubst %/,%,$(dir $(BGQ_INSTALL_DIR)))" >> $@
	@echo "%_change_date  `date +'%a %b %d %Y'` IBM" >> $@
	@echo "%_product_version  $(BGQ_RPMVERSION).$(BGQ_RPMRELEASE).$(BGQ_RPMMODIFICATION)" >> $@
	@echo "%_rpm_release  $(SVN_REVISION)" >> $@
	@echo "%_rpm_ver_rel  $(BGQ_RPMVERSION).$(BGQ_RPMRELEASE).$(BGQ_RPMMODIFICATION)-$(SVN_REVISION)" >> $@
	@echo "%_rpm_driver_lvl  $(BGQ_RPMDRIVER)" >> $@
	@echo "%_os_arch  `uname -m`" >> $@
	@echo "%_bgq_name  bgq$(BGQ_RPMSUFFIX)" >> $@
	@echo "%_bgqinternal_name  bgqinternal$(BGQ_RPMSUFFIX)" >> $@
	@echo "%_bgqtoolchain_name  bgqtoolchain$(BGQ_RPMSUFFIX)" >> $@
	@echo "%_bgqlinux_header_path $(BGQ_LINUX_HEADERS_PATH)/include" >> $@
	@echo "%__check_files %{nil}" >> $@
	@echo "%bg_install_root  $(firstword $(subst /, ,$(BGQ_INSTALL_DIR)))" >> $@ # like bgsys or bghome

ifneq ($(PACKAGING_GPG_NAME),no)
	@echo "%_signature gpg" >> $@
	@echo "%_gpg_name $(PACKAGING_GPG_NAME)" >> $@
	@echo "%_gpg_path $$HOME/.gnupg" >> $@
endif

clean:
	@echo "Cleaning rpms ..."
	@rm -rf rpmbuild

distclean: clean
	@echo "Distcleaning rpms ..."
	@rm -f ${BGQ_RAMDISK_PATH}/rpms/*.rpm
