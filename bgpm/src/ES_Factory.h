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

#ifndef _BGPM_ESFACTORY_H_  // Prevent multiple inclusion
#define _BGPM_ESFACTORY_H_

#include "bgpm/include/bgpm.h"
#include "EvtSet.h"


namespace bgpm {


//! ESFactory
/*!
  Event Set factory function to analyze events and return a specific event set type.
*/

int GenerateEventSet(Bgpm_Modes mode, int evtId, EvtSet **pNewEvtSet);


}

#endif
