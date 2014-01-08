/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */


#include <firmware/include/Firmware.h>
#include "Firmware_internals.h"

Personality_t FW_PERSONALITY FW_Personality  = PERSONALITY_DEFAULT();

int fw_getPersonality( Personality_t* personality, unsigned size ) {

  if ( sizeof(Personality_t) != size  ) {
    return -1;
  }

  memcpy( personality, &FW_Personality, size );
  return 0;
}

int fw_isIoNode() {

  //! @todo Hook this up to the testint DCRs

  return (FW_Personality.Kernel_Config.NodeConfig & PERS_ENABLE_IsIoNode) ? 1 : 0;
}
