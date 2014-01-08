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

#ifndef _BGPM_IMPL_TYPES_H_  // Prevent multiple inclusion
#define _BGPM_IMPL_TYPES_H_

#include "bgpm/include/types.h"
#include "CoreReservations.h"



//! \addtogroup bgpm_api_impl
//@{
/**
 * \file
 *
 * \brief BGPM implementation types & enums
 *
 */
//@}


/** Bgpm_Perspective
 * The Bgpm_Perspective indicates whether to count events from a hardware or software perspective.
 * \li BGPM_HW_PERSPECTIVE - where
 *     the caller assigns an event set to particular hardware threads,
 *     and events are counted without regard to the software threads
 *     using the hardware.
 *     There is no accounting for context switches.
 * \li BGPM_SW_PERSPECTIVE - where event sets are assigned for the
 *     current software thread, and events attributable to the software
 *     thread are counted.  For shared counters, events are accumulated while
 *     while the software thread is active, though may not be attributable to the
 *     thread.
 */
typedef enum eBgpm_Perspective {
    BGPM_HW_PERSPECTIVE,   //!< Accum hardware thread events w/o regard to software
    BGPM_SW_PERSPECTIVE    //!< Accum software thread attributable events where possible
} Bgpm_Perspective;




#endif
