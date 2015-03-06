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
 * \file components/devices/udp/UdpMessage.h
 * \brief ???
 */

#ifndef __components_devices_udp_UdpMessage_h__
#define __components_devices_udp_UdpMessage_h__

#include <pami.h>
#include "util/common.h"
#include "util/queue/Queue.h"
#include <stdint.h>
#include "trace.h"
//#include <netinet/in.h>
//#include <sys/socket.h>

namespace PAMI
{
  namespace Device
  {
   namespace UDP
   {
    struct UdpMsg
    {
       uint32_t            _device_dispatch_id;
       uint32_t            _metadata_size;
       uint32_t            _payload_size;
       char                _var_data[352];  // Metadata and payload

       UdpMsg( uint32_t device_dispatch_id, void * metadata, uint32_t metadatasize )
       {
          TRACE_COUT( "device_dispatch_id = " << device_dispatch_id )
          _device_dispatch_id = htonl(device_dispatch_id);
          _metadata_size = htonl(metadatasize);
          _payload_size = 0;
         memcpy( _var_data, metadata, metadatasize );
       }

       UdpMsg()
       {
       }

       inline size_t getSendSize()
       {
         //size_t tmp = sizeof(UdpMsg);
         size_t tmp = sizeof(_device_dispatch_id)
                  + sizeof(_metadata_size)
                  + sizeof(_payload_size)
                  + getMetadataSize()
                  + getPayloadSize() ;
         TRACE_COUT( "message send size = " << tmp )
         return tmp;
      }

       inline void setPayloadSize( size_t payload_size)
        {
          //TRACE_COUT( "Setting payload size to " << payload_size )
          _payload_size = htonl(payload_size);
        }

        inline uint32_t getDeviceDispatchId()
        {
          return ntohl(_device_dispatch_id);
        }

        inline size_t getMetadataSize()
        {
          //TRACE_COUT( "Getting metadata size =  " << ntohl(_metadata_size) )
          return ntohl(_metadata_size);
        }

        inline size_t getPayloadSize()
        {
          //TRACE_COUT( "Getting payload size =  " << ntohl(_payload_size) )
          return ntohl(_payload_size);
        }

        inline void * getMetadataAddr()
        {
          //TRACE_COUT( "Getting metadata addr = " <<  (void*)_var_data )
          return _var_data;
        }

        inline void * getPayloadAddr()
        {
          //TRACE_COUT( "Getting payload addr = " << (void *)( (uintptr_t)_var_data+getMetadataSize() ) )
          return (void *)( (uintptr_t)_var_data+getMetadataSize() );
        }

       inline void print()
       {
          TRACE_COUT( "Msg: dispatch_id = " << getDeviceDispatchId() <<
                      " meta size = " << getMetadataSize() <<
                      " pay size = " << getPayloadSize() )
          DUMP_HEX_DATA( getMetadataAddr(), getMetadataSize() );
          DUMP_HEX_DATA( getPayloadAddr(), getPayloadSize() );
       }

    };

