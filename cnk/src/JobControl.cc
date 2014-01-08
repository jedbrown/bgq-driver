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

// Includes
#include "Kernel.h"
#include "JobControl.h"
#include <ramdisk/include/services/JobctlMessages.h>
#include <netinet/in.h>
#include "NodeController.h"

// Define the jobControl object in static storage
JobControl jobControl;

using namespace bgcios::jobctl;

extern int Process_SetupMap(LoadJobMessage *loadMsg, AppState_t **ppAppState);
extern int Process_SetupAgentMap(LoadJobMessage *loadMsg, AppState_t *parentAppState);
extern int Process_SetupJob(bgcios::jobctl::SetupJobMessage *setupJobMsg);
extern int App_CleanupJob(CleanupJobMessage *inMsg);

extern "C" void UPC_Init(uint64_t jobID);


int
JobControl::init(cnv_pd *protectionDomain, cnv_cq *completionQ)
{
   // Register memory regions.
    assert(0 == cnv_reg_mr(&_inMessage1Region, protectionDomain, &_inMessage1, sizeof(_inMessage1), CNV_ACCESS_LOCAL_WRITE));

    assert(0 == cnv_reg_mr(&_inMessage2Region, protectionDomain, &_inMessage2, sizeof(_inMessage2), CNV_ACCESS_LOCAL_WRITE));

    assert(0 == cnv_reg_mr(&_outMessageRegionCmd, protectionDomain, &_outMessageCmd, sizeof(_outMessageCmd), CNV_ACCESS_LOCAL_WRITE));

    assert(0 == cnv_reg_mr(&_outMessageRegionAck, protectionDomain, &_outMessageAck, sizeof(_outMessageAck), CNV_ACCESS_LOCAL_WRITE));
    
   // Create queue pair.
   cnv_qp_init_attr attr;
   attr.send_cq = completionQ;
   attr.recv_cq = completionQ;

   assert(0 == cnv_create_qp(&_queuePair, protectionDomain, &attr));

   // Connect to jobctld on I/O node.
   struct sockaddr_in destAddress;
   destAddress.sin_family = AF_INET;
   destAddress.sin_port = BaseRdmaPort;
   destAddress.sin_addr.s_addr = INADDR_ANY;
   int err = cnv_connect(&_queuePair, (struct sockaddr *)&destAddress);
   Node_ReportConnect(err, destAddress.sin_addr.s_addr, destAddress.sin_port);
   if (err != 0) 
   {
       TRACE( TRACE_Jobctl, ("(E) JobControl::init%s: cnv_connect() failed, error %d\n", whoami(), err) );
       Kernel_Crash(RAS_KERNELCNVCONNECTFAIL);
   }
   TRACE( TRACE_Jobctl, ("(I) JobControl::init%s: connected to jobctld\n", whoami()) );
   
   // Obtain the lock.
   Kernel_Lock(&_lock);

   // Build DiscoverNode message in outbound message buffer.
   DiscoverNodeMessage *outMsg = (DiscoverNodeMessage *)_outMessageRegionCmd.addr;
   memset(outMsg, 0, sizeof(DiscoverNodeMessage));
   outMsg->header.service = bgcios::JobctlService;
   outMsg->header.version = ProtocolVersion;
   outMsg->header.type = DiscoverNode;
   outMsg->header.length = sizeof(DiscoverNodeMessage);
   outMsg->coords.aCoord = GetPersonality()->Network_Config.Acoord;
   outMsg->coords.bCoord = GetPersonality()->Network_Config.Bcoord;
   outMsg->coords.cCoord = GetPersonality()->Network_Config.Ccoord;
   outMsg->coords.dCoord = GetPersonality()->Network_Config.Dcoord;
   outMsg->coords.eCoord = GetPersonality()->Network_Config.Ecoord;
   outMsg->blockId = GetPersonality()->Network_Config.BlockID;
   outMsg->uci = GetPersonality()->Kernel_Config.UCI;
   outMsg->bridge.aCoord = GetPersonality()->Network_Config.cnBridge_A;
   outMsg->bridge.bCoord = GetPersonality()->Network_Config.cnBridge_B;
   outMsg->bridge.cCoord = GetPersonality()->Network_Config.cnBridge_C;
   outMsg->bridge.dCoord = GetPersonality()->Network_Config.cnBridge_D;
   outMsg->bridge.eCoord = GetPersonality()->Network_Config.cnBridge_E;

   // Send the message to jobctld.  Assumption is using _inMessage1Region for receiving first message
   err = exchangeMessages(completionQ);
   if (err != 0) {
      Kernel_Unlock(&_lock);
      return err;
   }

   // Save info returned in DiscoverNodeAck message.
   DiscoverNodeAckMessage *inMsg = (DiscoverNodeAckMessage *)_inMessage1Region.addr; //assumed  using _inMessage1Region
   uint64_t * data = (uint64_t * )inMsg;
   Kernel_WriteFlightLog(FLIGHTLOG, FL_JCLMSGRCV, data[0],data[1],data[2],data[3] );
   if (inMsg->header.type != DiscoverNodeAck) {
      Kernel_Unlock(&_lock);
      if (inMsg->header.returnCode == bgcios::VersionMismatch) {
         Kernel_WriteFlightLog(FLIGHTLOG, FL_VERMISMAT, inMsg->header.service, ProtocolVersion, inMsg->header.errorCode, 0); 
         RASBEGIN(3);
         RASPUSH(inMsg->header.service);
         RASPUSH(ProtocolVersion);
         RASPUSH(inMsg->header.errorCode);
         RASFINAL(RAS_KERNELVERSIONMISMATCH);
         Kernel_Crash(1);
      }
      return err;
   }

   NodeState.ServiceId = inMsg->serviceId;
   NodeState.ServiceDeviceAddr = inMsg->deviceAddress;
   NodeState.sysiodPortAddressDestination = inMsg->sysiod_port;
   NodeState.toolctldPortAddressDestination = inMsg->toolctld_port;
   strncpy(NodeState.NodeName, inMsg->nodeName, sizeof(NodeState.NodeName));
   strncpy(NodeState.Release, inMsg->release, sizeof(NodeState.Release));

   // Release the lock.
   Kernel_Unlock(&_lock);

   // Post receives
   postRecv(&_inMessage1Region);
   postRecv(&_inMessage2Region);

   return 0;
}

