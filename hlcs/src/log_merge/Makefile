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
# (C) Copyright IBM Corp.  2010, 2011                              
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


boost_include_dir = $(BGQ_INSTALL_DIR)/extlib/include
boost_lib_dir = $(BGQ_INSTALL_DIR)/extlib/lib


CXXFLAGS ?= -g

CXXFLAGS += -O2 -Wall -MMD -isystem $(BGQ_INSTALL_DIR) -isystem $(boost_include_dir)

UNAME_PROC=$(shell uname -i)
ifeq ($(UNAME_PROC),ppc64)
CXXFLAGS += -m64
LDFLAGS  += -m64
endif

LDFLAGS += \
 -L$(boost_lib_dir) -lboost_date_time-mt -lboost_filesystem-mt -lboost_program_options-mt -lboost_regex-mt -lboost_system-mt \
 -Wl,-rpath,$(boost_lib_dir) \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib \
 -llog4cxx


parts = \
 log_merge \
 Configuration \
 DirectoryInfo \
 File \
 FilenameMapper \
 Interval \
 LiveMonitor \
 LogFile \
 LogFiles \
 LogLineParser \
 LogMerge \
 StreamPool \
 TimestampFormat


objs = ${parts:%=%.o}


exe = log_merge

bg_bin_links := log_merge

install_files = $(addprefix $(BGQ_BIN_INSTALL_DIR)/,$(exe))

generated_files = *.o *.d $(exe)


.PHONY: default all install uninstall clean distclean test testsrc doc


default: all

all: $(exe) doc.all


doc doc.%:
	${MAKE} -C ${basename $@} ${subst .,,${suffix $@}}


$(exe):	$(objs)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


install: $(install_files) doc.install $(bg_links_targets)


uninstall:
	$(RM) $(install_files)
	${MAKE} -C doc $@


test: $(objs)
	$(MAKE) -C testsrc $@


testsrc: $(objs)
	$(MAKE) -C $@


clean distclean:
	$(RM) $(generated_files)
	$(MAKE) -C testsrc $@
	${MAKE} -C doc $@


-include *.d
