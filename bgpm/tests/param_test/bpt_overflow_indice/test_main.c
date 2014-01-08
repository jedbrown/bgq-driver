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

void Overflow_test(int);

 #include <stdlib.h>
 #include <stdio.h>

 double **mm_init(int n)
 {
    double **m;
     int i;
     m = (double**)malloc(n * sizeof(void*));
     for (i = 0; i < n; ++i)
     m[i] = calloc(n, sizeof(double));
     return m;
 }

 void mm_destroy(int n, double **m)
 {
    int i;
     for (i = 0; i < n; ++i) free(m[i]);
     free(m);
 }

 double **mm_gen(int n)
 {
     double **m, tmp = 1. / n / n;
     int i, j;
     m = mm_init(n);
     for (i = 0; i < n; ++i)
     for (j = 0; j < n; ++j)
     m[i][j] = tmp * (i - j) * (i + j);
     return m;
 }
 // better cache performance by transposing the second matrix
 double **mm_mul(int n, double *const *a, double *const *b)
 {
     int i, j, k;
     double **m, **c;
     m = mm_init(n); c = mm_init(n);
     for (i = 0; i < n; ++i) // transpose
     for (j = 0; j < n; ++j)
     c[i][j] = b[j][i];
     for (i = 0; i < n; ++i) {
     double *p = a[i], *q = m[i];
     for (j = 0; j < n; ++j) {
     double t = 0.0, *r = c[j];
     for (k = 0; k < n; ++k)
     t += p[k] * r[k];
     q[j] = t;
     }
    }
     mm_destroy(n, c);
     return m;
 }

 int matmul()
 {
     int n = 100;
     double **a, **b, **m;
     n = 6; 
     a = mm_gen(n); b = mm_gen(n);
     m = mm_mul(n, a, b);
     fprintf(stderr, "%lf\n", m[n/2][n/2]);
     mm_destroy(n, a); mm_destroy(n, b); mm_destroy(n, m);
     return 0;
 }


int main(int argc, char *argv[])
{
    Bgpm_ExitOnError(BGPM_FALSE);  // don't fail process on error

    TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);

    unsigned hOfSet = Bgpm_CreateEventSet();
    unsigned evtList[] = {
    PEVT_CYCLES,   
    PEVT_CYCLES,  //  (OF is Set)
    PEVT_LSU_COMMIT_LD_MISSES, //  (OF is Set)
    PEVT_INST_ALL,
    PEVT_INST_QFPU_FMUL,  // will overflow
    PEVT_INST_QFPU_FMUL,  // 2nd one will not
    PEVT_L1P_BAS_HIT,  // Extra Events
    PEVT_L1P_BAS_MISS, // Extra Events - should cause shared overflow.
    PEVT_IU_BARRIER_OP_STALL_CYC,
    PEVT_IU_FXU_ISSUE_COUNT,
    PEVT_IU_TOT_ISSUE_COUNT,
    PEVT_LSU_COMMIT_STS,
    PEVT_LSU_COMMIT_CACHEABLE_LDS, // OF is set
    PEVT_XU_BR_MISPRED_COMMIT,
    PEVT_INST_XU_FLD,
    PEVT_INST_XU_IADD
    };
    TEST_CHECK_EQUAL(Bgpm_AddEventList(hOfSet,evtList,16),0);
    Bgpm_SetOverflow(hOfSet, 4,10000);
    Bgpm_SetOverflow(hOfSet, 5,10000);
    Bgpm_SetOverflow(hOfSet, 7,10000);
    TEST_CHECK_EQUAL(Bgpm_Apply(hOfSet),0);
   
    matmul();    
 
    Bgpm_Punit_Handles_t  pH;
    TEST_CHECK_EQUAL(Bgpm_Punit_GetHandles(hOfSet,&pH),0);


    unsigned ovfIdxs[BGPM_MAX_OVERFLOW_EVENTS];
    unsigned len = BGPM_MAX_OVERFLOW_EVENTS;

    TEST_FUNCTION(Bgpm_GetOverflowEventIndices() PARAMETER TEST);

    TEST_CHECK_EQUAL(Bgpm_GetOverflowEventIndices(hOfSet, pH.allCtrMask , ovfIdxs, &len),0);

    TEST_FUNCTION_PARM(len = 0);
    len=0;
    TEST_CHECK_EQUAL(Bgpm_GetOverflowEventIndices(hOfSet, pH.allCtrMask , ovfIdxs,&len),BGPM_EARRAY_TOO_SMALL);

    TEST_FUNCTION_PARM( len <   number of events);
    len= -5;
    TEST_CHECK_EQUAL(Bgpm_GetOverflowEventIndices(hOfSet, pH.allCtrMask , ovfIdxs,&len),0);

    TEST_FUNCTION_PARM( len >   number of events);
    len=25;
    TEST_CHECK_EQUAL(Bgpm_GetOverflowEventIndices(hOfSet, pH.allCtrMask , ovfIdxs,&len),0);


    TEST_FUNCTION(Checking GetOverflowEventIndices by passing unsigned pointer for pLen);
    unsigned *pLen=NULL;

    TEST_FUNCTION_PARM(Testing pLen==NULL);
    pLen=NULL;
    TEST_CHECK_EQUAL(Bgpm_GetOverflowEventIndices(hOfSet, pH.allCtrMask , ovfIdxs, pLen),BGPM_EINV_PARM);


    TEST_FUNCTION_PARM(Testing pLen < number of events);
    pLen=&len;
    *pLen=-5; 
    TEST_CHECK_EQUAL(Bgpm_GetOverflowEventIndices(hOfSet, pH.allCtrMask , ovfIdxs, pLen),0);
    printf("pLen Adjusted : %d\n",*pLen);

    TEST_FUNCTION_PARM(Testing pLen > number of events);
    *pLen=35;
    TEST_CHECK_EQUAL(Bgpm_GetOverflowEventIndices(hOfSet, pH.allCtrMask , ovfIdxs, pLen),0);
    printf("pLen Adjusted : %d\n",*pLen);
  
    
    TEST_RETURN();
}

