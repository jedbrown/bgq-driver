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
# (C) Copyright IBM Corp.  2011, 2011                              
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

include ../../../Make.rules

OS_ARCH := $(shell uname -m)

ifeq  ($(OS_ARCH),x86_64)
        XML_LIB := libXML_s.a
else
ifeq ($(OS_ARCH),ppc64)
        XML_LIB += libXML_s.a
        CXXFLAG_64 := -m64
else
        XML_LIB += libXML.a
endif
endif

picture_test: test.cc Picture.h Picture.c
	g++ $(CXXFLAG_64) -c -o Picture.o -I$(BGQ_INSTALL_DIR) Picture.c
	g++ $(CXXFLAG_64) -o $@ -I$(BGQ_INSTALL_DIR) $< -lexpat ../library/$(XML_LIB) Picture.o

Picture.h: Picture.xml
	../xidl/xidlMain.py --cheader Picture.xml > $(BGQ_INSTALL_DIR)/xml/include/c_api/Picture.h

Picture.c: Picture.xml
	../xidl/xidlMain.py --cimpl Picture.xml > Picture.c

test: picture_test
	./picture_test MyPicture.xml


#install: picture_test

install:

clean:
	$(RM) -f Picture.c
	$(RM) -f picture_test
	$(RM) -f Picture.o

distclean: clean