int
JobControl::term(void)
{
   int rc = 0;

   // Disconnect from jobctld.
   int err = cnv_disconnect(&_queuePair);
   if (err != 0) {
      TRACE( TRACE_Jobctl, ("(E) JobControl::term%s: cnv_disconnect() failed, error %d\n", whoami(), err) );
      rc = err;
   }

   // Destroy queue pair.
   err = cnv_destroy_qp(&_queuePair);
   if (err != 0) {
      TRACE( TRACE_Jobctl, ("(E) JobControl::term%s: cnv_destroy_qp() failed, error %d\n", whoami(), err) );
      rc = err;
   }

   // Deregister memory regions.
   err = cnv_dereg_mr(&_outMessageRegionCmd);
   if (err != 0) {
      TRACE( TRACE_Jobctl, ("(E) JobControl::term%s: cnv_dereg_mr() failed for outbound message region, error %d\n", whoami(), err) );
      rc = err;
   }
      // Deregister memory regions.
   err = cnv_dereg_mr(&_outMessageRegionAck);
   if (err != 0) {
      TRACE( TRACE_Jobctl, ("(E) JobControl::term%s: cnv_dereg_mr() failed for outbound message region, error %d\n", whoami(), err) );
      rc = err;
   }
   err = cnv_dereg_mr(&_inMessage1Region);
   if (err != 0) {
      TRACE( TRACE_Jobctl, ("(E) JobControl::term%s: cnv_dereg_mr() failed for inbound message region, error %d\n", whoami(), err) );
      rc = err;
   }
   err = cnv_dereg_mr(&_inMessage2Region);
   if (err != 0) {
      TRACE( TRACE_Jobctl, ("(E) JobControl::term%s: cnv_dereg_mr() failed for inbound message region, error %d\n", whoami(), err) );
      rc = err;
   }

   TRACE( TRACE_Jobctl, ("(I) JobControl::term%s: termination is complete\n", whoami()) );
   return rc;
}

