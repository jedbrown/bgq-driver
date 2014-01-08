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

//! \file  SimIosController.h
//! \brief Declaration and inline methods for bgcios::iosctl::SimIosController class.

#ifndef IOSCTL_SIMIOSCONTROLLER_H
#define IOSCTL_SIMIOSCONTROLLER_H

// Includes
#include "IosController.h"
#include <netinet/in.h>
#include <tr1/memory>

namespace bgcios
{

namespace iosctl
{

//! \brief Handle job control service messages.

class SimIosController : public IosController
{
public:

   //! \brief  Default constructor.

   SimIosController(uint16_t simId);

   //! \brief  Default destructor.

   ~SimIosController();

   //! \brief  Open all connections needed by the service daemon.
   //! \param  dataChannelPort Port number for listening data channel socket.
   //! \return 0 when successful, errno when unsuccessful.

   int startup(in_port_t dataChannelPort);

   //! \brief  Close all connections needed by the service daemon.
   //! \return 0 when successful, errno when unsuccessful.

   int cleanup(void);

private:

};

//! Smart pointer for SimIosController object.
typedef std::tr1::shared_ptr<SimIosController> SimIosControllerPtr;

} // namespace iosctl

} // namespace bgcios

#endif // IOSCTL_SIMIOSCONTROLLER_H


