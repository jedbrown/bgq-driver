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
 -isystem $(BGQ_INSTALL_DIR)/extlib/include \
 -isystem $(BGQ_INSTALL_DIR)


LDFLAGS += \
 -Wl,--enable-new-dtags \
 -lcurl \
 -Wl,-rpath,'$$ORIGIN/../lib' \
 -L$(BGQ_INSTALL_DIR)/utility/lib -lbgutility \
  -Wl,-rpath,'$$ORIGIN/../../utility/lib' \
 -L$(BGQ_INSTALL_DIR)/db/lib -lbgdb \
  -Wl,-rpath,'$$ORIGIN/../../db/lib' \
 -L$(BGQ_INSTALL_DIR)/extlib/lib -lboost_filesystem-mt -lboost_program_options-mt -lboost_regex-mt \
  -Wl,-rpath,'$$ORIGIN/../../extlib/lib' \
 -llog4cxx


exes := \
 bgws_session \
 listblocks \
 createblock \
 deleteblock


common_parts := \
 BgwsOptions RequestData Response bgws utility
	
common_objs := ${common_parts:%=%.o}

libs := \
 ../capena-http/libcapena-http.so.0.1.0 \
 ../chiron-json/libchiron-json.so.0.1.0


default: all


all: $(exes) run.sh


bgws_session: bgws_session.o $(common_objs) $(libs)
	${cxxlink}

listblocks: listblocks.o $(common_objs) $(libs)
	${cxxlink}

createblock: createblock.o $(common_objs) $(libs)
	${cxxlink}

deleteblock: deleteblock.o $(common_objs) $(libs)
	${cxxlink}

run.sh:
	echo "#!/bin/sh" > $@
	echo "PATH=.:\$$PATH" >> $@
	echo "LD_LIBRARY_PATH=$(BGQ_INSTALL_DIR)/utility/lib:$(BGQ_INSTALL_DIR)/db/lib:$(BGQ_INSTALL_DIR)/extlib/lib:../capena-http:../chiron-json:. \$$@" >> $@
	chmod +x $@


test:


install: $(exes)


clean:
	$(RM) $(exes) *.o *.d run.sh


distclean: clean


uninstall:


.PHONY: default all test install clean distclean uninstall


-include *.d
