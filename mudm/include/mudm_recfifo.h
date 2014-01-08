/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (c) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/*                                                                  */
/* US Government Users Restricted Rights - Use, duplication or      */
/* disclosure restricted by GSA ADP Schedule Contract with IBM      */
/* Corporation.                                                     */
/*                                                                  */
/* This software is available to you under either the GNU General   */
/* Public License (GPL) version 2 or the Eclipse Public License     */
/* (EPL) at your discretion.                                        */
/*                                                                  */

//
// Inline functions for MU Data Mover
//
#ifndef	_MUDM_RECFIFO_H_ /* Prevent multiple inclusion */
#define	_MUDM_RECFIFO_H_

#include <spi/include/mu/Descriptor.h>

/**
 * \brief Reception Fifo Inline Poll Function Example
 *
 * An example that demostrates the use of the Reception fifo
 * inlines to poll packets. This poll routine only returns when all
 * packets have been processed from the reception fifos.
 *
 * \param[in]  rfifo  Reception fifo structure.
 * \param[in]  fn     Function pointer to the function to be called to
 *                    handle each packet.
 * \param[in]  clientdata  Pointer to client data to be passed to the
 *                         receive function fn.
 */
/* Note:  Copied from RecFifo.h and dependent on RecFifo.h */
__INLINE__ void RecFifoPoll (MUSPI_RecFifo_t       * rfifo,
			            MUSPI_RecvFunction_t    fn,
				    void                  * clientdata)
{    
  uint32_t wrap = 0;
  uint32_t cur_bytes = 0;
  uint32_t total_bytes = 0;
  uint32_t cumulative_bytes = 0;
  MUHWI_PacketHeader_t *hdr = 0;

  while ((total_bytes = MUSPI_getAvailableBytes (rfifo, &wrap)) != 0) 
  {
    if (wrap == 0) 
    {
      /* No fifo wrap.  Process each packet. */
      cumulative_bytes = 0;
      while (cumulative_bytes < total_bytes ) 
      {
	hdr = MUSPI_getNextPacketOptimized (rfifo, &cur_bytes);
	(*fn)(clientdata, hdr, cur_bytes); /* Call user-defined packet handler. */
	cumulative_bytes += cur_bytes;
	/* Touch head for next packet. */
      }
    }
    else 
    {
      /* Packets wrap around to the top of the fifo.  Handle the one packet
       * that wraps.
       */
      hdr = MUSPI_getNextPacketWrap (rfifo, &cur_bytes);
      (*fn)(clientdata, hdr, cur_bytes); /* Call user-defined packet handler.   */
    }

    /* Store the updated fifo head. */
    MUSPI_syncRecFifoHwHead (rfifo);
  }
}
#endif /* _MUDM_RECFIFO_H_ */
