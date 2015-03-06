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
 * \file components/devices/bgq/mu2/model/MulticastDmaModel.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_model_MulticastDmaModel_h__
#define __components_devices_bgq_mu2_model_MulticastDmaModel_h__

#include "components/devices/MulticastModel.h"
#include "components/memory/MemoryAllocator.h"
#include "util/trace.h"
#include "components/devices/bgq/mu2/Context.h"
#include "components/devices/bgq/mu2/msg/InjectDPutBase.h"
#include "components/devices/bgq/mu2/msg/InjectDPutMulticast.h"
#include "components/devices/bgq/mu2/msg/InjectDPutListMulticast.h"

#define MIN_COUNTERS 12
#define MAX_COUNTERS 26
#define MAX_VEC_SIZE 32

#ifndef L1_DCACHE_LINE_SIZE
#define L1_DCACHE_LINE_SIZE   64
#endif

#include "util/ccmi_debug.h"
#include "util/ccmi_util.h"

#undef DO_TRACE_ENTEREXIT 
#undef DO_TRACE_DEBUG     
#ifdef CCMI_TRACE_ALL
  #define DO_TRACE_ENTEREXIT 1
  #define DO_TRACE_DEBUG     1
#else
  #define DO_TRACE_ENTEREXIT 0
  #define DO_TRACE_DEBUG     0
