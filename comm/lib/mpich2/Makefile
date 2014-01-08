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

AUTOGEN_DEPS=$(foreach i, $(shell cd ${MPICH2DIR} && git ls-files *.m4 *.mk *.ac *.am), ${MPICH2DIR}/${i})
MPICH_DEPS=$(foreach i, $(shell cd ${MPICH2DIR} && git ls-files ), ${MPICH2DIR}/${i})

ifeq ("${_MPICH_TOOLS_BIN_}", "")
	include ${ROOTDIR}/Make.rules
	include make/${PLATFORM}.platform
	export TEXTFILTER_PATH=.:/usr/local/share
	export DOCTEXT_PATH=.:/usr/local/share/doctext
	LD_LIBRARY_PATH=
	LANG=en_US
endif

MPICH_TARGET?=${POST_PROCESS_LIST}

MPICH_TESTSUITE_BUILD=${MPICH2BUILD}-$(firstword ${MPICH_TARGET})/test/mpi

.SECONDARY: $(foreach i, ${MPICH_TARGET}, ${INSTALL_DIR}/bin/${i}/mpicc)
.SECONDARY: $(foreach i, ${MPICH_TARGET}, ${TOP_BUILD_DIR}/packaging/libopa-$(sort $(firstword $(subst ., ,${i}))).so)
.SECONDARY: $(foreach i, ${MPICH_TARGET}, ${TOP_BUILD_DIR}/packaging/libmpl-$(sort $(firstword $(subst ., ,${i}))).so)

