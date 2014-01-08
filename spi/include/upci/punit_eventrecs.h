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

#ifndef _UPCI_PUNIT_EVENTRECS_H_  // Prevent multiple inclusion
#define _UPCI_PUNIT_EVENTRECS_H_

#include "upc_evttbl.h"


//! \addtogroup upci_spi
//@{
/**
 * \file
 *
 * \brief UPCI Various Event Record structures.
 *
 */
//@}


__BEGIN_DECLS


//////////////////////////////////////////////////////////////////
/*!
 * \brief PunitEventRec Structure
 *
 * Contains assigned attributes for an event..
*/
typedef struct sUpci_PunitEventRec
{
    UPC_PunitEvtTableRec_t *   pEvent;      //!<  ptr to event table record for assigned event.
    UPC_EventSources_t         evtSrc;      //!<  Event Source Type (copy of event table src)
    ushort                     hwThread;    //!<  assigned hardware thread
    ushort                     p_Num;       //!<  assigned upc_p unit by core number
    ushort                     p_Ctr;       //!<  assigned upc_p counter number (0-23)
    ushort                     a2_Sig;      //!<  assigned pc bus a2 signal (if applicable)
    ushort                     c_Cgrp;      //!<  assigned upc_c counter group num (in mode0 this is 24 counter grp, mode 1 a 16 counter grp)
    ushort                     c_Ctr;       //!<  assigned upc_c counter number    (in mode0 this is 24 counter grp, mode 1 a 16 counter grp)

} Upci_PunitEventRec_t;
//! \note The counter group may not be the same as the unit, if the spare core is prior to the
//!       unit value - the associated upc_c counter group will be shifted 1 higher (with upc_p chicken switch active)
//!       They are also especially needed if we support mode1 in the future.



//! Upci_PunitEventRec_Dump
//! Dump object for debug purposes.
void Upci_PunitEventRec_Dump(unsigned indent, Upci_PunitEventRec_t * o);

__END_DECLS


#endif
