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


CXXFLAGS += \
 -iquote .. \
 -isystem $(BGQ_INSTALL_DIR) \
 -isystem $(BGQ_INSTALL_DIR)/extlib/include


LDFLAGS += \
 -rdynamic \
 -Wl,--enable-new-dtags \
 -lssl \
 -L$(BGQ_INSTALL_DIR)/extlib/lib \
  -llog4cxx \
  -Wl,-rpath,'$$ORIGIN/../../extlib/lib' \
 -L$(BGQ_INSTALL_DIR)/utility/lib \
  -lbgutility \
  -Wl,-rpath,'$$ORIGIN/../../utility/lib' \
 -L$(BGQ_INSTALL_DIR)/db/lib -lbgdb \
  -Wl,-rpath,'$$ORIGIN/../../db/lib' \
 -L$(BGQ_INSTALL_DIR)/hlcs/lib -lsecurity \
  -Wl,-rpath,'$$ORIGIN/../../hlcs/lib' \
 -L$(BGQ_INSTALL_DIR)/bgq_util/lib \
  -Wl,-rpath,'$$ORIGIN/../../bgq_util/lib' \
 -L$(BGQ_INSTALL_DIR)/ras/lib \
  -Wl,-rpath,'$$ORIGIN/../../ras/lib' \
 -Wl,-rpath,'$$ORIGIN/../lib'


install_files = $(call INSTALL_SBIN_TARGET, bgws_server)


subdirs := \
  . \
  blue_gene \
  blue_gene/diagnostics \
  blue_gene/service_actions \
  query \
  query/diagnostics \
  query/env \
  responder \
  responder/diagnostics \
  responder/environmentals \
  responder/summary \
  responder/hardware \
  teal \
  utility


blue_gene_parts := \
 diagnostics/Run \
 diagnostics/Runs \
 service_actions/CloseServiceAction \
 service_actions/EndServiceAction \
 service_actions/errors \
 service_actions/PrepareServiceAction \
 service_actions/ServiceActions


query_parts := \
 Alerts \
 AlertsOptions \
 Machine \
 Ras \
 RasIntervalOption \
 RasOptions \
 SystemSummary \
 diagnostics/Locations \
 diagnostics/LocationsOptions \
 diagnostics/Testcases \
 diagnostics/TestcasesOptions


query_env_parts := \
 AbstractOptions \
 BulkPowerOptions \
 CoolantOptions \
 FanOptions \
 IoCardOptions \
 LinkChipOptions \
 NodeOptions \
 NodeCardOptions \
 OpticalOptions \
 Query \
 ServiceCardOptions \
 ServiceCardTempOptions


responder_parts := \
 Alert \
 Alerts \
 BgwsServer \
 Block \
 Blocks \
 HardwareReplacements \
 Job \
 Jobs \
 Logging \
 PerformanceMonitoring \
 Ras \
 RasDetails \
 ServiceAction \
 ServiceActions \
 Session \
 Sessions \
 hardware/Machine \
 hardware/Hardware

responder_diagnostics_parts := \
 Block \
 Blocks \
 common \
 Locations \
 Run \
 Runs \
 RunsQueryOptions \
 Testcases

responder_environmentals_parts := \
 BulkPower \
 Coolant \
 Fan \
 IoCard \
 LinkChip \
 Node \
 NodeCard \
 Optical \
 ServiceCard \
 ServiceCardTemp

responder_summary_parts := \
 Jobs \
 Machine \
 Ras \
 System \
 Utilization

teal_parts := \
 errors \
 Teal \
 TlchalertExecution \
 TlrmalertExecution

utility_parts := \
 base64 \
 ChildProcess \
 ChildProcesses \
 EasyChildProcess \
 EzTimer \
 Pipe \
 SortDirection \
 utility

