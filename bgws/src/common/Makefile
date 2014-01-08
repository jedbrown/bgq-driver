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


lib_base_name := bgws_common

lib_major_vers := 0
lib_minor_vers := 1
lib_fix_vers := 0

lib_devel_name := lib$(lib_base_name).so
lib_major_name := $(lib_devel_name).$(lib_major_vers)
lib_full_name := $(lib_major_name).$(lib_minor_vers).$(lib_fix_vers)

generated_files := $(lib_full_name) $(lib_major_name) $(lib_devel_name)

install_files = $(call INSTALL_LIB_TARGET, $(generated_files))

parts := \
 common \
 ContentRange \
 ItemRange \
 RefreshBgwsServerConfiguration

objs := ${parts:%=%.o}


CXXFLAGS += \
 -iquote .. \
 -isystem $(BGQ_INSTALL_DIR) \
 -isystem $(BGQ_INSTALL_DIR)/extlib/include \
 -fPIC


LDFLAGS += \
 -shared \
 -Wl,--enable-new-dtags \
 -Wl,-soname,$(lib_major_name) \
 -L$(BGQ_INSTALL_DIR)/extlib/lib \
  -llog4cxx \
  -Wl,-rpath,'$$ORIGIN/../../extlib/lib' \
 -Wl,-rpath,'$$ORIGIN/../lib'


default: install

all: $(generated_files)


$(lib_full_name): $(objs) ../capena-http/libcapena-http.so.0.1.0 ../chiron-json/libchiron-json.so.0.1.0
	${cxxlink}

$(lib_major_name):
	$(RM) $@
	ln -s $(lib_full_name) $@

$(lib_devel_name):
	$(RM) $@
	ln -s $(lib_major_name) $@


test: $(generated_files)
	$(MAKE) -C testsrc

test.run: $(generated_files)
	$(MAKE) -C testsrc test


install: $(install_files) test.run


clean distclean:
	$(RM) *.o *.d $(generated_files)
	$(MAKE) -C testsrc clean


uninstall:
	$(RM) $(install_files)


.PHONY: default all test test.run install clean distclean uninstall


-include *.d
