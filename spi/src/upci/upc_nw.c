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

#include "stdio.h"

#define _UPC_NW_C_
#include "spi/include/upci/upc_nw.h"
#include "spi/include/upci/upc_a2.h"

//! \addtogroup upci_spi
//@{
/**
 * \file
 *
 * \brief UPCI Network Unit Operations
 */
//@}



uint64_t UPC_NW_CtrCtlMask(uint16_t linkMask, uint16_t ctrMask)
{
    int link;
    uint64_t ctrl = 0;

    //printf("linkMask=0x%04x  ctrMask=0x%04x\n", linkMask, ctrMask);

    ctrMask &= UPC_NW_ALL_CTRS;   // enable bits really only go thru bit 4.
    uint64_t fullCtrMask = ((uint64_t)ctrMask) << 48;  // left align ctr bits.

    //printf("fullCtrMask=0x%016lx\n", fullCtrMask);

    linkMask &= UPC_NW_ALL_LINKS;  // make sure don't have link value too large.
    while ( (link = upc_cntlz16(linkMask)) < 11 ) {
        ctrl |= fullCtrMask >> (link * 5);
        //printf(_AT_ " linkMask=0x%04x, link=%02d, ctrMask=0x%04x, ctrl=0x%016lx\n",
        //        linkMask, link, ctrMask, ctrl);

        linkMask &= ~UPC_NW_LINK_bit(link);
    }

    //printf("ctrl=0x%016lx\n", ctrl);

    return ctrl;
}



uint64_t UPC_NW_LinkCtrTotal(uint16_t linkMask, unsigned ctr)
{
    assert(ctr < ND_RESE_DCR__UPC_range);
    linkMask &= UPC_NW_ALL_LINKS;  // make sure don't have link value too large.

    int link;
    uint64_t total = 0;

    while ( (link = upc_cntlz16(linkMask)) < 16 ) {
        uint64_t curVal = UPC_NW_ReadCtr(link, ctr);
        //printf(_AT_ " read link=%02d, ctr=%d, val=0x%016lx\n", link, ctr, curVal);
        total += curVal;
        linkMask &= ~UPC_NW_LINK_bit(link);
    }

    return total;
}



void UPC_NW_SetLinkCtrVChannel(uint16_t linkMask, unsigned ctr, uint8_t vcMask)
{
    assert(ctr < ND_RESE_DCR__UPC_range);
    linkMask &= UPC_NW_ALL_LINKS;  // make sure don't have link value too large.

    int link;

    //printf(_AT_ " linkMask=0x%04x\n", linkMask);

    while ( (link = upc_cntlz16(linkMask)) < 11 ) {

        //printf(_AT_ " link=%02d, ctr=%d, vcMask=%02x\n", link, ctr, vcMask);

        UPC_NW_SetCtrVChannel(link, ctr, vcMask);
        linkMask &= ~UPC_NW_LINK_bit(link);
    }
}



