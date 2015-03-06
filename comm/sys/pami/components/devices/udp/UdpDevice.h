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
 * \file components/devices/udp/UdpDevice.h
 * \brief ???
 */

#ifndef __components_devices_udp_UdpDevice_h__
#define __components_devices_udp_UdpDevice_h__

#include "components/devices/BaseDevice.h"
#include "components/devices/FactoryInterface.h"
#include "components/devices/PacketInterface.h"
#include "components/devices/udp/UdpMessage.h"
#include "components/devices/udp/UdpSndConnection.h"
#include "components/devices/udp/UdpRcvConnection.h"
#include <map>
#include <list>
#include "util/ccmi_debug.h"
#include "trace.h"

#define DISPATCH_SET_SIZE 256
namespace PAMI
{
  namespace Device
  {
    namespace UDP
    {
    typedef struct udp_dispatch_info_t
    {
      Interface::RecvFunction_t  direct_recv_func;
      void                      *direct_recv_func_parm;
    }udp_dispatch_info_t;

#warning The template parameter T_SysDep is redundant
    template <class T_SysDep>
    class UdpDevice : public Interface::BaseDevice<UdpDevice<T_SysDep> >,
                      public Interface::PacketDevice<UdpDevice<T_SysDep> >
    {
    public:
      static const size_t packet_payload_size = 224;

        // Inner factory class
        class Factory : public Interface::FactoryInterface<Factory, UdpDevice, PAMI::Device::Generic::Device>
        {
          public:
            static inline UdpDevice * generate_impl (size_t clientid, size_t n, Memory::MemoryManager & mm, PAMI::Device::Generic::Device *gds)
            {
              if ( __global.mapping.activateUdp() != PAMI_SUCCESS ) abort();

              // Allocate an array of udp devices, one for each context in this
              // _task_ (from heap, not from shared memory)
              UdpDevice * devices;
              pami_result_t rc;
              rc = __global.heap_mm->memalign((void **)&devices, 16, sizeof(*devices) * n);
              PAMI_assertf(rc == PAMI_SUCCESS, "alloc failed for UdpDevice[%zu], errno=%d\n", n, errno);

              // Instantiate the udp devices
              size_t i;
              for (i = 0; i < n; ++i)
                {
                  new (&devices[i]) UdpDevice (clientid, n, i);
                }

              TRACE_ERR((stderr, "<< UdpDevice::Factory::generate_impl()\n"));
              return devices;
            };

            static inline pami_result_t init_impl (UdpDevice    * devices,
                                                  size_t         clientid,
                                                  size_t         contextid,
                                                  pami_client_t   client,
                                                  pami_context_t  context,
                                                  Memory::MemoryManager *mm,
                                                  PAMI::Device::Generic::Device * progress)
            {
              return getDevice_impl(devices, clientid, contextid).init (clientid, contextid, client, context, mm, progress);
            };

            static inline size_t advance_impl (UdpDevice * devices,
                                               size_t      clientid,
                                               size_t      contextid)
            {
              return getDevice_impl(devices, clientid, contextid).advance ();
            };

            static inline UdpDevice & getDevice_impl (UdpDevice * devices,
                                                      size_t        clientid,
                                                      size_t        contextid)
            {
              return devices[contextid];
            };
         };



      inline UdpDevice (size_t clientid, size_t ncontexts, size_t contextid) :
      Interface::BaseDevice<UdpDevice<T_SysDep> > (),
      Interface::PacketDevice<UdpDevice<T_SysDep> >(),
      _clientid (clientid),
      _ncontexts (ncontexts),
      _contextid (contextid)
      {
        TRACE_ADAPTOR((stderr,"<%#.8X>UdpDevice()\n",(int)this));
      };

      // Implement BaseDevice Routines

      inline ~UdpDevice () {
        if ( __global.mapping.isUdpActive() )
        {
         __global.mapping.deactivateUdp();
        }
      }

      inline pami_result_t setDispatchFunc (size_t                      dispatch,
                                  Interface::RecvFunction_t   direct_recv_func,
                                  void                      * direct_recv_func_parm,
                                  size_t & device_dispatch_id )
      {
         unsigned i;
          for (i=0; i<DISPATCH_SET_SIZE; i++)
          {
            unsigned id = dispatch * DISPATCH_SET_SIZE + i;
            if (_dispatch_table[id].direct_recv_func == NULL)
            {
              TRACE_COUT( "dispatch table ["<<id<<"].direct_recv_func being set to " << (void*)direct_recv_func )
              _dispatch_table[id].direct_recv_func=direct_recv_func;
              _dispatch_table[id].direct_recv_func_parm=direct_recv_func_parm;
              _dispatch_lookup[id] = _dispatch_table[id];
              device_dispatch_id = id;
              return PAMI_SUCCESS;
            }
          }
          return PAMI_NERROR;
      }

      inline pami_result_t init (size_t          clientid,
                                size_t          contextid,
                                pami_client_t    client,
                                pami_context_t   context,
                                Memory::MemoryManager *mm,
                                PAMI::Device::Generic::Device * progress)
      {
        init_impl (mm, context);
        return PAMI_SUCCESS;
      };

      inline int init_impl (Memory::MemoryManager *mm,
                            pami_context_t   context)
      {
        _context   = context;

	pami_result_t rc;
	rc = __global.heap_mm->memalign((void **)&_sndConnections, 0,
				__global.mapping.size() *  sizeof(*_sndConnections));
	PAMI_assertf(rc == PAMI_SUCCESS, "alloc of _sndConnections failed");
        // setup the connections
        size_t i;
        // Advance all the send UDP sockets
        for (i=0; i<__global.mapping.size(); i++ )
        {
           _sndConnections[i] = new UdpSndConnection( i );
        }

        _rcvConnection = new UdpRcvConnection( );

        return PAMI_SUCCESS;
      };

      inline pami_context_t getContext_impl ()
      {
        return _context;
      };

      inline size_t getContextOffset ()
      {
        return _contextid;
      };

      inline bool isInit_impl ()
      {
        return __global.mapping.isUdpActive();
      };

      inline int advance ()
      {
        static int dbg = 1;

        if(dbg) {
          TRACE_ADAPTOR((stderr,"<%#.8X>UdpDevice::advance_impl\n",(int)this));
          dbg = 0;
        }

        size_t i;
        // Advance all the send UDP sockets
        for (i=0; i<__global.mapping.size(); i++ )
        {
           _sndConnections[i]->advance();
        }

        if ( _rcvConnection->advance() == 0 )
        {
          // Packet received, so dispatch it
          Interface::RecvFunction_t rcvFun = _dispatch_table[_rcvConnection->getDeviceDispatchId()].direct_recv_func;
          if ( rcvFun != NULL )   // Ignoring stuff we don't expect, since we are UDP
          {
             TRACE_COUT( "Dispatching to _dispatch_table["<<_rcvConnection->getDeviceDispatchId()<<"] = "<< (void*)rcvFun )

             rcvFun( _rcvConnection->_msg.getMetadataAddr(),
                     _rcvConnection->_msg.getPayloadAddr(),
                     _rcvConnection->_msg.getPayloadSize(),
                     _dispatch_table[_rcvConnection->getDeviceDispatchId()].direct_recv_func_parm,
                     NULL );
          } else {
            TRACE_COUT( "Receive function NULL!  _dispatch_table["<<_rcvConnection->getDeviceDispatchId()<<"]" )
          }
        }

        return PAMI_SUCCESS;

      };

      // Implement MessageDevice Routines
      static const size_t metadata_size = 0;  // TODO need to tune these   WAS 128  or 16
      static const size_t payload_size  = 224;

      // Implement Packet Device Routines
      inline int    read_impl(void * dst, size_t bytes, void * cookie)
      {
        return PAMI_UNIMPL;
      }

      inline size_t peers_impl ()
      {
        return __global.mapping.size();
      }

      inline size_t task2peer_impl (size_t task)
      {
        PAMI_assert(task < __global.mapping.size());
        return task;
      }
      inline void post(size_t task, UdpSendMessage* msg)
      {
#warning need to enable for endpoints -> task+contextid
        _sndConnections[task]->enqueueMsg(msg);
      }

      size_t                                    _dispatch_id;
      UdpSndConnection                       ** _sndConnections;
      UdpRcvConnection			      * _rcvConnection;
      std::map<int, udp_dispatch_info_t>        _dispatch_lookup;
      udp_dispatch_info_t                       _dispatch_table[256*DISPATCH_SET_SIZE];
      pami_context_t _context;
      size_t _clientid;
      size_t _ncontexts;
      size_t _contextid;
    };
  };
   };
};
#endif // __components_devices_upd_upddevice_h__
