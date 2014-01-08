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

include ../../../Make.bgws.rules
include ../../../../Make.rules


CXXFLAGS +=\
 -iquote .. \
 -DBOOST_TEST_DYN_LINK -isystem $(BGQ_INSTALL_DIR)/extlib/include


LDFLAGS += \
 -L$(BGQ_INSTALL_DIR)/extlib/lib -lboost_unit_test_framework-mt -Wl,-rpath,$(BGQ_INSTALL_DIR)/extlib/lib \
 -lboost_date_time-mt \
 -Wl,-rpath,..


exes := \
 json_test \
 Parser_test \
 Formatter_test


default: all


all: $(exes)


%_test: %_test.o ../libchiron-json.so.0.1.0
	${cxxlink}

.SECONDARY: $(exes:%=%.o)

test: $(exes:%=%.run)


%.run: %
	@echo "Running" $<
	@./$< &> $@.tmp ; RES=$$? ; if [ $$RES != 0 ]; then echo "$< Failed:" ; cat $@.tmp; rm $@.tmp; exit 1; else mv $@.tmp $@; fi


install:

clean:
	$(RM) *.o *.d $(exes) *.run *.tmp


distclean: clean


.PHONY: default all test install clean distclean


-include *.d
