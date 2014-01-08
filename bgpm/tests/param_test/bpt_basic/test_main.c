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


#define NUM_THREADS 68
#define INVALID_PARM -1
int Dummy=-1;


void BgpmModeTest(int mode)
{
   TEST_FUNCTION_PARM(Checking InValid Mode);
   TEST_CHECK_EQUAL(Bgpm_Init(Dummy),BGPM_ENOTIMPL);

   TEST_FUNCTION_PARM(Success case);
   TEST_CHECK_EQUAL(Bgpm_Init(mode),0);

   Bgpm_Disable(); 
}

int main(int argc, char *argv[])
{
    Bgpm_ExitOnError(BGPM_FALSE);  // don't fail process on error
    TEST_FUNCTION(Bgpm_Init() API test);
    printf("\nChecking SWDISTRIB  Mode\n");
    BgpmModeTest(BGPM_MODE_SWDISTRIB);     
    printf("\nChecking HWDISTRIB  Mode\n");
    BgpmModeTest(BGPM_MODE_HWDISTRIB);     
    printf("\nChecking LLDISTRIB  Mode\n");
    BgpmModeTest(BGPM_MODE_LLDISTRIB);     
    printf("\nChecking DEFAULT  Mode\n");
    BgpmModeTest(BGPM_MODE_DEFAULT);     

    ////// Bgpm_Delete_EventSet Test///////
    TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);
    int hPuSet = Bgpm_CreateEventSet();
    TEST_FUNCTION(Bgpm_DeleteEventSet() API test); 
    TEST_FUNCTION_PARM(Invalid Event Handle);
    TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(Dummy),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hPuSet),0);
    TEST_RETURN();
}
     
