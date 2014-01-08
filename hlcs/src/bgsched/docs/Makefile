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
include ../../../Make.hlcs.rules
include ../../../../Make.rules

CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/hlcs/include
CXXFLAGS += -MD
CXXFLAGS += -MP
LDFLAGS  += -lboost_date_time-mt
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib

all: docs

install: docs

clean:
	$(RM) *.o *.d
	$(RM) doxygen.out
	$(RM) $(FILTERS) $(RUNJOB_PLUGIN_SAMPLE)

distclean: clean

RUNJOB_PLUGIN_SAMPLE = runjob_plugin_sample
RUNJOB_PLUGIN_FILTER = runjob_plugin_filter

$(RUNJOB_PLUGIN_FILTER): $(RUNJOB_PLUGIN_FILTER).o
	$(CXX) -o $@ $^ $(LDFLAGS)

$(RUNJOB_PLUGIN_SAMPLE): $(RUNJOB_PLUGIN_SAMPLE).o
	$(CXX) -o $@ $^ -L $(BGQ_BUILD_DIR)/hlcs/src/bgsched -lbgsched $(LDFLAGS)

FILTERS += $(RUNJOB_PLUGIN_FILTER)
SAMPLES += $(RUNJOB_PLUGIN_SAMPLE)

docs: doxygen.out

doxygen.out: $(SAMPLES) $(FILTERS) Doxyfile.internal Doxyfile.external
	@$(MKDIR) -p $(BGQ_INSTALL_DIR)/hlcs/docs/bgsched
	cd ../../../ && cat src/bgsched/docs/Doxyfile.external | sed 's:BGQ_INSTALL_DIR:$(BGQ_INSTALL_DIR):' | doxygen - > src/bgsched/docs/doxygen.out
	cd ../../../ && cat src/bgsched/docs/Doxyfile.internal | sed 's:BGQ_INSTALL_DIR:$(BGQ_INSTALL_DIR):' | doxygen - &> src/bgsched/docs/doxygen.out

-include *.d
