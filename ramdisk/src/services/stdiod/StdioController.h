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

//! \file  StdioController.h
//! \brief Declaration and inline methods for bgcios::stdio::StdioController class.

#ifndef STDIO_STDIOCONTROLLER_H
#define STDIO_STDIOCONTROLLER_H

// Includes
#include "StdioConfig.h"
#include <ramdisk/include/services/common/ServiceController.h>
#include <ramdisk/include/services/common/InetSocket.h>
#include <tr1/memory>

namespace bgcios
{

namespace stdio
{

//! \brief Handle standard I/O service messages.

class StdioController : public bgcios::ServiceController
{
public:

   //! \brief  Default constructor.
   //! \param  config Configuration from command line and properties file.

   StdioController(StdioConfigPtr config) : bgcios::ServiceController() { _config = config; }

   //! \brief  Default destructor.

   ~StdioController();

   //! \brief  Open all connections needed by the service daemon.
   //! \param  dataChannelPort Port number for listening data channel socket.
   //! \return 0 when successful, errno when unsuccessful.

   virtual int startup(in_port_t dataChannelPort);

   //! \brief  Close all connections needed by the service daemon.
   //! \return 0 when successful, errno when unsuccessful.

   virtual int cleanup(void) { return 0; }

   //! \brief  Monitor for events from all connections.
   //! \return Nothing.

   virtual void eventMonitor(void) { return; }

protected:

   //! Configuration from command line and properties file.
   StdioConfigPtr _config;

};

//! Smart pointer for StdioController object.
typedef std::tr1::shared_ptr<StdioController> StdioControllerPtr;

} // namespace stdio

} // namespace bgcios

#endif // STDIO_STDIOCONTROLLER_H
