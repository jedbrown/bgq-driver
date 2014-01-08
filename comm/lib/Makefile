# -*- mode: makefile-gmake; -*-
# begin_generated_IBM_copyright_prolog                             #
#                                                                  #
# This is an automatically generated copyright prolog.             #
# After initializing,  DO NOT MODIFY OR MOVE                       #
#  --------------------------------------------------------------- #
#                                                                  #
# (C) Copyright IBM Corp.  2007, 2008                              #
# IBM CPL License                                                  #
#                                                                  #
#  --------------------------------------------------------------- #
#                                                                  #
# end_generated_IBM_copyright_prolog                               #

ifeq ("${_MPICH_TOOLS_BIN_}", "")
	ROOTDIR = $(shell readlink -f $$(pwd)/..)
	include ${ROOTDIR}/Make.rules
endif

SUBDIRS=mpich2
GITBRANCH = $(shell git branch --contains `git log -n 1 --pretty=format:%H` | sed -e s/\*// | grep -v "no branch" | sed -e s/[\ ]*// | head -n1)
.PHONY: ${SUBDIRS} dev-rule

all install default: mpich2
dev-rule:
	if [[ ! -d dev ]]; then						\
	git clone ssh://git01.rchland.ibm.com/var/git/bgq/comm/lib/dev --branch=${GITBRANCH}; \
	if [[ -n "${GITDATE}" ]]; then (cd dev && git checkout `git log --before="${GITDATE}" -n 1 --pretty=format:%H`); fi; \
	fi

clone: dev-rule

is_current:
	@if [ "`cd dev && git log -n 1 --pretty=format:%H`" != "`git ls-remote ssh://git01.rchland.ibm.com/var/git/bgq/comm/lib/dev --heads origin ${GITBRANCH} | cut -f1`" ]; then \
	echo "COMM COMPONENT REQUIRES BUILD (bgq/comm/lib/dev)"; fi

${SUBDIRS}: dev-rule
	${MAKE} -sC $@


docs:
	@ echo no docs in lib/dev yet
clean distclean:
	for dir in ${SUBDIRS}; do ${MAKE} -C $$dir $@; done
