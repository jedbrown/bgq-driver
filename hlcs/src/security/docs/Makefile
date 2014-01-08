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

doxygen_version := $(subst 1.7,1.6,$(findstring 1.6, $(shell doxygen --version))$(findstring 1.7, $(shell doxygen --version)))
required_doxygen_version := 1.6
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)

all: docs

install: all

clean:
	$(RM) *.o
	$(RM) *.d
	$(RM) $(FILTER)
	$(RM) doxygen.out

distclean: clean

uninstall:
	rm -rf $(BGQ_INSTALL_DIR)/hlcs/docs/security

FILTER = filter
$(FILTER): $(FILTER).o
	$(CXX) -o $@ $^ -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib $(LDFLAGS)

docs: doxygen.out

doxygen.out: Doxyfile | $(FILTER)
ifeq ($(doxygen_version),$(required_doxygen_version))
	@echo "generating documentation"
	@mkdir -p $(BGQ_INSTALL_DIR)/hlcs/docs
	-cd ../../../../ && cat hlcs/src/security/docs/$^ | sed 's:BGQ_INSTALL_DIR:$(BGQ_INSTALL_DIR):g' | doxygen - 2>&1 | grep -v bgqdb > hlcs/src/security/docs/doxygen.out
else
	@echo "Will not generate docs because doxygen version is" $(shell doxygen --version) "and required version is" $(required_doxygen_version)
	@touch $@
endif

