
#
# bgq scripts installation file.
#
#
#
# Install the script files into the scripts installation directory.
#

include ../Make.rules

all: install


SRC_SED_FILES	= svnupdate \
	      svncommit \
	      svncheckout \
	      runmesa.sh \
	      viewmesa.sh \
              runtwinstar.sh \
	      tsaetscan.sh

SRC_FILES = hwsim_env.sh \
	    randomseeds.pl \
            runfctest.sh \
	    addregression.sh \
	    bgqddxglibctest \
            RunMambo.tcl.tmpl \
            RunMambo.sh \
            RunMamboRegression.sh \
	    RunMmcsLite.sh \
	    MmcsLiteSetup.sh \
	    mmcslite.fwext.tpl \
	    mmcslite.fwddr.tpl \
	    mmcslite.cnk.tpl \
            LlRunFctest.sh \
	    gettwinstarsession.sh \
	    run-helpers.sh \
	    calibrate_watson_midplane \
            check_opt_health.sh \
            check_cable_spares.py \
	    snapbug.pl \
	    bgRA

#
# todo: find a way to check for uniqueness among the targets
#
DEST_FILES = $(addprefix $(BGQ_INSTALL_DIR)/scripts/, $(SRC_SED_FILES) $(SRC_FILES))


install:  $(DEST_FILES) $(call TLD_INSTALL_PACKAGING_TARGET, scripts.specsrc)


$(DEST_FILES): $(BGQ_INSTALL_DIR)/scripts

$(BGQ_INSTALL_DIR)/scripts:
	mkdir -p $@

clean:
	-rm -f dep.fileI      


distclean: clean
	-rm -rf autom4te.cache
	-rm -rf autom4te.cache
	-rm -f hwsim_env.sh
	-rm -f runfctest.sh
	-rm -f RunMambo.sh
	-rm -f RunMamboRegression.sh
	-rm -f RunMmcsLite.sh
	-rm -f MmcsLiteSetup.sh


#
# dynamically build the rules for copying the dest files
#
dep.files: Makefile $(SRC_SED_FILES) $(SRC_FILES)
	@-rm -f dep.files
	instdir=`echo $(BGQ_INSTALL_DIR) | sed 's/\//\\\\\\//g' `; \
	for f in $(SRC_SED_FILES); do \
	    destname=$(BGQ_INSTALL_DIR)/scripts/$$f ; \
	    echo "$$destname: $$f" >>dep.files ; \
	    echo "	rm -f \$$@" >>dep.files ; \
	    echo "	sed -e 's|\$$\$$BGQHOME/bgq/scripts/hwsim_env.sh|$$instdir/scripts/hwsim_env.sh|g' \$$< >\$$@" >>dep.files ; \
	    echo "	chmod +x-w $$destname " >>dep.files; \
	done; \
	for f in $(SRC_FILES); do \
	    destname=$(BGQ_INSTALL_DIR)/scripts/$$f ; \
	    echo "$$destname: $$f" >>dep.files ; \
	    echo "	cp -f \$$< \$$@" >>dep.files ; \
	done


ifeq ($(filter distclean clean,$(MAKECMDGOALS)),)
-include dep.files
endif






