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

ROOTDIR = $(shell readlink -f $$(pwd)/..)
TOP_BUILD_DIR = ${ROOTDIR}/build
PAMIBUILD = ${TOP_BUILD_DIR}/pami
GITBRANCH = $(shell git branch --contains `git log -n 1 --pretty=format:%H` | sed -e s/\*// | grep -v "no branch" | sed -e s/[\ ]*// | head -n1)
include ${ROOTDIR}/Make.rules

PAMI_TARGET?=gcc gcc.ndebug
ifneq ($(origin XL_PAMI), undefined)
PAMI_TARGET+=xl.ndebug
endif

PAMI_DEPS=$(foreach i, $(shell cd pami && git ls-files ), pami/${i})

.PHONY: $(PAMI_TARGET) clean distclean docs

MAKEFILE_INPUTS = buildtools/configure pami/GNUmakefile.in $(shell find . -type f -iname '*make*.in')


all install default: $(PAMI_TARGET) install-doc

include make/*.build

pami/GNUmakefile.in:
	git clone ssh://git01.rchland.ibm.com/var/git/pami --branch=${GITBRANCH}
	@if [ -n "${GITDATE}" ]; then (cd pami && git checkout `git log --before="${GITDATE}" -n 1 --pretty=format:%H`); fi

buildtools/configure.in:
	git clone ssh://git01.rchland.ibm.com/var/git/buildtools --branch=${GITBRANCH}
	@if [ -n "${GITDATE}" ]; then (cd buildtools && git checkout `git log --before="${GITDATE}" -n 1 --pretty=format:%H`); fi
buildtools/pami.lnk: buildtools/configure.in
	${RM} buildtools/pami
	ln -sv ${ROOTDIR}/sys/pami buildtools/pami
	touch $@
buildtools/configure: buildtools/configure.in buildtools/pami.lnk
	cd buildtools && ./autogen.sh
	touch $@

clone: pami/GNUmakefile.in buildtools/configure.in

is_current:
	@if [ "`cd pami && git log -n 1 --pretty=format:%H`" != "`git ls-remote ssh://git01.rchland.ibm.com/var/git/pami --heads origin ${GITBRANCH} | cut -f1`" ]; then \
	echo "COMM COMPONENT REQUIRES BUILD (pami)"; fi
	@if [ "`cd buildtools && git log -n 1 --pretty=format:%H`" != "`git ls-remote ssh://git01.rchland.ibm.com/var/git/buildtools --heads origin ${GITBRANCH} | cut -f1`" ]; then \
	echo "COMM COMPONENT REQUIRES BUILD (buildtools)"; fi


${PAMIBUILD}/%/Makefile: ${MAKEFILE_INPUTS}
	mkdir -p $(dir $@)
	cd $(dir $@) && ${ROOTDIR}/sys/buildtools/configure			\
	  --with-target=${PLATFORMlc}						\
	  --with-installdir=${INSTALL_DIR}					\
	  --with-bgfloor=${BGQ_INSTALL_DIR}					\
	  --with-libname=pami-$(lastword $(subst /, ,$(dir $@)))		\
	  --without-silence							\
	  --with-pami-extension=torus_network,async_progress,bgq_l2atomic,is_local_task	\
	  $(EXTRA_CONFIGURE_OPTIONS)


clean:
	${RM} -r ${PAMIBUILD}
distclean: clean
	${RM} buildtools/configure buildtools/pami.lnk buildtools/pami

docs:
	${MAKE} -sC ${PAMIBUILD}/$(firstword $(PAMI_TARGET))  docs

install-doc: pami/GNUmakefile.in
	${MAKE} -sC pami/sys INSTALL_DIR=${INSTALL_DIR}
	${RM} -f ${INSTALL_DIR}/doc.err ${INSTALL_DIR}/doc.out
