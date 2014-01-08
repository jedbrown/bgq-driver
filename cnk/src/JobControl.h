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

#ifndef _CNK_JOBCONTROL_H
#define _CNK_JOBCONTROL_H

// Includes
#include "Kernel.h"
#include <cnk/include/Verbs.h>
#include <ramdisk/include/services/MessageHeader.h>

class JobControl
{
public:

   //! \brief  Initialize when node is booted.
   //! return 0 when successful, errno when unsuccessful.

   int init(cnv_pd *protectionDomain, cnv_cq *completionQ);

   //! \brief  Terminate when node is shutdown.
   //! return 0 when successful, errno when unsuccessful.

   int term(void);

   //! \brief  Handle an inbound message.
   //! param   wr_id is the work request id which is the alias to the inbound message region
   //! return 0 when successful, errno when unsuccessful.

   int processMessage(uint64_t wr_id);

   //! \brief  Handle a SetupJob message received from jobctld.
   //! \return 0 when successful, errno when unsuccessful.

   int setupJob(cnv_mr * inMsgRegion);

   //! \brief  Handle a CleanupJob message received from jobctld.
   //! \return 0 when successful, errno when unsuccessful.

   int cleanupJob(cnv_mr * inMsgRegion);

   //! \brief  Handle a LoadJob message received from jobctld.
   //! \return 0 when successful, errno when unsuccessful.

   int loadJob(cnv_mr * inMsgRegion);

   //! \brief  Send a LoadJobAckMessage to jobctld.
   //! \param  jobId Job identifier.
   //! \param  returnCode Return code value.
   //! \param  errorCode Error code value.
   //! \return 0 when successful, errno when unsuccessful.

   int loadJobAck(uint64_t jobId, uint32_t returnCode, uint32_t errorCode);

   //! \brief  Handle a StartJob message received from jobctld.
   //! \return 0 when successful, errno when unsuccessful.

   int startJob(cnv_mr * inMsgRegion);

   //! \brief  Handle a SignalJob message received from jobctld.
   //! return 0 when successful, errno when unsuccessful.

   int signalJob(cnv_mr * inMsgRegion);

   //! \brief  Send a ExitJobMessage to jobctld.
   //! \param  jobId Job identifier.
   //! \param  exitStatus Exit status of the job.
   //! \return 0 when successful, errno when unsuccessful.

   int exitJob(uint64_t jobId, uint32_t exitStatus);

   //! \brief  Handle a ExitJobAck message received from jobctld.
   //! return 0 when successful, errno when unsuccessful.

   int exitJobAck(cnv_mr * inMsgRegion);

   //! \brief  Send a ExitProcess message to jobctld.
   //! \param  jobId Job identifier.
   //! \param  rank of the process.
   //! \param  exitStatus Exit status of the process.
   //! \return 0 when successful, errno when unsuccessful.

   int exitProcess(uint64_t jobId, uint32_t rank, uint32_t exitStatus, int agent);

   //! \brief  Handle a ExitProcessAck message received from jobctld.
   //! \return 0 when successful, errno when unsuccessful.

   int exitProcessAck(cnv_mr * inMsgRegion);

   //! \brief  Handle a Completed message received from jobctld.
   //! \return 0 when successful, errno when unsuccessful.

   int completed(cnv_mr * inMsgRegion);

   //! \brief  Get the queue pair number.
   //! \return Queue pair number.

   uint32_t getQueuePairNum(void) const { return _queuePair.qp_num; }

private:

   //! \brief  Post a receive using the inbound message region.
   //! \return 0 when successful, errno when unsuccessful.

   int postRecv(cnv_mr * inMsgRegion);

   //! \brief  Post a send using the outbound message region.
   //! \param  block is the one of 0-maxBlocks at offset(blocksize * block)
   //! \return 0 when successful, errno when unsuccessful.

  int postSendBlock(cnv_mr& _outMessageRegion);

  int postSendAck(){return postSendBlock(_outMessageRegionAck);}

  int postSendCmd(){return postSendBlock(_outMessageRegionCmd);}
   //! \brief  Send a request message from outbound message region and receive a reply message to inbound message region.
   //! \return 0 when successful, errno when unsuccessful.

   int exchangeMessages(cnv_cq *completionQ);

   //! \brief  Wait for an ack message to be received by the node controller thread.
   //! \return Nothing.

   void waitForAck(void);

   //! Sequence id for tracking message exchanges.
   uint32_t _sequenceId;

   //! Queue pair connected to jobctld.
   cnv_qp _queuePair;

   //! Memory region for outbound command messages
   struct cnv_mr _outMessageRegionCmd;
   //! Memory region for outbound ack messages 
   struct cnv_mr _outMessageRegionAck;

   //! Memory region for inbound messages.
   struct cnv_mr _inMessage1Region;

      //! Memory region for inbound Ack messages.
   struct cnv_mr _inMessage2Region;

   //! Storage for outbound messages.
   char _outMessageCmd[512];
   char _outMessageAck[512];

   //! Storage for inbound messages.
   char _inMessage1[bgcios::SmallMessageRegionSize];

   //! Storage for inbound Ack messages.
   char _inMessage2[bgcios::SmallMessageRegionSize];

   //! Lock to synchronize use of queue pair.
   Lock_Atomic_t _lock;

   //! Indicator to notify sender that ack message has been received.
   //! \note A waiter must hold the lock.
   Lock_Atomic_t _isAckMessageReady;
};

extern class JobControl jobControl;

#endif // _CNK_JOBCONTROL_H

