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

default: install

ROOTDIR=$(shell readlink -f `pwd`/../..)

ifeq ("${_MPICH_TOOLS_BIN_}", "")
	include ${ROOTDIR}/Make.rules
	include make/${PLATFORM}.platform
	export TEXTFILTER_PATH=.:/usr/local/share
	export DOCTEXT_PATH=.:/usr/local/share/doctext
	LD_LIBRARY_PATH=
	LANG=en_US
endif

include make/*.build

DEFAULT=${PLATFORM_DEFAULT_MPICH2}
POST_INSTALL_LIST=$(foreach i, ${POST_PROCESS_LIST},${i}-install)

install: ${POST_INSTALL_LIST} ${EXTRA_PLATFORM_INSTALL_DEPENDENCIES}
install: ${POST_INSTALL_LIST} ${INSTALL_DIR}/mpich2-testsuite-1.2.tar.gz
# Create the XL versions of the compiler scripts
	@ for i in ${POST_PROCESS_LIST} ; do				\
		perl genmpixl.pl ${INSTALL_DIR}/$$i ${PLATFORM};	\
	done

%-install: % man html
	${MAKE} -C ${MPICH2BUILD}-$< install

man: ${DEFAULT}
# Build the man pages and copy back to the source area for install
	${MAKE} -C ${MPICH2BUILD}-${DEFAULT} mandoc
	mkdir -p ${MPICH2DIR}/man/
	rsync -a --delete ${MPICH2BUILD}-${DEFAULT}/man/ ${MPICH2DIR}/man/

html: ${DEFAULT}
# Build the html pages and copy back to the source area for install
	${MAKE} -C ${MPICH2BUILD}-${DEFAULT} htmldoc
	mkdir -p ${MPICH2DIR}/www/
	rsync -a --delete ${MPICH2BUILD}-${DEFAULT}/www/ ${MPICH2DIR}/www/

${MPICH2BUILD}-${DEFAULT}/test/mpi/mpich2-testsuite-1.2.tar.gz: ${DEFAULT}
	cd ${MPICH2BUILD}-${DEFAULT}/test/mpi && make dist

${INSTALL_DIR}/mpich2-testsuite-1.2.tar.gz: ${MPICH2BUILD}-${DEFAULT}/test/mpi/mpich2-testsuite-1.2.tar.gz
	for tmpdir in $(shell mktemp -d) ; do								\
		echo "tmpdir = $$tmpdir";								\
		tar -C $$tmpdir -xzf ${MPICH2BUILD}-${DEFAULT}/test/mpi/mpich2-testsuite-1.2.tar.gz;	\
		echo "TOP_INSTALL_DIR ?= ${INSTALL_DIR}" > $$tmpdir/mpich2-testsuite-1.2/GNUmakefile;	\
		echo "TESTSUITE_CONFIGURE_EXTRA ?= ${TESTSUITE_CONFIGURE_EXTRA}" >> $$tmpdir/mpich2-testsuite-1.2/GNUmakefile;	\
		cat testsuite/GNUmakefile >> $$tmpdir/mpich2-testsuite-1.2/GNUmakefile;  		\
		cp testsuite/*.patch $$tmpdir/mpich2-testsuite-1.2;				\
		tar -C $$tmpdir -czf $$tmpdir/mpich2-testsuite-1.2.tar.gz mpich2-testsuite-1.2;		\
		cp $$tmpdir/mpich2-testsuite-1.2.tar.gz ${INSTALL_DIR};					\
		rm -rf $$tmpdir;									\
	done


# ***************************************************************************
# * This step needs to be performed the first time after checking out MPICH *
# ***************************************************************************
autoconf: ${MPICH2DIR}/Makefile.in
${MPICH2DIR}/Makefile.in:
	@echo "============= Updating the MPICH2 directory =========="
	cd ${MPICH2DIR} && ./autogen.sh

clean:
	${RM} -r ${BUILDDIR}
distclean: clean
	${RM} ${MPICH2DIR}/Makefile.in
re-configure:
	${RM} -v ${MPICH2BUILD}-*/Makefile
re-autoconf:
	${RM} -v ${MPICH2BUILD}-*/Makefile
	${RM} -v ${MPICH2DIR}/Makefile.in


DOCS="${INSTALL_DIR}/www/mpi:pamid"
docs:
	${RM} -r ${DOCS}
	mkdir -p ${DOCS}
	perl ${ROOTDIR}/tools/doxygen.pl											\
		--subdirs="${MPICH2DIR}/src/mpid/pamid ${MPICH2DIR}/src/mpi/romio/adio/ad_bg ${ROOTDIR}/sys/dcmf/include"	\
		--output=${DOCS}												\
		OPTIMIZE_OUTPUT_FOR_C=YES											\
		PROJECT_NAME='"MPICH2 PAMI ADI"'										\
		HTML_OUTPUT=.													\
		${ROOTDIR}/tools/Doxyfile											\
		> ${DOCS}.out.txt 2> ${DOCS}.err.txt

list-build:
	@ echo "Build list    = ${POST_PROCESS_LIST}"
	@ echo "Install list  = ${POST_INSTALL_LIST}"
	@ echo "Default build = ${DEFAULT}"