int
JobControl::processMessage(uint64_t wr_id)
{
   struct cnv_mr * inMsgRegion = (struct cnv_mr *)wr_id;
   bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)(inMsgRegion->addr);
   TRACE( TRACE_Jobctl, ("(I) JobControl::processMessage%s received message %u for service %u\n", whoami(), msghdr->type, msghdr->service) );
   uint64_t * data = (uint64_t * )msghdr;
   Kernel_WriteFlightLog(FLIGHTLOG, FL_JCLMSGRCV, data[0],data[1],data[2],data[3] );

   switch (msghdr->type) {
      case SetupJob: setupJob(inMsgRegion); break;
      case LoadJob: loadJob(inMsgRegion); break;
      case StartJob: startJob(inMsgRegion); break;
      case SignalJob: signalJob(inMsgRegion); break;
      case ExitJobAck: exitJobAck(inMsgRegion); break;
      case ExitProcessAck: exitProcessAck(inMsgRegion); break;
      case CleanupJob: cleanupJob(inMsgRegion); break;
      default:
         printf("(E) JobControl::processMessage(): JobctlService message type %u is not supported\n", msghdr->type);
         // Build error message.
         break;
   }

   return 0;
}

int
JobControl::setupJob(cnv_mr * inMsgRegion)
{
   // Get pointer to inbound SetupJob message.
   SetupJobMessage *inMsg = (SetupJobMessage *)inMsgRegion->addr;
   TRACE( TRACE_Jobctl, ("(I) JobControl::setupJob%s: Job %lu: received SetupJob message\n", whoami(), inMsg->header.jobId) );

   // No locking, oversubscription prevented at IO node

   // Call the interface to setup the node for the job.
   int returnCode = Process_SetupJob(inMsg);

   // Build the SetupJobAck message.
   SetupJobAckMessage *outMsg = (SetupJobAckMessage *)_outMessageRegionAck.addr;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   
   // Re-post receive
   postRecv(inMsgRegion);

   outMsg->header.type = SetupJobAck;
   outMsg->header.length = sizeof(SetupJobAckMessage);
   outMsg->header.returnCode = returnCode;
   outMsg->header.errorCode = 0;

   // Send the SetupJobAck message to jobctld.
   postSendAck();

   Kernel_WriteFlightLog(FLIGHTLOG, FL_JOBSETUP_, inMsg->header.jobId, inMsg->numRanksPerNode, inMsg->numRanks, inMsg->blockId);
   TRACE( TRACE_Jobctl, ("(I) JobControl::setupJob%s: Job %lu: sent SetupJobAck messsage, return code %u, error code %u\n",
                         whoami(), outMsg->header.jobId, outMsg->header.returnCode, outMsg->header.errorCode) );
   return 0;
}

int
JobControl::cleanupJob(cnv_mr * inMsgRegion)
{
   // Get pointer to inbound CleanupJob message.
   CleanupJobMessage *inMsg = (CleanupJobMessage *)inMsgRegion->addr;
   TRACE( TRACE_Jobctl, ("(I) JobControl::cleanupJob%s: Job %lu: received CleanupJob message\n", whoami(), inMsg->header.jobId) );

   // No locking, oversubscription prevented at IO node

   // Call the interface to cleanup the node for the job.
   int err =  App_CleanupJob(inMsg);


   // Build the CleanupJobAck message.
   CleanupJobAckMessage *outMsg = (CleanupJobAckMessage *)_outMessageRegionAck.addr;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));

   // Repost the receive
   postRecv(inMsgRegion);

   outMsg->header.type = CleanupJobAck;
   outMsg->header.length = sizeof(CleanupJobAckMessage);
   if (err == 0) {
      outMsg->header.returnCode = bgcios::Success;
      outMsg->header.errorCode = 0;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = 0;
   }
   outMsg->epilogReturnCode = bgcios::Success;
   outMsg->epilogErrorCode = 0;

   // Send the CleanupJobAck message to jobctld.
   postSendAck();

   Kernel_WriteFlightLog(FLIGHTLOG, FL_JOBCLEAND, inMsg->header.jobId, 0, 0, 0);
   TRACE( TRACE_Jobctl, ("(I) JobControl::cleanupJob%s: Job %lu: sent CleanupJobAck message, return code %u, error code %u\n",
                         whoami(), outMsg->header.jobId, outMsg->header.returnCode, outMsg->header.errorCode) );
   return 0;
}

