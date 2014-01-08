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
LDFLAGS += -llog4cxx
LDFLAGS += -lboost_date_time-mt
LDFLAGS += -lboost_filesystem-mt
LDFLAGS += -lboost_serialization-mt
LDFLAGS += -lboost_program_options-mt
LDFLAGS += -lboost_system-mt
LDFLAGS += -lboost_unit_test_framework-mt
LDFLAGS += -L ../../../common/commands -lrunjob_command
LDFLAGS += -Wl,-rpath,$(BGQ_BUILD_DIR)/hlcs/src/runjob/common

TESTERS   = $(patsubst %.cc,%,$(wildcard *.cc))


all: $(TESTERS:%=out_%)


out_%: test_%
	@strip $<
	@echo "execute $@"
	$(SILENT)BG_PROPERTIES_FILE=$(BGQ_INSTALL_DIR)/utility/etc/bg.properties.tpl ./$< 1> $(patsubst test_%, out_%, $<)


test_authority: authority.o ../../../server/commands/grant_job_authority/Options.o ../../../server/commands/revoke_job_authority/Options.o Makefile
	@echo "link $@"
	$(SILENT)$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS) -L $(BGQ_BUILD_DIR)/hlcs/src/security -lsecurity -Wl,-rpath,$(BGQ_BUILD_DIR)/hlcs/src/security


test_kill_job: kill_job.o ../../../server/commands/kill_job/Options.o Makefile
	@echo "link $@"
	$(SILENT)$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)


test_locate_rank: locate_rank.o ../../../server/commands/locate_rank/Options.o Makefile
	@echo "link $@"
	$(SILENT)$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)


test_sample: sample.o Makefile
	@echo "link $@"
	$(SILENT)$(SILENT)$(CXX) -o $@ $(filter-out Makefile, $^) $(LDFLAGS)


install: all 


clean:
	$(RM) test_* out_*
	$(RM) *.o
	$(RM) *.d


distclean: clean

.DELETE_ON_ERROR:

-include *.d
