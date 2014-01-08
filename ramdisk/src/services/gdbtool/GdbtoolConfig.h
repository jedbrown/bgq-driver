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

//! \file  GdbtoolConfig.h
//! \brief Declaration and inline methods for bgcios::gdbsvr::GdbtoolConfig class.

#ifndef GDBTOOL_GDBTOOLCONFIG_H
#define GDBTOOL_GDBTOOLCONFIG_H

// Includes
#include <ramdisk/include/services/common/CiosConfig.h>

namespace bgcios
{

namespace gdbtool
{

//! \brief Configuration for GDB server tool.

class GdbtoolConfig : public bgcios::CiosConfig
{
public:

   //! \brief  Default constructor.
   //! \param  argc Number of command line arguments.
   //! \param  argv Array of pointers to command line arguments.

   GdbtoolConfig(int argc, char **argv);

   //! \brief  Get the value of the listen_port configuration variable.
   //! \return Port number for listening connection.

   uint16_t getListenPort(void) const;

   //! \brief  Get the value of the rank configuration variable.
   //! \return Rank of process to debug.

   uint32_t getRank(void) const;

};

} // namespace gdbtool

} // namespace bgcios

#endif // GDBTOOL_GDBTOOLCONFIG_H

