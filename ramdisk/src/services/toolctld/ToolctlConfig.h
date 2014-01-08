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

//! \file  ToolctlConfig.h
//! \brief Declaration and inline methods for bgcios::toolctl::ToolctlConfig class.

#ifndef TOOLCTL_TOOLCTLCONFIG_H
#define TOOLCTL_TOOLCTLCONFIG_H

// Includes
#include <ramdisk/include/services/common/CiosConfig.h>

namespace bgcios
{

namespace toolctl
{

//! \brief Configuration for tool control daemon.

class ToolctlConfig : public bgcios::CiosConfig
{
public:

   //! \brief  Default constructor.
   //! \param  argc Number of command line arguments.
   //! \param  argv Array of pointers to command line arguments.

   ToolctlConfig(int argc, char **argv);

   //! \brief  Get the value of the service_id configuration variable.
   //! \return Unique identifier for this instance of daemon.

   uint32_t getServiceId(void) const;

};

} // namespace toolctl

} // namespace bgcios

#endif // TOOLCTL_TOOLCTLCONFIG_H

