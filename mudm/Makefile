include ../Make.rules

docs_install_dir=$(BGQ_INSTALL_DIR)/mudm/docs/internal

OS := $(shell uname)
OS_ARCH := $(shell uname -m)


DIRS = 
DIRS += include
DIRS += src

# BEAM_RESULTS += --beam::ipa=all --beam::data=$(BGQ_INSTALL_DIR)/firmware/beam

TARGETS_ALL		= $(addsuffix .all,$(DIRS))
TARGETS_INSTALL		= $(addsuffix .install,$(DIRS))
TARGETS_CLEAN		= $(addsuffix .clean,$(DIRS))
TARGETS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))
TARGETS_DOCS		= $(addsuffix .docs,$(DIRS))

TARGETS = $(TARGETS_ALL)       \
	  $(TARGETS_INSTALL)   \
	  $(TARGETS_CLEAN)     \
	  $(TARGETS_DISTCLEAN)

BUILD_LIBS :=  include.install
BUILD_DIRS := $(filter-out $(BUILD_LIBS),$(TARGETS_INSTALL))


# +++
#
# ---

.SUFFIXES:

.PHONY: docs default all install clean dist_clean $(TARGETS)

default: install docs

#
# dependency ordering...
# this must occure BEFORE the more general rule that provdides the commands
# to do the submake
$(BUILD_DIRS):: $(BUILD_LIBS)

# make the all also do an install so the top level default make works..
all: install

# +++
#
# ---

ras_scan:
	mkdir -p $(BGQ_INSTALL_DIR)/mudm/ras/metadata
	$(BGQ_BUILD_DIR)/scripts/rasScan.py '$(BGQ_BUILD_DIR)/mudm/src' '$(BGQ_INSTALL_DIR)/mudm/ras/metadata' 'mudm_ras.xml' '$(BGQ_INSTALL_DIR)/ras/include/ras.xml'

install: this.install $(TARGETS_INSTALL) ras_scan $(call TLD_INSTALL_PACKAGING_TARGET, mudm.specsrc)

clean:	this.clean $(TARGETS_CLEAN)

distclean: this.distclean $(TARGETS_DISTCLEAN)

fresh:: distclean install

# +++
#	Each element in "$TARGETS" is of the form:
#		subdirectory.target
#	"$(basename $@)" picks off "subdirectory"
#	"$(subst .,,$(suffix $@))" picks off "target"
# ---

$(TARGETS)::
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))


# +++
#	Targets to be executed in the current directory
# ---

this.all:
	@echo "making all"

this.install:
	@echo "making install"

this.clean:
	@echo "making clean"
	rm -f *~

this.distclean: this.clean
	@echo "making distclean"
	-@${RM} -rf $(BGQ_INSTALL_DIR)/mudm

print:
	@echo TARGETS_ALL=${TARGETS_ALL}
	@echo TARGETS_INSTALL=${TARGETS_INSTALL}
	@echo BUILD_DIRS=${BUILD_DIRS}
	@echo TARGETS=$(TARGETS)

docs: Doxyfile
	$(MKDIR) -p $(docs_install_dir)
	sed -e s^@OUTPUT_DIR@^$(docs_install_dir)^ Doxyfile | doxygen - &> doxygen.out

