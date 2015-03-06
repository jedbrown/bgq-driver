/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

/// \file components/devices/bgq/mu2/CollectiveNetwork.h
///
/// \brief MU Collective Network core definitions

#ifndef __components_devices_bgq_mu2_CollectiveNetwork_h__
#define __components_devices_bgq_mu2_CollectiveNetwork_h__

#include <hwi/include/bqc/classroute.h>
#include <hwi/include/bqc/MU_PacketHeader.h>

// CAUTION! These two defines MUST refer to the same Virtual Channel!

/// \brief Virtual Channel value to use for MUSPI Classroute routines
///
/// This must be the same VC as specified by PAMI_MU_CR_PKT_VC.
///
#define PAMI_MU_CR_SPI_VC	BGQ_CLASS_INPUT_VC_SUBCOMM

/// \brief Virtual Channel value to use for MU Collective Packet headers
///
/// This must be the same VC as specified by PAMI_MU_CR_DCR_VC.
///
#define PAMI_MU_CR_PKT_VC	MUHWI_PACKET_VIRTUAL_CHANNEL_USER_SUB_COMM

#endif // __components_devices_bgq_mu2_CollectiveNetwork_h__
