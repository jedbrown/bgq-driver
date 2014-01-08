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
/* (C) Copyright IBM Corp.  2007, 2012                              */
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


#include "Kernel.h"
#include <fcntl.h>
#include <spi/include/kernel/location.h>

__BEGIN_DECLS

uint64_t sc_JOBCOORDS(SYSCALL_FCN_ARGS)
{
    BG_JobCoords_t *jobcoords = (BG_JobCoords_t *)r3;
        
    if(! VMM_IsAppAddress(jobcoords, sizeof(BG_JobCoords_t)))
    {
        return CNK_RC_SPI(EFAULT);
    }
    AppState_t *app = GetMyAppState();
    jobcoords->corner.a     = app->corner.aCoord;
    jobcoords->corner.b     = app->corner.bCoord;
    jobcoords->corner.c     = app->corner.cCoord;
    jobcoords->corner.d     = app->corner.dCoord;
    jobcoords->corner.e     = app->corner.eCoord;
    jobcoords->corner.core  = app->corner.core;
    jobcoords->shape.a      = app->shape.aCoord;
    jobcoords->shape.b      = app->shape.bCoord;
    jobcoords->shape.c      = app->shape.cCoord;
    jobcoords->shape.d      = app->shape.dCoord;
    jobcoords->shape.e      = app->shape.eCoord;
    jobcoords->shape.core   = app->shape.core;
    jobcoords->isSubBlock   = app->subBlockActive;
    return CNK_RC_SPI(0);
}

__END_DECLS
