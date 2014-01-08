
include ../Make.rules

DIRS = 
DIRS += include
DIRS += src

TARGETS_ALL		= $(addsuffix .all,$(DIRS))
TARGETS_INSTALL		= $(addsuffix .install,$(DIRS))
TARGETS_CLEAN		= $(addsuffix .clean,$(DIRS))
TARGETS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))
TARGETS_UNINSTALL	= $(addsuffix .uninstall,$(DIRS))

TARGETS  = $(TARGETS_ALL)
TARGETS += $(TARGETS_INSTALL)
TARGETS += $(TARGETS_CLEAN)
TARGETS += $(TARGETS_DISTCLEAN)
TARGETS += $(TARGETS_UNINSTALL)

BUILD_LIBS :=  include.install
BUILD_DIRS := $(filter-out $(BUILD_LIBS),$(TARGETS_INSTALL))


.PHONY: stats default all install clean distclean uninstall $(TARGETS)

default:	install

$(BUILD_DIRS):: $(BUILD_LIBS)

all: install

all:		$(TARGETS_ALL)
install:	$(TARGETS_INSTALL) $(call TLD_INSTALL_PACKAGING_TARGET, hlcs.specsrc)
clean:		$(TARGETS_CLEAN)
distclean:	$(TARGETS_DISTCLEAN)
uninstall:	$(TARGETS_UNINSTALL)

$(TARGETS)::
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))

