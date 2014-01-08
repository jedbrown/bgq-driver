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

include ../../../../Make.rules
include ../../../Make.hlcs.rules
include ../Make.rules

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


.PHONY: install all default clean distclean test $(TARGETS) $(DIRS)

CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)/ramdisk/include
LDFLAGS  += -L ../common -lrunjob_common -Wl,-rpath,$(BGQ_LIB_INSTALL_DIR)
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/db/lib -lbgdb
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib
LDFLAGS  += -lboost_date_time-mt
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib

MISC_FILTER = misc_filter
$(MISC_FILTER): $(MISC_FILTER).o
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $^ $(LDFLAGS)

RUNJOB_FILTER = runjob_filter
$(RUNJOB_FILTER): $(RUNJOB_FILTER).o
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $^ $(LDFLAGS)


RUNJOB_MUX_FILTER = runjob_mux_filter
$(RUNJOB_MUX_FILTER): $(RUNJOB_MUX_FILTER).o
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $^ $(LDFLAGS)


RUNJOB_SERVER_FILTER = runjob_server_filter
$(RUNJOB_SERVER_FILTER): $(RUNJOB_SERVER_FILTER).o
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $^ $(LDFLAGS)


RUNJOB_COMMAND_FILTER = runjob_command_filter
$(RUNJOB_COMMAND_FILTER): $(RUNJOB_COMMAND_FILTER).o
	@echo "link $@"
	$(SILENT)$(CXX) -o $@ $^ $(LDFLAGS)


FILTERS += $(MISC_FILTER)
FILTERS += $(RUNJOB_FILTER)
FILTERS += $(RUNJOB_MUX_FILTER)
FILTERS += $(RUNJOB_SERVER_FILTER)
FILTERS += $(RUNJOB_COMMAND_FILTER)


all:            $(FILTERS) $(TARGETS_ALL)
install:        $(FILTERS) $(TARGETS_INSTALL)
clean:          $(TARGETS_CLEAN)
	$(RM) *.o
	$(RM) *.d
	$(RM) $(FILTERS)
distclean:      clean $(TARGETS_DISTCLEAN)
uninstall:      $(TARGETS_UNINSTALL)

# both doc dirs depend on the filter
html.install man.install:  $(FILTERS)
html.all man.all:          $(FILTERS)

$(TARGETS):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))
