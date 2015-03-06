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
 * \file algorithms/protocols/tspcoll/ShmHybridBcast.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/ShmHybridBcast.h"
#include "algorithms/protocols/tspcoll/SHMReduceBcast.h"
#include "algorithms/protocols/tspcoll/local/FixedLeader.h"
#include "algorithms/protocols/tspcoll/Team.h"

#include "algorithms/protocols/tspcoll/cau_collectives.h"

template <class T_NI, class T_Device>
xlpgas::ShmHybridBcast<T_NI,T_Device>::ShmHybridBcast (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset, void* device_info, T_NI* ni) :
  xlpgas::Collective<T_NI> (ctxt, comm, kind, tag, NULL, NULL, ni) {
  typedef xlpgas::cau_device_info<T_NI> device_info_type;
  this->_device_info = device_info;
  this->_buf_size = ((device_info_type*)device_info)->shm_buffers()._bcast_buf_sz / 2; // divide by two only for 2LB
  //here cache the is leader information and the required topologies;
  typedef xlpgas::cau_device_info<T_NI> device_info_type;
  PAMI_GEOMETRY_CLASS* geometry = ((device_info_type*)(this->_device_info))->geometry();
  team        = (PAMI::Topology*)(geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
  local_team  = (PAMI::Topology*)(geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX));
  leader_team = (PAMI::Topology*)(geometry->getTopology(PAMI::Geometry::MASTER_TOPOLOGY_INDEX));
  //the text topology we don't cache for now; used only to extract the is leader info
  PAMI::Topology* my_master_team = (PAMI::Topology*)(geometry->getTopology(PAMI::Geometry::LOCAL_MASTER_TOPOLOGY_INDEX));
  this->_is_leader = my_master_team->isEndpointMember(this->rank());
}

template <class T_NI>
void next_repeated_phase (void* ctxt, void * arg, pami_result_t){
  typedef xlpgas::if_else_coll_continuation<T_NI> ARGS;
  ARGS* hb_args = (ARGS*)arg;//hybrid bcast args
  if(hb_args->left_to_send > 0){
    xlpgas::Collective<T_NI>* b = (xlpgas::Collective<T_NI>*)hb_args->shm_bcast;    

    //fprintf(stderr, "L%d NRP LEFTOVER ROOT =%d\n",b->rank(),hb_args->leader_root);

    //here we need to increment the buf size and decrement left to send
    hb_args->sbuf  = (void*)( (char*)(hb_args->sbuf) + hb_args->buf_size );
    hb_args->dbuf  = (void*)( (char*)(hb_args->dbuf) + hb_args->buf_size );
    size_t to_send = (hb_args->left_to_send > hb_args->buf_size) ? hb_args->buf_size : hb_args->left_to_send ;    
    b->reset(hb_args->local_root,
	     hb_args->sbuf,
	     hb_args->dbuf,
	     to_send);
    if(hb_args->left_to_send > hb_args->buf_size)
      hb_args->left_to_send  -= hb_args->buf_size;
    else
      hb_args->left_to_send = 0;
    
    b->setComplete(next_repeated_phase<T_NI>, arg);
    b->kick();
  }
  else {
    //fprintf(stderr, "L%d NRP non LEFTOVER ROOT =%d\n",shm_bcast->rank(),hb_args->leader_root);
    hb_args->shmem_finished = true;
    // if leader will start global bcast; 
    // if required only which is controlled by b being null or not
    xlpgas::Broadcast<T_NI>* b = (xlpgas::Broadcast<T_NI>*)hb_args->p2p_bcast;
    if(b == NULL) {
      //the shared memory part is finished; there is no leader component user call back is called
      if (hb_args->cb) {
        //fprintf(stderr, "L%d NRP LEFTOVER ROOT CALL BACK RETURN=%d\n",shm_bcast->rank(),hb_args->leader_root);
        hb_args->cb (hb_args->pami_ctxt, hb_args->arg, PAMI_SUCCESS);
      }
      return;//no continuation; done
    }

    //need reset here; we can't have advance/progress between reset and kick of the p2p
    //fprintf(stderr, "L%d NRP P2P RESET KICK=%d\n", b->rank(),hb_args->leader_root);
    b->reset(hb_args->leader_root,
             hb_args->o_dbuf,
             hb_args->o_dbuf,
             hb_args->ttype,
             hb_args->tcount);
    b->kick();
  }
}


