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

include ../../../Make.bgws.rules
include ../../../../Make.rules


CXXFLAGS +=\
 -DBOOST_TEST_DYN_LINK -isystem $(BGQ_INSTALL_DIR)/extlib/include \
 -isystem $(BGQ_INSTALL_DIR)


LDFLAGS += \
 -Wl,--enable-new-dtags \
 -L$(BGQ_INSTALL_DIR)/extlib/lib -lboost_unit_test_framework-mt \
 -Wl,-rpath-link,$(BGQ_INSTALL_DIR)/extlib/lib:$(BGQ_INSTALL_DIR)/utility/lib


exes := \
 test_FormCodec \
 test_MediaType \
 test_UriEscaping \
 test_UriPath \
 test_UriQueryGen \
 test_UriQueryParse \
 test_Uri \
 test_UriParse


run_targets = ${exes:%=%.run}


test_%: test_%.o ../libcapena-http.so
	${cxxlink}


test_%.run: test_%
	LD_LIBRARY_PATH=$(BGQ_INSTALL_DIR)/extlib/lib:$(BGQ_INSTALL_DIR)/utility/lib:.. ./$< > $@.tmp
	mv $@.tmp $@


clean_cmds := $(RM) *.o *.d $(exes) *.run *.tmp


default: all


all: $(exes) server.all


${exes:%=%.o}:


test: $(run_targets) server.test


server.all:
	${MAKE} -C server all

server.test:
	${MAKE} -C server test


install:


clean:
	$(clean_cmds)
	${MAKE} -C server $@


distclean:
	$(clean_cmds)
	${MAKE} -C server $@


.PHONY: default all test install clean distclean


-include *.d
