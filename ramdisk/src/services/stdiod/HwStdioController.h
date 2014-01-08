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

//! \file  HwStdioController.h
//! \brief Declaration for bgcios::stdio::HwStdioController class.

#ifndef STDIO_HWSTDIOCONTROLLER_H
#define STDIO_HWSTDIOCONTROLLER_H

// Includes
#include "StdioController.h"
#include "Job.h"
#include <ramdisk/include/services/common/PointerMap.h>
#include <ramdisk/include/services/common/RdmaCompletionChannel.h>
#include <ramdisk/include/services/common/RdmaClient.h>
#include <ramdisk/include/services/common/RdmaServer.h>
#include <tr1/memory>
#include <deque>


namespace bgcios
{

namespace stdio
{


//! \brief Handle standard I/O service messages when running on hardware.

class HwStdioController : public StdioController
{
public:

   //! \brief  Default constructor.
   //! \param  config Configuration from command line and properties file.

   HwStdioController(StdioConfigPtr config);

   //! \brief  Default destructor.

   ~HwStdioController();

   //! \brief  Open all connections needed by the service daemon.
   //! \param  dataChannelPort Port number for listening data channel socket.
   //! \return 0 when successful, errno when unsuccessful.

   int startup(in_port_t dataChannelPort);

   //! \brief  Close all connections needed by the service daemon.
   //! \return 0 when successful, errno when unsuccessful.

   int cleanup(void);

   //! \brief  Monitor for events from all channels.
   //! \return Nothing.

   void eventMonitor(void);

private:

   //! \brief  Handle events from data channel.
   //! \param  authOnly When true, only handle an Authenticate message.
   //! \return 0 when successful, errno when unsuccessful.

   int dataChannelHandler(InetSocketPtr authOnly = InetSocketPtr());

   //! \brief  Handle events from command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int commandChannelHandler(void);

   //! \brief  Handle events from rdma event channel.
   //! \return Nothing.

   void eventChannelHandler(void);

   //! \brief  Handle events from completion channel.
   //! \return Nothing.

   void completionChannelHandler(void);

   //! \brief  Handle an Authenticate message received from data channel.
   //! \param  channel data channel
   //! \return 0 when successful, errno when unsuccessful.

   int authenticate(InetSocketPtr channel);

   //! \brief  Handle a StartJob message received from data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int startJob(void);

   //! \brief  Handle a ReadStdin message received from completion channel.
   //! \param  client Client connection message was received from.
   //! \return Nothing

   void readStdin(const RdmaClientPtr& client);

   //! \brief  Handle a ReadStdinAck message received from data channel.
   //! \return Nothing.

   void readStdinAck(void);

   //! \brief  Handle an Interrupt message received from command channel.
   //! \param  source Path to source command channel that message was received from.
   //! \return 0 when successful, errno when unsuccessful.

   int interrupt(const std::string source);

   //! \brief  Handle a WriteStdout or WriteStderr message received from completion channel.
   //! \param  client Client connection message was received from.
   //! \return Nothing (void) after handling

   void writeStdio(const RdmaClientPtr& client);

   //! \brief  Handle a CloseStdio message received from completion channel.
   //! \param  client Client connection message was received from.
   //! \return Nothing

   void closeStdio(const RdmaClientPtr& client);

   //! \brief  Handle a ChangeConfig message received from data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int changeConfig(void);

   //! \brief  Handle a Reconnect message received from data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int reconnect(void);

   //! \brief  Handle a Terminate message received from command channel.
   //! \param  source Path to source command channel that message was received from.
   //! \return 0 when successful, errno when unsuccessful.

   int terminate(const std::string source);

   //! \brief  Add a client for a valid StdioMsg
   bool addValidStdioMsg(const RdmaClientPtr& client);

   // list of Messages
   std::deque< RdmaClientPtr > _dequeStdioMsgInClient;

   //! Listener for RDMA connections.
   bgcios::RdmaServerPtr _rdmaListener;

   //! Protection domain for all resources.
   bgcios::RdmaProtectionDomainPtr _protectionDomain;

   //! Completion channel for all completion queues.
   bgcios::RdmaCompletionChannelPtr _completionChannel;

   //! Map of all active clients indexed by queue pair number.
   bgcios::PointerMap<uint32_t, RdmaClientPtr> _clients;

   //! Map of currently running jobs indexed by job id.
   bgcios::PointerMap<uint64_t, JobPtr> _jobs;

   //! Typedef for job list iterator.
   typedef bgcios::PointerMap <uint64_t, JobPtr>::const_iterator job_list_iterator;

};

//! Smart pointer for HwStdioController object.
typedef std::tr1::shared_ptr<HwStdioController> HwStdioControllerPtr;

} // namespace stdio

} // namespace bgcios

#endif // STDIO_STDIOCONTROLLER_H

