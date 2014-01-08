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

//! \file  IosctlMessages.h
//! \brief Declarations for bgcios::iosctl message classes.

#ifndef IOSCTLMESSAGES_H
#define IOSCTLMESSAGES_H

// Includes
#include <ramdisk/include/services/MessageHeader.h>
#include <inttypes.h>

namespace bgcios
{

namespace iosctl
{

const uint16_t ErrorAck                = 1000;
const uint16_t Ready                   = 1001;

const uint16_t Terminate               = 1003;
const uint16_t TerminateAck            = 1004;
const uint16_t StartNodeServices       = 1005;

const uint16_t AllocateRegion          = 1007;
const uint16_t AllocateRegionAck       = 1008;
const uint16_t ReleaseRegion           = 1009;
const uint16_t ReleaseRegionAck        = 1010;
const uint16_t Interrupt               = 1011;
const uint16_t InterruptAckInvalid     = 1012;


//! Size of an iosctl message (fixed size since messages are sent on datagram sockets).
const uint32_t IosctlMessageSize = 40;

//! Current version of protocol.
const uint8_t ProtocolVersion = 1;

//! Message to acknowledge a message that is in error.

struct ErrorAckMessage
{
   struct MessageHeader header;        //!< Message header.
   uint64_t reserved0;                 //!< Reserved to pad to message size.
};

//! Message to report a service is ready.

struct ReadyMessage
{
   struct MessageHeader header;        //!< Message header.
   uint32_t serviceId;                 //!< Unique id given to services.
   uint32_t port;                      //!< using local port for this service (see service value)
};


//! Message to tell a service to terminate.

struct TerminateMessage
{
   struct MessageHeader header;        //!< Message header.
   uint64_t reserved0;                 //!< Reserved to pad to message size.
};

//! Message to acknowledge a terminate request was received.

struct TerminateAckMessage
{
   struct MessageHeader header;        //!< Message header.
   uint64_t reserved0;                 //!< Reserved to pad to message size.
};

//! Message to start the services needed by a compute node.

struct StartNodeServicesMessage
{
   struct MessageHeader header;        //!< Message header.
   uint32_t serviceId;                 //!< Unique id given to services.
   uint32_t CNtorus;                 //!< Reserved to pad to message size.
};


//! Message to allocate a memory region.

struct AllocateRegionMessage
{
   struct MessageHeader header;        //!< Message header.
   uint64_t reserved0;                 //!< Reserved to pad to message size.
};

//! Message to acknowledge allocating a memory region.

struct AllocateRegionAckMessage
{
   struct MessageHeader header;        //!< Message header.
   uint32_t regionId;                  //!< Id of allocated memory region.
   uint32_t reserved0;                 //!< Reserved to pad to message size.
};

//! Message to release a memory region.

struct ReleaseRegionMessage
{
   struct MessageHeader header;        //!< Message header.
   uint32_t regionId;                  //!< Id of allocated memory region.
   uint32_t reserved0;                 //!< Reserved to pad to message size.
};

//! Message to acknowledge releasing a memory region.

struct ReleaseRegionAckMessage
{
   struct MessageHeader header;        //!< Message header.
   uint64_t reserved0;                 //!< Reserved to pad to message size.
};

//! Message to interrupt an outstanding operation.

struct InterruptMessage
{
   struct MessageHeader header;        //!< Message header.
   int signo;                          //!< Signal number that interrupted operation.
   uint32_t reserved0;                 //!< Reserved to pad to message size.
};


} // namespace iosctl

} // namespace bgcios

#endif // IOSCTLMESSAGES_H


