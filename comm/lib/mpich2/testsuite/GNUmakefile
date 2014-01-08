
.PHONY: clean runjob 

TESTLIST ?= testlist
ifneq ($(origin TEST), undefined)
	TESTLIST = testlist.$(TEST)
endif

INSTALLED_MPICC = $(sort $(shell find $(TOP_INSTALL_DIR)/ -name mpicc 2>/dev/null))
MPI_INSTALL_DIRS = $(subst /mpicc,,$(INSTALLED_MPICC))
MPI_INSTALL_NAMES = $(notdir $(MPI_INSTALL_DIRS))
BUILD_DIRECTORIES = $(patsubst %,build/%,$(MPI_INSTALL_NAMES))
RUNTEST_TARGETS = $(patsubst %,build/%/runtests,$(MPI_INSTALL_NAMES))

help:
	@echo
	@echo "The GNUmakefile in this directory is a wrapper for the original"
	@echo "mpich2 testsuite Makefile in order to provide additional test"
	@echo "functionality. The original mpich2 testsuite Makefile can be"
	@echo "invoked using the '-f' parameter:"
	@echo
	@echo "  make -f Makefile"
	@echo
	@echo
	@echo "================================================================"
	@echo
	@echo "A build directory is automatically configured for each mpich2"
	@echo "installation that is found. By default, this particular build of"
	@echo "the mpich2 testsuite will look for mpicc scripts under the"
	@echo "directory:"
	@echo
	@echo "  $(TOP_INSTALL_DIR)"
	@echo
	@echo "The root directory to search for mpich2 installations can be"
	@echo "specified using the 'TOP_INSTALL_DIR' environment variable."
	@echo "For example:"
	@echo
	@echo "  TOP_INSTALL_DIR=/bgsys/drivers/ppcfloor make"
	@echo
	@echo "An alternate testlist can be specified using the 'TEST'"
	@echo "environment variable which will apply a similarly named patch"
	@echo "file to the build directory to create the required testlist"
	@echo "files. For example, the following will cause the 'testlist.bvt'"
	@echo "files to be used when running the test harness:"
	@echo
	@echo "  TEST=bvt make"
	@echo
	@echo
	@echo "================================================================"
	@echo
	@echo "The 'runjob' rule can be used to execute the test harness for"
	@echo "a bgq mpich2 installation. The first mpich2 installation found"
	@echo "will be tested when the"
	@echo
	@echo "  make runjob"
	@echo
	@echo "rule is invoked. Each individual mpich2 installation can be"
	@echo "tested with the 'make runjob-<installation>' rule, such as:"
	@echo
	@echo "  make runjob-gcc runjob-xl runjob xl.legacy"
	@echo
	@echo "All bgq mpich2 installations can be tested by using the"
	@echo "'make runjob-all' rule."
	@echo
	@echo "The 'make runjob' rules require the 'BLOCK' environment variable"
	@echo "to be set. All other environment variables are optional and"
	@echo "include:"
	@echo
	@echo "  BLOCK         : required"
	@echo "  PPN           : default is '4'"
	@echo "  TIMEOUT       : default is '234'"
	@echo "  XMLFILE       : default is 'summary_<BLOCK>_ppn<PPN>.xml'"
	@echo

debug:
	@echo "INSTALLED_MPICC   : $(INSTALLED_MPICC)"
	@echo "MPI_INSTALL_DIRS  : $(MPI_INSTALL_DIRS)"
	@echo "MPI_INSTALL_NAMES : $(MPI_INSTALL_NAMES)"
	@echo "BUILD_DIRECTORIES : $(BUILD_DIRECTORIES)"
	@echo "RUNTEST_TARGETS   : $(RUNTEST_TARGETS)"

testsuite_srcdir = $(realpath .)

$(RUNTEST_TARGETS): 
	mkdir -p $(dir $@)
	(											\
	cd $(dir $@) &&										\
	CC=$(filter %/$(patsubst build/%/runtests,%,$@),$(MPI_INSTALL_DIRS))/mpicc		\
	CXX=$(filter %/$(patsubst build/%/runtests,%,$@),$(MPI_INSTALL_DIRS))/mpicxx		\
	FC=$(filter %/$(patsubst build/%/runtests,%,$@),$(MPI_INSTALL_DIRS))/mpif90		\
	F77=$(filter %/$(patsubst build/%/runtests,%,$@),$(MPI_INSTALL_DIRS))/mpif77		\
	$(testsuite_srcdir)/configure								\
	--enable-strictmpi $(TESTSUITE_CONFIGURE_EXTRA)						\
	)


clean:
	rm -rf build 

%/$(TESTLIST): %/runtests
	( cd $(dir $@) && if [ -f ../../$(notdir $@).patch ]; then patch -p1 < ../../$(notdir $@).patch; fi )

runjob-%: PPN ?= 4
runjob-%: TIMEOUT ?= 234
runjob-%: RUNJOB ?= runjob
runjob-%: VERBOSE ?= 1
runjob-%: XMLFILE ?= summary_$(BLOCK)_ppn$(PPN).xml
$(patsubst %,runjob-%,$(MPI_INSTALL_NAMES)): runjob-%: build/%/$(TESTLIST)
	@echo "###########################################################################################"
	@echo "#"
	@echo "#  runjob testing in $(dir $<)"
	@echo "#"
	@echo "#  output is in $(realpath $(dir $<))/$(XMLFILE)"
	@echo "#"
	@echo "###########################################################################################"
	@echo
	(									\
	cd $(dir $<) && 							\
	VERBOSE=$(VERBOSE) 							\
	MPITEST_PROGRAM_WRAPPER=" --ranks-per-node $(PPN) --block $(BLOCK) --timeout $(TIMEOUT) $(RUNJOB_OPTIONS) : " \
	./runtests -srcdir=$(testsuite_srcdir)					\
	         -tests=$(TESTLIST)						\
	         -mpiexec=$(RUNJOB)						\
	         -xmlfile=$(XMLFILE)						\
	)

runjob: $(patsubst %,runjob-%,$(firstword $(MPI_INSTALL_NAMES)))

runjob-all: $(patsubst %,runjob-%,$(MPI_INSTALL_NAMES))


