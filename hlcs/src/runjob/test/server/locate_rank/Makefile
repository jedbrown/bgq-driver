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

# these unit tests should not be run in parallel since they insert/remove rows
# in various tables and make assumptions about the current hardware availability
.NOTPARALLEL:

include ../../../../../../Make.rules
include ../../../../../Make.hlcs.rules
include ../../../Make.rules

CXXFLAGS += -Wall
CXXFLAGS += -Wno-old-style-cast
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/utility/include
CXXFLAGS += -DBOOST_TEST_DYN_LINK
CXXFLAGS += -DBOOST_TEST_MAIN

LDFLAGS += -L ../../../common -lrunjob_common
LDFLAGS += -Wl,-rpath,$(BGQ_BUILD_DIR)/hlcs/src/runjob/common
LDFLAGS += -lboost_filesystem-mt
LDFLAGS += -lboost_program_options-mt
LDFLAGS += -lboost_system-mt
LDFLAGS += -lboost_unit_test_framework-mt
LDFLAGS += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib
LDFLAGS += -L $(BGQ_INSTALL_DIR)/bgq_util/lib -lbgq_util
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/bgq_util/lib
LDFLAGS += -L $(BGQ_INSTALL_DIR)/control/lib -lbgqconfig
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/control/lib
LDFLAGS += ../../../server/handlers/locate_rank/ActiveBlock.o
LDFLAGS += ../../../server/handlers/locate_rank/ActiveJob.o
LDFLAGS += ../../../server/handlers/locate_rank/Block.o
LDFLAGS += ../../../server/handlers/locate_rank/Exception.o
LDFLAGS += ../../../server/handlers/locate_rank/HistoryBlock.o
LDFLAGS += ../../../server/handlers/locate_rank/HistoryJob.o
LDFLAGS += ../../../server/handlers/locate_rank/Job.o
LDFLAGS += ../../../server/job/RankOffset.o

TESTERS   = $(patsubst %.cc,%,$(wildcard *.cc))


# we don't want to execute these unit tests by default since they assume a certain machine configuration
# and hardware availability. The test rule is used to explicitly run the tests
all:  $(TESTERS:%=%.o)
test: $(TESTERS:%=out_%)


out_%: test_%
	@strip $<
	@echo "execute $@"
	$(SILENT)./$< 1> $(patsubst test_%, out_%, $<)


test_active_job: active_job.o Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_error_path: error_path.o Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_history_job_active_block: history_job_active_block.o Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_history_job_history_block: history_job_history_block.o Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_np_ranks_per_node: np_ranks_per_node.o Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)

test_small_blocks: small_blocks.o Makefile
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)



install: all 


clean:
	$(RM) test_* out_*
	$(RM) *.o
	$(RM) *.d


distclean: clean

.DELETE_ON_ERROR:

-include *.d
