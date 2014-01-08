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

//! \addtogroup upci_spi
//@{
/**
 * \file
 *
 * \brief A2 PC Event Apply/Get operations
 *
 */
//@}


#include "spi/include/upci/upc_a2_pc.h"


#define Apply_RMW(REG,UPREG) \
    if (pA2pc->REG##Mask) { \
        uint32_t spr = mfspr(SPRN_##UPREG); \
        spr &= ~(pA2pc->REG##Mask);         \
        spr |= pA2pc->REG;                  \
        mtspr(SPRN_##UPREG, spr);           \
    }


void Upci_A2PC_ApplyRegs(const Upci_A2PC_t *pA2pc)
{
    Apply_RMW(cesr,CESR);
    Apply_RMW(aesr,AESR);
    Apply_RMW(iesr1,IESR1);
    Apply_RMW(iesr2,IESR2);
    Apply_RMW(xesr1,XESR1);
    Apply_RMW(xesr2,XESR2);
    Apply_RMW(xesr3,XESR3);
    Apply_RMW(xesr4,XESR4);
    Apply_RMW(mesr1,MESR1);
    Apply_RMW(mesr2,MESR2);
}




#define Get_SPR(REG,UPREG) pA2pcVal->REG = mfspr(SPRN_##UPREG)
void Upci_A2PC_GetRegs(Upci_A2PC_Val_t *pA2pcVal)
{
    Get_SPR(cesr,CESR);
    Get_SPR(aesr,AESR);
    Get_SPR(iesr1,IESR1);
    Get_SPR(iesr2,IESR2);
    Get_SPR(xesr1,XESR1);
    Get_SPR(xesr2,XESR2);
    Get_SPR(xesr3,XESR3);
    Get_SPR(xesr4,XESR4);
    Get_SPR(mesr1,MESR1);
    Get_SPR(mesr2,MESR2);
}
