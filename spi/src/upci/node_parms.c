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

#include <assert.h>
#include <stdio.h>
#include "spi/include/kernel/upci.h"
#include "spi/include/upci/node_parms.h"


static UPC_Lock_t        upciNodeParmsLock = 0;  // only one thread of process should init node parms
static UpciBool_t            upciNodeParmsInited = UpciFalse;
static Upci_Node_Parms_t upciNodeParms = UPCI_NODE_PARMS_INIT;


const Upci_Node_Parms_t *Upci_GetNodeParms()
{
    if (! upciNodeParmsInited) {
        UPC_Lock(&upciNodeParmsLock);
        if (! upciNodeParmsInited) {
            int rc = Kernel_Upci_GetNodeParms(&upciNodeParms);
            if (rc != 0) {
                UPC_FATAL_ERR("Kernel_Upci_GetNodeParms() failure rc = %d\n", rc);
            }
            #if 0
            printf("NodeParms:\n"
                            "   cpuSpeed=%ld\n"
                            "   nwSpeed=%ld\n"
                            "   cycleRatio=%lf\n"
                            "   spareCore=%d\n"
                            "   dd1enabled=%d\n"
                            "   nodeConfig=0x%016lx\n\n",
                            upciNodeParms.cpuSpeed, upciNodeParms.nwSpeed,
                            upciNodeParms.cycleRatio, upciNodeParms.spareCore,
                            upciNodeParms.dd1Enabled, upciNodeParms.nodeConfig);
            #endif
            upciNodeParmsInited = UpciTrue;
        }
        UPC_Unlock(&upciNodeParmsLock);
    }
    return &upciNodeParms;
}