    class UdpSendMessage
    {
      public:
        inline UdpSendMessage (pami_context_t               context,
                            pami_event_function   fn,
                            void               * cookie,
                            uint32_t             device_dispatch_id,
                            void               * metadata,
                            size_t               metasize,
                            struct iovec       * iov,
                            size_t               niov,
                            bool                 single_pkt) :
          _msg( device_dispatch_id, metadata, metasize ),
          _context (context),
          _done_fn (fn),
          _cookie (cookie),
          _iov(iov),
          _niov(niov),
          _single_pkt(single_pkt),
          _offset(0),
          _on_idx(0),
          _complete(false)
        {
          TRACE_COUT( "Creating message" )
          TRACE_COUT( "   dispatch id = " << device_dispatch_id << " metadata len = " << metasize )
          TRACE_COUT( "   niov = " << niov )
          for ( int kk=0; kk < niov; kk++ )
          {
             TRACE_COUT( "     iov["<<kk<<"].iov_base = " << iov[kk].iov_base <<
                         "     iov["<<kk<<"].iov_len = " << iov[kk].iov_len )
          }
          size_t left = 352 - metasize;    // TODO need to make size variable
          void * insert_pt = (void *)((uintptr_t)_msg._var_data+metasize);
          size_t i, amount;
          TRACE_COUT( "  amount = " << amount << " insert_pt = " << insert_pt << " _msg._var_data = " << (void *)_msg._var_data << "  metasize = " << metasize )
          for ( i=0; i<niov; i++ )
          {
             TRACE_COUT( "   i = " << i << " left = " << left << " amount = " << amount << " _offset = " << _offset << " _on_idx = " << _on_idx << " insert_pt = " << insert_pt << " iov[i].iov_base = " << iov[i].iov_base << " iov[i].iov_len = " << iov[i].iov_len )
             amount = iov[i].iov_len;
             if (left - amount < 0 ) {
               TRACE_COUT( " Can't fit the whole thing: left = " << left << " and amount = " << amount )
               _offset = left;
               amount = left;
               _on_idx = i;
             }
                          TRACE_COUT( "   i = " << i << " left = " << left << " amount = " << amount << " _offset = " << _offset << " _on_idx = " << _on_idx << " insert_pt = " << insert_pt << " iov[i].iov_base = " << iov[i].iov_base << " iov[i].iov_len = " << iov[i].iov_len )
             memcpy( insert_pt, iov[i].iov_base, amount );
             insert_pt = (void *)((uintptr_t)insert_pt + amount);
             left -= amount;
             if ( left == 0 ) break;
          }

          _msg.setPayloadSize( (352 - metasize) - left );

          if ( _single_pkt == true || ( _offset == 0 && _on_idx == 0 ) )
          {
            _complete = true;
            TRACE_COUT( "Completed   _offset = " << _offset << "   _on_idx = " << _on_idx )
          }
        }

        inline void nextPacket()
        {
          // if no more to do return true;
          if (_complete == true ) return;  // should I abort?
          TRACE_COUT( "Doing the next packet of a multipacket" )
          size_t metasize = _msg.getMetadataSize();
          size_t left = 352 - metasize;    // TODO need to make size variable
          void * insert_pt = (void *)((uintptr_t)_msg._var_data+metasize);
          void * take_from;
          size_t i, amount;
          for ( i=_on_idx; i<_niov; i++ )
          {
             amount = _iov[i].iov_len-_offset;
             take_from = (void*)((uintptr_t)_iov[i].iov_base+_offset);
             if (left - amount < 0 ) {
               // Can't fit the whole thing
               _offset = left;
               amount = left;
               _on_idx = i;
             } else {
               _offset = 0;
               _on_idx = 0;
             }
             memcpy( insert_pt, take_from, amount );
             insert_pt = (void *)((uintptr_t)insert_pt + amount);
             left -= amount;
             if ( left == 0 ) break;
          }

         _msg.setPayloadSize( (352 - metasize) - left );

          if ( _offset == 0 && _on_idx == 0 )
          {
            _complete = true;
            TRACE_COUT( "Completed" )
          }
        }


        inline void done()
        {
           if (_done_fn != NULL )
           {
              _done_fn(_context,_cookie,PAMI_SUCCESS);
           }
        }

       inline void setPayloadSize( size_t payload_size)
        {
          _msg._payload_size = htonl(payload_size);
        }

        inline void * getAddr()
        {
          return &_msg;
        }

        inline size_t getSendSize()
        {
          return _msg.getSendSize();
        }

        inline uint32_t getDeviceDispatchId()
        {
          return ntohl(_msg._device_dispatch_id);
        }

        inline size_t getMetadataSize()
        {
          return ntohl(_msg._metadata_size);
        }

        inline size_t getPayloadSize()
        {
          return ntohl(_msg._payload_size);
        }

        UdpMsg              _msg;
        pami_context_t       _context;
        pami_event_function  _done_fn;
        void              * _cookie;
        void              * _metadata;
        struct iovec      * _iov;
        size_t              _niov;
        bool                _single_pkt;
        size_t              _offset;
        size_t              _on_idx;
        bool                _complete;

      protected:
      private:
    };
   };
  };
};

#endif // __components_devices_udp_udpbasemessage_h__
