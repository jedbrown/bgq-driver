# 
#	Makefile for Firmware-Extension Testcases
#
#	$(SRC):	
#		list of files to be linked with kernel to make test
#	$(TEST):
#		name of the test
#                 = name of current directory
#	$(FW_OBJ):
#		object files needed to make the test
#
# ---

include ../../Make.rules

#SILENT=@
SILENT=

.PHONY: default all install clean dist_clean subdirs $(SUBDIRS)

#
# 	Standard targets
#

#
# RB. build linux only on ppc for now.
KLINUXBUILD=
ifeq (${BGQ_HOST_OS},ppc)
  KLINUXBUILD=klinuxbuild
endif


default:	install 
	@$(MAKE) ras_scan

force: clean install

all:	cnkbuild ${KLINUXBUILD}

install: all

clean:
	$(MAKE) -f Makefile_cnk clean
	$(MAKE) -f Makefile_klinux clean

distclean:
	$(MAKE) -f Makefile_cnk distclean
	$(MAKE) -f Makefile_klinux distclean

fresh:	clean all objdump install

cnk:      cnkbuild
cnkbuild: 
	$(MAKE) -f Makefile_cnk

linux: klinuxbuild
klinuxbuild:
	$(MAKE) -f Makefile_klinux

ras_scan:
	$(MKDIR) -p $(BGQ_INSTALL_DIR)/mudm/ras/metadata/
	$(BGQ_BUILD_DIR)/scripts/rasScan.py $(BGQ_BUILD_DIR)/mudm/src $(BGQ_INSTALL_DIR)/mudm/ras/metadata mudm_ras.xml $(BGQ_INSTALL_DIR)/ras/include/ras.xml

