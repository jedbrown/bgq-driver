# -*- mode: makefile-gmake; -*-
# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
#  --------------------------------------------------------------- 
# Licensed Materials - Property of IBM                             
# Blue Gene/Q 5765-PER 5765-PRP                                    
#                                                                  
# (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           
# US Government Users Restricted Rights -                          
# Use, duplication, or disclosure restricted                       
# by GSA ADP Schedule Contract with IBM Corp.                      
#                                                                  
#  --------------------------------------------------------------- 
#                                                                  
# end_generated_IBM_copyright_prolog                               
#

.DEFAULT_GOAL := install
install:
ROOTDIR=$(shell readlink -f $$(pwd))
GITBRANCH = $(shell git branch --contains `git log -n 1 --pretty=format:%H` | sed -e s/\*// | grep -v "no branch" | sed -e s/[\ ]*// | head -n1)
include Make.rules
include Make.extra
.PHONY: all install default lib sys clean distclean docs

install: ${COMM_EXTRA_TARGETS} all
all default: lib HEAD-all compiler

${ROOTDIR}/Make.config.rules:
	perl tools/select_build
${INSTALL_DIR}:
	mkdir -p ${INSTALL_DIR}

clean distclean:
	${MAKE} -C sys $@
	${MAKE} -C lib $@
	${RM} -r build
	find . -name '*~' -print -delete , -name '.#*' -print -delete , -name 'core.*' -print -delete


lib: sys
lib  sys: ${INSTALL_DIR}
	${MAKE} ${JLEVEL} -sC $@

docs: lib-docs sys-docs
%-docs:
	${MAKE} -sC $* docs


REPOS=. sys/buildtools sys/pami lib/dev
CMD=status
git:
	@for i in ${REPOS}; do (cd $$i && pwd | grep --color . && git ${CMD}); done
update-all:
	${MAKE} -s git CMD="remote update -p"
rebase-all:
	${MAKE} -s git CMD="rebase origin/master"
branch-all:
	${MAKE} -s git CMD="branch -v|\grep ^*"
clone:
	${MAKE} -C sys $@
	${MAKE} -C lib $@

is_current:
	@if [ "`git log -n 1 --pretty=format:%H`" != "`git ls-remote ssh://git01.rchland.ibm.com/var/git/bgq/comm --heads origin ${GITBRANCH} | cut -f1`" ]; then \
	echo "COMM COMPONENT REQUIRES BUILD (bgq/comm)"; fi
	${MAKE} -C sys $@
	${MAKE} -C lib $@

HEAD-all: ${INSTALL_DIR}
	@rm -f ${INSTALL_DIR}/HEAD
	@for i in ${REPOS}; do (cd $$i && pwd | grep --color . && echo "`git log -n1 --pretty=format:%H` $$i" >> ${INSTALL_DIR}/HEAD); done

compiler: ${INSTALL_DIR}
	@if [ "${PLATFORM}" = "BGQ" ]; then \
	rm -f ${INSTALL_DIR}/COMPILER; \
	( \
	if [ -f ${HOST_CC} ]; then			\
	echo "#";					\
	echo "# ${HOST_CC} -v";				\
	echo "#";					\
        ${HOST_CC} -v 2>&1;				\
	fi;						\
	if [ -f ${FAST_CC} ]; then			\
	echo;						\
	echo "#";					\
	echo "# ${FAST_CC} -qversion=verbose";		\
	echo "#";					\
	${FAST_CC} -qversion=verbose;			\
	fi;						\
	if [ -f ${FAST_F77} ]; then			\
	echo;						\
	echo "#";					\
	echo "# ${FAST_F77} -qversion=verbose";		\
	echo "#";					\
	${FAST_F77} -qversion=verbose;			\
	fi;						\
	if [ -f ${FAST_F90} ]; then			\
	echo;						\
	echo "#";					\
	echo "# ${FAST_F90} -qversion=verbose";		\
	echo "#";					\
	${FAST_F90} -qversion=verbose;			\
	fi;						\
	) >> ${INSTALL_DIR}/COMPILER; fi

debug:
	@echo
	@echo
	@echo "HOST_AR         = $(HOST_AR)"
	@echo "HOST_LD         = $(HOST_LD)"
	@echo "HOST_CC         = $(HOST_CC)"
	@echo "HOST_CXX        = $(HOST_CXX)"
	@echo "HOST_F77        = $(HOST_F77)"
	@echo "HOST_F90        = $(HOST_F90)"
	@echo
	@echo


print-xl:
	echo ${XL}
	ls -1 "${word 1, ${FAST_CC}}" 		\
	      "${word 1, ${FAST_CXX}}"		\
	      "${word 1, ${FAST_F77}}"		\
	      "${word 1, ${FAST_F90}}"
