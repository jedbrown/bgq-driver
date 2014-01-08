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
include ${ROOTDIR}/Make.rules
include make/${PLATFORM}.platform
include make/*.build

export TEXTFILTER_PATH=.:/usr/local/share
export DOCTEXT_PATH=.:/usr/local/share/doctext
LD_LIBRARY_PATH=
LANG=en_US

DEFAULT=${PLATFORM_DEFAULT_MPICH2}
POST_INSTALL_LIST=$(foreach i, ${POST_PROCESS_LIST},${i}-install)

install: ${POST_INSTALL_LIST}
# Create the XL versions of the compiler scripts
	@ for i in ${POST_PROCESS_LIST} ; do				\
		perl genmpixl.pl ${INSTALL_DIR}/$$i ${PLATFORM};	\
	done
# Create libfmpich_ for the GA library
#	@ for i in ${POST_PROCESS_LIST} ; do													\
#		${HOST_NM} --defined ${INSTALL_DIR}/$$i/lib/libfmpich.a | perl -ne 'print "$${1} $${1}_\n" if m/^\d{8} T (\w+)$$/' > objnames;	\
#		${HOST_OBJCOPY} --redefine-syms=objnames ${INSTALL_DIR}/$$i/lib/libfmpich.a ${INSTALL_DIR}/$$i/lib/libfmpich_.a;		\
#	done
#	@ ${RM} objnames

pre-install: ${DEFAULT}
# Build the man pages and copy back to the source area for install
	${MAKE} -C ${MPICH2BUILD}-${DEFAULT} mandoc
	mkdir -p ${MPICH2DIR}/man/
	rsync -a --delete ${MPICH2BUILD}-${DEFAULT}/man/ ${MPICH2DIR}/man/
	${MAKE} -C ${MPICH2BUILD}-${DEFAULT} htmldoc
	${MAKE} -C ${MPICH2BUILD}-${DEFAULT} htmldoc-local
	mkdir -p ${MPICH2DIR}/www/
	rsync -a --delete ${MPICH2BUILD}-${DEFAULT}/www/ ${MPICH2DIR}/www/


# ***************************************************************************
# * This step needs to be performed the first time after checking out MPICH *
# ***************************************************************************
autoconf: ${MPICH2DIR}/Makefile.in
${MPICH2DIR}/Makefile.in: ${MPICH2DIR}/Makefile.sm
	@echo "============= Updating the MPICH2 directory =========="
	cd ${MPICH2DIR} && maint/updatefiles
	@touch --no-create ${MPICH2DIR}/Makefile.in

clean:
	${RM} -r ${BUILDDIR}
distclean: clean
	${RM} ${MPICH2DIR}/Makefile.in
re-configure:
	${RM} -v ${MPICH2BUILD}-*/Makefile
re-autoconf:
	${RM} -v ${MPICH2BUILD}-*/Makefile
	${RM} -v ${MPICH2DIR}/Makefile.in


DOCS="${INSTALL_DIR}/www/mpi:dcmfd"
docs:
	${RM} -r ${DOCS}
	mkdir -p ${DOCS}
	perl ${ROOTDIR}/tools/doxygen.pl											\
		--subdirs="${MPICH2DIR}/src/mpid/dcmfd ${MPICH2DIR}/src/mpi/romio/adio/ad_bg ${ROOTDIR}/sys/dcmf/include"	\
		--output=${DOCS}												\
		OPTIMIZE_OUTPUT_FOR_C=YES											\
		PROJECT_NAME='"MPICH2 DCMFd ADI"'										\
		HTML_OUTPUT=.													\
		${ROOTDIR}/tools/Doxyfile											\
		> ${DOCS}.out.txt 2> ${DOCS}.err.txt


# include ../Make.rules
# MPICH2_VERSION=1.1
# OUR_DEV=${LIB_DEV}/mpich2
# source:
# 	rm -rf ${OUR_DEV}
# 	tar -C ${LIB_DEV} -zxf ${TARBALL_DIRECTORY}/mpich2-${MPICH2_VERSION}.tar.gz
# 	mv ${OUR_DEV}-${MPICH2_VERSION} ${OUR_DEV}
# 	zcat patch.gz | patch -p0 --force --directory=${OUR_DEV} > /dev/null
# 	zcat patch.gz | ../../tools/patch-delete --directory=${OUR_DEV}


list-build:
	@ echo "Build list    = ${POST_PROCESS_LIST}"
	@ echo "Install list  = ${POST_INSTALL_LIST}"
	@ echo "Default build = ${DEFAULT}"