template <class T_NI>
void next_local_phase (void* ctxt, void * arg, pami_result_t){
  typedef xlpgas::if_else_coll_continuation<T_NI> ARGS;
  ARGS* hb_args = (ARGS*)arg;//hybrid bcast args
  xlpgas::SHMLargeBcast<T_NI>* b = (xlpgas::SHMLargeBcast<T_NI>*)hb_args->shm_bcast;
  size_t to_send = (hb_args->nbytes > hb_args->buf_size) ? hb_args->buf_size : hb_args->nbytes ;

  if(hb_args->nbytes > hb_args->buf_size)
    hb_args->left_to_send  = hb_args->nbytes - hb_args->buf_size;
  else
    hb_args->left_to_send = 0;

  //the leaders component is finished; user call back is called on the node that owns the root
  if(hb_args->shmem_finished == true){
    if (hb_args->cb){
      hb_args->cb (hb_args->pami_ctxt, hb_args->arg, PAMI_SUCCESS);
    }
  }
  else{
    //this is a non same location as root node; this is the callback of the p2p step;
    //it starts the local shmem bcast
    //fprintf(stderr, "L%d N LOCAL PHASE LEFTOVER ROOT=%d\n",b->rank(),hb_args->leader_root);
    hb_args->p2p_bcast = NULL;
    b->reset(hb_args->local_root,
             hb_args->sbuf,
             hb_args->dbuf,
             to_send);
    b->setComplete(next_repeated_phase<T_NI>, arg);
    //if(b->rank()==0) fprintf(stderr, "NLP->kicks SHMEM %d \n", hb_args->buf_size);
    b->kick();
  }
}

template <class T_NI, class T_Device>
bool xlpgas::ShmHybridBcast<T_NI,T_Device>::isdone () const {
  if(shm_bcast == NULL) return true;
  else return shm_bcast->isdone();
}

template <class T_NI, class T_Device>
void xlpgas::ShmHybridBcast<T_NI,T_Device>::kick () {
  if(leader_team->size()>1 && leader_team->isEndpointMember(_root_endpoint)){//if root is leader
    //leader will kick the p2p bcast; this in turn will kick the shmem part as well
    if(this->_is_leader){
      p2p_bcast->setComplete(next_local_phase<T_NI>,&args);
      p2p_bcast->reset(args.leader_root, 
                       args.o_sbuf,
                       args.o_dbuf,
                       args.ttype,
                       args.tcount);
      //fprintf(stderr, "L%d KICK P2P for root=%d\n",p2p_bcast->rank(),args.leader_root);
      p2p_bcast->kick();
    }
    //all others except leader on a node not the same as root kicks the shmem part; 
    //the leader will kick the shmem part on the call back of p2p 
    else {
      shm_bcast->setComplete(next_repeated_phase<T_NI>,&args);
      //fprintf(stderr, "L%d KICK SHM non root (r=%d) \n",shm_bcast->rank(),_root);
      shm_bcast->kick();
    }
  }
  else {//general case
    if(local_team->isEndpointMember(_root_endpoint)) {
      //fprintf(stderr, "L%d KICK SHM ROOT =%d\n",shm_bcast->rank(),_root);
      shm_bcast->setComplete(next_repeated_phase<T_NI>,&args);
      shm_bcast->kick();
    }
    else {
      if (leader_team->size() > 1){
        //leader will kick the p2p bcast; this in turn will kick the shmem part as well
	if( this->_is_leader){
          p2p_bcast->setComplete(next_local_phase<T_NI>,&args);
          p2p_bcast->reset(args.leader_root, 
                           args.o_dbuf,
                           args.o_dbuf,
                           args.ttype,
                           args.tcount);
          //fprintf(stderr, "L%d KICK P2P for root=%d\n",p2p_bcast->rank(),args.leader_root);
          p2p_bcast->kick();
        }
        //all others except leader on a node not the same as root kicks the shmem part; 
        //the leader will kick the shmem part on the call back of p2p 
        else {
          shm_bcast->setComplete(next_repeated_phase<T_NI>,&args);
          //fprintf(stderr, "L%d KICK SHM non root (r=%d) \n",shm_bcast->rank(),_root);
          shm_bcast->kick();
        }
      }
    }
  }//else; the root  is not leader
}