include make/*.build

install: ${MPICH_TARGET} $(foreach i, ${MPICH_TARGET}, ${TOP_BUILD_DIR}/packaging/libmpich-${i}.so) ${INSTALL_DIR}/mpich2-testsuite-1.2.tar.gz

$(MPICH_TESTSUITE_BUILD)/mpich2-testsuite-1.2.tar.gz: ${MPICH2BUILD}-$(firstword ${MPICH_TARGET})/Makefile
	@echo "============= Generate mpich testsuite distribution ============="
	make -C $(MPICH_TESTSUITE_BUILD) -j1 dist > /dev/null 2>&1

${INSTALL_DIR}/mpich2-testsuite-1.2.tar.gz: $(MPICH_TESTSUITE_BUILD)/mpich2-testsuite-1.2.tar.gz
	@echo "============= Customize mpich testsuite distribution ============"
	for tmpdir in $(shell mktemp -d) ; do					\
		tar -C $$tmpdir -xzf $(MPICH_TESTSUITE_BUILD)/mpich2-testsuite-1.2.tar.gz;	\
		echo "TOP_INSTALL_DIR ?= ${INSTALL_DIR}" > $$tmpdir/mpich2-testsuite-1.2/GNUmakefile;		\
		echo "TESTSUITE_CONFIGURE_EXTRA ?= ${TESTSUITE_CONFIGURE_EXTRA}" >> $$tmpdir/mpich2-testsuite-1.2/GNUmakefile;	\
		cat testsuite/GNUmakefile >> $$tmpdir/mpich2-testsuite-1.2/GNUmakefile;	\
		cp testsuite/*.patch $$tmpdir/mpich2-testsuite-1.2;	\
		tar -C $$tmpdir -czf ${INSTALL_DIR}/mpich2-testsuite-1.2.tar.gz mpich2-testsuite-1.2;	\
		rm -rf $$tmpdir;	\
	done

ifeq ("${PLATFORM}", "BGQ")
${MPICH_TARGET}: %: ${INSTALL_DIR}/bin/%/mpixlc ${INSTALL_DIR}/%/DEPRECATED ${INSTALL_DIR}/sys/DEPRECATED ${INSTALL_DIR}/sys-fast/DEPRECATED
	@echo "mpich $* installed in ${INSTALL_DIR}"
endif

autoconf: ${MPICH2DIR}/Makefile.in
${MPICH2DIR}/Makefile.in: ${AUTOGEN_DEPS}
	@echo "============= Updating the MPICH2 directory =========="
	cd ${MPICH2DIR} && ./autogen.sh

${INSTALL_DIR}/bin/%/mpixlc: ${INSTALL_DIR}/bin/%/mpicc
	@echo "generate $* mpixl* compiler scripts"
	perl genmpixl.pl $(dir $<) ${PLATFORM}

${INSTALL_DIR}/bin/%/mpicc: ${MPICH2BUILD}-%/Makefile ${MPICH_DEPS}
	${MAKE} -C $(dir $<) install


${TOP_BUILD_DIR}/packaging/libmpich.so:
	@echo "============= Generate packaging symlinks for mpich backward compatibility ============="
	@mkdir -p ${TOP_BUILD_DIR}/packaging
	ln -fs libmpichcxx-gcc.so $(dir $@)/libcxxmpich.so
	ln -fs libmpichcxx-gcc.so.8 $(dir $@)/libcxxmpich.so.8
	ln -fs libmpichcxx-gcc.so.8.0.1 $(dir $@)/libcxxmpich.so.8.0.1
	ln -fs libmpichf77-gcc.so $(dir $@)/libfmpich.so
	ln -fs libmpichf77-gcc.so.8 $(dir $@)/libfmpich.so.8
	ln -fs libmpichf77-gcc.so.8.0.1 $(dir $@)/libfmpich.so.8.0.1
	ln -fs libmpichf90-gcc.so $(dir $@)/libmpichf90.so
	ln -fs libmpichf90-gcc.so.8 $(dir $@)/libmpichf90.so.8
	ln -fs libmpichf90-gcc.so.8.0.1 $(dir $@)/libmpichf90.so.8.0.1
	ln -fs libmpich-gcc.so $(dir $@)/libmpich.so
	ln -fs libmpich-gcc.so.8 $(dir $@)/libmpich.so.8
	ln -fs libmpich-gcc.so.8.0.1 $(dir $@)/libmpich.so.8.0.1


${TOP_BUILD_DIR}/packaging/libmpich-%.so: ${TOP_BUILD_DIR}/packaging/libopa-$(sort $(firstword $(subst ., ,%))).so	\
	                                  ${TOP_BUILD_DIR}/packaging/libmpl-$(sort $(firstword $(subst ., ,%))).so
	@echo "============= Generate packaging symlinks for mpich ============="
	@mkdir -p ${TOP_BUILD_DIR}/packaging
	ln -fs ${INSTALL_DIR}/lib/libmpich-$*.so $(dir $<)/libmpich-$*.so
	ln -fs ${INSTALL_DIR}/lib/libmpich-$*.so.8 $(dir $<)/libmpich-$*.so.8
	ln -fs ${INSTALL_DIR}/lib/libmpich-$*.so.8.0.1 $(dir $<)/libmpich-$*.so.8.0.1
	ln -fs ${INSTALL_DIR}/lib/libmpichcxx-$*.so $(dir $<)/libmpichcxx-$*.so
	ln -fs ${INSTALL_DIR}/lib/libmpichcxx-$*.so.8 $(dir $<)/libmpichcxx-$*.so.8
	ln -fs ${INSTALL_DIR}/lib/libmpichcxx-$*.so.8.0.1 $(dir $<)/libmpichcxx-$*.so.8.0.1
	ln -fs ${INSTALL_DIR}/lib/libmpichf77-$*.so $(dir $<)/libmpichf77-$*.so
	ln -fs ${INSTALL_DIR}/lib/libmpichf77-$*.so.8 $(dir $<)/libmpichf77-$*.so.8
	ln -fs ${INSTALL_DIR}/lib/libmpichf77-$*.so.8.0.1 $(dir $<)/libmpichf77-$*.so.8.0.1
	ln -fs ${INSTALL_DIR}/lib/libmpichf90-$*.so $(dir $<)/libmpichf90-$*.so
	ln -fs ${INSTALL_DIR}/lib/libmpichf90-$*.so.8 $(dir $<)/libmpichf90-$*.so.8
	ln -fs ${INSTALL_DIR}/lib/libmpichf90-$*.so.8.0.1 $(dir $<)/libmpichf90-$*.so.8.0.1

${INSTALL_DIR}/lib/libopa-%.a: ${BUILDDIR}/opa-%/Makefile
	${MAKE} -C $(dir $<) install

${TOP_BUILD_DIR}/packaging/libopa-%.so: 
	@echo "============= Generate packaging symlinks for opa ============="
	@mkdir -p ${TOP_BUILD_DIR}/packaging
	ln -fs ${INSTALL_DIR}/lib/libopa-$*.so $(dir $@)/libopa-$*.so
	ln -fs ${INSTALL_DIR}/lib/libopa-$*.so.0 $(dir $@)/libopa-$*.so.0
	ln -fs ${INSTALL_DIR}/lib/libopa-$*.so.0.0.0 $(dir $@)/libopa-$*.so.0.0.0

${INSTALL_DIR}/lib/libmpl-%.a: ${BUILDDIR}/mpl-%/Makefile
	${MAKE} -C $(dir $<) install

${TOP_BUILD_DIR}/packaging/libmpl-%.so: 
	@echo "============= Generate packaging symlinks for mpl ============="
	@mkdir -p ${TOP_BUILD_DIR}/packaging
	ln -fs ${INSTALL_DIR}/lib/libmpl-$*.so $(dir $@)/libmpl-$*.so
	ln -fs ${INSTALL_DIR}/lib/libmpl-$*.so.1 $(dir $@)/libmpl-$*.so.1
	ln -fs ${INSTALL_DIR}/lib/libmpl-$*.so.1.0.0 $(dir $@)/libmpl-$*.so.1.0.0

# Create symlinks for backward campatibility reasons .. this could be removed at some point
${INSTALL_DIR}/sys/DEPRECATED:
	@mkdir -p ${INSTALL_DIR}/sys/lib
	@echo "" > ${INSTALL_DIR}/sys/DEPRECATED
	@echo "This directory, and all the files it contains, will be removed after V1R2M1." >> ${INSTALL_DIR}/sys/DEPRECATED
	@echo "" >> ${INSTALL_DIR}/sys/DEPRECATED
	ln -sf ${INSTALL_DIR}/include ${INSTALL_DIR}/sys/include

# Create symlinks for backward campatibility reasons .. this could be removed at some point
${INSTALL_DIR}/sys-fast/DEPRECATED:
	@mkdir -p ${INSTALL_DIR}/sys-fast/lib
	@echo "" > ${INSTALL_DIR}/sys-fast/DEPRECATED
	@echo "This directory, and all the files it contains, will be removed after V1R2M1." >> ${INSTALL_DIR}/sys-fast/DEPRECATED
	@echo "" >> ${INSTALL_DIR}/sys-fast/DEPRECATED
	ln -sf ${INSTALL_DIR}/include ${INSTALL_DIR}/sys-fast/include

# Create symlinks for backward campatibility reasons .. this could be removed at some point
${INSTALL_DIR}/%/DEPRECATED:
	@rm -rf ${INSTALL_DIR}/$*
	@mkdir -p ${INSTALL_DIR}/$*/lib
	@echo "" > $@
	@echo "This directory, and all the files it contains, will be removed after V1R2M1." >> $@
	@echo "===" >> $@
	@echo "Instead, use the mpi compile scripts from the directory" >> $@
	@echo "${INSTALL_DIR}/bin/$*/." >> $@
	@echo "" >> $@
	@rm -vf ${INSTALL_DIR}/$*/bin
	ln -fs ${INSTALL_DIR}/bin/$* ${INSTALL_DIR}/$*/bin;
	@rm -vf ${INSTALL_DIR}/$*/include
	ln -fs ${INSTALL_DIR}/include ${INSTALL_DIR}/$*/include;

#%-install: % man html

man html: $(firstword ${MPICH_TARGET})
	${MAKE} -C ${MPICH2BUILD}-$(firstword ${MPICH_TARGET}) $@doc install-$@-local

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
