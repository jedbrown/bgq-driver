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
# (C) Copyright IBM Corp.  2004, 2011                              
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

include ../../Make.db.rules
include ../../../Make.rules

cxxdb_parts :=\
 AbstractStatement \
 BasicConnection \
 Column \
 Columns \
 Connection \
 ConnectionHandle \
 ConnectionPool \
 diagnostics \
 Environment \
 EnvironmentHandle \
 exceptions \
 Parameter \
 Parameters \
 PooledConnection \
 QueryStatement \
 ResultSet \
 sql_util \
 Statement \
 StatementHandle \
 Transaction \
 UpdateStatement

tableapi_parts :=\
 DBConnection \
 DBConnectionPool \
 dbdataconv \
 DBObj \
 TxObject \
 utilcli

job_parts :=\
 exception \
 InsertInfo \
 Operations \
 RemoveInfo \
 Shape \
 SubBlockRas \
 types \
 sqlStrings.gen
 
filtering_parts :=\
 BlockFilter \
 BlockSort \
 Duration \
 getBlocks \
 getJobs \
 JobFilter \
 JobSort \
 TimeInterval

SRC  =  BGQDBlib.cc
SRC +=  BlockDatabaseInfo.cc
SRC +=  dataconv.cc
SRC +=  Exception.cc
SRC +=  extract_db.cc
SRC +=  GenBlockParams.cc
SRC +=  genblock.cc
SRC +=  init.cc
SRC +=  ioUsage.cc
SRC +=  isBlockFree.cc
SRC +=  postProcessRas.cc
SRC +=  setBlockAction.cc
SRC +=  setBlockStatus.cc
SRC +=  swfailure.cc
SRC +=  utility.cc
SRC +=  killMidplaneJobs.cc
SRC +=  putRAS.cc
SRC +=  queryMissing.cc
SRC +=  queryError.cc
SRC +=  copyBlock.cc
SRC +=  ${job_parts:%=job/%.cc}
SRC +=  ${tableapi_parts:%=tableapi/%.cc}
SRC +=  $(wildcard tableapi/gensrc/*.cc)
SRC +=  ${cxxdb_parts:%=cxxdb/%.cc}
SRC +=  ${filtering_parts:%=filtering/%.cc}

lib_base       = bgdb
lib_major_vers = 1
lib_minor_vers = 0
lib_minor_rev  = 0
lib_short      = lib$(lib_base).so
lib_vers       = $(lib_short).$(lib_major_vers)
lib_full       = $(lib_vers).$(lib_minor_vers).$(lib_minor_rev)

CXXFLAGS += -Wall
CXXFLAGS += -W
# the following two warnings are disabled to kludge around the gensrc code
CXXFLAGS += -Wno-unused
CXXFLAGS += -Wno-format
CXXFLAGS += -MD -MP
CXXFLAGS += -pthread
CXXFLAGS += -fpic
CXXFLAGS += -iquote ../../../db/include/api
CXXFLAGS += -iquote .
CXXFLAGS += -isystem $(BGQ_INSTALL_DIR)

LDFLAGS += -Wl,-soname,$(lib_vers)
LDFLAGS += -lodbc
LDFLAGS += -L$(BGQ_INSTALL_DIR)/bgq_util/lib -lbgq_util -Wl,-rpath,$(BGQ_INSTALL_DIR)/bgq_util/lib
LDFLAGS += -L$(BGQ_INSTALL_DIR)/control/lib -lbgqconfig -lmcserver -Wl,-rpath,$(BGQ_INSTALL_DIR)/control/lib
LDFLAGS += -L$(BGQ_INSTALL_DIR)/ras/lib -lras -Wl,-rpath,$(BGQ_INSTALL_DIR)/ras/lib
LDFLAGS += -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS += -L$(BGQ_INSTALL_DIR)/extlib/lib -lboost_date_time-mt -Wl,-rpath,$(BGQ_INSTALL_DIR)/extlib/lib
LDFLAGS += -shared

INSTALL_LIST = $(call INSTALL_LIB_TARGET,$(lib_short) $(lib_vers) $(lib_full))

job_sql_files := \
  job/sql/insertQueryBlockStatus.txt \
  job/sql/insertQuerySubNodeUser.txt \
  job/sql/insertJob.txt \
  job/sql/removeInsertHistory.txt

default: all

all: install

job/sqlStrings.gen.h job/sqlStrings.gen.cc: $(generate_sql_strings) $(job_sql_files)
	$(generate_sql_strings) \
	  --generate=$(suffix $@) \
	  --header job/sqlStrings.gen.h \
	  --header-guard BGQDB_JOB_SQL_H \
	  --impl job/sqlStrings.gen.cc \
	  --include sqlStrings.gen.h \
	  --namespace BGQDB::job::sql \
	  $(job_sql_files)

job/sqlStrings.gen.o: job/sqlStrings.gen.h # explicitly ref the header.

$(SRC:%.cc=%.o): | job/sqlStrings.gen.h
    # I think this will make sure creates .h first... if not use job/Operations.o & other users of sqlStrings.gen.hpp rather than .o-s

$(lib_full): $(SRC:%.cc=%.o)
	$(CXX) -shared $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(lib_vers): $(lib_full)
	ln -sf $< $@

$(lib_short): $(lib_vers)
	ln -sf $< $@

install: $(INSTALL_LIST)

uninstall:
	$(RM) $(INSTALL_LIST)

clean distclean:
	$(RM) *.o tableapi/*.o tableapi/gensrc/*.o cxxdb/*.o job/*.o filtering/*.o
	$(RM) *.d tableapi/*.d tableapi/gensrc/*.d cxxdb/*.d job/*.d filtering/*.d
	$(RM) $(lib_short) $(lib_vers) $(lib_full) job/sqlStrings.gen.h job/sqlStrings.gen.cc

.PHONY: default all install clean distclean

-include *.d
-include cxxdb/*.d
-include job/*.d
-include filtering/*.d
-include tableapi/*.d
-include tableapi/gensrc/*.d
