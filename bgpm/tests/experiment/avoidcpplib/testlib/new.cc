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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#pragma weak _Znam
#pragma weak _Znwm
#pragma weak _ZdlPv
#pragma weak _ZdaPv

void *operator new(size_t size)
{
    fprintf(stderr, "My operator new\n");
    void *p = malloc(size);
    if (p == NULL) {
        fprintf(stderr, "ERROR: Out of Memory in operator new\n");
        assert(0);
    }
    return(p);
}



void *operator new[](size_t size)
{
    fprintf(stderr, "My operator new[]\n");
    void *p = malloc(size);
    if (p == NULL) {
        fprintf(stderr, "ERROR: Out of Memory in operator new[]\n");
        assert(0);
    }
    return(p);
}



void operator delete(void *p)
{
    fprintf(stderr, "My operator delete\n");
    if (p) free(p);
}



void operator delete[](void *p)
{
    fprintf(stderr, "My operator delete[]\n");
    if (p) free(p);
}


