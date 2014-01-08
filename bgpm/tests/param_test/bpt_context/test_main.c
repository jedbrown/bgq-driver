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


#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();

int Dummy=-1;
void SetContxt_test(int);


int main(int argc, char *argv[])
{
     Bgpm_ExitOnError(BGPM_FALSE); 

     TEST_FUNCTION(SetContext()-BGPM_CTX_KERNEL PARAMETER TEST);
     TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);
     SetContxt_test(BGPM_CTX_KERNEL);
     Bgpm_Disable();

     TEST_FUNCTION(SetContext()-BGPM_CTX_USER PARAMETER TEST);
     TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);
     SetContxt_test(BGPM_CTX_USER);
     Bgpm_Disable();

     TEST_FUNCTION(SetContext()-BGPM_CTX_DEFAULT PARAMETER TEST);
     TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);
     SetContxt_test(BGPM_CTX_DEFAULT);
     Bgpm_Disable();

     TEST_FUNCTION(SetContext()-BGPM_CTX_BOTH PARAMETER TEST);
     TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);
     SetContxt_test(BGPM_CTX_BOTH);
     Bgpm_Disable();

     TEST_FUNCTION(AllowMixedContext()  PARAMETER TEST);
     TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);
     TEST_CHECK_EQUAL(Bgpm_AllowMixedContext(Dummy),BGPM_EINV_SET_HANDLE);
   
     //Bgpml1pModes *pMode=NULL;
     //TEST_FUNCTION(GetL1pMode()  PARAMETER TEST);
     //TEST_FUNCTION_PARM(Invalid Event Handle);
     //TEST_CHECK_EQUAL(Bgpm_GetL1pMode(Dummy,pMode),BGPM_EINV_SET_HANDLE); //API Not available
     Bgpm_Disable(); 

     TEST_RETURN();
}

void SetContxt_test(ctxt)
{
     int hPuSet;
     unsigned measList[] = {
     PEVT_IU_IS1_STALL_CYC,
     PEVT_CYCLES,
     PEVT_INST_ALL
     };

     hPuSet=Bgpm_CreateEventSet();
     int numMeasEvts = sizeof(measList) / sizeof(int);
     Bgpm_Context  pContext;
     TEST_FUNCTION_PARM(Invalid Event Handle);
     TEST_CHECK_EQUAL(Bgpm_SetContext(Dummy,ctxt),BGPM_EINV_SET_HANDLE);
     TEST_FUNCTION_PARM(Invalid Context);
     TEST_CHECK_EQUAL(Bgpm_SetContext(hPuSet,-1),BGPM_EINV_PARM);
     ////////Adding Events to Event handle////////////////////////
     TEST_CHECK_EQUAL(Bgpm_AddEventList(hPuSet,measList,numMeasEvts),0);
     TEST_FUNCTION_PARM(Trying to set after adding events to EventSet) ;

     if(ctxt==BGPM_CTX_DEFAULT || ctxt == BGPM_CTX_BOTH){ // CTXT_BOTH And CTX_DEFAULT are allowed to set here. 
     TEST_CHECK_EQUAL(Bgpm_SetContext(hPuSet,ctxt),0);
     } 
     else{
     TEST_CHECK_EQUAL(Bgpm_SetContext(hPuSet,ctxt),BGPM_ECTX_MISMATCH);
     }

     TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hPuSet),0)
     hPuSet=Bgpm_CreateEventSet();
     //////////////////////////////////////////////////
     TEST_FUNCTION_PARM(Success Case) ;
     printf("Setting Context to : %d",ctxt);
     TEST_CHECK_EQUAL(Bgpm_SetContext(hPuSet,ctxt),0);

     TEST_FUNCTION(GetContext() PARAMETER TEST);
     TEST_FUNCTION_PARM(Invalid Event handle);
     TEST_CHECK_EQUAL(Bgpm_GetContext(Dummy,&pContext),BGPM_EINV_SET_HANDLE);
     TEST_FUNCTION_PARM(Success Case);
     TEST_CHECK_EQUAL(Bgpm_GetContext(hPuSet,&pContext),0);
     TEST_CHECK_EQUAL(pContext,ctxt);
}