parts := \
 bgws_server \
 blue_gene \
 AbstractResponder \
 BgwsServer \
 BlueGene \
 BlockingOperationsThreadPool \
 CheckUserAdminExecution \
 CheckUserAdminExecutor \
 dbConnectionPool \
 DiagnosticsRunIdOption \
 DynamicConfiguration \
 Error \
 JobIdOption \
 MultiWildcardOption \
 PwauthExecution \
 PwauthExecutor \
 ras \
 RequestRange \
 ResponderFactory \
 ServerStats \
 Session \
 Sessions \
 SortInfo \
 SortSpec \
 StringDbColumnOption \
 TimeIntervalOption \
 UserInfo \
 WhereClause \
 WildcardOption \
 sqlStrings.gen \
 ${teal_parts:%=teal/%} \
 ${blue_gene_parts:%=blue_gene/%} \
 ${query_parts:%=query/%} \
 ${query_env_parts:%=query/env/%} \
 ${responder_parts:%=responder/%} \
 ${responder_diagnostics_parts:%=responder/diagnostics/%} \
 ${responder_environmentals_parts:%=responder/environmentals/%} \
 ${responder_summary_parts:%=responder/summary/%} \
 ${utility_parts:%=utility/%}


objs := ${parts:%=%.o}


query_files := \
 queries/jobsChart.txt \
 queries/rasChart.txt \
 queries/summaryHardwareProblems.txt \
 queries/summaryDiagnostics.txt \
 queries/summaryIoStatus.txt \
 queries/summaryMachineUsage.txt \
 queries/summaryServiceActions.txt \
 queries/utilizationChart.txt

parts_using_query_files := \
 responder/summary/Jobs \
 responder/summary/Ras \
 responder/summary/System \
 responder/summary/Utilization

objs_using_query_files = ${parts_using_query_files:%=%.o}

libs := \
 ../capena-http/libcapena-http.so.0.1.0 \
 ../chiron-json/libchiron-json.so.0.1.0 \
 ../common/libbgws_common.so.0.1.0

bg_sbin_links := bgws_server


default: all

all: bgws_server run.sh


generate_sql_strings := $(BGQ_INSTALL_DIR)/db/bin/generateSqlStrings.py


sqlStrings.gen.hpp sqlStrings.gen.cc: $(generate_sql_strings) $(query_files)
	$(generate_sql_strings) \
	  --generate=$(suffix $@) \
	  --header sqlStrings.gen.hpp \
	  --header-guard BGWS_SQL_H \
	  --impl sqlStrings.gen.cc \
	  --namespace bgws::sql \
	  $(query_files)

sqlStrings.gen.o: sqlStrings.gen.hpp # explicitly add the header.


$(objs_using_query_files): | sqlStrings.gen.hpp


bgws_server: $(objs) $(libs)
	${cxxlink}


run.sh:
	echo "#!/bin/sh" > $@
	echo "LD_LIBRARY_PATH=\$$LD_LIBRARY_PATH:../capena-http:../chiron-json:../common:$(BGQ_INSTALL_DIR)/extlib/lib:$(BGQ_INSTALL_DIR)/utility/lib:$(BGQ_INSTALL_DIR)/db/lib:$(BGQ_INSTALL_DIR)/hlcs/lib:$(BGQ_INSTALL_DIR)/bgq_util/lib:$(BGQ_INSTALL_DIR)/ras/lib ./bgws_server \$$@" >> run.sh
	chmod +x run.sh

testsrc: $(objs)
	${MAKE} -C $@


test: $(objs)
	${MAKE} -C testsrc $@


install: $(install_files) $(bg_links_targets) run.sh test


clean:
	$(RM) bgws_server run.sh *.gen.* $(subdirs:%=%/*.d) $(subdirs:%=%/*.o)
	$(MAKE) -C testsrc $@

distclean: clean

uninstall:
	$(RM) $(install_files)


-include *.d
-include blue_gene/*.d
-include blue_gene/diagnostics/*.d
-include blue_gene/service_actions/*.d
-include query/*.d
-include query/diagnostics/*.d
-include query/env/*.d
-include responder/*.d
-include responder/diagnostics/*.d
-include responder/environmentals/*.d
-include responder/summary/*.d
-include responder/hardware/*.d
-include teal/*.d
-include utility/*.d


.PHONY: default all test install clean distclean uninstall testsrc