int
JobControl::loadJob(cnv_mr * inMsgRegion)
{
   // Get pointer to inbound LoadJob message.
   LoadJobMessage *inMsg = (LoadJobMessage *)inMsgRegion->addr;
   TRACE( TRACE_Jobctl, ("(I) JobControl::loadJob%s: Job %lu: received LoadJob message for executable %s\n",
                         whoami(), inMsg->header.jobId, inMsg->arguments) );

   // No locking, oversubscription prevented at IO node

   // We are processing the Application Load Message. Extract the following from the message: 
   //   1. Parameter information for the Application (populate AppState structure after we run the Process_MapSetup() )
   //   2. Environment Variables for the Application (populate the AppState structure after we determine this form Process_MapSetup() )
   //   3. Path information to the executable - pass this information to the app leader at time of the IPI interrupt
   // 

   // Call the interface to setup the requested process configuration. If there is a problem with the request, the
   // following function will return an error.
   AppState_t *appState;
   int map_results = Process_SetupMap(inMsg, &appState);
   if (map_results != bgcios::Success)
   {
      TRACE( TRACE_Jobctl, ("(E) Invalid Process Mapping request. Return code from Process_SetupMap(): %d\n", map_results) );

      // Build LoadJobAck message in outbound message buffer.
      LoadJobAckMessage *outMsg = (LoadJobAckMessage *)_outMessageRegionAck.addr;
      memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
      outMsg->header.type = LoadJobAck;
      outMsg->header.length = sizeof(LoadJobAckMessage);
      outMsg->header.returnCode = map_results;
      outMsg->header.errorCode = 0;

      // Get the next message.
      postRecv(inMsgRegion);

      // Send the LoadJobAck message to jobctld.
      postSendAck();

      return 0;
   }

   // Setup the process structure for the App Agent processes passing in the parent appState pointer
   Process_SetupAgentMap(inMsg, appState);

   // Make sure UPC hardware is set to defaults
   // Only needs to be processed on one thread within the core.
   // Also have similar code in AppSetup.cc
   if (ProcessorThreadID() == 0)
   {
       UPC_Init(appState->JobID);
   }

   // Wake up all of the hardware threads used by the job on this node.
   uint64_t appThreadMask = appState->AppThreadsMask;
   int threadCount = appState->Active_Cores * 4;
   for (int i = cntlz64(appThreadMask); threadCount; i++)
   {
       if (appThreadMask & _BN(i))
       {
           IPI_load_application(i);
       }
       threadCount--;
   }

   // re-post 
   postRecv(inMsgRegion);

   return 0;
}

int
JobControl::loadJobAck(uint64_t jobId, uint32_t returnCode, uint32_t errorCode)
{
   // Build the LoadJobAck message.
   LoadJobAckMessage *outMsg = (LoadJobAckMessage *)_outMessageRegionAck.addr;
   outMsg->header.service = bgcios::JobctlService;
   outMsg->header.type = LoadJobAck;
   outMsg->header.length = sizeof(LoadJobAckMessage);
   outMsg->header.jobId = jobId;
   outMsg->header.returnCode = returnCode;
   outMsg->header.errorCode = errorCode;
   outMsg->header.sequenceId = _sequenceId++;
   outMsg->header.rank = 0; // Is it OK to use GetMyProcess()->Rank;
   outMsg->header.version = ProtocolVersion;

   // Send the LoadJobAck message to jobctld.
   postSendAck();

   Kernel_WriteFlightLog(FLIGHTLOG, FL_JOBLOADED, outMsg->header.jobId, returnCode, errorCode, 0);
   TRACE( TRACE_Jobctl, ("(I) JobControl::loadJobAck%s: Job %lu: sent LoadJobAck message, return code %u, error code %u\n",
                         whoami(), outMsg->header.jobId, outMsg->header.returnCode, outMsg->header.errorCode) );
   return 0;
}

