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
///
/// \file components/devices/bgq/mu2/model/AMMulticastModel.h
/// \brief ???
///

#ifndef __components_devices_bgq_mu2_model_AMMulticastModel_h__
#define __components_devices_bgq_mu2_model_AMMulticastModel_h__

#include <hwi/include/bqc/MU_PacketHeader.h>

#include <spi/include/mu/DescriptorBaseXX.h>
#include <spi/include/mu/DescriptorWrapperXX.h>
#include <spi/include/mu/Pt2PtMemoryFIFODescriptorXX.h>
#include <spi/include/kernel/memory.h>

#include "components/devices/MulticastModel.h"
#include "components/devices/bgq/mu2/Context.h"
#include "components/devices/bgq/mu2/msg/InjectAMMulticast.h"
#include "math/a2qpx/Core_memcpy.h"

#include "util/trace.h"
#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI {
  namespace Device {
    namespace MU {

      class ShortAMMulticastModel: public Interface::AMMulticastModel<ShortAMMulticastModel, MU::Context, sizeof(InjectAMMulticast)>
      {
      protected:
	struct AMMulticastHdr {
	  uint32_t     root;
	  uint16_t     bytes;        /*As this is a short protocol, bytes is 16 bit*/
	  uint16_t     metasize;
	  uint64_t     connection_id; /* Permit a 64 bit connection id */
	};

	static const size_t SC_MAXRANKS  = 128; // arbitrarily based on CCMI::Executor::ScheduleCache::SC_MAXRANKS
      public:

	static const unsigned sizeof_msg      = sizeof(InjectAMMulticast);
	static const size_t   immediate_bytes = 64; //MU::Context::immediate_payload_size;
	static const unsigned channel_id = 0;
	static const uint64_t alignment  = 32UL;
	
	ShortAMMulticastModel (pami_client_t    client,
			       pami_context_t   context,
			       MU::Context    & mucontext,
			       pami_result_t  & status):
	Interface::AMMulticastModel<ShortAMMulticastModel, MU::Context, sizeof(InjectAMMulticast)> (mucontext, status),
	  _mucontext(mucontext),
	  _channel(_mucontext.injectionGroup.channel[channel_id]),
	  _myrank(__global.mapping.task()),
	  _client (client),
	  _ctxt (context)
	  {
	    status = PAMI_ERROR;
	    if (__global.useMU())
	      status = init();
	  }
       
	~ShortAMMulticastModel () {}

	pami_result_t init () 
	{
	  // Zero-out the descriptor models before initialization
	  memset((void *)&_singlepkt, 0, sizeof(MUSPI_DescriptorBase));
	  
	  // --------------------------------------------------------------------
	  // Set the common base descriptor fields
	  // --------------------------------------------------------------------
	  MUSPI_BaseDescriptorInfoFields_t base;
	  memset((void *)&base, 0, sizeof(base));

	  base.Pre_Fetch_Only  = MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO;
	  base.Payload_Address = 0;
	  base.Message_Length  = 0;
	  base.Torus_FIFO_Map  = 0;
	  base.Dest.Destination.Destination = 0;

	  _singlepkt.setBaseFields (&base);
	  
	  // --------------------------------------------------------------------
	  // Set the common point-to-point descriptor fields
	  // --------------------------------------------------------------------
	  MUSPI_Pt2PtDescriptorInfoFields_t pt2pt;
	  memset((void *)&pt2pt, 0, sizeof(pt2pt));
	  
	  pt2pt.Hints_ABCD = 0;
	  pt2pt.Skip       = 0;
	  pt2pt.Misc1 =
	    MUHWI_PACKET_USE_DETERMINISTIC_ROUTING |
	    MUHWI_PACKET_DO_NOT_DEPOSIT |
	    MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE;
	  pt2pt.Misc2 =
	    MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC;
	  	 
	  _singlepkt.setDataPacketType (MUHWI_PT2PT_DATA_PACKET_TYPE);
	  _singlepkt.PacketHeader.NetworkHeader.pt2pt.Byte8.Size = 16;
	  _singlepkt.setPt2PtFields (&pt2pt);
	
	  // --------------------------------------------------------------------
	  // Set the common memory fifo descriptor fields
	  // --------------------------------------------------------------------
	  MUSPI_MemoryFIFODescriptorInfoFields_t memfifo;
	  memset ((void *)&memfifo, 0, sizeof(memfifo));
	  
	  memfifo.Rec_FIFO_Id    = 0;
	  memfifo.Rec_Put_Offset = 0;
	  memfifo.Interrupt      = MUHWI_DESCRIPTOR_INTERRUPT_ON_PACKET_ARRIVAL;
	  memfifo.SoftwareBit    = 0;
	  
	  _singlepkt.setMemoryFIFOFields (&memfifo);
	  _singlepkt.setMessageUnitPacketType (MUHWI_PACKET_TYPE_FIFO);

	  //Skip link checksumming the packet header
	  _singlepkt.setPt2PtSkip(8);	

	  // --------------------------------------------------------------------
	  // Set the network header information in the descriptor models to
	  // differentiate between a single-packet transfer and a multi-packet
	  // transfer
	  // --------------------------------------------------------------------
	  MemoryFifoPacketHeader * hdr = NULL;
	  
	  hdr = (MemoryFifoPacketHeader *) & _singlepkt.PacketHeader;
	  hdr->setSinglePacket (true);	  	  	  	  

	  return PAMI_SUCCESS;
	}
	
	pami_result_t registerMcastRecvFunction (int                                dispatch_id,
						 pami_dispatch_multicast_function   recv_func,
						 void                             * async_arg) 
	{
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
	      hdr = (MemoryFifoPacketHeader *) & _singlepkt.PacketHeader;
	      hdr->setDispatchId (id);	 
	      return PAMI_SUCCESS;
	    }
	  
	  fprintf (stderr, "Dispatch Failed\n");
	  
	  return PAMI_ERROR;
	}

	static int  dispatch_func (void   * metadata,
				   void   * payload,
				   size_t   bytes,
				   void   * recv_func_parm,
				   void   * cookie) 
	{	  
	  ShortAMMulticastModel *model = (ShortAMMulticastModel *) recv_func_parm;
	  AMMulticastHdr *amhdr = (AMMulticastHdr *) metadata;

	  PAMI_Callback_t         cb_done = {0};
	  size_t                  rcvlen;
	  pami_pipeworkqueue_t  * pwqptr;
	  
	  //PAMI_assert (model != NULL);
	  //PAMI_assert (model->_recv_func != NULL);

	  model->_recv_func (model->_ctxt,
			     (const pami_quad_t *) payload,
			     amhdr->metasize,
			     amhdr->connection_id,
			     amhdr->root,
			     amhdr->bytes,
			     model->_async_arg,
			     &rcvlen,
			     &pwqptr,
			     &cb_done);

	  //if (rcvlen == 0)
	  //  return PAMI_SUCCESS;
	  
	  if (rcvlen > 0) {
	    PipeWorkQueue *pwq = (PipeWorkQueue *)pwqptr;
	    char *buf = pwq->bufferToProduce();	  
	    char *src = (char *)payload + amhdr->metasize*sizeof(pami_quad_t);
	    memcpy(buf, src, rcvlen);
	    pwq->produceBytes(rcvlen);
	  }

	  if (cb_done.function)
	    cb_done.function(model->_ctxt, cb_done.clientdata, PAMI_SUCCESS);
	  
	  return PAMI_SUCCESS;
	}


	void prepareDesc (MUSPI_DescriptorBase     * desc,
			  uint64_t                   paddr,
			  unsigned                   bytes,
			  unsigned                   metasize,
			  unsigned                   connection_id) 
	{
	  _singlepkt.clone(*desc);
	  // Set the payload information.
	  desc->setPayload (paddr, metasize*sizeof(pami_quad_t) + bytes);
	  
	  MemoryFifoPacketHeader *hdr = (MemoryFifoPacketHeader*) &desc->PacketHeader;
	  AMMulticastHdr amhdr;
	  amhdr.root          = _myrank;
	  amhdr.bytes         = bytes;
	  amhdr.metasize      = metasize;
	  amhdr.connection_id = connection_id;
	  hdr->setMetaData(&amhdr, sizeof(AMMulticastHdr));
	}
	
	pami_result_t postImmediate (pami_task_t            * ranks,
				     size_t                   nranks,
				     char                   * src,
				     size_t                   bytes,
				     const pami_quad_t      * metadata,
				     size_t                   metasize,
				     pami_event_function      cb_done,
				     void                   * cookie,
				     unsigned                 connection_id) __attribute__((noinline, weak));	
	
	pami_result_t postLong (uint8_t               (&state)[sizeof_msg],
													Topology            * topology,
				PipeWorkQueue          * src,
				size_t                   bytes,
				const pami_quad_t      * metadata,
				size_t                   metasize,
				pami_event_function      cb_done,
				void                   * cookie,
				unsigned                 connection_id) __attribute__((noinline, weak));	
	
	pami_result_t postMulticastImmediate_impl(size_t                client,
						  size_t                context, 
						  pami_multicast_t    * mcast,
						  void                * devinfo=NULL) 
	{
	  TRACE_FN_ENTER();

	  /// \todo stop using rank lists directly
	  pami_task_t rank_storage[SC_MAXRANKS];
	  pami_task_t *ranks = rank_storage;
	  Topology *dst_topology = (Topology *)mcast->dst_participants;
	  TRACE_FORMAT("dst_topology %p",dst_topology);
	  size_t nranks = dst_topology->size();
	  dst_topology->list((void**)&ranks);	 // endpoint or ranks, don't care since always context 0

	  CCMI_assert(ranks != NULL);
	  //PAMI_assert(ranks[0] == __global.mapping.task());	  
	  PipeWorkQueue *spwq = (PipeWorkQueue *) mcast->src;
	  char *src = NULL;
	  unsigned sbytes = 0;
	  
	  if (spwq != NULL) {
	    src = spwq->bufferToConsume();
	    sbytes = spwq->bytesAvailableToConsume();
	  }

	  pami_result_t rc = PAMI_ERROR;
	  if (likely(sbytes == mcast->bytes && mcast->bytes <= immediate_bytes))
	  {
	    rc = postImmediate (ranks,
				nranks,
				src,
				mcast->bytes,
				mcast->msginfo,
				mcast->msgcount,
				mcast->cb_done.function,	       
				mcast->cb_done.clientdata,
				mcast->connection_id);	     
	  }

	  TRACE_FN_EXIT();
	  return rc;
	}

	pami_result_t postMulticast_impl(uint8_t               (&state)[sizeof_msg],
					 size_t                client,
					 size_t                context, 
					 pami_multicast_t    * mcast,
					 void                * devinfo=NULL) 
	{
	  /// \todo stop using rank lists directly
	  Topology *dst_topology = (Topology *)mcast->dst_participants;
	  //PAMI_assert(ranks[0] == __global.mapping.task());	  
	  PipeWorkQueue *spwq = (PipeWorkQueue *) mcast->src;
	  
	  //we can only send 512-16 bytes
	  PAMI_assert(mcast->bytes <= 496);

	  return postLong (state,
			   dst_topology,
			   spwq,
			   mcast->bytes,
			   mcast->msginfo,
			   mcast->msgcount,
			   mcast->cb_done.function,	       
			   mcast->cb_done.clientdata,
			   mcast->connection_id);
	}
	
	static pami_result_t advance (pami_context_t     context,
				      void             * cookie) 
	{
	  InjectAMMulticast *msg = (InjectAMMulticast *) cookie;
	  bool done = msg->advance();

	  if (done) return PAMI_SUCCESS;	      
	  return PAMI_EAGAIN;
	}

      protected:
	MUSPI_DescriptorBase                    _singlepkt;
	MU::Context                           & _mucontext;	
	MU::InjChannel                        & _channel;
	pami_dispatch_multicast_function        _recv_func;
	void                                  * _async_arg;
	uint32_t                                _dispatch;
	uint32_t                                _myrank;
	pami_client_t                           _client;
	pami_context_t                          _ctxt;
      };

      pami_result_t ShortAMMulticastModel::postImmediate (pami_task_t            * ranks,
							  size_t                   nranks,
							  char                   * src,
							  size_t                   bytes,
							  const pami_quad_t      * metadata,
							  size_t                   metasize,
							  pami_event_function      cb_done,
							  void                   * cookie,
							  unsigned                 connection_id) 
      {	  	
	//PAMI_assert (nranks > 0);	  	  
	size_t ndesc = _channel.getFreeDescriptorCount();
	
	if (unlikely(ndesc < nranks))
	  return PAMI_ERROR;
	
	MUSPI_DescriptorBase * desc = (MUSPI_DescriptorBase *)_channel.getNextDescriptor ();	  
	void * vaddr;
	uint64_t paddr;	  
	//get the payload of the last descriptor
	_channel.getDescriptorPayload (desc + nranks - 1, vaddr, paddr);	  	  
	prepareDesc (desc, paddr, bytes, metasize, connection_id);
	uint16_t rfifo_base =  *(uint16_t *)(&desc->PacketHeader.messageUnitHeader.Packet_Types.Memory_FIFO);

	//if (likely(metasize > 0)) 
	//_int8Cpy (vaddr, metadata, metasize * sizeof(pami_quad_t));

	if (likely(metasize == 1)) {
	  uint64_t *sp = (uint64_t *) metadata;	    
	  uint64_t *dp = (uint64_t *) vaddr;
	  *dp ++ = *sp ++;
	  *dp    = *sp;
	}
	else if (metasize > 1)
	  memcpy(vaddr, metadata, metasize *sizeof(pami_quad_t)); 

	char *dst = (char*)vaddr + metasize * sizeof(pami_quad_t);
	if (bytes > 0)	  
	  memcpy(dst, src, bytes);
	
	size_t cidx = 0;
	MUHWI_Destination_t   dest;
	uint16_t              rfifo;
	uint64_t              map;
	_mucontext.pinFifo (ranks[cidx],
			    0,
			    dest,
			    rfifo,
			    map);	  
	
	// Initialize the injection fifo descriptor in-place.
	desc->setDestination (dest.Destination.Destination);
	//desc->setRecFIFOId (rfifo);
	*(uint16_t *)(&desc->PacketHeader.messageUnitHeader.Packet_Types.Memory_FIFO) = rfifo_base | (rfifo << 6); 
	desc->setTorusInjectionFIFOMap (map);
	//TRACE_ERR(stderr, "Sending msg from payload pa %lx\n", paddr);
	//MUSPI_DescriptorDumpHex ((char *)"Immediate Multicast", desc);

  register double fp0  FP_REGISTER(0);
  register double fp1  FP_REGISTER(1);
	VECTOR_LOAD_NU (desc,  0, fp0);
	VECTOR_LOAD_NU (desc, 32, fp1);	
	for (cidx = 1; cidx < nranks; cidx++) {
	  MUSPI_DescriptorBase * memfifo = desc + cidx;	    
	  _mucontext.pinFifo (ranks[cidx],
			      0,
			      dest,
			      rfifo,
			      map);

	  VECTOR_STORE_NU (memfifo,  0, fp0);
	  VECTOR_STORE_NU (memfifo, 32, fp1);
	  //desc->clone(*memfifo);	  
	  // Initialize the injection fifo descriptor in-place.
	  memfifo->setDestination (dest.Destination.Destination);
	  //memfifo->setRecFIFOId (rfifo);
	  //Set the reception fifo without a load instruction
	  *(uint16_t *)(&memfifo->PacketHeader.messageUnitHeader.Packet_Types.Memory_FIFO) = rfifo_base | (rfifo << 6); 	  
	  memfifo->setTorusInjectionFIFOMap (map);
	  //MUSPI_DescriptorDumpHex ((char *)"Immediate Multicast", memfifo);
	}
	
	//Advance all the descriptors
	_channel.injFifoAdvanceDescMultiple (nranks);
	if (cb_done)
	  cb_done (_ctxt, cookie, PAMI_SUCCESS);
	
	return PAMI_SUCCESS;
      }
      

      pami_result_t ShortAMMulticastModel::postLong (uint8_t               (&state)[sizeof_msg],
																										 Topology            * topology,
						     PipeWorkQueue          * spwq,
						     size_t                   bytes,
						     const pami_quad_t      * metadata,
						     size_t                   metasize,
						     pami_event_function      cb_done,
						     void                   * cookie,
						     unsigned                 connection_id) 
	{
	  if (metasize*sizeof(pami_quad_t) + bytes > MU::Context::packet_payload_size)
	    return PAMI_ERROR;
	
	  //TRACE_ERR (stderr, "Long Multicast\n");
	  InjectAMMulticast *msg = new (state) InjectAMMulticast (_mucontext,
								  _channel,
								  topology,
								  spwq,
								  bytes,
								  metasize,
								  cb_done,	       
								  cookie);
	
	  MUSPI_DescriptorBase *desc = &msg->_desc;
	  Kernel_MemoryRegion_t memRegion;
	  int rc = 0;
	  rc = Kernel_CreateMemoryRegion (&memRegion, msg->packetBuf(), MU::Context::packet_payload_size);
	  PAMI_assert ( rc == 0 );
	  uint64_t paddr = (uint64_t)memRegion.BasePa + ((uint64_t)msg->packetBuf() - (uint64_t)memRegion.BaseVa);
	  
	  prepareDesc (desc,  paddr, bytes, metasize, connection_id);
	  if (metasize == 1) {
	    uint64_t *dst = (uint64_t *) msg->packetBuf();
	    uint64_t *src = (uint64_t *) metadata;	    
	    *dst ++ = *src ++;
	    *dst    = *src;
	  }
	  else if (metasize > 1)
	    memcpy(msg->packetBuf(), metadata, metasize *sizeof(pami_quad_t));	    
	  bool done = msg->advance();
	  
	  if (!done) {
	    PAMI::Device::Generic::GenericThread *work = new (msg->workobj()) 
	      PAMI::Device::Generic::GenericThread(advance, msg);
	    _mucontext.getProgressDevice()->postThread(work);
	  }
	  
	  return PAMI_SUCCESS;
	}
    
    };
  };
};
#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG
#endif
