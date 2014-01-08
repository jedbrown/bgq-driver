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


lib_base_name := chiron-json

lib_major_vers := 0
lib_minor_vers := 1
lib_fix_vers := 0

lib_devel_name := lib$(lib_base_name).so
lib_major_name := $(lib_devel_name).$(lib_major_vers)
lib_full_name := $(lib_major_name).$(lib_minor_vers).$(lib_fix_vers)

generated_files := $(lib_full_name) $(lib_major_name) $(lib_devel_name)

install_files = $(call INSTALL_LIB_TARGET, $(generated_files))

parts := \
 Array \
 Formatter \
 functions \
 Object \
 Parser \
 Value \
 value_types

objs := ${parts:%=%.o}


CXXFLAGS += \
 -fPIC \
 -isystem $(BGQ_INSTALL_DIR)/extlib/include

LDFLAGS += \
 -shared \
 -Wl,--enable-new-dtags \
 -Wl,-soname,$(lib_major_name) \
 -L$(BGQ_INSTALL_DIR)/extlib/lib -lboost_date_time-mt -Wl,-rpath,'$$ORIGIN/../../extlib/lib'


default: install

all: $(generated_files) doc


$(lib_full_name): $(objs)
	${cxxlink}

$(lib_major_name):
	$(RM) $@
	ln -s $(lib_full_name) $@

$(lib_devel_name):
	$(RM) $@
	ln -s $(lib_major_name) $@


testsrc: $(lib_devel_name)
	${MAKE} -C $@


test: $(generated_files)
	${MAKE} -C testsrc $@

doc:
	${MAKE} -C $@


install: $(install_files) test
	$(MAKE) -C doc $@

clean:
	$(RM) *.o *.d $(generated_files)
	${MAKE} -C doc $@
	${MAKE} -C testsrc $@


distclean: clean


uninstall:
	$(RM) $(install_files)
	$(MAKE) -C doc $@


.PHONY: default all testsrc test doc install clean distclean uninstall


-include *.d
