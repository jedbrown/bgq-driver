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
 -isystem $(BGQ_INSTALL_DIR) \
 -DBOOST_TEST_DYN_LINK -isystem $(BGQ_INSTALL_DIR)/extlib/include


LDFLAGS += \
 -L$(BGQ_INSTALL_DIR)/db/lib -lbgdb -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib \
 -L$(BGQ_INSTALL_DIR)/extlib/lib -lboost_program_options-mt -lboost_regex-mt -lboost_unit_test_framework-mt -lboost_date_time-mt -Wl,-rpath,$(BGQ_INSTALL_DIR)/extlib/lib


exes= \
 testChildProcess \
 testSortInfo \
 testTimeIntervalOption \
 testWhereClause


%.run: %
	@echo "Running" $<
	@BG_PROPERTIES_FILE=$(BGQ_INSTALL_DIR)/utility/etc/bg.properties.tpl ./$< &> $@.tmp ; RES=$$? ; if [ $$RES != 0 ]; then echo $< Failed ; cat $@.tmp; rm $@.tmp; exit 1; else mv $@.tmp $@; fi


default: all

all: $(exes)


testChildProcess: testChildProcess.o ../utility/ChildProcess.o ../utility/ChildProcesses.o ../utility/EasyChildProcess.o ../utility/Pipe.o
	${cxxlink}

testSortInfo: testSortInfo.o ../SortInfo.o ../SortSpec.o ../utility/SortDirection.o
	${cxxlink}

testTimeIntervalOption: testTimeIntervalOption.o ../TimeIntervalOption.o ../WhereClause.o $(BGQ_INSTALL_DIR)/db/lib/libbgdb.so
	${cxxlink}

testWhereClause: testWhereClause.o ../WhereClause.o
	${cxxlink}


test: ${exes:%=%.run}


install:

clean:
	$(RM) *.o *.d $(exes) *.run


distclean:


.PHONY: default all test install clean distclean

-include *.d
