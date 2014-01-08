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
#include "spi/include/upci/testlib/upc_p_debug.h"


#define PRINTERR(...) fprintf(stderr, "ERROR(" _AT_ ") " __VA_ARGS__);
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }
#define BGPMDIE(_funcname_) { DIE(_funcname_ ": %d; %s\n", Bgpm_LastErr(), Bgpm_LastErrStrg()); }



#define ADD_EVENT(hEvtSet, evtId)  \
{                                    \
    int rc = Bgpm_AddEvent(hEvtSet, evtId); \
    if (rc < 0) BGPMDIE("Bgpm_AddEvent"); \
}



void PrintCounts(unsigned hEvtSet)
{
    // event has total cycles and total instructions
    int i;
    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cycles = 0;
    uint64_t instrs = 0;

    for (i=0; i<numEvts; i++) {
        int evtId = Bgpm_GetEventId(hEvtSet, i);
        if (evtId == PEVT_CYCLES) {
            if (Bgpm_ReadEvent(hEvtSet, i, &cycles)) BGPMDIE("Bgpm_ReadEvent");
        }
        else if (evtId == PEVT_INST_ALL) {
            if (Bgpm_ReadEvent(hEvtSet, i, &instrs)) BGPMDIE("Bgpm_ReadEvent");
        }
    }

    printf("Counter Results:\n");
    uint64_t cnt;
    for (i=0; i<numEvts; i++) {
        if (Bgpm_ReadEvent(hEvtSet, i, &cnt)) BGPMDIE("Bgpm_ReadEvent");
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        if (!label) BGPMDIE("Bgpm_GetEventLabel");
        if (cycles && instrs) {
            printf("    %-40s = %-10ld,  %%Instr=%4.1f%%, %%Cycles=%4.1f%%\n",
                    label, cnt, ((double)cnt)/instrs*100.0, ((double)cnt)/cycles*100.0);
        }
        else if (instrs) {
            printf("    %-40s = %-10ld,  %%Instr=%4.1f%%\n",
                    label, cnt, ((double)cnt)/instrs*100.0);
        }
        else if (cycles) {
            printf("    %-40s = %-10ld,  %%Cycles=%4.1f%%\n",
                    label, cnt, ((double)cnt)/cycles);
        }
        else {
            printf("    %-40s = %-10ld\n", label, cnt);
        }
    }
}






int numEvtSets = 0;
void InitEvtSets()
{
    int rc = Bgpm_Init(BGPM_MODE_SWDISTRIB);
    if (rc < 0) BGPMDIE("Bgpm_Init");

    int hEvtSet = Bgpm_CreateEventSet();
    if (hEvtSet < 0) BGPMDIE("Bgpm_CreateEventSet");
    ADD_EVENT(hEvtSet, PEVT_AXU_FP_EXCEPT);
    ADD_EVENT(hEvtSet, PEVT_AXU_FP_ENAB_EXCEPT);
    ADD_EVENT(hEvtSet, PEVT_AXU_INSTR_COMMIT);
    ADD_EVENT(hEvtSet, PEVT_AXU_CR_COMMIT);
    ADD_EVENT(hEvtSet, PEVT_INST_QFPU_ALL);
    ADD_EVENT(hEvtSet, PEVT_INST_ALL);
    ADD_EVENT(hEvtSet, PEVT_CYCLES);
    numEvtSets++;

}



const long arraySize = 1024L*1024L*1024L * 2L / sizeof(uint64_t);

#define NUM_SAMPLES  4000
#define VEC_SIZE 4

#define RESETFSB(bit) asm volatile( "mtfsb0 %0;" : : "i" (bit-32))

