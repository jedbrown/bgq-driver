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
 -iquote ../../ \
 -isystem $(BGQ_INSTALL_DIR) \
 -DBOOST_TEST_DYN_LINK -isystem $(BGQ_INSTALL_DIR)/extlib/include


LDFLAGS += \
 -L$(BGQ_INSTALL_DIR)/extlib/lib -lboost_unit_test_framework-mt \
 -Wl,-rpath,../../capena-http:../../chiron-json:$(BGQ_INSTALL_DIR)/extlib/lib:.. \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib

ld_library_path := ../../capena-http:../../chiron-json:$(BGQ_INSTALL_DIR)/extlib/lib:$(BGQ_INSTALL_DIR)/utility/lib


exes= \
 testContentRange \
 testRange \
 testRefreshBgwsServerConfiguration


%.run: %
	@echo "Running" $<
	@BG_PROPERTIES_FILE=$(BGQ_INSTALL_DIR)/utility/etc/bg.properties.tpl LD_LIBRARY_PATH=$(ld_library_path) ./$< &> $@.tmp ; RES=$$? ; if [ $$RES != 0 ]; then echo $< Failed ; cat $@.tmp; rm $@.tmp; exit 1; else mv $@.tmp $@; fi


default: all

all: $(exes)


testContentRange: testContentRange.o ../libbgws_common.so
	${cxxlink}

testRange: testRange.o ../libbgws_common.so
	${cxxlink}

testRefreshBgwsServerConfiguration: testRefreshBgwsServerConfiguration.o ../libbgws_common.so
	${cxxlink}


test: ${exes:%=%.run}


install:

clean:
	$(RM) *.o *.d $(exes) *.run


distclean:


.PHONY: default all test install clean distclean

-include *.d
