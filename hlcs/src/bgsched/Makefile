# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# Licensed Materials - Property of IBM                             
#                                                                  
# Blue Gene/Q                                                      
#                                                                  
# (C) Copyright IBM Corp.  2004, 2012                              
#                                                                  
# US Government Users Restricted Rights -                          
# Use, duplication or disclosure restricted                        
# by GSA ADP Schedule Contract with IBM Corp.                      
#                                                                  
# This software is available to you under the                      
# Eclipse Public License (EPL).                                    
#                                                                  
# ================================================================ 
#                                                                  
# end_generated_IBM_copyright_prolog                               

include ../../Make.hlcs.rules
include ../../../Make.rules
include Make.bgsched.rules
include realtime/Make.parts
include allocator/Make.parts
include core/Make.parts
include runjob/Make.parts

SUBDIRS = allocator realtime core runjob samples docs

lib_base=bgsched
lib_major_vers=1
lib_minor_vers=2
lib_minor_rev=1

lib_short=lib$(lib_base).so
lib_vers=$(lib_short).$(lib_major_vers)
lib_full=$(lib_vers).$(lib_minor_vers).$(lib_minor_rev)

SRCS         = bgsched.cc
SRCS        += Block.cc
SRCS        += BlockImpl.cc
SRCS        += BlockFilter.cc
SRCS        += BlockFilterImpl.cc
SRCS        += Cable.cc
SRCS        += CableImpl.cc
SRCS        += ComputeHardware.cc
SRCS        += ComputeHardwareImpl.cc
SRCS        += Coordinates.cc
SRCS        += CoordinatesImpl.cc
SRCS        += DatabaseException.cc
SRCS        += Dimension.cc
SRCS        += Hardware.cc
SRCS        += HardwareImpl.cc
SRCS        += InitializationException.cc
SRCS        += InputException.cc
SRCS        += InternalException.cc
SRCS        += IOBlock.cc
SRCS        += IOBlockFilter.cc
SRCS        += IOBlockFilterImpl.cc
SRCS        += IOBlockImpl.cc
SRCS        += IODrawer.cc
SRCS        += IODrawerImpl.cc
SRCS        += IOHardware.cc
SRCS        += IOHardwareImpl.cc
SRCS        += IOLink.cc
SRCS        += IOLinkImpl.cc
SRCS        += IONode.cc
SRCS        += IONodeImpl.cc
SRCS        += Job.cc
SRCS        += JobImpl.cc
SRCS        += JobFilter.cc
SRCS        += JobFilterImpl.cc
SRCS        += Midplane.cc
SRCS        += MidplaneImpl.cc
SRCS        += Node.cc
SRCS        += NodeImpl.cc
SRCS        += NodeBoard.cc
SRCS        += NodeBoardImpl.cc
SRCS        += RuntimeException.cc
SRCS        += SchedUtil.cc
SRCS        += Shape.cc
SRCS        += ShapeImpl.cc
SRCS        += Switch.cc
SRCS        += SwitchImpl.cc
SRCS        += SwitchSettings.cc
SRCS        += SwitchSettingsImpl.cc
SRCS        += TimeInterval.cc
SRCS        += utility.cc

OBJS        = $(SRCS:%.cc=%.o)

bgsched.o: CXXFLAGS += -DMAJOR_VERSION=$(lib_major_vers) -DMINOR_VERSION=$(lib_minor_vers) -DMOD_VERSION=$(lib_minor_rev)

allocator_objs_d=$(allocator_objs:%=allocator/%)
realtime_objs_d=$(realtime_objs:%=realtime/%)
core_objs_d=$(core_objs:%=core/%)
runjob_objs_d=$(runjob_objs:%=runjob/%)

lib_objs=$(OBJS) $(allocator_objs_d) $(realtime_objs_d) $(core_objs_d) $(runjob_objs_d)

exe_cxxflags=$(CXXFLAGS)

ifneq ($(BITS),32)
exe_cxxflags +=-m64
endif

LDFLAGS += -Wl,-soname,$(lib_vers)
LDFLAGS += -L $(BGQ_INSTALL_DIR)/bgq_util/lib -lbgq_util
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/bgq_util/lib
LDFLAGS += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib
LDFLAGS += -L $(BGQ_INSTALL_DIR)/hlcs/lib -lsecurity
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib
LDFLAGS += -L../libmmcs_client -lmmcs_client
LDFLAGS += $(realtime_ldflags)
LDFLAGS += $(core_ldflags)
LDFLAGS += -ldl

