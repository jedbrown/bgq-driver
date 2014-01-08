/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */


#ifndef _CRC_EXCHANGE_H_ // Prevent multiple inclusion.
#define _CRC_EXCHANGE_H_

#include <spi/include/mu/Descriptor.h>
#include <spi/include/mu/GIBarrier.h>
#include <spi/include/kernel/MU.h>

typedef struct CrcExchange_Packet_t {
    uint64_t se_crc;     // sender CRC
    uint64_t re_crc;     // receiver CRC
    uint64_t direction;  // link direction
    uint64_t state;   
} CrcExchange_Packet_t;
    

typedef struct CrcExchange_Data_t {

    // 

    int                         initialized;

    // Injection FIFO data:

    MUSPI_InjFifoSubGroup_t     injection_fifo_subgroup;
    Kernel_InjFifoAttributes_t  injection_fifo_attributes;
    Kernel_MemoryRegion_t       injection_fifo_memory_region;

    ALIGN_L1_CACHE char         injection_fifo[16*BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES];

    // Reception FIFO data

    Kernel_MemoryRegion_t       reception_fifo_memory_region;
    Kernel_RecFifoAttributes_t  reception_fifo_attributes;
    MUSPI_RecFifoSubGroup_t     reception_fifo_subgroup;
    
    ALIGN_L1_CACHE char         reception_fifo[16*64];
    uint64_t                    packets_received;

    ALIGN_L1_CACHE CrcExchange_Packet_t  crc_data[10];

    MUHWI_Destination_t         my_location;

    MUHWI_Descriptor_t          mem_fifo_descriptor  __attribute__((__aligned__(64))) ;
} CrcExchange_Data_t;

int crcx_initialize( void );
int crcx_exchange_crcs( MUSPI_GIBarrier_t* barrier );

#endif
