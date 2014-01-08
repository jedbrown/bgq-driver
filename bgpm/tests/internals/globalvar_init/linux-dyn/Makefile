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
VPATH = ..
TEST_OBJS := test_main.o
TESTNAME := $(notdir $(shell cd ..;pwd))

CFLAGS := -I. -I.. $(CFLAGS) -fopenmp -D_LINUX_
CXXFLAGS := $(subst -static,,$(CXXFLAGS)) -D_LINUX_ -fopenmp
LIBS += gomp pthread 
LIBS := $(subst bgpm,,$(LIBS))
LIBS := $(subst SPI_upci_cnk,,$(LIBS))
LIBS := $(subst SPI_cnk,,$(LIBS))
LIBS := $(subst SPI,,$(LIBS))
LIBS += Vars

LDFLAGS := $(subst static,dynamic,$(LDFLAGS))
LIB_DIRS += .
LIBSO = libVars.so

export LD_LIBRARY_PATH:=.:$(LD_LIBRARY_PATH)
#export OMP_NUM_THREADS=24
#export OMP_SCHEDULE=DYNAMIC
#export OMP_NESTED=TRUE
#export OMP_DYNAMIC=FALSE

PROG_ARGS = 16    # num threads per proc
JOB_PPN = 4       # num processes


.PHONY: run clean

$(TESTNAME).elf : $(LIBSO) test_main.o
	g++ -o $(TESTNAME).elf test_main.o -dynamic -L. -lrt -lstdc++ -lpthread -lc -lgomp -lVars


$(LIBSO) : vars.o vars.cc
	g++ -shared -Wl,-soname,libVars.so -o $(LIBSO) vars.o -lc -lstdc++  

clean  : 
	-rm *.o *.so *.elf

run : 
	./$(TESTNAME).elf $(PROG_ARGS)
	

