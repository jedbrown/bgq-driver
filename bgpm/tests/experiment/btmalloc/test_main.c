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
#include "malloc.h"

typedef  unsigned char      uint8_t;
typedef  unsigned long long uint64_t;

#ifdef INCL_BTMALLOC
#include "BackTraceMalloc.h"
#define  BTMCMD(_cmd_) _cmd_
#else
#define  BTMCMD(_cmd_)
#endif


//! Define gymnastics to create a compile time AT string.
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define _AT_ __FILE__ ":" TOSTRING(__LINE__)


int main(int argc, char *argv[])
{
    BTMCMD(BTMalloc());

    fprintf(stderr,
            "Check BT Malloc Corruption Checks; options:\n"
            "  0=no prob\n"
            "  1=head\n"
            "  2=tail\n"
            "  3=double free\n"
            "  4=unaligned tail\n"
            "  5=size corrupt\n"
            "  6=good realloc\n"
            "Use option value 10, 20, 30, etc to use BTProbe to catch rather than free operation.\n");

    if (argc < 2) {
        fprintf(stderr, "no option passed\n");
        exit(1);
    }
    int opt = atoi(argv[1]);

    int useBTProbe = opt >= 10 ? 1 : 0;
    opt = opt >= 10 ? opt / 10 : opt;

    fprintf(stderr, "opt=%d, useBTProbe=%d\n", opt, useBTProbe);

    switch (opt) {
    case 1: { // head
        size_t size = 64;
        uint8_t *array = (uint8_t*)malloc(size);
        int i;
        for (i=0; i<size/8+1+2; i++ ) {
            fprintf(stderr, "[%03d] 0x%016llx\n", i, ((uint64_t*)array)[i-2]);
        }
        *(array-3) = 0xFF;
        if (useBTProbe) { BTMCMD(BTProbe(array,_AT_)); }
        free(array);
        break;
    }

    case 2: { // tail
        size_t size = 64;
        uint8_t *array = (uint8_t*)malloc(size);
        *(array+size+3) = 0xFF;
        if (useBTProbe) { BTMCMD(BTProbe(array,_AT_)); }
        free(array);
        break;
    }

    case 3: {  // double free
        size_t size = 72;
        uint8_t *array = (uint8_t*)malloc(size);
        free(array);
        if (useBTProbe) { BTMCMD(BTProbe(array,_AT_)); }
        free(array);
        break;
    }

    case 4: { // unaligned tail
        size_t size = 77;
        uint8_t *array = (uint8_t*)malloc(size);
        *(array+size+1) = 0xFF;
        if (useBTProbe) { BTMCMD(BTProbe(array,_AT_)); }
        free(array);
        break;
    }

    case 5: { // size corrupt
        size_t size = 2051;
        uint64_t *array = (uint64_t*)malloc(size);
        *(array-2) = 0xFFFFFF;
        if (useBTProbe) { BTMCMD(BTProbe(array,_AT_)); }
        free(array);
        break;
    }

    case 6: { // size corrupt
        size_t size = 77;
        uint64_t *array = (uint64_t*)malloc(size);
        *(array+5) = 0xFFFFFF;
        int i;
        for (i=0; i<size/8+4; i++ ) {
            fprintf(stderr, "[%03d] 0x%016llx\n", i, ((uint64_t*)array)[i-2]);
        }

        array = (uint64_t*)realloc(array, size+25);
        for (i=0; i<size/8+4; i++ ) {
            fprintf(stderr, "[%03d] 0x%016llx\n", i, ((uint64_t*)array)[i-2]);
        }

        if (*(array+5) != 0xFFFFFF) {
            fprintf(stderr, _AT_ " ERROR: realloc didn't copy data correctly\n");
        }
        if (useBTProbe) { BTMCMD(BTProbe(array,_AT_)); }
        free(array);
        break;
    }

    default: {
        size_t size = 2048;
        uint8_t *array = (uint8_t*)malloc(size);
        //BTMCMD(BTProbe(array,_AT_));
        free(array);
        break;
    }

    }



    return 0;
}