int
JobControl::startJob(cnv_mr * inMsgRegion)
{
   // Get pointer to inbound StartJob message.
   StartJobMessage *inMsg = (StartJobMessage *)inMsgRegion->addr;
   TRACE( TRACE_Jobctl, ("(I) JobControl::startJob%s: Job %lu: received StartJob message\n", whoami(), inMsg->header.jobId) );

   // No locking, oversubscription prevented at IO node

   // Find the AppState object for the job.
   uint32_t returnCode = bgcios::Success;
   AppState_t *appState = App_GetAppFromJobid(inMsg->header.jobId);
   if (appState != NULL) {
       uint64_t cyclesPerMicro = GetPersonality()->Kernel_Config.FreqMHz;
       uint64_t timeBaseCycles = GetTimeBase();

       uint64_t raw_usecs = timeBaseCycles / cyclesPerMicro;
       appState->Timeshift =  inMsg->currentTime - raw_usecs;
       appState->JobStartTime = inMsg->currentTime;
       int rc = App_Start(appState);
       if (rc != 0) {
          returnCode = bgcios::JobIdError; // the targeted job is not in the correct state to allow a start operation.
       }
   }
   else {
      printf("(E) job %lu not found when handling StartJob message\n", inMsg->header.jobId);
      returnCode = bgcios::JobIdError;
   }

   // Build the StartJobAck message.
   StartJobAckMessage *outMsg = (StartJobAckMessage *)_outMessageRegionAck.addr;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   outMsg->header.type = StartJobAck;
   outMsg->header.length = sizeof(StartJobAckMessage);
   outMsg->header.returnCode = returnCode;
   outMsg->header.errorCode = 0;

   // Post a receive since now done with inbound
   postRecv(inMsgRegion);
   // Send the StartJobAck message to jobctld.
   postSendAck();

   Kernel_WriteFlightLog(FLIGHTLOG_high, FL_JOBSTARTD, inMsg->header.jobId, inMsg->currentTime, 0, 0);
   TRACE( TRACE_Jobctl, ("(I) JobControl::startJob%s: Job %lu: sent StartJobAck message, return code %u, error code %u\n",
                         whoami(), outMsg->header.jobId, outMsg->header.returnCode, outMsg->header.errorCode) );
   return 0;
}

int
JobControl::signalJob(cnv_mr * inMsgRegion)
{
   // Get pointer to inbound SignalJob message.
   SignalJobMessage *inMsg = (SignalJobMessage *)inMsgRegion->addr;
   TRACE( TRACE_Jobctl, ("(I) JobControl::signalJob%s: Job %lu: received SignalJob message with signal %d\n", whoami(), inMsg->header.jobId, inMsg->signo) );

   // Build the SignalJobAck message.
   SignalJobAckMessage *outMsg = (SignalJobAckMessage *)_outMessageRegionAck.addr;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   outMsg->header.type = SignalJobAck;
   outMsg->header.length = sizeof(SignalJobAckMessage);
   outMsg->header.returnCode = bgcios::Success;
   outMsg->header.errorCode = 0;
   // Obtain local copy of the signal so that we can post the receive and send the ack before we actually signal the job.
   int signo_local = inMsg->signo;

   // Post a receive since now done with inbound
   postRecv(inMsgRegion);
   // Send the SignalJobAck message to jobctld.
   postSendAck();
   // Signal the job. We purposely do this after we sent the SignalJobAck.
   App_SignalJob(outMsg->header.jobId, signo_local);

   Kernel_WriteFlightLog(FLIGHTLOG_high, FL_JOBSIGNAL, outMsg->header.jobId, signo_local, 0, 0);
   TRACE( TRACE_Jobctl, ("(I) JobControl::signalJob%s: Job %lu: delivered signal %d\n", whoami(), outMsg->header.jobId, signo_local) );

   return 0;
}

