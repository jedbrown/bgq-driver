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


lib_base_name := capena-http

lib_major_vers := 0
lib_minor_vers := 1
lib_fix_vers := 0

lib_devel_name := lib$(lib_base_name).so
lib_major_name := $(lib_devel_name).$(lib_major_vers)
lib_full_name := $(lib_major_name).$(lib_minor_vers).$(lib_fix_vers)

generated_files := $(lib_full_name) $(lib_major_name) $(lib_devel_name)

install_files = $(call INSTALL_LIB_TARGET, $(generated_files))

parts := \
 http/http \
 http/MediaType \
 http/uri/Path \
 http/uri/Query \
 http/uri/Uri \
 http/uri/utility \
 server/AbstractResponder \
 server/ErrorResponder \
 server/Connection \
 server/exception \
 server/Request \
 server/ResponderStreambuf \
 server/Response \
 server/Server


objs := ${parts:%=%.o}


CXXFLAGS += \
 -isystem $(BGQ_INSTALL_DIR) \
 -isystem $(BGQ_INSTALL_DIR)/extlib/include \
 -fPIC


LDFLAGS += \
 -shared \
 -Wl,--enable-new-dtags \
 -Wl,-soname,$(lib_major_name) \
 -L$(BGQ_INSTALL_DIR)/extlib/lib \
  -lboost_regex-mt -lboost_system-mt -lboost_thread-mt \
  -llog4cxx \
  -Wl,-rpath,'$$ORIGIN/../../extlib/lib' \
 -L$(BGQ_INSTALL_DIR)/utility/lib \
  -lbgutility \
  -Wl,-rpath,'$$ORIGIN/../../utility/lib'


default: install

all: $(generated_files) doc.all


doc.all:
	${MAKE} -C doc all

doc.install:
	${MAKE} -C doc install


$(lib_full_name): $(objs)
	${cxxlink}

$(lib_major_name):
	$(RM) $@
	ln -s $(lib_full_name) $@

$(lib_devel_name):
	$(RM) $@
	ln -s $(lib_major_name) $@


testsrc: $(generated_files)
	$(MAKE) -C testsrc


test: testsrc
	$(MAKE) -C testsrc $@

test.run: $(generated_files)
	$(MAKE) -C testsrc test


install: $(install_files) doc.install test.run


clean distclean:
	$(RM) *.d http/*.o http/*.d server/*.o server/*.d http/uri/*.o http/uri/*.d $(generated_files)
	${MAKE} -C testsrc $@
	${MAKE} -C doc $@


uninstall:
	$(RM) $(install_files)
	${MAKE} -C doc $@


.PHONY: default all test install clean distclean uninstall testsrc


-include http/*.d server/*.d
