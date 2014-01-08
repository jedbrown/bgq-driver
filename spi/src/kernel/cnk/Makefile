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
include ../../../../Make.rules

CFLAGS  = -O2 -g -Wall

# path for BGQ installation
CFLAGS += -I$(BGQ_INSTALL_DIR)/gnu/runtime -I$(BGQ_INSTALL_DIR)

# path for CNK implementation of SPIs
CFLAGS += -I$(BGQ_INSTALL_DIR)/spi/include/kernel/cnk

# Tells compiler to create dependency file
CFLAGS += -MMD -MF .dep.$<.d

# for additional command line args, ie "make ARGS=-Dxxx=yyy"
CFLAGS += $(ARGS)

CXXFLAGS = $(CFLAGS) -fno-exceptions

CXX     = $(BGQ_CROSS_CXX)
CC      = $(BGQ_CROSS_CC)
FC      = $(BGQ_CROSS_FC)
AR      = $(BGQ_CROSS_AR) crv

NAME=

TARGET_STATIC_LIB = libSPI_cnk.a
LIB_OBJS = memory_impl.o
TARGET_DIR = $(BGQ_INSTALL_DIR)/spi/lib

install:	all

all:	$(TARGET_DIR)/$(TARGET_STATIC_LIB)
#	@echo "usage: make NAME=prog [ compile | dis | syms | read | run | clean ]"
#	@echo "   or: make check"

$(TARGET_DIR)/$(TARGET_STATIC_LIB):	$(TARGET_STATIC_LIB)
			$(MKDIR) -p $(TARGET_DIR)
			$(CP) $(TARGET_STATIC_LIB) $(TARGET_DIR)


$(TARGET_STATIC_LIB):	$(LIB_OBJS)
			$(AR) $(TARGET_STATIC_LIB) $(LIB_OBJS)

$(NAME):

fft:	fft.o
	$(CC) -o fft fft.o -lm

math:	math.o
	$(CC) -o math math.o -lm

omp:	omp.o
	$(CC) -o omp omp.o -lxlsmp_64

omp.o:	omp.c
	ssh descartes "(cd `pwd` && /opt/ibmcmp/vac/bg/9.0/bin/cc -q64 -qcpluscmt -qnostdinc -I$(BGQ_INSTALL_DIR)/crosstools/powerpc64-quattro-linux/include -I$(BGQ_INSTALL_DIR)/crosstools/lib/gcc/powerpc64-quattro-linux/4.2.0/include -qsmp=omp -qthreaded -g -c omp.c)"

compile:$(NAME)

dis:	$(NAME)
	$(BGQ_CROSS_OBJDUMP) -d -S $(NAME)

syms:	$(NAME)
	$(BGQ_CROSS_OBJDUMP) -t $(NAME)

ssyms:	$(NAME)
	$(BGQ_CROSS_OBJDUMP) -t $(NAME) | sort | fgrep 0000

size:	$(NAME)
	$(BGQ_CROSS_SIZE) --format=SysV  $(NAME)

elf:	$(NAME)
	readelf -a $(NAME)

clean:
	$(RM) $(NAME) $(NAMES)
	$(RM) $(TARGET_STATIC_LIB) 
	$(RM) *.o *.listing *.nm *.map *.elf *.objdump *.readelf .dep.*.d *~

distclean:clean
	$(RM) $(TARGET_DIR)/$(TARGET_STATIC_LIB)

# run instruction simulator
run-isim:	$(NAME)
	@echo
	@echo "=== running $(NAME) === "
	@$(BGQ_INSTALL_DIR)/gnu/bin/ISIM $(NAME) | /bin/fgrep 'tid['



# run instruction simulator (MSIM)
run:    $(NAME)
	@echo
	@echo "=== running $(NAME) === "
	@$(BGQ_INSTALL_DIR)/gnu/bin/MSIM $(NAME) | /bin/fgrep 'tid['
	#@$(BGQ_INSTALL_DIR)/gnu/bin/MSIM $(NAME)


# run instruction simulator (MSIM)
run-debug:    $(NAME)
	@echo
	@echo "=== running $(NAME) === "
	#@$(BGQ_INSTALL_DIR)/gnu/bin/MSIM $(NAME) | /bin/fgrep 'tid['
	@$(BGQ_INSTALL_DIR)/gnu/bin/MSIM $(NAME)


# run instruction simulator (cycle mode)
run-cycle:	$(NAME)
	@echo
	@echo "=== running in cycle mode $(NAME) === "
	@$(BGQ_INSTALL_DIR)/gnu/bin/MSIM $(NAME) cycle | /bin/fgrep 'tid['
	#@$(BGQ_INSTALL_DIR)/gnu/bin/MSIM cycle $(NAME)


# run instruction simulator (fast/warmup mode)
run-fast:	$(NAME)
run-warm:	$(NAME)
	@echo
	@echo "=== running in warmup (fast) mode $(NAME) === "
	@$(BGQ_INSTALL_DIR)/gnu/bin/MSIM $(NAME) warm | /bin/fgrep 'tid['
	#@$(BGQ_INSTALL_DIR)/gnu/bin/MSIM $(NAME) warm


# run instruction simulator (cycle mode)
# number of cores = 4
run-cycle-cores:	$(NAME)
	@echo
	@echo "=== running in cycle mode $(NAME) === "
	@$(BGQ_INSTALL_DIR)/gnu/bin/MSIM $(NAME) c4 cycle | /bin/fgrep 'tid['
	#@$(BGQ_INSTALL_DIR)/gnu/bin/MSIM $(NAME) c4 cycle

# execute some simple checks of the C runtime.
# these are arranged in order of increasing complexity (each test
# depends on primitives tested by its predecessor in the list).
  NAMES =
  NAMES += memory_impl.o

# compile MSIM for all testcases in NAMES list
compile-tests:
	for N in $(NAMES); do $(MAKE) NAME=$$N compile; done

# run MSIM for all testcases in NAMES list
run-tests:
	for N in $(NAMES); do $(MAKE) NAME=$$N run; done

# run MSIM for all testcaes in MAMES list, compare results 
run-check:
	@$(MAKE) -s run-tests 1>check.actual 2>/dev/null && diff -q check.actual check.expected >/dev/null && echo "all tests passed" || echo "one or more tests failed"

# Includes the dependency file from a previous make
ifeq ($(filter distclean clean,$(MAKECMDGOALS)),)
-include .dep.*.d
endif