int
JobControl::exitJob(uint64_t jobId, uint32_t exitStatus)
{
   TRACE( TRACE_Jobctl, ("(I) JobControl::exitJob%s: Job %lu: building ExitJob message, status=0x%016x, signal=%u\n", whoami(), jobId, exitStatus, exitStatus&0xFF) );

   // Obtain the lock.
   Kernel_Lock(&_lock);

   // Build the message.
   ExitJobMessage *outMsg = (ExitJobMessage *)_outMessageRegionCmd.addr;
   outMsg->header.service = bgcios::JobctlService;
   outMsg->header.version = ProtocolVersion;
   outMsg->header.type = ExitJob;
   outMsg->header.length = sizeof(ExitJobMessage);
   outMsg->header.jobId = jobId;
   outMsg->header.returnCode = bgcios::Success;
   outMsg->header.errorCode = 0;
   outMsg->header.sequenceId = _sequenceId++;
   outMsg->header.rank = 0; // Rank has no meaning for the exitJob message. Also control blocks holding the rank have been initialized. Just set zero.
   outMsg->status = exitStatus;
   outMsg->endTime = 0; // This field should have been defined as usecs (to match startJob time) and not time_t. However change would ripple through control system.

   // Send the message to jobctld.
   postSendCmd();

   // Wait for ExitJobAck message to be received.
   waitForAck();

   // Release the lock.
   Kernel_Unlock(&_lock);

   Kernel_WriteFlightLog(FLIGHTLOG_high, FL_JOBEXITED, outMsg->header.jobId, outMsg->status, 0, 0);
   TRACE( TRACE_Jobctl, ("(I) JobControl::exitJob%s: Job %lu: notified ExitJobAck message was received\n", whoami(), jobId) );
   return 0;
}

int
JobControl::exitJobAck(cnv_mr * inMsgRegion)
{
   ExitJobAckMessage *inMsg = (ExitJobAckMessage *)inMsgRegion->addr;
   TRACE( TRACE_Jobctl, ("(I) JobControl::exitJobAck%s: Job %lu: received ExitJobAck message\n", whoami(), inMsg->header.jobId) );

   // Post a buffer for receiving  
   postRecv(inMsgRegion);

   // Tell thread that sent ExitJob message that the ExitJobAck message was received.
   fetch_and_add(&(_isAckMessageReady), 1);

   return 0;
}

int
JobControl::exitProcess(uint64_t jobId, uint32_t rank, uint32_t exitStatus, int agentinfo)
{
   TRACE( TRACE_Jobctl, ("(I) JobControl::exitProcess%s: Job %lu: building ExitProcess message, rank %u, status=0x%08x, signal=%u, agent=%d\n",
                         whoami(), jobId, rank, exitStatus, exitStatus&0xFF, agentinfo) );

   // Obtain the lock.
   Kernel_Lock(&_lock);

   // Build the message.
   ExitProcessMessage *outMsg = (ExitProcessMessage *)_outMessageRegionCmd.addr;
   outMsg->header.service = bgcios::JobctlService;
   outMsg->header.type = ExitProcess;
   outMsg->header.length = sizeof(ExitProcessMessage);
   outMsg->header.jobId = jobId;
   outMsg->header.returnCode = bgcios::Success;
   outMsg->header.errorCode = 0;
   outMsg->header.sequenceId = _sequenceId++;
   outMsg->header.rank = rank; 
   outMsg->header.version = ProtocolVersion;
   outMsg->status = exitStatus;
   outMsg->agent = agentinfo;

   // Send the message to jobctld.
   postSendCmd();

   // Wait for ExitProcessAck message to be received.
   waitForAck();

   // Release the lock.
   Kernel_Unlock(&_lock);

   Kernel_WriteFlightLog(FLIGHTLOG_high, FL_JOBPROCEX, outMsg->header.jobId, outMsg->header.rank, outMsg->status, outMsg->agent);
   TRACE( TRACE_Jobctl, ("(I) JobControl::exitProcess%s: Job %lu: notified ExitProcessAck message was received\n", whoami(), outMsg->header.jobId) );

   return 0;
}

int
JobControl::exitProcessAck(cnv_mr * inMsgRegion)
{
   ExitProcessAckMessage *inMsg = (ExitProcessAckMessage *)inMsgRegion->addr;
   TRACE( TRACE_Jobctl, ("(I) JobControl::exitProcessAck%s: Job %lu: received ExitProcessAck message for rank %u\n", whoami(), inMsg->header.jobId, inMsg->header.rank) );

   // Get the next message.
   postRecv(inMsgRegion);

   // Tell thread that sent ExitProcess message that the ExitProcessAck message was received.
   fetch_and_add(&(_isAckMessageReady), 1);

   return 0;
}