#endif

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      class MUCounterSet {
      public:
	bool            _initialized;
	int             _ncounters;
	uint16_t        _b_batids[MAX_VEC_SIZE];  /// The base address table id for payload
	uint16_t        _b_offsets[MAX_VEC_SIZE];
	uint32_t        _c_batid;	
	volatile uint64_t      _counterVec[MAX_VEC_SIZE] __attribute__((__aligned__(L1_DCACHE_LINE_SIZE)));

	MUCounterSet () : _initialized(false), _ncounters(0) {}

	void initialize(MU::Context     & device, 
			int               ncounters, 
			pami_result_t   & status) 
	{
	  if (_initialized) {
	    if (_ncounters <= ncounters)
	      status = PAMI_SUCCESS;
	    else
	      status = PAMI_ERROR;
	    return;
	  }

	  if (ncounters > MAX_VEC_SIZE) {
	    status = PAMI_ERROR;
	    return;
	  }

	  int32_t rcBAT = 0;
	  status = PAMI_SUCCESS;

	  size_t id_count = device.getContextCount();
	  if (id_count > 1) {
	    int nc0 = ncounters/2;
	    int nc1 = ncounters - nc0;

	    rcBAT = device.allocateBatIds (nc0, _b_batids);  
	    if (rcBAT == -1)
	    {
	      status = PAMI_ERROR;
	      return;
	    }

	    uint16_t batid = 0;
	    rcBAT = device.allocateBatIds (1, &batid);  
	    if (rcBAT == -1)
	    {
	      //release allocated batids
	      device.freeBatIds(nc0, _b_batids);
	      status = PAMI_ERROR;
	      return;
	    }
	    _c_batid = batid;	  

	    size_t peer = (device.getContextOffset()+1)%id_count;
	    rcBAT = device.allocateBatIdsForPeer(nc1, peer, _b_batids+nc0);
	    if (rcBAT == -1)
	    {
	      status = PAMI_ERROR;
	      device.freeBatIds (nc0, _b_batids);
	      device.freeBatIds (1,  & batid);
	      return;
	    }
	    _ncounters = ncounters;	    

	    for (int i = 0; i < _ncounters; ++i) {
	      if (i < nc0)
		_b_offsets[i] = device.getContextOffset();
	      else
		_b_offsets[i] = peer;
	    }
	  }
	  else {
	    rcBAT = device.allocateBatIds (ncounters, _b_batids);  
	    if (rcBAT == -1)
	    {
	      status = PAMI_ERROR;
	      return;
	    }
	    _ncounters = ncounters;	  
	    
	    uint16_t batid = 0;
	    rcBAT = device.allocateBatIds (1, &batid);  
	    if (rcBAT == -1)
	    {
	      //release allocated batids
	      device.freeBatIds(ncounters, _b_batids);
	      status = PAMI_ERROR;
	      return;
	    }
	    _c_batid = batid;	  

	    for (int i = 0; i < _ncounters; ++i) 
	      _b_offsets[i] = device.getContextOffset();
	  }

	  _initialized = true;

	  int rc = 0;
	  memset ((void *)_counterVec, 0, sizeof(_counterVec));
	  Kernel_MemoryRegion_t memRegion;
	  rc = Kernel_CreateMemoryRegion (&memRegion, (void *)_counterVec, sizeof(_counterVec));
	  PAMI_assert ( rc == 0 );
	  uint64_t paddr = (uint64_t)memRegion.BasePa +
	    ((uint64_t)(void *)_counterVec - (uint64_t)memRegion.BaseVa);
	  
	  uint64_t atomic_address = MUSPI_GetAtomicAddress(paddr, MUHWI_ATOMIC_OPCODE_STORE_ADD);
	  device.setBatEntry (_c_batid, atomic_address);	  
	}

	void getBatIDs (uint16_t  *& base_batids,
			uint16_t  *& base_offsets,
			uint32_t  &  counter_batid) 
	{	  
	  base_batids = _b_batids;
	  base_offsets = _b_offsets;
	  counter_batid = _c_batid;
	}

	volatile uint64_t *getCounters() {
	  return (volatile uint64_t *)_counterVec;
	}
      };

      ///
      /// \brief MulticastDmaModel to inject dput p2p multicasts on the network
      //
      class MulticastDmaModel : public Interface::MulticastModel < MulticastDmaModel, MU::Context, 1 > {
      public:
	struct MulticastDmaRecv {	  	  
	  void initialize (pami_multicast_t *mcast) {
	    TRACE_FN_ENTER();	    
	    _connid  = mcast->connection_id;
	    _inuse   = 1;
	    _pwq     = (PAMI::PipeWorkQueue *)mcast->dst;
	    _fn      = mcast->cb_done.function;
	    _cookie  = mcast->cb_done.clientdata;
	    TRACE_FN_EXIT();		    
	  }
	  
	  void produceBytes (int bytes) {
	    TRACE_FN_ENTER();
	    _pwq->produceBytes (bytes);
	    TRACE_FN_EXIT();		    
	  }
	  
	  unsigned                _connid;
	  unsigned                _inuse;
	  PipeWorkQueue         * _pwq;
	  pami_event_function     _fn;	  
	  void                  * _cookie; 
	};

	static const size_t sizeof_msg = 1;

	MulticastDmaModel (pami_client_t    client,
			   pami_context_t   context,
			   MU::Context                 & device, 
			   pami_result_t               & status) : 
	  Interface::MulticastModel<MulticastDmaModel, MU::Context, 1> (device, status), 
	  _context(context),
	  _mucontext(device),
	  _nActiveRecvs(0),
	  _nActiveSends(0),
	  _nRecvsComplete(0),
	  _nSendsComplete(0),
	  _callConsumeBytes (true),
	  _localMulticast(true),
	  _npolling(true),
	  _advanceFlag(true),
	  _mytask(__global.mapping.task())	    
	  //_curBaseAddress(0)
	  {	    
	    TRACE_FN_ENTER();
	    char * buf;

	    __global.heap_mm->memalign((void**)&buf, 64, sizeof(InjectDPutMulticast) * MAX_COUNTERS);  
	    memset(buf, 0, sizeof(InjectDPutMulticast) * MAX_COUNTERS);
	    for (int i = 0; i < MAX_COUNTERS; ++i) {
	      char *sbuf = buf + sizeof(InjectDPutMulticast) * i;
	      new (sbuf) InjectDPutMulticast(device);
	    }
	    _sends_axial = (InjectDPutMulticast *)buf;

	    __global.heap_mm->memalign((void**)&buf, 64, sizeof(InjectDPutListMulticast) * MAX_COUNTERS);  
	    memset(buf, 0, sizeof(InjectDPutListMulticast) * MAX_COUNTERS);
	    for (int i = 0; i < MAX_COUNTERS; ++i) {
	      char *sbuf = buf + sizeof(InjectDPutListMulticast) * i;
	      new (sbuf) InjectDPutListMulticast(device);
	    }
	    _sends_list = (InjectDPutListMulticast *)buf;
	    
	    __global.heap_mm->memalign((void**)&buf, 64, sizeof(InjectDPutBase *) * MAX_COUNTERS);  
	    memset(buf, 0, sizeof(InjectDPutBase*) * MAX_COUNTERS);
	    _msg_vec = (InjectDPutBase **) buf;
	    
	    __global.heap_mm->memalign((void**)&_recvs, 64, sizeof(MulticastDmaRecv) * MAX_COUNTERS); 
	    memset(_recvs, 0, sizeof(MulticastDmaRecv) * MAX_COUNTERS);	    

	    memset (_counterShadowVec, 0, sizeof(_counterShadowVec));
	    
	    if (__global.mapping.tSize() == 1)
	      _localMulticast = false;
	    
	    ///// Get the BAT IDS ///////////////
	    //// Setup CounterVec in BAT 
	    _ncounters = MAX_COUNTERS;
	    //First try MAX_COUNTERS
	    _mu_counterset.initialize (device, MAX_COUNTERS, status);
	    if (status != PAMI_SUCCESS) {
	      _ncounters = MIN_COUNTERS;
	      _mu_counterset.initialize (device, MIN_COUNTERS, status);
	      
	      //Min failed return failure
	      if (status != PAMI_SUCCESS)
		return;	      
	    }
	    _counterVec = _mu_counterset.getCounters();
	    _mu_counterset.getBatIDs(_b_batids, _b_offsets, _c_batid);

	    _me = *_mucontext.getMuDestinationSelf();
	    new (&_work) PAMI::Device::Generic::GenericThread(advance, this);

	    initModels ();	    
	    status = PAMI_SUCCESS;
	    TRACE_FN_EXIT();
	  }

	  void setLocalMulticast (bool val) {
	    _localMulticast = val;
	  }
	  
	  void callConsumeBytesOnMaster (bool val) {
	    _callConsumeBytes = val;
	  }

	  void disableAdvance() {
	    //Model externally polled
	    _npolling = false; 
	    _advanceFlag = false; 
	  }

	  void initModels() {
	    TRACE_FN_ENTER();
	    int i = 0;
	    buildP2pDputModelDescriptor (_modeldesc, _b_batids[0], _c_batid);
	    MUSPI_DescriptorBase * dput;
	    for (i = 0; i < MAX_COUNTERS; i++) {
	      dput = &_sends_list[i]._desc;
	      _modeldesc.clone (*dput);	      
	    }

	    _modeldesc.setDeposit (MUHWI_PACKET_DEPOSIT);	    	    
	    for (i = 0; i < MAX_COUNTERS; i++) {
	      dput = &_sends_axial[i]._desc;
	      _modeldesc.clone (*dput);
	    }	    

	    TRACE_FN_EXIT();		    
	  };
	  
	  ~MulticastDmaModel () 
	    {
	      TRACE_FN_ENTER();
	      TRACE_FN_EXIT();		    
	    }
	  	  
	  void processSend (pami_multicast_t *mcast,
			    void             *devinfo = NULL) 
	  {
	    TRACE_FN_ENTER();
	    size_t length = mcast->bytes;
	    PAMI_assert (mcast->connection_id < _ncounters);
	    PAMI::Topology *dst_topology = (PAMI::Topology *)mcast->dst_participants;
	    PipeWorkQueue *pwq = (PAMI::PipeWorkQueue *)mcast->src;
	    size_t idx = _nActiveSends++;

	    InjectDPutBase *msg = NULL;
	    MUSPI_DescriptorBase * dput = NULL;
	    if (dst_topology->type() == PAMI_AXIAL_TOPOLOGY) {
	      TRACE_STRING("Process Send with axial topo");

	      pami_coord_t *ll=NULL;
	      pami_coord_t *ur=NULL;
	      pami_coord_t *ref=NULL;
	      unsigned char *isTorus=NULL;
	    
	      pami_result_t result = PAMI_SUCCESS;
	      result = dst_topology->axial(&ll, &ur, &ref, &isTorus);
	      PAMI_assert(result == PAMI_SUCCESS);
	      
	      msg = (InjectDPutBase *) &_sends_axial[idx];
	      dput = &_sends_axial[idx]._desc;

	      // Clone the single-packet model descriptor into the injection fifo
	      dput->PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Counter_Offset = mcast->connection_id * sizeof(uint64_t);
	      dput->PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id = _b_batids[mcast->connection_id];	    
	      
	      _sends_axial[idx].initialize ( _context,
					     mcast->cb_done.function,
					     mcast->cb_done.clientdata,	 
					     &_me, 
					     ref, 
					     isTorus, 
					     ll,
					     ur,
					     pwq,
					     length,
					     _localMulticast );	    
	    }
	    else {
	      TRACE_STRING("Process Send with topology");
	      
	      msg  = (InjectDPutBase *) &_sends_list[idx];
	      dput = &_sends_list[idx]._desc;

	      // Clone the single-packet model descriptor into the injection fifo
	      dput->PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Counter_Offset = mcast->connection_id * sizeof(uint64_t);
	      dput->PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id = _b_batids[mcast->connection_id];
	      
	      _sends_list[idx].initialize ( _context,
					    mcast->cb_done.function,
					    mcast->cb_done.clientdata,	 
					    dst_topology,
					    pwq,
					    length,
					    _localMulticast );	    
	    }

	    _msg_vec[idx] = msg;
	    //printf ("Calling msg advance\n");
	    bool done = msg->advance();	    
	    if (!done && _npolling) {
	      _npolling = false;
	      PAMI::Device::Generic::GenericThread *work = (PAMI::Device::Generic::GenericThread *)&_work;
	      _mucontext.getProgressDevice()->postThread(work);
	    }
	    else if (done) {
	      _nActiveSends --;
	    }	      
	    TRACE_FN_EXIT();		    
	  };

	  void processRecv (pami_multicast_t *mcast,
			    void             *devinfo = NULL) 
	  {
	    TRACE_FN_ENTER();
	    int connid = mcast->connection_id;
	    PAMI_assert(connid < (int)_ncounters);	
	    
	    int idx = _nActiveRecvs ++;
	    PAMI_assert (_nActiveRecvs <= _ncounters);
	    PAMI_assert(_recvs[idx]._inuse == 0);

	    //printf ("In process recv connid %d, idx %d, length %d\n", 
	    //    connid, idx, (int)mcast->bytes);

	    _counterVec[connid]    = mcast->bytes;
	    _counterShadowVec[idx] = mcast->bytes;
	    _recvs[idx].initialize(mcast);

	    Kernel_MemoryRegion_t memRegion;
	    char *payload = (char *) ((PAMI::PipeWorkQueue *)mcast->dst)->bufferToProduce();
	    uint rc = 0;
	    rc = Kernel_CreateMemoryRegion (&memRegion, payload, mcast->bytes);
	    PAMI_assert ( rc == 0 );
	    uint64_t paddr = (uint64_t)memRegion.BasePa +
	      ((uint64_t)payload - (uint64_t)memRegion.BaseVa);
		    
	    //printf ("Calling Set Bat Entry %d, %lx\n",(int)_b_batids[connid], paddr);
	    _mucontext.setBatEntryForPeer (_b_batids[connid], 
					   _b_offsets[connid],
					   paddr);

	    if (_npolling) {
	      _npolling = false;
	      PAMI::Device::Generic::GenericThread *work = (PAMI::Device::Generic::GenericThread *)&_work;
	      _mucontext.getProgressDevice()->postThread(work);
	    }
	    TRACE_FN_EXIT();		    
	  }
  
	  pami_result_t postMulticastImmediate_impl(size_t                client,
						    size_t                context, 
						    pami_multicast_t    * mcast,
						    void                * devinfo=NULL) 
	  {
	    TRACE_FN_ENTER();
	    //printf ("In post multicast immediate\n");

	    // Get the source data buffer/length and validate (assert) inputs
	    PAMI::Topology *root_topo = (PAMI::Topology*)mcast->src_participants;
	    TRACE_FORMAT("root_topo %p/%zu, root %u, mytask %u", root_topo,  root_topo?root_topo->size():-1,root_topo?root_topo->index2Rank(0):-1,_mytask);

	    PAMI_assert(root_topo != NULL);
	    if (root_topo->index2Rank(0) == _mytask)
	      processSend (mcast, devinfo);
	    else
	      processRecv (mcast, devinfo);
	    TRACE_FN_EXIT();
	    return PAMI_SUCCESS;
	  }

	  /// \see PAMI::Device::Interface::MulticastModel::postMulticast
	  pami_result_t postMulticast_impl(uint8_t (&state)[MulticastDmaModel::sizeof_msg],
					   size_t            client,
					   size_t            context,
					   pami_multicast_t *mcast,
					   void             *devinfo = NULL) 
	  {
	    TRACE_FN_ENTER();
	    TRACE_FN_EXIT();		    	    
	    return PAMI_SUCCESS;
	  }

	  static pami_result_t advance (pami_context_t     context,
					void             * cookie) 
	    {
	      MulticastDmaModel *model = (MulticastDmaModel *)cookie;	      
	      for (int i =0; i < 4; ++i) {
		model->advance_recvs(context);		
		model->advance_sends (context);
		
		if (model->_nActiveRecvs == model->_nRecvsComplete) {
		  model->reset_recvs(context);
		  break;
		}
	      }
	      
	      //fprintf (stderr, "MulticastDmaModel::advance() nsends %d nrecvs %d sends complete %d recvs complete %d\n", 
	      //       model->_nActiveSends, model->_nActiveRecvs, 
	      //       model->_nSendsComplete, model->_nRecvsComplete);

	      if (model->_nActiveRecvs == 0 && model->_nActiveSends == 0) {
		model->_npolling = model->_advanceFlag;
		return PAMI_SUCCESS;
	      }
	      
	      return PAMI_EAGAIN;
	    }

	  size_t advance_recvs (pami_context_t     context)
	  {
	    TRACE_FN_ENTER();
	    size_t events = 0;
	    unsigned i = 0;
	    for (i=0; i < _nActiveRecvs; i++)  {
	      MulticastDmaRecv *recv = &_recvs[i];
	      if (recv->_inuse) {
		uint32_t cid = recv->_connid;
		uint64_t cc = _counterVec[cid];
		uint64_t bytes = _counterShadowVec[i] - cc;
		if (bytes > 0) {
		  ppc_msync();
		  recv->produceBytes (bytes);
		  _counterShadowVec[i] = cc;
		  
		  //Trigger a send check on counter i??
		  if (cc == 0) {
		    recv->_inuse = 0;
		    _nRecvsComplete++;	 
		  }
		  events = 1;
		}
	      }	 
	    }
	    TRACE_FN_EXIT();		    	    
	    return events;
	  }

	  void reset_recvs (pami_context_t context) {	    
	    unsigned nrecvs = _nActiveRecvs;
	    _nActiveRecvs = 0;
	    _nRecvsComplete = 0;
	    
	    unsigned i = 0;
	    for (i=0; i < nrecvs; i++)  {
	      MulticastDmaRecv *recv = &_recvs[i];
#if ASSERT_LEVEL //disable in fast mode
	      _mucontext.setBatEntryForPeer (_b_batids[_recvs[i]._connid], 
					     _b_offsets[_recvs[i]._connid],
					     0);  
#endif
	      if (recv->_fn)
		recv->_fn (context, recv->_cookie, PAMI_SUCCESS);
	    }
	  }	  	  

	  void advance_sends (pami_context_t     context)
	  {
	    TRACE_FN_ENTER();
	    unsigned i = 0; 
	    for (i=0; i < _nActiveSends; i++)  {
	      if (!_msg_vec[i]->done() && _msg_vec[i]->isActive()) {
		bool done = _msg_vec[i]->advance();
		if (done) 
		  _nSendsComplete++;
	      }
	    }
	    
	    if (_nSendsComplete == _nActiveSends) {
	      _nSendsComplete = 0;
	      _nActiveSends = 0;
	    }
	    TRACE_FN_EXIT();
	  }

	  
      protected:
	  pami_context_t                             _context;
	  MU::Context                              & _mucontext;
	  unsigned                                   _nActiveRecvs;
	  unsigned                                   _nActiveSends;
	  unsigned                                   _nRecvsComplete;
	  unsigned                                   _nSendsComplete;
	  unsigned                                   _ncounters;
	  bool                                       _callConsumeBytes;
	  bool                                       _localMulticast;
	  bool                                       _npolling;	 
	  bool                                       _advanceFlag;
	  pami_task_t                                _mytask;
	  uint32_t                                   _c_batid;                 /// The base address table id for counter
	  uint16_t                                 * _b_batids;  /// The base address table id for payload
	  uint16_t                                 * _b_offsets; /// Context offsets where those bats were allocated
	  MUHWI_Destination_t                        _me;
	  uint64_t                                   _counterShadowVec[MAX_VEC_SIZE]; //A list of shadow counters
	  MulticastDmaRecv                         * _recvs; 
	  InjectDPutMulticast                      * _sends_axial;
	  InjectDPutListMulticast                  * _sends_list;
	  InjectDPutBase                          ** _msg_vec;
	  pami_work_t                                _work;	  
	  MUSPI_DescriptorBase                       _modeldesc;         /// Model descriptor
	  
	  ///These counters are indexed by the connetion id known to
	  ///the remote node
	  volatile uint64_t                       * _counterVec;
	  
	  static MUCounterSet                       _mu_counterset;
      } __attribute__((__aligned__(L1_DCACHE_LINE_SIZE)));
    };
  };
};

#undef DO_TRACE_ENTEREXIT 
#undef DO_TRACE_DEBUG     

#endif