INSTALL_LIST  =  $(call INSTALL_LIB_TARGET, $(lib_short) $(lib_vers) $(lib_full))
INSTALL_LIST  += $(call INSTALL_BIN_TARGET, realtime_client)
INSTALL_LIST  += $(call INSTALL_BIN_TARGET, test_core)

.PHONY: $(SUBDIRS) all clean default distclean docs install tags

default: all samples

samples: install
	$(MAKE) -C $@

all: $(lib_short) test_core test_main test_filter realtime_client test_kill_job reboot_io_node test_query_block_action test_deallocate

$(lib_full): $(lib_objs)
	$(CXX) -shared $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(lib_vers): $(lib_full)
	ln -sf $< $@

$(lib_short): $(lib_vers)
	ln -sf $< $@

install: all docs $(INSTALL_LIST)
	@for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

uninstall:
	$(RM) $(INSTALL_LIST)	

clean:
	$(RM) *.o *.o_64 $(lib_short) $(lib_vers) $(lib_full)
	$(RM) *.d
	$(RM) test_main realtime_client test_filter test_core plugin_tester test_kill_job test_query_block_action test_deallocate
	@for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

distclean: clean
	$(RM) $(INSTALL_LIST)
	@for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

test_main: test_main.o $(lib_short)
	$(CXX) $(exe_cxxflags) -Wl,-rpath,. -o $@ $^

test_allocator: test_allocator.o $(lib_short)
	$(CXX) $(exe_cxxflags) -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib:$(BGQ_INSTALL_DIR)/utility/lib:$(BGQ_INSTALL_DIR)/extlib/lib -o $@ $^

test_filter: test_filter.o $(lib_short)
	$(CXX) $(exe_cxxflags) -Wl,-rpath,. -o $@ $^ -lboost_date_time-mt

test_kill_job: test_kill_job.o $(lib_short)
	$(CXX) $(exe_cxxflags) -Wl,-rpath,. -o $@ $^

realtime_client: realtime_client.o $(lib_short)
	$(CXX) $(exe_cxxflags) -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib:$(BGQ_INSTALL_DIR)/extlib/lib:$(BGQ_INSTALL_DIR)/utility/lib:$(BGQ_INSTALL_DIR)/db/lib -o $@ $^ -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb  -L $(BGQ_INSTALL_DIR)/extlib/lib -llog4cxx -lboost_program_options-mt

test_core: test_core.o $(lib_short)
	$(CXX) $(exe_cxxflags) -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib:$(BGQ_INSTALL_DIR)/utility/lib:$(BGQ_INSTALL_DIR)/extlib/lib -o $@ $^ -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility -L $(BGQ_INSTALL_DIR)/extlib/lib -llog4cxx -lboost_date_time-mt

test_query_block_action: test_query_block_action.o $(lib_short)
	$(CXX) $(exe_cxxflags) -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib:$(BGQ_INSTALL_DIR)/utility/lib:$(BGQ_INSTALL_DIR)/extlib/lib -o $@ $^ -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility -L $(BGQ_INSTALL_DIR)/extlib/lib -llog4cxx -lboost_system-mt -lboost_program_options-mt

test_deallocate: test_deallocate.o $(lib_short)
	$(CXX) $(exe_cxxflags) -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib:$(BGQ_INSTALL_DIR)/utility/lib:$(BGQ_INSTALL_DIR)/extlib/lib -o $@ $^ -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility -L $(BGQ_INSTALL_DIR)/extlib/lib -llog4cxx -lboost_system-mt -lboost_program_options-mt

reboot_io_node: reboot_io_node.o $(lib_short)
	$(CXX) $(exe_cxxflags) -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib:$(BGQ_INSTALL_DIR)/utility/lib:$(BGQ_INSTALL_DIR)/extlib/lib -o $@ $^ -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility -lboost_thread-mt -L $(BGQ_INSTALL_DIR)/extlib/lib -lboost_program_options-mt

plugin_tester: plugin_tester.o install
	$(CXX) -o $@ plugin_tester.o $(exe_cxxflags) -L. -lbgsched -Wl,-rpath,.

docs: $(lib_short)
	$(MAKE) --directory $@

tags:
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q *.cc *.h ../../include/bgsched allocator realtime core

-include *.d
-include allocator/*.d
-include realtime/*.d
-include core/*.d
-include runjob/*.d
