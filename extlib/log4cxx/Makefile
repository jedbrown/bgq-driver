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
# (C) Copyright IBM Corp.  2010, 2012                              
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


include ../../Make.rules
include ../Make.extlib.rules


#
# a little magic incantation 
LOG4XX_LDFLAGS=
ifeq (${BGQ_HOST_OS},x86_64)
LOG4XX_LDFLAGS='LDFLAGS=-L/lib64 -L/usr/lib64'
endif


.PHONY: default all install clean distclean


default: all

all: install


$(BGQ_INSTALL_DIR)/extlib/lib/liblog4cxx.so.10.0.0: apache-log4cxx-0.10.0.tar.gz patch.0.patch
	tar -xzf apache-log4cxx-0.10.0.tar.gz
	patch -p0 < patch.0.patch
	cd apache-log4cxx-0.10.0 ; CXXFLAGS=-m64 ./configure --prefix=$(BGQ_INSTALL_DIR)/extlib ${LOG4XX_LDFLAGS} --disable-dependency-tracking --disable-doxygen
	cd apache-log4cxx-0.10.0 ; $(MAKE)
	cd apache-log4cxx-0.10.0 ; $(MAKE) install

install: $(BGQ_INSTALL_DIR)/extlib/lib/liblog4cxx.so.10.0.0


clean:


distclean:
	rm -r apache-log4cxx-0.10.0


uninstall:
	$(RM) -r $(BGQ_INSTALL_DIR)/extlib/lib/liblog4cxx* $(BGQ_INSTALL_DIR)/extlib/lib/pkgconfig $(BGQ_INSTALL_DIR)/extlib/include/log4cxx

