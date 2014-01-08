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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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

//! \file  JobController.cc
//! \brief Methods for bgcios::jobctl::JobController class.

// Includes
#include "JobController.h"
#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/common/logging.h>

using namespace bgcios::jobctl;

LOG_DECLARE_FILE("cios.jobctld");


JobController::~JobController()
{
   _dataChannel.reset();
}

int
#ifdef LOG_CIOS_INFO_DISABLE
JobController::startup(in_port_t)
#else
JobController::startup(in_port_t dataChannelPort)
#endif
{
   LOG_CIOS_INFO_MSG("data channel listener on port " << dataChannelPort << " was ignored");
   return 0;
}

int
JobController::heartbeat(void)
{
   // Get pointer to inbound Heartbeat message.
   HeartbeatMessage *inMsg = (HeartbeatMessage*)_inboundMessage;

   // Build HeartbeatAckMessage message in outbound buffer.
   HeartbeatAckMessage* outMsg = (HeartbeatAckMessage*)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   outMsg->header.type = HeartbeatAck;
   outMsg->header.length = sizeof(HeartbeatAckMessage);
   outMsg->header.returnCode = Success;

   // compare current time against overloaded "job ID"
   if ( inMsg->header.jobId ) {
       const time_t now( time(NULL) );
       const time_t previous( static_cast<time_t>(inMsg->header.jobId) );
       const time_t difference = now - previous;
       if ( difference > _config->getHeartbeatTimeout() ) {
           LOG_WARN_MSG_FORCED(
                   __FUNCTION__ << " difference of " << difference << " seconds is greater than configured " << 
                   _config->getHeartbeatTimeout() << " value"
                   );
           // TODO trap and/or dump flight logs?
       } else {
           LOG_CIOS_DEBUG_MSG(__FUNCTION__ << " still alive " << difference << "s difference" );
       }
   } else {
       LOG_CIOS_DEBUG_MSG(__FUNCTION__ << " still alive");
   }

   return sendToDataChannel(outMsg); 
}