template <class T_NI, class T_Device>
void xlpgas::ShmHybridBcast<T_NI,T_Device>::reset (int root, 
						   const void         * sbuf, 
						   void               * rbuf, 
						   TypeCode           * type,
						   size_t               typecount) {
  typedef xlpgas::if_else_coll_continuation<T_NI> ARGS;

  _root = root;
  _root_endpoint = team->index2Endpoint(_root);

  size_t nbytes = type->GetDataSize() * typecount;
  if(team->size()==1){
    memcpy(rbuf, sbuf, nbytes);
    if (this->_cb_complete){ 
      this->_cb_complete (this->_pami_ctxt, this->_arg, PAMI_SUCCESS); 
    }
    return ;
  }

  assert (shm_bcast != NULL);
  args.shm_bcast      = (void*)shm_bcast;
  args.shmem_finished = false;
  args.nbytes         = nbytes;
  args.ttype          = type;
  args.tcount         = typecount;

 if(leader_team->size()>1) {
   if( this->_is_leader ){
      assert (p2p_bcast != NULL);
      args.p2p_bcast    = (void*)p2p_bcast;

      //compute the leader root; This is the leader of the shared
      //memory node where the real root of bcast resides; Not an
      //efficient op for now; can be improved;
      size_t nranks;
      pami_task_t* leader_ranks = (pami_task_t*)malloc(leader_team->size()*sizeof(pami_task_t));
      leader_team->getRankList(leader_team->size(), leader_ranks, &nranks);
      assert(nranks == leader_team->size());
      args.leader_root = -1; 
      pami_task_t root_rank = team->index2Rank(root);
      for(size_t l=0;l<nranks;++l){
        if(__global.mapping.isPeer(root_rank, leader_ranks[l])) {
          args.leader_root = leader_team->rank2Index(leader_ranks[l]);
          break;
        }
      }
      assert(args.leader_root != -1);
      free(leader_ranks);
      p2p_bcast->setContext(this->_pami_ctxt);
      p2p_bcast->setComplete(next_local_phase<T_NI>,&args);
    }
    else {
      //non leader node; skips this step
      args.p2p_bcast = NULL;
    }
  }
  else {
    args.p2p_bcast = NULL;
  }

  args.buf_size     = this->_buf_size;
  args.left_to_send = (nbytes > this->_buf_size) ? nbytes - this->_buf_size : 0;
  args.pami_ctxt    = this->_pami_ctxt;
  args.cb           = this->_cb_complete; 
  args.arg          = this->_arg;

  size_t to_send = (nbytes <= this->_buf_size) ? nbytes : this->_buf_size;
  
  if(local_team->isEndpointMember(_root_endpoint) && ( !leader_team->isEndpointMember(_root_endpoint) || leader_team->size()==1) ) {
    args.local_root   = local_team->endpoint2Index(_root_endpoint);//translate global root to local index
    args.sbuf         = const_cast<void*>(sbuf);
    args.dbuf         = rbuf;
    args.o_sbuf = args.sbuf;
    args.o_dbuf = args.dbuf;
    shm_bcast->reset(args.local_root,
		     sbuf,
		     rbuf,
		     to_send);
    shm_bcast->setComplete(next_repeated_phase<T_NI>,&args);
  }
  else {
    args.local_root   = 0;//always the local master bcasts data received from P2P
    args.sbuf         = const_cast<void*>(rbuf);
    args.dbuf         = rbuf;
    args.o_sbuf       = args.sbuf;
    args.o_dbuf       = args.dbuf;
    args.p2p_bcast    = NULL;//these are the nodes that receives data in the P2P phase
                             //they will stop when local bcast is done
    shm_bcast->reset(args.local_root, 
		     rbuf,
		     rbuf,
		     to_send);
    shm_bcast->setComplete(next_repeated_phase<T_NI>,&args);
  }
}


