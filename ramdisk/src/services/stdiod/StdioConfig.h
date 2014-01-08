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

//! \file  StdioConfig.h
//! \brief Declaration and inline methods for bgcios::stdio::StdioConfig class.

#ifndef STDIO_STDIOCONFIG_H
#define STDIO_STDIOCONFIG_H

// Includes
#include <ramdisk/include/services/common/CiosConfig.h>
#include <tr1/memory>

namespace bgcios
{

namespace stdio
{

//! \brief Configuration for standard I/O daemon.

class StdioConfig : public bgcios::CiosConfig
{
public:

   //! \brief  Default constructor.
   //! \param  argc Number of command line arguments.
   //! \param  argv Array of pointers to command line arguments.

   StdioConfig(int argc, char **argv);

   //! \brief  Get the value of the listen_port configuration variable.
   //! \return Port number for listening connection.

   uint16_t getListenPort(void) const;

   //! \brief  Get the value of the send_buffer_size configuration variable.
   //! \return Size of send buffer (in bytes) for control system connection.

   int getSendBufferSize(void) const;
};

//! Smart pointer for StdioConfig object.
typedef std::tr1::shared_ptr<StdioConfig> StdioConfigPtr;

} // namespace stdio

} // namespace bgcios

#endif // STDIO_STDIOCONFIG_H
