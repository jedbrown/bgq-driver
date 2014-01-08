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

#define _UPC_MODE_C_
#include <assert.h>
#include <stdio.h>
#include "spi/include/upci/upc_c.h"
#include "spi/include/kernel/upci.h"
#include "spi/include/upci/node_parms.h"



int Upci_Mode_Init(Upci_Mode_t *pMode, unsigned upcMode, UPC_Ctr_Mode_t ctrMode, unsigned unit)
{

    assert(upcMode == 0 || upcMode == 2);  // don't support mode 1 yet

    pMode->upcMode = upcMode;
    pMode->ctrMode = ctrMode;

    pMode->pNodeParms = Upci_GetNodeParms();

    int rc = Kernel_Upci_Mode_Init(upcMode, ctrMode, unit);
    return rc;
}



void Upci_Mode_Free()
{
    Kernel_Upci_Mode_Free();
}




