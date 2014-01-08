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
# (C) Copyright IBM Corp.  2011, 2011                              
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

include ../../../../../Make.rules
include ../../../../Make.hlcs.rules
include ../../Make.rules

DIRS += $(subst /Makefile,,$(wildcard */Makefile))

TARGETS_ALL		= $(addsuffix .all,$(DIRS))
TARGETS_INSTALL		= $(addsuffix .install,$(DIRS))
TARGETS_CLEAN		= $(addsuffix .clean,$(DIRS))
TARGETS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))
TARGETS_UNINSTALL	= $(addsuffix .uninstall,$(DIRS))


TARGETS  = $(TARGETS_ALL)
TARGETS += $(TARGETS_INSTALL)
TARGETS += $(TARGETS_CLEAN)
TARGETS += $(TARGETS_DISTCLEAN)
TARGETS += $(TARGETS_UNINSTALL)


CXXFLAGS += -Wall
CXXFLAGS += -Wno-old-style-cast
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/ramdisk/include
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/utility/include
CXXFLAGS += -pthread
CXXFLAGS += -DBOOST_TEST_DYN_LINK
CXXFLAGS += -DBOOST_TEST_MAIN

LDFLAGS += -L ../../common -lrunjob_common
LDFLAGS += -L ../../client -lrunjob_client
LDFLAGS += -Wl,-rpath,$(BGQ_BUILD_DIR)/hlcs/src/runjob/common
LDFLAGS += -Wl,-rpath,$(BGQ_BUILD_DIR)/hlcs/src/runjob/client
LDFLAGS += -lboost_filesystem-mt
LDFLAGS += -lboost_program_options-mt
LDFLAGS += -lboost_system-mt
LDFLAGS += -lboost_unit_test_framework-mt
LDFLAGS += -lboost_serialization-mt
LDFLAGS += -L $(BGQ_INSTALL_DIR)/bgq_util/lib -lbgq_util
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/bgq_util/lib
LDFLAGS += -llog4cxx

TESTERS   = $(patsubst %.cc,%,$(wildcard *.cc))

all:            $(TARGETS_ALL) $(TESTERS:%=out_%)
install:        $(TARGETS_INSTALL) $(TESTERS:%=out_%)
distclean:      clean $(TARGETS_DISTCLEAN)
uninstall:      $(TARGETS_UNINSTALL)
clean:          $(TARGETS_CLEAN)
	$(RM) test_* out_*
	$(RM) *.o
	$(RM) *.d

$(TARGETS):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))

DEPENDENCIES += Makefile


all: $(TESTERS:%=out_%) 

out_%: test_%
	@strip $<
	@echo "execute $@"
	$(SILENT)BG_PROPERTIES_FILE=$(BGQ_INSTALL_DIR)/utility/etc/bg.properties.tpl ./$< 1> $(patsubst test_%, out_%, $<)

test_args: args.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_block test_exe test_mapping: LDFLAGS += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib

test_block: block.o $(DEPENDENCIES) 
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_corner: corner.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_cwd: cwd.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_env_all: env_all.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_envs: envs.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_exe: exe.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_exp_env: exp_env.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_file_output: file_output.o $(DEPENDENCIES) ../../client/options/Label.o ../../client/FileOutput.o ../../client/Output.o
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_help: help.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_label: label.o $(DEPENDENCIES) ../../client/options/Label.o
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_mapping: mapping.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_mapping_file: mapping_file.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_misc: misc.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_np: np.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_raise: raise.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_ranks: ranks.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_shape: shape.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_socket: socket.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_stdinrank: stdinrank.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_strace: strace.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_stream_output: stream_output.o $(DEPENDENCIES) ../../client/options/Label.o ../../client/StreamOutput.o ../../client/Output.o
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_timeout: timeout.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_tool: tool.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_totalview: totalview.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_version: version.o $(DEPENDENCIES)
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

install: all 


.DELETE_ON_ERROR:

-include *.d
