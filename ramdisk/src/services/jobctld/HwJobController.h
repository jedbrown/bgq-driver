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

//! \file  HwJobController.h
//! \brief Declaration and inline methods for bgcios::jobctl::HwJobController class.

#ifndef JOBCTL_HWJOBCONTROLLER_H
#define JOBCTL_HWJOBCONTROLLER_H

// Includes
#include "JobController.h"
#include "ComputeNode.h"
#include <ramdisk/include/services/common/RdmaClient.h>
#include <ramdisk/include/services/common/RdmaServer.h>
#include <ramdisk/include/services/common/RdmaProtectionDomain.h>
#include <ramdisk/include/services/common/RdmaCompletionChannel.h>
#include <ramdisk/include/services/common/PointerMap.h>
#include <ramdisk/include/services/JobctlMessages.h>
#include <tr1/memory>
#include <poll.h>

namespace bgcios
{

namespace jobctl
{

//! \brief Handle job control service messages when running on hardware.

class HwJobController : public JobController
{
public:

   //! \brief  Default constructor.
   //! \param  config Configuration from command line and properties file.

   HwJobController(JobctlConfigPtr config);

   //! \brief  Default destructor.

   ~HwJobController();

   //! \brief  Open all connections needed by the service daemon.
   //! \param  dataChannelPort Port number for listening data channel socket.
   //! \return 0 when successful, errno when unsuccessful.

   int startup(in_port_t dataChannelPort);

   //! \brief  Close all connections needed by the service daemon.
   //! \return 0 when successful, errno when unsuccessful.

   int cleanup(void);

   //! \brief  Handle events from all connections.
   //! \return Nothing.

   void eventMonitor(void);

private:

   //! \brief  Handle events from command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int commandChannelHandler(void);

   //! \brief  Handle events from data channel.
   //! \param  dataChannel When not null, only handle an Authenticate message.
   //! \return 0 when successful, errno when unsuccessful.

   int dataChannelHandler(InetSocketPtr dataChannel = InetSocketPtr());

   //! \brief  Handle events from rdma event channel.
   //! \return Nothing.

   void eventChannelHandler(void);

   //! \brief  Handle events from completion channel.
   //! \return Nothing.

   void completionChannelHandler(void);

   //! \brief  Handle an Authenticate message received from data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int authenticate(InetSocketPtr channel);

   //! \brief  First step in handling a DiscoverNode message received from completion channel.
   //! \param  client Client connection message was received from.
   //! \param  mh     Message pointer
   //! \return Nothing.

   void discoverNodeStep1(const RdmaClientPtr& client, bgcios::MessageHeader * mh);

   //! \brief  Second step in handling a DiscoverNode message received from command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int discoverNodeStep2(void);

   //! \brief  Handle a SetupJob message received from data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int setupJob(void);

   //! \brief  Handle a SetupJobAck message received from completion channel.
   //! \param  mh     Message pointer
   //! \return Nothing.

   void setupJobAck(bgcios::MessageHeader * mh);

   //! \brief  Handle a LoadJob message received from data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int loadJob(void);

   //! \brief  Handle a LoadJobAck message received from completion channel.
   //! \param  mh     Message pointer
   //! \return Nothing.

   void loadJobAck(bgcios::MessageHeader * mh);

   //! \brief  Handle a LoadJobAck message received from command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int loadJobAck(void);

   //! \brief  Handle a StartJob message received from data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int startJob(void);

   //! \brief  Handle a StartJobAck message received from completion channel.
   //! \param  mh     Message pointer
   //! \return Nothing.

   void startJobAck(bgcios::MessageHeader * mh);

   //! \brief  Handle a StartTool message received from data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int startTool(void);

   //! \brief  Handle a EndTool message received from data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int endTool(void);

   //! \brief  Handle a CheckToolStatus message received from data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int checkToolStatus(void);

   //! \brief  Handle a ExitTool message sent to data channel.
   //! \param  job Job that had a tool end.
   //! \param  toolId Tool identifier of tool that ended.
   //! \return 0 when successful, errno when unsuccessful.

   int exitTool(JobPtr job, uint32_t toolId);

   //! \brief  Handle a ExitToolAck message received from data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int exitToolAck(void);

   //! \brief  Handle a SignalJob message received from data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int signalJob(void);

   //! \brief  Handle a SignalJobAck message received from completion channel.
   //! \param  mh     Message pointer
   //! \return Nothing.

   void signalJobAck(bgcios::MessageHeader * mh);

   //! \brief  Handle a CleanupJob message received from data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int cleanupJob(void);

   //! \brief  Handle a CleanupJobAck message received from completion channel.
   //! \param  mh     Message pointer
   //! \return Nothing.

   void cleanupJobAck(bgcios::MessageHeader * mh);

   //! \brief  Handle a CleanupJobAck message received from command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int cleanupJobAck(void);

   //! \brief  Handle a Reconnect message received from data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int reconnect(void);

   //! \brief  Handle a ExitProcess message received from completion channel.
   //! \param  client Client connection message was received from.
   //! \param  mh     Message pointer
   //! \return Nothing.

   void exitProcess(const RdmaClientPtr& client, bgcios::MessageHeader * mh);

   //! \brief  Handle a ExitJob message received from completion channel.
   //! \param  client Client connection message was received from.
   //! \param  mh     Message pointer
   //! \return Nothing.

   void exitJob(const RdmaClientPtr& client, bgcios::MessageHeader * mh);

   //! \brief  Handle a ChangeConfig message received from data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int changeConfig(void);

   //! \brief  Handle a Terminate message received from command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int terminate(void);

   //! Listener for RDMA connections.
   bgcios::RdmaServerPtr _rdmaListener;

   //! Bound listener.
   bgcios::RdmaServerPtr _boundServer;

   //! Protection domain for all resources.
   bgcios::RdmaProtectionDomainPtr _protectionDomain;

   //! Completion channel for all completion queues.
   bgcios::RdmaCompletionChannelPtr _completionChannel;

   //! Map of all active connections indexed by queue pair number.
   bgcios::PointerMap<uint32_t, RdmaClientPtr> _clients;

   //! Map of connected compute nodes indexed by service id.
   bgcios::PointerMap<uint64_t, ComputeNodePtr> _cnodes;

   //! Typedef for compute node list iterator.
   typedef bgcios::PointerMap <uint64_t, ComputeNodePtr>::const_iterator cnode_list_iterator;

   //! Service id for next client that sends DiscoverNode message.
   volatile uint32_t _nextServiceId;

   //! Number of tools that are currently active.
   uint32_t _activeTools;

   //! Suspend the completion channel to avoid hang conflict on sendto between jobctld and iosd:
   //! jobctld sending bgcios::iosctl::StartNodeServicesMessage to iosd via sendto
   //! when iosd is doing a sendto to jobctld with bgcios::iosctl::StartNodeServicesMessage (hang)
   struct pollfd * pollfdCompletionChannel;  //

};

//! Smart pointer for HwJobController object.
typedef std::tr1::shared_ptr<HwJobController> HwJobControllerPtr;

} // namespace jobctl

} // namespace bgcios

#endif // JOBCTL_HWJOBCONTROLLER_H

