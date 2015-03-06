# ------------------------------------------------------------------------- #
# Licensed Materials - Property of IBM                                      #
# Blue Gene/Q 5765-PER 5765-PRP                                             #
# © Copyright IBM Corp. 2012 All Rights Reserved                            #
# US Government Users Restricted Rights - Use, duplication or disclosure    #
#   restricted by GSA ADP Schedule Contract with IBM Corp.                  #
#                                                                           #
# This software is available to you under the Eclipse Public License (EPL). #
# ------------------------------------------------------------------------- #

CC ?= /bgsys/drivers/ppcfloor/comm/gcc/bin/mpicc
CFLAGS=-g
LDLIBS=-lm


OBJECTS=main.o simple_query.o simple_barrier.o simple_async_progress.o simple_memregion_registry.o accumulate_data_functions.o

TESTS=test.cnk noncontig_test.cnk

.PHONEY: all clean
all: $(TESTS)

clean:
	$(RM) *.o *.cnk

%.cnk: %.o $(OBJECTS)
	$(CC) $(LDFLAGS) $< $(OBJECTS) -o $@ $(LDLIBS) 






