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

include ../../Make.bgws.rules
include ../../../Make.rules


CXXFLAGS +=\
 -Wall \
 -DBOOST_TEST_DYN_LINK -isystem $(BGQ_INSTALL_DIR)/extlib/include \
 -iquote ..


LDFLAGS += \
 -lcurl \
 -L$(BGQ_INSTALL_DIR)/extlib/lib -lboost_unit_test_framework-mt -lboost_regex-mt -Wl,-rpath,$(BGQ_INSTALL_DIR)/extlib/lib


exes := \
 test_create_block \
 test_delete_block \
 test_get_blocks


default: all


all: $(exes)


install: all


test_create_block: test_create_block.o utility.o ../capena-http/http/http.o
	${cxxlink}


test_delete_block: test_delete_block.o utility.o ../capena-http/http/http.o
	${cxxlink}


test_get_blocks: test_get_blocks.o utility.o ../capena-http/http/http.o
	${cxxlink}


test: $(exes)
	./test_create_block
	./test_delete_block
	./test_get_blocks


clean:
	$(RM) *.o *.d $(exes)


distclean: clean

uninstall:


.PHONY: default all install clean distclean test uninstall


-include *.d
