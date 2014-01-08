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


lib_base_name := bgws_client

lib_major_vers := 0
lib_minor_vers := 1
lib_fix_vers := 0

lib_devel_name := lib$(lib_base_name).so
lib_major_name := $(lib_devel_name).$(lib_major_vers)
lib_full_name := $(lib_major_name).$(lib_minor_vers).$(lib_fix_vers)


CXXFLAGS += \
 -iquote .. \
 -isystem $(BGQ_INSTALL_DIR)/extlib/include \
 -isystem $(BGQ_INSTALL_DIR) \
 -fPIC


LDFLAGS += \
 -Wl,--enable-new-dtags \
 -lcurl \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/$(tld)/lib \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility \
  -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib \
 -L$(BGQ_INSTALL_DIR)/db/lib -lbgdb \
  -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib \
 -L$(BGQ_INSTALL_DIR)/extlib/lib -lboost_program_options-mt -lboost_regex-mt -llog4cxx \
  -Wl,-rpath,$(BGQ_INSTALL_DIR)/extlib/lib


admin_exes := \
 bgws_server_log_level \
 bgws_server_refresh_config \
 bgws_server_status \
 create_block \
 delete_block

user_exes := \
 list_blocks \
 list_jobs


exes := $(admin_exes) $(user_exes) list_ras

bgws_client_parts := \
 BgwsClient \
 BgwsOptions \
 CurlHandle \
 JobsSummaryFilter \
 RequestData \
 Response \
 utility


bgws_client_objs := ${bgws_client_parts:%=%.o}


lib_files := $(lib_full_name) $(lib_major_name) $(lib_devel_name)

generated_files := $(lib_files) $(exes)


libs := \
 ../capena-http/libcapena-http.so.0.1.0 \
 ../chiron-json/libchiron-json.so.0.1.0 \
 ../common/libbgws_common.so.0.1.0 \
 


install_files = $(call INSTALL_SBIN_TARGET, $(admin_exes)) $(call INSTALL_BIN_TARGET, $(user_exes)) $(call INSTALL_LIB_TARGET, $(lib_files))

bg_sbin_links := $(admin_exes)
bg_bin_links := $(user_exes)


default: all


all: $(generated_files)


$(lib_full_name): LDFLAGS += -shared -Wl,-soname,$(lib_major_name)
$(lib_full_name): $(bgws_client_objs) $(libs)
	${cxxlink}

$(lib_major_name):
	$(RM) $@
	ln -s $(lib_full_name) $@

$(lib_devel_name):
	$(RM) $@
	ln -s $(lib_major_name) $@


bgws_server_log_level: bgws_server_log_level.o $(lib_full_name)
	${cxxlink}

bgws_server_refresh_config: bgws_server_refresh_config.o $(lib_full_name)
	${cxxlink}

bgws_server_status: bgws_server_status.o $(lib_full_name)
	${cxxlink}

list_blocks: list_blocks.o $(lib_full_name)
	${cxxlink}

list_jobs: list_jobs.o $(lib_full_name)
	${cxxlink}

create_block: create_block.o $(lib_full_name)
	${cxxlink}

delete_block: delete_block.o $(lib_full_name)
	${cxxlink}

list_ras: list_ras.o $(lib_full_name)
	${cxxlink}


test:


install: $(install_files) $(bg_links_targets) list_ras


clean:
	$(RM) $(generated_files) *.o *.d


distclean: clean


uninstall:
	$(RM) $(install_files)


.PHONY: default all test install clean distclean uninstall


-include *.d
