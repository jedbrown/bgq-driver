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

include ../Make.rules
include ../../../Make.hlcs.rules
include ../../../../Make.rules

.PHONY: clean tags $(TARGETS)


common_libbase       = runjob_common
common_libmajor_vers = 1
common_libminor_vers = 0
common_libminor_rev  = 0
common_libshort      = lib$(common_libbase).so
common_libvers       = $(common_libshort).$(common_libmajor_vers)
common_libfull       = $(common_libvers).$(common_libminor_vers).$(common_libminor_rev)

command_libbase       = runjob_command
command_libmajor_vers = 1
command_libminor_vers = 0
command_libminor_rev  = 0
command_libshort      = lib$(command_libbase).so
command_libvers       = $(command_libshort).$(command_libmajor_vers)
command_libfull       = $(command_libvers).$(command_libminor_vers).$(command_libminor_rev)

CXXFLAGS += -fpic
CXXFLAGS += -fvisibility=default

# linker flags for both libraries
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS  += -lbgutility
LDFLAGS  += -lboost_program_options-mt
LDFLAGS  += -shared
LDFLAGS  += -lboost_filesystem-mt

# linker flags for common library
COMMON_LDFLAGS  += $(LDFLAGS)
COMMON_LDFLAGS  += -L $(BGQ_INSTALL_DIR)/bgq_util/lib
COMMON_LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/bgq_util/lib
COMMON_LDFLAGS  += -lbgq_util
COMMON_LDFLAGS  += -lboost_date_time-mt
COMMON_LDFLAGS  += -Wl,-soname,$(common_libvers)

# linker flags for command library
COMMAND_LDFLAGS += $(LDFLAGS)
COMMAND_LDFLAGS += -L. -l$(common_libbase)
COMMAND_LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/hlcs/lib
COMMAND_LDFLAGS += -Wl,-soname,$(command_libvers)


COMMON_SRC  = $(wildcard *.cc) $(wildcard tool/*.cc)
COMMAND_SRC = $(wildcard commands/*.cc)


INSTALL_LIST           = $(call INSTALL_LIB_TARGET,$(common_libshort) $(common_libvers) $(common_libfull))
INSTALL_LIST          += $(call INSTALL_LIB_TARGET,$(command_libshort) $(command_libvers) $(command_libfull))


all: $(common_libshort) $(command_libshort)


$(common_libfull): $(COMMON_SRC:%.cc=%.o) Makefile
	@echo "link $@"
	$(SILENT)$(CXX) $(filter-out Makefile, $^) -o $@ $(COMMON_LDFLAGS) 


$(common_libvers): $(common_libfull)
	ln -sf $< $@


$(common_libshort): $(common_libvers)
	ln -sf $< $@


$(command_libfull): $(COMMAND_SRC:%.cc=%.o) Makefile | $(common_libshort)
	@echo "link $@"
	$(SILENT)$(CXX) $(filter-out Makefile, $^) -o $@ $(COMMAND_LDFLAGS) 


$(command_libvers): $(command_libfull)
	ln -sf $< $@


$(command_libshort): $(command_libvers)
	ln -sf $< $@


install: $(INSTALL_LIST)


clean: 
	$(RM) *.o *.d
	$(RM) commands/*.o commands/*.d
	$(RM) tool/*.o tool/*.d
	$(RM) $(common_libshort) $(common_libvers) $(common_libfull)
	$(RM) $(command_libshort) $(command_libvers) $(command_libfull)


uninstall:
	$(RM) $(INSTALL_LIST)


distclean: clean


-include *.d
-include commands/*.d
