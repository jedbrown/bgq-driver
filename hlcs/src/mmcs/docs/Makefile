# Licensed Machine Code Source Materials                          #
#                                                                 #
# Product(s):                                                     #
#     Blue Gene/Q Licensed Machine Code                           #
#                                                                 #
# (C) Copyright IBM Corp.  2010, 2011                             #
#                                                                 #
# The Source code for this program is not published  or otherwise #
# divested of its trade secrets,  irrespective of what has been   #
# deposited with the U.S. Copyright Office.                       #
# --------------------------------------------------------------- #

include ../../../../Make.rules
include ../../../Make.hlcs.rules

DIRS = man
DIRS += html

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

.PHONY: $(DIRS) clean docs install distclean

default:        install
all:            install $(TARGETS_ALL)
install:		$(TARGETS_INSTALL)
clean:			$(TARGETS_CLEAN)
distclean:	clean   $(TARGETS_DISTCLEAN)
uninstall:              $(TARGETS_UNINSTALL)

$(TARGETS)::
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))
