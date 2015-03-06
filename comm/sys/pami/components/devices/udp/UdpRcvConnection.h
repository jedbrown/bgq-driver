/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file components/devices/udp/UdpRcvConnection.h
 * \brief ???
 */

#ifndef __components_devices_udp_UdpRcvConnection_h__
#define __components_devices_udp_UdpRcvConnection_h__

#include "components/devices/udp/UdpMessage.h"
#include "util/ccmi_debug.h"
#include "trace.h"

#define IOV_MAX 256  // TODO WHat is the right value for this?

#define DISPATCH_SET_SIZE 256
namespace PAMI
{
  namespace Device
  {
  namespace UDP
  {
    class UdpRcvConnection
    {
    public:

      UdpRcvConnection():
       _msg()
      {
        __global.mapping.udpAddr( _rcvFd );
      }

      inline int advance()
      {
         struct sockaddr_storage _their_addr;
         socklen_t               _their_addr_len = sizeof(_their_addr);
         int bytes_rcv = recvfrom(_rcvFd, &_msg, sizeof(_msg), MSG_DONTWAIT, (struct sockaddr*)&_their_addr, &_their_addr_len);
         if ( bytes_rcv == -1 && errno == EWOULDBLOCK ) return -1;
         if ( bytes_rcv == -1 )
         {
           abort();
         }

         TRACE_COUT( "Packet received.  Bytes = " << bytes_rcv )
         _msg.print();
         // All of the packet is available
         return 0;
      }

      inline uint32_t getDeviceDispatchId()
      {
         return _msg.getDeviceDispatchId();
      }

      int                     _rcvFd;     // UDP socket for this Connection

      UdpMsg                  _msg;
    protected:
    private:

     };
    };
  };
};
#endif // __components_devices_udp_udprcvconnection_h__