int
JobControl::postRecv(cnv_mr * inMsgRegion)
{

   // Build scatter/gather element for inbound message.
   struct cnv_sge recv_sge;
   recv_sge.addr = (uint64_t)inMsgRegion->addr;
   recv_sge.length = inMsgRegion->length;
   recv_sge.lkey = inMsgRegion->lkey;

   // Build receive work request.
   struct cnv_recv_wr recvRequest;
   memset(&recvRequest, 0, sizeof(recvRequest));
   recvRequest.next = NULL;
   recvRequest.sg_list = &recv_sge;
   recvRequest.num_sge = 1;
   recvRequest.wr_id = (uint64_t)inMsgRegion;

   // Post a receive for inbound message.
   cnv_recv_wr *badRecvRequest;
   int err = cnv_post_recv(&_queuePair, &recvRequest, &badRecvRequest);
   if (err != 0) {
      return err;
   }

   return 0;
}

int
JobControl::postSendBlock(cnv_mr& _outMessageRegion)
{
   // Build scatter/gather element for outbound message.
   bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)_outMessageRegion.addr;
   struct cnv_sge send_sge;
   send_sge.addr = (uint64_t)_outMessageRegion.addr;
   send_sge.length = msghdr->length;
   send_sge.lkey = _outMessageRegion.lkey;
   if (_outMessageRegion.length < msghdr->length){
     printf("Message Regions size error in postSendBlock\n");
   }
   // Build receive work request.
   struct cnv_send_wr sendRequest;
   memset(&sendRequest, 0, sizeof(sendRequest));
   sendRequest.next = NULL;
   sendRequest.sg_list = &send_sge;
   sendRequest.num_sge = 1;
   sendRequest.opcode = CNV_WR_SEND;
// sendRequest.wr_id = 0;

   // Post a receive for inbound message.  ???
   // Build a trap to detect no receive posted....
   uint64_t * data = (uint64_t * )msghdr;
   Kernel_WriteFlightLog(FLIGHTLOG, FL_JCLMSGSND, data[0],data[1],data[2],data[3] );
   cnv_send_wr *badSendRequest;
   int err = cnv_post_send(&_queuePair, &sendRequest, &badSendRequest);
   if (err != 0) {
      return err;
   }

   return 0;
}

int
JobControl::exchangeMessages(cnv_cq *completionQ)
{
   // Post a receive for the inbound message.
   postRecv(&_inMessage1Region);

   // Post a send for the outbound message.
   postSendCmd();

   // Wait for completions for the work requests posted above.
   int totalCompletions = 2;
   int numCompletions = 0;
   cnv_wc completions[totalCompletions];
   int foundCompletions = 0;

   while (numCompletions < totalCompletions) {
      // Remove available completions from the completion queue after getting completion event.
      int err = cnv_poll_cq(completionQ, totalCompletions, completions, &foundCompletions, CNV_ANY_PROC_ID);
      if (err != 0) {
         printf("(E) failed to remove completions, error %d\n", err);
         return err;
      }

      // Process each found completion.
      for (int index = 0; index < foundCompletions; ++index) {
         numCompletions += 1;
         if ((completions[index].opcode == CNV_WC_RECV) && (completions[index].status != CNV_WC_SUCCESS)) {
            printf("(E) recv failed, status %d\n", completions[index].status);
            return EINVAL;
         }
         if ((completions[index].opcode == CNV_WC_SEND) && (completions[index].status != CNV_WC_SUCCESS)) {
            printf("(E) send failed, status %d\n", completions[index].status);
            return EINVAL;
         }
      }
   }

   return 0;
}

void
JobControl::waitForAck(void)
{
   // Wait for the ack message to be received.
   uint32_t ready;
   ThreadPriority_Low();
   while ((ready = fetch(&(_isAckMessageReady))) == 0);
   ThreadPriority_High();

   // Reset for next message.
   fetch_and_sub(&(_isAckMessageReady), 1);

   return;
}


