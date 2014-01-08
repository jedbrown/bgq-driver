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
include ../../../Make.bgsched.rules

.PHONY: clean tags

TOP_DIR=hlcs
TOP_INSTALL_DIR=$(BGQ_INSTALL_DIR)/$(TOP_DIR)

LDFLAGS  += -shared
LDFLAGS  += -Wl,-soname,$(lib_vers)
 
SRCS +=  FirstFitPlugin.cc 

CONFIG_FILE = firstfit_plugin_logging.configuration

OBJS = $(SRCS:%.cc=%.o)

lib_base       = firstfit
lib_major_vers = 1
lib_minor_vers = 0
lib_minor_rev  = 0
lib_short      = lib$(lib_base).so
lib_vers       = $(lib_short).$(lib_major_vers)
lib_full       = $(lib_vers).$(lib_minor_vers).$(lib_minor_rev)

# define installation directories
tld=hlcs
install_base_dir=$(BGQ_INSTALL_DIR)/$(tld)
etc_install_dir=$(install_base_dir)/etc

default: all

all: $(lib_short)

lib_objs=$(OBJS)

$(lib_full): $(lib_objs)
	$(CXX) -shared $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(lib_vers): $(lib_full)
	ln -sf $< $@

$(lib_short): $(lib_vers)
	ln -sf $< $@
	
$(BGQ_LIB_INSTALL_DIR)/%: ./% | $(BGQ_LIB_INSTALL_DIR)
	cp -fP $^ $@

INSTALL_LIST  =  $(call INSTALL_LIB_TARGET, $(lib_short) $(lib_vers) $(lib_full))
INSTALL_LIST +=  $(etc_install_dir)/$(CONFIG_FILE)

$(etc_install_dir):
	$(MKDIR) -p $@

$(etc_install_dir)/%: ./% | $(etc_install_dir)
	$(CP) $< $@

install: $(INSTALL_LIST)

uninstall:
	$(RM) $(INSTALL_LIST)
	$(RM) $(etc_install_dir)/$(CONFIG_FILE)

clean:
	$(RM) $(OBJS)
	$(RM) $(lib_short) $(lib_vers) $(lib_full)
	$(RM) *.d

distclean: clean
	$(RM) $(BGQ_LIB_INSTALL_DIR)/$(lib_full)
	$(RM) $(BGQ_LIB_INSTALL_DIR)/$(lib_vers)
	$(RM) $(BGQ_LIB_INSTALL_DIR)/$(lib_short)

print:
	echo $(INSTALL_LIST)

-include *.d
