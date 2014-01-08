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
# (C) Copyright IBM Corp.  2012, 2012                              
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

include ../../../Make.hlcs.rules
include ../../../../Make.rules


lib_short := bgmasterclient
lib_dev := lib$(lib_short).so
lib_soname := $(lib_dev).1
lib := $(lib_soname).0.0


CXXFLAGS += \
 -isystem $(BGQ_INSTALL_DIR) \
 -iquote .. \
 -Wall \
 -Wall \
 -Wconversion \
 -O2 \
 -MMD \
 -pthread \
 -fPIC

LDFLAGS += \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility -lcxxsockets \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib \
 -L$(BGQ_INSTALL_DIR)/xml/lib -lXML \
 -Wl,-rpath,$(BGQ_INSTALL_DIR)/xml/lib \
 -lboost_date_time-mt \
 -Wl,-soname,$(lib_soname)


parts := \
 BGMasterClientApi \
 ListAgents

objs := ${parts:%=%.o}

common_parts := \
 Ids \
 Protocol

common_objs := ${common_parts:%=../common/%.o}

protocol_parts := \
 BGMasterClientProtocolSpec
 
protocol_objs := ${protocol_parts:%=../protocol/%.o}


clean_cmd := $(RM) *.o *.d $(lib) $(lib_soname) $(lib_dev)


$(BGQ_INSTALL_DIR)/hlcs/lib/%: %
	$(MKDIR) ${dirname $@}
	$(CP) $< $@


default: all

all: $(lib) $(lib_soname) $(lib_dev) $(BGQ_INSTALL_DIR)/hlcs/lib/$(lib) $(BGQ_INSTALL_DIR)/hlcs/lib/$(lib_soname) $(BGQ_INSTALL_DIR)/hlcs/lib/$(lib_dev)

$(lib): $(objs) $(common_objs) $(protocol_objs)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared -o $@ $^

$(lib_soname): $(lib)
	ln -s $(lib) $@

$(lib_dev): $(lib_soname)
	ln -s $(lib_soname) $@



install: all

clean:
	$(clean_cmd)

distclean:
	$(clean_cmd)


.PHONY: default all install clean distclean


-include *.d