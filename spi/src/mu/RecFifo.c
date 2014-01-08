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
/* (C) Copyright IBM Corp.  2008, 2012                              */
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


/**
 * \file RecFifo.c
 *
 * \brief C File containing Reception Fifo Function Implementations
 *        and Reception Fifo Registration Table
 */


#include <stdio.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <spi/include/mu/RecFifo.h>
#include <spi/include/mu/InjFifo.h>
#include <string.h>


/**
 * \brief Declaration of the Reception Fifo Poll Registration Table.
 */
MUSPI_RecvInfoTable_t   _muspi_RecvInfo;


/**
 * \brief Next Free Registration Id in the Reception Fifo Poll Registration Table.
 */
uint32_t                _muspi_RecvInfoNextFreeId = 0;


/**
 * \brief Handle a Reception Fifo Poll Wrap Condition
 *
 * This function is called when the head and tail pointers of a reception
 * fifo indicate that the packets wrap.  That is, tail < head.
 * This function processes all of the packets at the end of the fifo,
 * including the one that wraps.  It updates both the software head pointer
 * and the hardware head pointer, and returns the number of packets
 * processed.  It does not process any packets that may still be in the
 * fifo after the wrapped packet.  The function may prematurely end
 * the processing of packets if the specified npackets has been reached.
 *
 * \param[in]  rec_fifo  Pointer to reception fifo structure.
 * \param[in]  npackets  Maximum number of packets to process.
 *
 * \returns  Number of packets processed.
 */

uint32_t MUSPI_RecFifoPollWrap (MUSPI_RecFifo_t   *rec_fifo,
				uint32_t           npackets) 
{
  uint32_t num_processed = 0;   /* Number of packets processed */
  
  MUSPI_RecvFunction_t      recv_func_ptr; /* Pointer to receive function*/
  void                    * recv_func_parm;/* Receive function parameter */
  int                       recv_func_id;  /* Function ID from the packet */
                                           /* header */
  
  MUHWI_PacketHeader_t * packet_ptr;       /* Pointer to packet header    */
  uint32_t               packet_bytes;     /* Number of bytes in the packet*/
  void                 * va_head;          /* Snapshot of the fifo's head */
  void                 * va_end;           /* Fifo's end pointer */
  int32_t                has_wrapped = 0;
  uint32_t               start_bytes, end_bytes;


  MUSPI_Fifo_t  *fifo_ptr = &rec_fifo->_fifo;
  va_head = fifo_ptr->va_head;  
  va_end  = fifo_ptr->va_end;  

//  fprintf (stderr, "Wrap Poll\n");

  /* Call msync */
  _bgq_msync();
  
  /* We know the fifo does not wrap */
  while ((!has_wrapped) && (num_processed < npackets)) {
    packet_ptr     = (MUHWI_PacketHeader_t *) va_head;
    //Byte8 is identical between pt2pt and collective packets
    packet_bytes   = (packet_ptr->NetworkHeader.pt2pt.Byte8.Size + 1) << 5;
    
    /* Standardize messaging header bytes */
    recv_func_id   = packet_ptr->messageUnitHeader.Packet_Types.Memory_FIFO.Unused2[0];
    recv_func_ptr  = _muspi_RecvInfo.table[recv_func_id].recvFunction;
    recv_func_parm = _muspi_RecvInfo.table[recv_func_id].recvParam;
    MUSPI_assert ( recv_func_ptr != NULL );
    
    num_processed++;
    
    if (va_head + packet_bytes < va_end) {
      va_head = (void*) ( (uint64_t) va_head + packet_bytes );     
      
      (*recv_func_ptr)( recv_func_parm, 
			packet_ptr, 
			packet_bytes );    
    }
    else {
      end_bytes = va_end - va_head;  
      start_bytes = packet_bytes - end_bytes;
      
      memcpy (rec_fifo->_wrapBuf, va_head, end_bytes);
      memcpy (rec_fifo->_wrapBuf + end_bytes, fifo_ptr->va_start, start_bytes);
      
      (*recv_func_ptr)( recv_func_parm, 
			(MUHWI_PacketHeader_t *) rec_fifo->_wrapBuf,
			packet_bytes );    
      
      va_head = fifo_ptr->va_start + start_bytes;
      has_wrapped = 1;
    }
  } 

  /* Update the fifo head pointer */
  fifo_ptr->va_head = va_head;  
  MUSPI_setHwHead (fifo_ptr, va_head - MUSPI_getStartVa(fifo_ptr));

  return num_processed;
}
