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
# (C) Copyright IBM Corp.  2008, 2012                              
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

CC=/bgsys/drivers/ppcfloor/gnu-linux/bin/powerpc64-bgq-linux-gcc
LD=/bgsys/drivers/ppcfloor/gnu-linux/bin/powerpc64-bgq-linux-gcc
FLOOR=/bgsys/drivers/ppcfloor/
SPI_DIR=/bgsys/drivers/ppcfloor/spi/
SPI_CNK_LIB=SPI_cnk
SPI_LIB=SPI
CFLAGS=-O3 -g -I $(FLOOR) -I $(SPI_DIR)/include/kernel/cnk
TARGET_A=msg_alltoall
TARGET_P=msg_pingpong
TARGET_C=msg_collective

all : $(TARGET_A) $(TARGET_P) $(TARGET_C)

$(TARGET_A): $(TARGET_A).o msg_common.o
	@echo "Linking Executable $@ ..."
	$(LD) -o $(TARGET_A).cnk $(TARGET_A).o msg_common.o -lpthread -static -L $(SPI_DIR)/lib -l $(SPI_LIB) -l $(SPI_CNK_LIB)


$(TARGET_A).o: $(TARGET_A).c 
	$(CC) $(CFLAGS) -c $<

$(TARGET_P): $(TARGET_P).o msg_common.o
	@echo "Linking Executable $@ ..."
	$(LD) -o $(TARGET_P).cnk $(TARGET_P).o msg_common.o -lpthread -static -L $(SPI_DIR)/lib -l $(SPI_LIB) -l $(SPI_CNK_LIB)


$(TARGET_P).o: $(TARGET_P).c 
	$(CC) $(CFLAGS) -c $<

$(TARGET_C): $(TARGET_C).o msg_common.o
	@echo "Linking Executable $@ ..."
	$(LD) -o $(TARGET_C).cnk $(TARGET_C).o msg_common.o -lpthread -static -L $(SPI_DIR)/lib -l $(SPI_LIB) -l $(SPI_CNK_LIB)

$(TARGET_C).o: $(TARGET_C).c 
	$(CC) $(CFLAGS) -c $<


msg_common.o: msg_common.c
	$(CC) $(CFLAGS) -c $<	

clean:
	rm -f *.o *.cnk *~
