include ../../Make.rules
include ../Make.extlib.rules

# bjam will take care of building this stuff in parallel
.NOTPARALLEL:

.PHONY: uninstall clean distclean

MAKECPUS=$(shell grep -c '^processor' /proc/cpuinfo)

#do nothing by default
all install:

reallyinstall: boost_1_49_0/b2

boost_1_49_0: boost_1_49_0.tar.bz2
	tar xjf $<
	touch $@

boost_1_49_0/bootstrap.sh: boost_1_49_0
	(cd $< && ./bootstrap.sh --prefix=$(BGQ_INSTALL_DIR)/extlib)
	touch $@

boost_1_49_0/b2: boost_1_49_0/bootstrap.sh
	(cd boost_1_49_0 && ./b2 -j$(MAKECPUS) --layout=tagged threading=multi link=shared address-model=64 install)
	touch $@

distclean: clean
	$(RM) -rf $(BGQ_INSTALL_DIR)/extlib/include/boost
	$(RM) -rf $(BGQ_INSTALL_DIR)/extlib/lib/libboost*
	$(RM) -rf boost_1_49_0

clean:
	-(cd boost_1_49_0 && ./b2 --clean-all)
