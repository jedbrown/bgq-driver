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


include ../../../../Make.rules


vpath %.o ..


boost_include_dir = $(BGQ_INSTALL_DIR)/extlib/include
boost_lib_dir = $(BGQ_INSTALL_DIR)/extlib/lib

CXXFLAGS ?=	-g 

CXXFLAGS += -O2 -Wall -MMD -isystem $(boost_include_dir) -isystem $(BGQ_INSTALL_DIR) -iquote ..


LDFLAGS += \
 -L$(boost_lib_dir) -lboost_date_time-mt -lboost_filesystem-mt -lboost_program_options-mt -lboost_regex-mt -lboost_unit_test_framework-mt \
 -Wl,-rpath,$(boost_lib_dir) \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib


testers = \
 testTimestampFormat \
 testLogLineParser \
 testStreamPool \
 testFile \
 testFilenameMapper \
 testInterval \
 testLogRecord \
 testLogFile \
 testLogFiles \
 testConfiguration \
 testDirectoryInfo \
 testLogMerge


tester_results = ${addsuffix .result,$(testers)}


define run_tester
@echo "Running $<:"
@BG_PROPERTIES_FILE=$(BGQ_INSTALL_DIR)/utility/etc/bg.properties.tpl ./$<
touch $@
endef


%.result: %
	${run_tester}


%: %.o
	$(CXX) $^ $(LOADLIBES) $(LDLIBS) -o $@ $(LDFLAGS)


generated_files = *.o *.d *.result $(testers)


.PHONY: default all clean distclean test


default: all

all: test

test: $(tester_results)


testConfiguration: testConfiguration.o Configuration.o Interval.o common.o

testDirectoryInfo: testDirectoryInfo.o common.o DirectoryInfo.o

testFile: testFile.o File.o StreamPool.o common.o

testFilenameMapper: testFilenameMapper.o FilenameMapper.o common.o

testInterval: testInterval.o common.o

testLogFile: testLogFile.o LogFile.o File.o FilenameMapper.o Interval.o LogLineParser.o TimestampFormat.o StreamPool.o common.o

testLogFiles: testLogFiles.o LogFiles.o File.o FilenameMapper.o Interval.o LogFile.o LogLineParser.o TimestampFormat.o StreamPool.o common.o

testLogLineParser: testLogLineParser.o LogLineParser.o TimestampFormat.o common.o

testLogMerge: testLogMerge.o LogMerge.o Configuration.o DirectoryInfo.o File.o FilenameMapper.o Interval.o LogFiles.o LogFile.o LogLineParser.o TimestampFormat.o StreamPool.o common.o LiveMonitor.o

testLogRecord: testLogRecord.o common.o

testStreamPool: testStreamPool.o StreamPool.o common.o

testTimestampFormat: testTimestampFormat.o TimestampFormat.o common.o


clean distclean:
	$(RM) $(generated_files)


-include *.d
