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

include ../../../../Make.rules
include ../../../Make.hlcs.rules


CXXFLAGS += \
  -isystem $(DB2PATH)/include

LDFLAGS += \
  -shared \
  -L$(DB2PATH)/lib -ldb2

SHARED_LIB_NAME=libregexpMatchUDF.so

.PHONY: default all install clean distclean


default: $(SHARED_LIB_NAME)

all: default


$(SHARED_LIB_NAME): regexpMatchUDF.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^


install: $(SHARED_LIB_NAME)
	$(MKDIR) -p $(BGQ_INSTALL_DIR)/hlcs/lib
	$(CP) $(SHARED_LIB_NAME) $(BGQ_INSTALL_DIR)/hlcs/lib/


clean:
	$(RM) *.o *.o_64 *.d $(SHARED_LIB_NAME)


distclean: clean


-include *.d
