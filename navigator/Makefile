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
# (C) Copyright IBM Corp.  2010, 2011                              
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

include ../Make.rules


install_dir := $(BGQ_INSTALL_DIR)/$(bgq_tld)

installed_navigator_conf := $(install_dir)/etc/navigator.conf

navigator_files := ${shell find WebContent -type f -not -wholename "*.svn*"}

navigator_script_files := ${shell find script -type f -not -wholename "*.svn*"}

installed_navigator_files := $(addprefix  $(install_dir)/,$(navigator_files))


unzipped_dojo_file := build/dojo/dojo/dojo.js


install_targets := \
 $(installed_navigator_files) \
 build/build-report.txt \
 $(installed_navigator_conf) \
 $(install_dir)/WebContent/resources/diags/tests.properties \
 $(install_dir)/WebContent/resources/doc/bgpm \
 $(install_dir)/WebContent/resources/doc/bgsched \
 $(install_dir)/WebContent/resources/doc/bgws \
 $(install_dir)/WebContent/resources/doc/pami \
 $(install_dir)/WebContent/resources/doc/spi \
 $(install_dir)/WebContent/resources/doc/RasEventBook.htm \
 $(call TLD_INSTALL_PACKAGING_TARGET, navigator.specsrc)


default: $(install_targets) doc.default

all: $(install_targets) doc.all

install: $(install_targets) doc.install

doc.default:
	${MAKE} -C doc

doc.all doc.install doc.clean doc.distclean doc.uninstall:
	${MAKE} -C doc $(subst doc.,,$@)


$(install_dir)/WebContent/%: WebContent/%
	@$(MKDIR) -p $(dir $@)
	@$(CP) $< $@
	@echo "Installing $<"


$(unzipped_dojo_file):
	$(MKDIR) -p build
	tar -C build -xzmf $(BGQ_DOJO) > /dev/null # BGQ_DOJO is configured with --with-dojo.


build/build-report.txt: $(navigator_script_files) $(unzipped_dojo_file)
	BUILD_REPORT_DIR=`pwd`/build; \
	cd build/dojo/util/buildscripts; \
	sh build.sh \
	  --release \
	  --profile ../../../../build \
	  --releaseDir $(install_dir)/WebContent/resources/script \
	  --releaseName bg_navigator \
	  --buildReportDir "$$BUILD_REPORT_DIR"


$(installed_navigator_conf): navigator.conf
	$(MKDIR) -p $(dir $@)
	$(CP) $< $@

$(install_dir)/WebContent/resources/diags/tests.properties:
	$(MKDIR) -p $(dir $@)
	$(RM) $@
	$(LN) -sf ../../../../diags/etc/tests.properties $@

$(install_dir)/WebContent/resources/doc/bgpm:
	$(MKDIR) -p $(dir $@)
	$(RM) $@
	$(LN) -sf ../../../../bgpm/docs/html $@

$(install_dir)/WebContent/resources/doc/bgsched:
	$(MKDIR) -p $(dir $@)
	$(RM) $@
	$(LN) -sf ../../../../hlcs/docs/bgsched/external $@

$(install_dir)/WebContent/resources/doc/bgws:
	$(MKDIR) -p $(dir $@)
	$(RM) $@
	$(LN) -sf ../../../../bgws/doc/html $@

$(install_dir)/WebContent/resources/doc/pami:
	$(MKDIR) -p $(dir $@)
	$(RM) $@
	$(LN) -sf ../../../../comm/share/doc/pami/html $@

$(install_dir)/WebContent/resources/doc/spi:
	$(MKDIR) -p $(dir $@)
	$(RM) $@
	$(LN) -sf ../../../../spi/doc/html $@

$(install_dir)/WebContent/resources/doc/RasEventBook.htm:
	$(MKDIR) -p $(dir $@)
	$(RM) $@
	$(LN) -sf ../../../../ras/etc/RasEventBook.htm $@


navigator.clean:
	$(RM) build/build-report.txt

navigator.distclean:
	$(RM) -r build

navigator.uninstall:
	$(RM) -r $(BGQ_INSTALL_DIR)/navigator

clean: doc.clean navigator.clean

distclean: doc.distclean navigator.distclean

uninstall: doc.uninstall navigator.uninstall


setup_devel:
	$(MKDIR) -p WebContent/resources/script/bg_navigator/
	$(LN) -sf ../../../../build/dojo/dijit ../../../../build/dojo/dojo ../../../../build/dojo/dojox ../../../../build/dojo/util WebContent/resources/script/bg_navigator/
	$(LN) -sf ../../../../script/bluegene WebContent/resources/script/bg_navigator/
	$(MKDIR) -p WebContent/resources/diags/
	$(LN) -sf ../../../../work/diags/etc/tests.properties WebContent/resources/diags/
	$(LN) -sf ../../../../work/bgsched/external WebContent/resources/doc/bgsched
	$(LN) -sf ../../../../work/comm/share/doc/pami/html WebContent/resources/doc/pami
	$(LN) -sf ../../../../work/bgpm/docs/html WebContent/resources/doc/bgpm
	$(LN) -sf ../../../../work/ras/etc/RasEventBook.htm WebContent/resources/doc/
	$(LN) -sf ../../../../work/bgws/doc/html WebContent/resources/doc/bgws


.PHONY: default all install clean distclean uninstall doc.default doc.install doc.clean doc.distclean doc.uninstall navigator.clean navigator.distclean navigator.uninstall setup_devel
