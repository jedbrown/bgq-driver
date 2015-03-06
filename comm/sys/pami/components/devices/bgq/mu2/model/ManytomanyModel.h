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
 * \file components/devices/bgq/mu2/model/ManytomanyModel.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_model_ManytomanyModel_h__
#define __components_devices_bgq_mu2_model_ManytomanyModel_h__

#include <hwi/include/bqc/MU_PacketHeader.h>

#include <spi/include/mu/DescriptorBaseXX.h>
#include <spi/include/mu/DescriptorWrapperXX.h>
#include <spi/include/mu/Pt2PtMemoryFIFODescriptorXX.h>
#include <spi/include/kernel/memory.h>

#include "components/devices/bgq/mu2/Context.h"
#include "components/devices/bgq/mu2/msg/InjectAMMulticast.h"
#include "math/Memcpy.x.h"

#include <pami.h>
#include "components/devices/ManytomanyModel.h"
#include "components/devices/bgq/mu2/msg/InjectAMManytomany.h"
#include "M2MPipeWorkQueue.h"

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
/*
      static const size_t m2m_state_bytes = sizeof(InjectAMManytomany<size_t, 0>); 
      class ManytomanyModel : public Interface::ManytomanyModel<ManytomanyModel<T_Int, T_Single>, MU::Context, m2m_state_bytes>
*/
      template <typename T_Int, bool T_Single>
      class M2MState
      {
        public:
        static const size_t bytes = sizeof(InjectAMManytomany<T_Int,T_Single>); 
      };

      template <typename T_Int, bool T_Single>
      class ManytomanyModel : public Interface::ManytomanyModel<ManytomanyModel<T_Int, T_Single>, MU::Context, M2MState<T_Int, T_Single>::bytes>
      {
      protected:
        static const uint64_t AnyFIFOMap =    
        MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM |
        MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP | 
        MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BM |
        MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BP | 
        MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CM |
        MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CP |
        MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM |
        MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP |
        MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM |
        MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP;

        class M2mRecv
        {
        public:
          M2MPipeWorkQueueT<T_Int, T_Single> * _pwq;
          pami_callback_t                      _cb_done;
          bool                                 _init; 
          uint32_t                             _n_incomplete;

          M2mRecv (): _init(false)
          {
          }

          bool initialized ()
          {
            return _init;
          }

          void initialize (pami_context_t                       ctxt,   
                           pami_dispatch_manytomany_function    recv_func, 
                           void                               * async_arg, 
                           unsigned                             connid);

          bool processPacket (uint32_t         srcidx,
                              size_t           poffset,
                              size_t           pbytes,
                              void           * payload)
          {
            size_t bytes_produced = _pwq->getBytesProduced(srcidx);
            //Get the start of the buffer and then add packet offset to it
            char * buf = (char *)_pwq->getBufferBase(srcidx) + poffset;
            //Compute the total bytes expected for this src idx
            size_t total_bytes = _pwq->getTotalBytes(srcidx);
            //Is this the last packet?
            size_t is_complete = (total_bytes - bytes_produced - pbytes - 1) >> 63;
            _n_incomplete  -= is_complete;
            //Compute how many bytes can be copied 
            size_t bytes = total_bytes - poffset;
            //Core_memcpy (buf, payload, bytes);

            if (bytes >= 512) {
	      _pwq->produceBytes(srcidx, 512);            
	      bytes = 512;
	      if(likely(((uint64_t)buf & 0x1F) == 0)) {
		quad_copy_512(buf, (char *)payload);
		//pull in the next packet
		char *nextp = (char *)payload + 512;
		muspi_dcbt(nextp, 0);
		muspi_dcbt(nextp, 64);
		muspi_dcbt(nextp, 128);
		muspi_dcbt(nextp, 192);
		muspi_dcbt(nextp, 256);
		muspi_dcbt(nextp, 320);
		muspi_dcbt(nextp, 384);
		muspi_dcbt(nextp, 448);		
	      }
	      else
		memcpy (buf, payload, 512);
	    }
            else {
	      _pwq->produceBytes(srcidx, bytes);            
	      //bytes left less then 512 we only have a single packet
              _int64Cpy(buf, (char *)payload, bytes);
	    }
	    
	    
            //printf ("Process Packet from src %d bytes avail %ld packet bytes %ld\n", srcidx, bytes_available, pbytes);
            return (_n_incomplete == 0);
          }

	  void processCompletion (pami_context_t   ctxt) {
	    _init = false;
	    if (likely(_cb_done.function != NULL))
	      _cb_done.function (ctxt, _cb_done.clientdata, PAMI_SUCCESS);
	  }

        };

      public:
        static const size_t sizeof_msg = M2MState<T_Int, T_Single>::bytes;

        ManytomanyModel (pami_client_t client, pami_context_t context, MU::Context &device, pami_result_t &status) :  
        Interface::ManytomanyModel<ManytomanyModel<T_Int, T_Single>, MU::Context, M2MState<T_Int, T_Single>::bytes> (device, status),
        _mucontext(device),
        _cached_connid ((uint32_t)-1),
        _cached_recv (NULL),
        _client(client),
        _ctxt(context)
        {
          initDesc();
          status = PAMI_SUCCESS;
        }

        ~ManytomanyModel<T_Int, T_Single> ()
        {
        }

        void initDesc ()
        {
          // Zero-out the descriptor models before initialization
          memset((void *)&_modeldesc, 0, sizeof(MUSPI_DescriptorBase));

          // --------------------------------------------------------------------
          // Set the common base descriptor fields
          // --------------------------------------------------------------------
          MUSPI_BaseDescriptorInfoFields_t base;
          memset((void *)&base, 0, sizeof(base));

          base.Pre_Fetch_Only  = MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO;
          base.Payload_Address = 0;
          base.Message_Length  = 0;
          base.Torus_FIFO_Map  = AnyFIFOMap;
          base.Dest.Destination.Destination = 0;

          _modeldesc.setBaseFields (&base);

          // --------------------------------------------------------------------
          // Set the common point-to-point descriptor fields
          // --------------------------------------------------------------------
          MUSPI_Pt2PtDescriptorInfoFields_t pt2pt;
          memset((void *)&pt2pt, 0, sizeof(pt2pt));

          pt2pt.Hints_ABCD = 0;
          pt2pt.Skip       = 8;

	  uint32_t routing = MUHWI_PACKET_USE_DYNAMIC_ROUTING;
	  uint32_t zone = MUHWI_PACKET_ZONE_ROUTING_1;
	  uint32_t vc = MUHWI_PACKET_VIRTUAL_CHANNEL_DYNAMIC;
	  //Check for DD1 (set routing to deterministic in DD1)

	  if ((__global.mapping.size() / __global.mapping.tSize()) > 512)
	    zone = MUHWI_PACKET_ZONE_ROUTING_0;

	  bool isDD2 = true;
	  // Determine if this is running on DD2 hardware
	  uint32_t pvr; // Processor version register
	  int rc;
	  rc = Kernel_GetPVR( &pvr );
	  assert(rc==0);
	  if ( pvr == SPRN_PVR_DD1 )
	    isDD2 = false;
	  else
	    isDD2 = true;

	  if (!isDD2) {
	    //printf("DD1 Hardware: use deterministic routing for M2M\n");
	    zone = 0;
	    routing = MUHWI_PACKET_USE_DETERMINISTIC_ROUTING;
	    vc = MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC;
	  }
	  else { //DD2
	    char *key = getenv ("PAMI_M2M_ROUTING");
	    if(key != NULL && strcasecmp(key, "DETERMINISTIC") == 0){
	      zone = 0;
	      routing = MUHWI_PACKET_USE_DETERMINISTIC_ROUTING;
	      vc = MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC;    
	    }
	    else {
	      key = getenv ("PAMI_M2M_ZONE");
	      if(key != NULL && strcasecmp(key, "0") == 0)
		zone = MUHWI_PACKET_ZONE_ROUTING_0;
	      else if(key != NULL && strcasecmp(key, "1") == 0)
		zone = MUHWI_PACKET_ZONE_ROUTING_1;
	      else if(key != NULL && strcasecmp(key, "2") == 0)
		zone = MUHWI_PACKET_ZONE_ROUTING_2;
	      else if(key != NULL && strcasecmp(key, "3") == 0)
		zone = MUHWI_PACKET_ZONE_ROUTING_3;
	    }
	  }

	  char *key = getenv("PAMI_VERBOSE");
	  if (__global.mapping.task() == 0)
	    if ( key != NULL && (strcmp(key, "0")!=0) ) 
	      printf ("Configure manytomany with routing %s zone %d\n",
		      (routing == MUHWI_PACKET_USE_DETERMINISTIC_ROUTING)? "DETERMINISTIC":"DYNAMIC",
		    zone>>3 /*convert zone to [0-3]*/);
	  
          pt2pt.Misc1 = routing |
	    MUHWI_PACKET_DO_NOT_DEPOSIT |
	    MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE;
	  
          pt2pt.Misc2 = vc | zone; 
          _modeldesc.setPt2PtFields (&pt2pt);
          _modeldesc.setDataPacketType (MUHWI_PT2PT_DATA_PACKET_TYPE);
          _modeldesc.PacketHeader.NetworkHeader.pt2pt.Byte8.Size = 16;

          // ---------------------------------------------------------
          // Set the common memory fifo descriptor fields
          // ---------------------------------------------------------
          MUSPI_MemoryFIFODescriptorInfoFields_t memfifo;
          memset ((void *)&memfifo, 0, sizeof(memfifo));

          memfifo.Rec_FIFO_Id    = 0;
          memfifo.Rec_Put_Offset = 0;
          memfifo.Interrupt      = 0; //MUHWI_DESCRIPTOR_INTERRUPT_ON_PACKET_ARRIVAL;
          memfifo.SoftwareBit    = 0;

          _modeldesc.setMemoryFIFOFields (&memfifo);
          _modeldesc.setMessageUnitPacketType (MUHWI_PACKET_TYPE_FIFO);

          // --------------------------------------------------------------------
          // Set the network header information in the descriptor models to
          // differentiate between a single-packet transfer and a multi-packet
          // transfer
          // --------------------------------------------------------------------
          MemoryFifoPacketHeader * hdr = NULL;    
          hdr = (MemoryFifoPacketHeader *) & _modeldesc.PacketHeader;
          hdr->setSinglePacket (false); 

          //MUSPI_DescriptorDumpHex ((char *)"Model", &_modeldesc);
        } 

        void *getRecvFromMap (uint32_t connid)  __attribute__((__noinline__, weak));

        M2mRecv *getRecvObject (uint32_t connid)
        {
          if (unlikely(connid != _cached_connid))
          {
            _cached_connid = connid;
            _cached_recv   = (M2mRecv *)getRecvFromMap(connid);
          }

          PAMI_assert (_cached_recv != NULL);
          return _cached_recv;
        }

        pami_result_t registerManytomanyRecvFunction_impl(int dispatch_id,
                                                          pami_dispatch_manytomany_function recv_func,
                                                          void  *async_arg)
        {
          // Implementation
          _dispatch        = dispatch_id;
          _recv_func       = recv_func;
          _async_arg       = async_arg;

          uint16_t id = 0;
          if (_mucontext.registerPacketHandler (_dispatch,
                                                dispatch_func,
                                                this,
                                                id))
          {
            MemoryFifoPacketHeader * hdr = NULL;        
            hdr = (MemoryFifoPacketHeader *) & _modeldesc.PacketHeader;
            hdr->setDispatchId (id);   
            return PAMI_SUCCESS;
          }

          return PAMI_ERROR;
        } 

        static int  dispatch_func (void   * metadata,
                                   void   * payload,
                                   size_t   bytes,
                                   void   * recv_func_parm,
                                   void   * cookie) 
        {
          ManytomanyModel<T_Int, T_Single> *model = (ManytomanyModel<T_Int, T_Single> *) recv_func_parm;
          M2mHdr *amhdr = (M2mHdr *) metadata;
          unsigned connid    =  amhdr->connid;
          M2mRecv *recv =  model->getRecvObject(connid); 
          //PAMI_assert (model != NULL);
          //PAMI_assert (model->_recv_func != NULL);

          MUHWI_PacketHeader_t  * hdr = (MUHWI_PacketHeader_t *) ((char*)cookie - 32);
          //We only support an alltoall 2GB in size per destination
          size_t offset = hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_LSB;

          bool done = recv->processPacket (amhdr->srcidx, offset, bytes, payload);    
	  if(unlikely(done)) {
	    recv->processCompletion(model->_ctxt);
	    model->_cached_connid = (unsigned) -1;
	  }

          return PAMI_SUCCESS;
        }   

        pami_result_t postManytomany_impl(uint8_t (&state)[M2MState<T_Int, T_Single>::bytes],
                                          pami_manytomany_t *m2minfo,
                                          void              *devinfo)
        {
          // Implementation
          //Check size and construct shortm2m msg	  
          // else
          // call multipacket alltoall msg
          Topology *topo = m2minfo->send.participants;    

          uint32_t srcidx = topo->rank2Index(__global.mapping.task());
          InjectAMManytomany<T_Int, T_Single> *msg = new (state) InjectAMManytomany<T_Int, T_Single> 
                                                     (_mucontext, 
                                                      topo,
                                                      (M2MPipeWorkQueueT<T_Int, T_Single> *)m2minfo->send.buffer, 
                                                      m2minfo->cb_done.function, 
                                                      m2minfo->cb_done.clientdata,
                                                      m2minfo->connection_id, 
                                                      _modeldesc,
                                                      srcidx,
                                                      _ctxt);

          //Advance message
          bool done = msg->advance();
          if (!done)
          {
            //Post work to generic device
            pami_work_t *wobj = msg->workobj();
            PAMI::Device::Generic::GenericThread *work = new (wobj) PAMI::Device::Generic::GenericThread (advance, msg);
            _mucontext.getProgressDevice()->postThread(work);
          }

          return PAMI_SUCCESS;
        }

        static pami_result_t advance (pami_context_t     context,
                                      void             * cookie)
        {
          InjectAMManytomany<T_Int, T_Single> *msg = (InjectAMManytomany<T_Int, T_Single> *) cookie;        
          if (msg->advance())
            return PAMI_SUCCESS;

          return PAMI_EAGAIN;
        }

      protected:
        MUSPI_DescriptorBase                    _modeldesc;
        MU::Context                           & _mucontext; 
        uint32_t                                _cached_connid;
        uint32_t                                _dispatch;
        M2mRecv                               * _cached_recv;
        pami_dispatch_manytomany_function       _recv_func;
        void                                  * _async_arg;
        pami_client_t                           _client;
        pami_context_t                          _ctxt;
        std::map<int, M2mRecv>                  _recvmap; 
      }; // class ManytomanyModel

      template <typename T_Int, bool T_Single>
      void *ManytomanyModel<T_Int, T_Single>::getRecvFromMap(uint32_t connid)
      {
	M2mRecv *recv =  (M2mRecv *)&_recvmap[connid];

	if (unlikely(!recv->initialized()))
	  recv->initialize (_ctxt, _recv_func, _async_arg, connid);

        return recv;
      }

      template <typename T_Int, bool T_Single>
      void ManytomanyModel<T_Int, T_Single>::M2mRecv::initialize (pami_context_t                       ctxt,  
                                                                  pami_dispatch_manytomany_function    recv_func, 
                                                                  void                               * async_arg, 
                                                                  unsigned                             connid) 
      {
        _init = true;
        pami_manytomanybuf_t *rbuf;
        recv_func (ctxt,
                   async_arg,
                   connid,
                   NULL,
                   0,
                   &rbuf,
                   &_cb_done);    
        _pwq = (M2MPipeWorkQueueT<T_Int, T_Single> *) rbuf->buffer;
	_n_incomplete = _pwq->numActive();
      }

    };
  };
};
#endif // __components_device_manytomanymodel_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
