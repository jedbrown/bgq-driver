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


#include "stdlib.h"
#include "stdio.h"

#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"
#include "spi/include/l1p/sprefetch.h"


#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();


#define NUM_THREADS 68


int main(int argc, char *argv[])
{
    TEST_MODULE(Create a Big overflow on Mu counters);

    Bgpm_Init(BGPM_MODE_HWDISTRIB);

    Upci_Node_Parms_t nodeParms;
    Kernel_Upci_GetNodeParms(&nodeParms);
    UPC_IO_EnableUPC(BGPM_FALSE, nodeParms.nodeConfig);

    UPC_C_Dump_State();
    UPC_L2_Dump_State();

    UPC_IO_DumpState(0,nodeParms.nodeConfig);

    UPC_C_Start_Sync_Counting();

    TEST_RETURN();
}