///////////////////////////////////////////////////////////////////////
//                    Pipelined version starts here; 
///////////////////////////////////////////////////////////////////////
template <class T_NI, class T_Device>
xlpgas::ShmHybridPipelinedBcast<T_NI,T_Device>::ShmHybridPipelinedBcast (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset, void* device_info, T_NI* ni) :
  xlpgas::Collective<T_NI> (ctxt, comm, kind, tag, NULL, NULL, ni) {
  typedef xlpgas::cau_device_info<T_NI> device_info_type;
  this->_device_info = device_info;
  //below is divided by two because we use double buffer
  this->_buf_size = ((device_info_type*)device_info)->shm_buffers()._bcast_buf_sz/2;
  _flag = true;
}

template <class T_NI, class T_Device>
bool xlpgas::ShmHybridPipelinedBcast<T_NI,T_Device>::isdone () const {
  return _flag;
}

template <class T_NI, class T_Device>
void xlpgas::ShmHybridPipelinedBcast<T_NI,T_Device>::kick () {
  _onebuf_bcast->kick();
}


template <class T_NI,class T_Device>
void next_pipelined_phase (void* ctxt, void * arg, pami_result_t){
  typedef xlpgas::args_pipeline<T_NI> ARGS;
  ARGS* hb_args = (ARGS*)arg;//hybrid bcast args
  if(hb_args->left_to_send > 0){
    xlpgas::ShmHybridBcast<T_NI,T_Device>* b = (xlpgas::ShmHybridBcast<T_NI,T_Device>*)hb_args->onebuf_bcast;    
    //here we need to increment the buf size and decrement left to send
    hb_args->sbuf  = (void*)( (char*)(hb_args->sbuf) + hb_args->buf_size );
    hb_args->dbuf  = (void*)( (char*)(hb_args->dbuf) + hb_args->buf_size );
    size_t to_send = (hb_args->left_to_send > hb_args->buf_size) ? hb_args->buf_size : hb_args->left_to_send ;    
    b->reset(hb_args->root,
	     hb_args->sbuf,
	     hb_args->dbuf,
	     hb_args->ttype,
             to_send);
    if(hb_args->left_to_send > hb_args->buf_size)
      hb_args->left_to_send  -= hb_args->buf_size;
    else
      hb_args->left_to_send = 0;
    //fprintf(stderr, "L%d NPL reset bsize=%d leftover=%d\n",  b->rank(), to_send, hb_args->left_to_send);
    b->setComplete(next_pipelined_phase<T_NI,T_Device>, arg);
    b->kick();
  }
  else {
    hb_args->cb (hb_args->pami_ctxt, hb_args->arg, PAMI_SUCCESS);
    //set done on the cuurent collective
    xlpgas::ShmHybridPipelinedBcast<T_NI,T_Device>* pb = (xlpgas::ShmHybridPipelinedBcast<T_NI,T_Device>*)hb_args->pipelined_bcast;
    pb->set_done();
  }
}


template <class T_NI, class T_Device>
void xlpgas::ShmHybridPipelinedBcast<T_NI,T_Device>::reset (int                 root, 
                                                            const void        * sbuf, 
                                                            void              * rbuf, 
                                                            TypeCode          * type,
                                                            size_t              typecount) {
  size_t nbytes = type->GetDataSize() * typecount;
  args.root     = root;
  args.buf_size = this->_buf_size;
  args.nbytes   = nbytes;
  args.ttype    = (TypeCode*)PAMI_TYPE_BYTE;
  args.tcount   = nbytes;
  args.sbuf     = const_cast<void*>(sbuf);
  args.dbuf     = rbuf;
  args.o_sbuf   = args.sbuf;
  args.o_dbuf   = args.dbuf;
  args.pami_ctxt= this->_pami_ctxt;
  args.cb       = this->_cb_complete; 
  args.arg      = this->_arg;
  args.left_to_send = (nbytes > this->_buf_size) ? nbytes - this->_buf_size : 0;
  _flag         = false; 

  assert (_onebuf_bcast != NULL);
  args.onebuf_bcast    = (void*)_onebuf_bcast;
  args.pipelined_bcast    = (void*)this;
  _onebuf_bcast->setComplete(next_pipelined_phase<T_NI,T_Device>,&args);
  //size_t to_send = (nbytes <= this->_buf_size) ? nbytes : this->_buf_size;
  //next line is very important; 
  _onebuf_bcast->setContext(this->_pami_ctxt);//don't move after reset; and you always have to set it
  _onebuf_bcast->reset(root,sbuf,rbuf,(TypeCode*)PAMI_TYPE_BYTE,nbytes);
}