void FPExceptionsVector()
{
    double veca[VEC_SIZE];
    double vecb[VEC_SIZE];
    double vecc[VEC_SIZE];
    double vecd[VEC_SIZE];
    double vece[VEC_SIZE];

    int i;
    for (i=0; i<VEC_SIZE; i++) {
        veca[i] = 0.0;
        vecb[i] = 1.0;
        vecc[i] = 2.0;
        vecd[i] = 3.0;
        vece[i] = 4.0;
    }

    int loops = 0;
    unsigned setNum = 0;

    if (Bgpm_Apply(setNum)) BGPMDIE("Bgpm_Apply");
    //Bgpm_DumpEvtSet(setNum, 0);
    //UPC_P_Dump_State( 0 );
    if (Bgpm_ResetStart(setNum) < 0) BGPMDIE("Bgpm_ResetStart");

    for (loops=0; loops<NUM_SAMPLES; loops++) {
        for (i=0; i<VEC_SIZE; i++) {
            vecb[i] = vecb[i] + vecc[i];
            vecd[i] = vecc[i] * vecd[i];
            vece[i] = vecc[i] / veca[i];
        }
    }

    Bgpm_Stop(setNum);
    i = 0;
    printf ("i=%d, a=%lf, b=%lf, c=%lf, d=%lf, e=%lf\n", i, veca[i], vecb[i], vecc[i], vecd[i], vece[i]);
    i = 1;
    printf ("i=%d, a=%lf, b=%lf, c=%lf, d=%lf, e=%lf\n", i, veca[i], vecb[i], vecc[i], vecd[i], vece[i]);
    i = 2;
    printf ("i=%d, a=%lf, b=%lf, c=%lf, d=%lf, e=%lf\n", i, veca[i], vecb[i], vecc[i], vecd[i], vece[i]);
    i = 3;
    printf ("i=%d, a=%lf, b=%lf, c=%lf, d=%lf, e=%lf\n", i, veca[i], vecb[i], vecc[i], vecd[i], vece[i]);
    PrintCounts(setNum);
}



void FPExceptionsScalar()
{
    volatile double a = 0.0;
    volatile double b = 1.0;
    volatile double c = 2.0;
    volatile double d = 3.0;
    volatile double e = 4.0;

    int loops = 0;
    unsigned setNum = 0;

    if (Bgpm_Apply(setNum)) BGPMDIE("Bgpm_Apply");
    Bgpm_DumpEvtSet(setNum, 0);
    UPC_P_Dump_State( 0 );

    printf("reset fpscr exception bits 35 - 51, 33,32\n");
    RESETFSB(35);
    RESETFSB(36);
    RESETFSB(37);
    RESETFSB(38);
    RESETFSB(39);
    RESETFSB(40);
    RESETFSB(41);
    RESETFSB(42);
    RESETFSB(43);
    RESETFSB(44);
    RESETFSB(45);
    RESETFSB(46);
    RESETFSB(47);
    RESETFSB(48);
    RESETFSB(49);
    RESETFSB(50);
    RESETFSB(51);
    RESETFSB(33);
    RESETFSB(32);

    union {
        double fp;
        uint64_t ui;
    } fpscr;
    double register ftmp = 0;
    asm volatile(
             "mffs %1;"
             "stfd %1,%0"
             : "=m" (fpscr.fp) : "b" (ftmp) : "memory");
    printf("before fpscr=0x%016lx\n", fpscr.ui);

    if (Bgpm_ResetStart(setNum) < 0) BGPMDIE("Bgpm_ResetStart");

    for (loops=0; loops<NUM_SAMPLES; loops++) {
            b = b + c;
            d = c * d;
            e = c / a;
    }
    Bgpm_Stop(setNum);

    asm volatile(
             "mffs %1;"
             "stfd %1,%0"
             : "=m" (fpscr.fp) : "b" (ftmp) : "memory");
    printf("after  fpscr=0x%016lx\n", fpscr.ui);

    printf ("a=%lf, b=%lf, c=%lf, d=%lf, e=%lf\n", a, b, c, d, e);
    PrintCounts(setNum);
}






int main(int argc, char *argv[])
{
    printf("Cycle thru stall counters\n");

    InitEvtSets();
    FPExceptionsScalar();

    return 0;
}

