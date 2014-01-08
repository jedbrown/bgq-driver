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
BASEDIR = ${ROOTDIR}/build/pami
GITBRANCH = $(shell git branch --contains `git log -n 1 --pretty=format:%H` | sed -e s/\*// | grep -v "no branch" | sed -e s/[\ ]*// | head -n1)
include ${ROOTDIR}/Make.rules

ifneq ($(origin XL_PAMI), undefined)
XL_PAMI_TARGET = ${BASEDIR}-xlfast/libpami.a
endif

.PHONY: ${BASEDIR}/libpami.a ${BASEDIR}-fast/libpami.a ${XL_PAMI_TARGET} clean distclean docs

MAKEFILE_INPUTS = buildtools/configure pami/GNUmakefile.in $(shell find . -type f -iname '*make*.in')


all install default: ${BASEDIR}/libpami.a ${BASEDIR}-fast/libpami.a ${XL_PAMI_TARGET} install-doc


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

${BASEDIR}/Makefile: ${MAKEFILE_INPUTS}
	mkdir -p ${BASEDIR}
	cd ${BASEDIR} && ${ROOTDIR}/sys/buildtools/configure ${PAMI_CONFIG}
${BASEDIR}/libpami.a: ${BASEDIR}/Makefile
	${MAKE} -sC ${BASEDIR}

${BASEDIR}-fast/Makefile: ${MAKEFILE_INPUTS}
	mkdir -p ${BASEDIR}-fast
	cd ${BASEDIR}-fast && ${ROOTDIR}/sys/buildtools/configure ${PAMI_CONFIG_FAST}
${BASEDIR}-fast/libpami.a: ${BASEDIR}-fast/Makefile
	${MAKE} -sC ${BASEDIR}-fast

${BASEDIR}-xlfast/Makefile: ${MAKEFILE_INPUTS}
	mkdir -p ${BASEDIR}-xlfast
	echo "xlfast config options: ${PAMI_CONFIG_XLFAST}"
	cd ${BASEDIR}-xlfast && ${ROOTDIR}/sys/buildtools/configure ${PAMI_CONFIG_XLFAST}
${BASEDIR}-xlfast/libpami.a: ${BASEDIR}-xlfast/Makefile
	${MAKE} -sC ${BASEDIR}-xlfast


clean:
	${RM} -r ${BASEDIR}{,-fast,-xlfast}
distclean: clean
	${RM} buildtools/configure buildtools/pami.lnk buildtools/pami

docs:
	${MAKE} -sC ${BASEDIR} docs

install-doc: pami/GNUmakefile.in
	${MAKE} -sC pami/sys INSTALL_DIR=${INSTALL_DIR}
