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
 * \file algorithms/protocols/tspcoll/AMExchange.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_AMExchange_h__
#define __algorithms_protocols_tspcoll_AMExchange_h__

#include "algorithms/protocols/tspcoll/Team.h"
#include "algorithms/protocols/tspcoll/CollExchange.h"
#include "algorithms/protocols/tspcoll/binomial_exchange.h"
#include "algorithms/protocols/tspcoll/Allreduce.h"

#undef TRACE
#ifdef DEBUG_COLL
#define TRACE(x)  fprintf x;
#else
#define TRACE(x)
#endif

/* ****************************************************************
      Generic infrastructure for am collectives; more specifically
      they are one sided; their invocations by one thread is not
      dependent on the other threads status
  **************************************************************** */
namespace xlpgas
{
  void  am_collectives_reg       (xlpgas_AMHeaderReg_t); //register all am collectives

  template <class AMHeader, class Derived, class T_NI>
  class AMExchange : public Collective<T_NI>
  {
  public:
    typedef Derived                 derived_type;
    typedef binomial_exchange<Team> exchange_tree;

    AMExchange(){}

    AMExchange (int ctxt, int team_id, AMHeader* ucb, int parent=-1)  :
    Collective<T_NI> (ctxt, xlpgas::Team::get (ctxt, team_id), MAXKIND, 0, NULL, NULL), _team_id(team_id), _parent(parent) {
	this->_rcvcount = 0;
	this->_sndcount = 0;
	this->_children = 0;
	this->_children_rcv = 0;
	this->_off_rbuf = 0;
	this->_temp = NULL;
	this->_header = ucb;
	assert (this->_header != NULL);
	this->_header->senderID      = this->_comm->ordinal();
	this->_header->dest_ctxt     = ctxt;
	this->_header->team_id       = team_id;
	this->_header->root          = -1;
	this->_header->parent_state  = NULL;
	MUTEX_INIT(&_mutex);
      }

    void clear(void){
      if(this->_parent != -1 && this->_header != NULL){
	__global.heap_mm->free(_header); //it was allocated with malloc except for root
      }
      if( this->_parent != -1 && this->_temp != NULL) {
	__global.heap_mm->free(_temp);//it was allocated with malloc;
      }
    }

    static void cb_toc_senddone (void * ctxt, void * arg);
    static void cb_top_senddone (void * ctxt, void * arg);

    virtual void kick    ();
    virtual bool isdone  (void) const;

    virtual void root(void){}//acton to be performed on the root
    virtual void allocate_space_result(void){}
    virtual void* parent_incomming(void){return NULL;}
    virtual void merge_data (void) {}
    virtual void adjust_header(size_t){}

    int get_root(void){// the root of the one sided collective
      return _root;
    }

    static xlpgas_local_addr_t cb_p_incoming (const struct xlpgas_AMHeader_t * hdr,
					      void (** completionHandler)(void *, void *),
					      void ** arg);
    static void cb_p_recvcomplete (void * unused, void * arg);

    static xlpgas_local_addr_t cb_c_incoming (const struct xlpgas_AMHeader_t * hdr,
					      void (** completionHandler)(void *, void *),
					      void ** arg);
    static void cb_c_recvcomplete (void * unused, void * arg);

    void set_parent(void* ps){
      _parent_state = ps;
    }

  protected:
    const void    * _sbuf;          /* send buffer    */
    void          * _rbuf;          /* receive buffer */
    size_t          _slen;          /* snd msg length     */
    size_t          _rlen;          /* rcv msg length     */
    int             _root;
    int             _team_id;
    int             _parent;
    size_t          _children;
    size_t          _sndcount;
    int             _rcvcount;

    int             _children_rcv;
    int             _off_rbuf;
    //am_cb_incomming _user_cb_incomming;
    xlpgas_event_function  _user_cb_received;
    void*            _parent_state;
    void*            _cookie;
    bool             _ret_values; //if there are ret values to wait for;
    bool             _amex_allocated; //if amex allocates the space for
                                      // receiving data; if yes _rbuf will be freed in destructor;
    void * _temp; //for gathering data
    size_t _to_be_received;

    xlpgas_AMHeaderReg_t  FROMCHILD_AM;
    AMHeader* _header;
  }; /* AMExchange */


/* **************************************************************** */
/*                   do all the sends from current node             */
/* **************************************************************** */
template <class AMHeader, class Derived, class T_NI>
void AMExchange<AMHeader,Derived, T_NI>::kick   () {
  /* UNLOCK */
  int _numphases = 0;
  int myrelrank = (this->_comm->ordinal() + this->_comm->size() - _root) % this->_comm->size();
  //simulate local am sends and callbacks
  bool return_values = _ret_values;
  // #warning Compiler warning (remove me...unused variable)
  if(return_values);

  _header->parent_state = this;

  //compute how many chldren current node (myrelrank) has;
  for (int n=this->_comm->size()-1; n>0; n>>=1) _numphases++;
  for (int i=0; i<_numphases; i++) { //just count how many children !!! info needed before sending messages
    //send to all children of root in the binomial tree
    int  dist       = 1<<(_numphases-1-i);
    int  sendmask   = (1<<(_numphases-i))-1;
    int  destrelrank= myrelrank + dist;
    bool dosend     = ((myrelrank&sendmask)==0)&&(destrelrank < (int)this->_comm->size());
    if(dosend){
      _children++;
    }//if do send
  }//for

  //we know how many children; allocate space for the result;
  //reduce, gather does something here; bcast is empty
  allocate_space_result();

  if(myrelrank == 0){//if root copy data locally explicit; the rest
		     //will use the messaging library to copy data
		     //implicit
    root();//virtual call into each specialization
  }
// #warning "Do AMSend"
#if 0
  for (int i=0, phase=_numphases; i<_numphases; i++) {
    //send to all children of root in the binomial tree
    int  dist       = 1<<(_numphases-1-i);
    int  sendmask   = (1<<(_numphases-i))-1;
    int  destrelrank= myrelrank + dist;
    bool dosend     = ((myrelrank&sendmask)==0)&&(destrelrank < (int)this->_comm->size());
    if(dosend){
      int  destindex  = (destrelrank + _root)%this->_comm->size();
      xlpgas_endpoint_t dst = this->_comm->index2Endpoint (destindex);
      ((AMHeader&)(_header->hdr)).dest_ctxt = dst.ctxt;

      this->_header->len = this->_slen;
      size_t nbytes;
      if(this->_ret_values) nbytes = 0;
      else nbytes = this->_slen;

      adjust_header(dist);
      xlpgas_tsp_amsend (_ctxt,
			 dst,
			 &(_header->hdr),
			 (xlpgas_local_addr_t) _sbuf,
			 nbytes,
			 cb_toc_senddone,
			 this);

    }//if do send
  }//for
#endif

// #warning "Do AMSend"
#if 0
  if(return_values){
    //printf("L%d :LCH Return temp_state=%x\n", XLPGAS_MYNODE, this);
    if (_children == 0) {
      //if leaf; send info up to the parent; Intermediate children will send
      //in the completion handler of the AM received from children
      if(_parent != -1){
	xlpgas_endpoint_t dst = this->_comm->index2Endpoint (this->_parent);
	((AMHeader&)(_header->hdr)).dest_ctxt = dst.ctxt;
	_header->hdr.handler   = XLPGAS_TSP_AMREDUCE_CREQ;
	this->_header->parent_state  = this->_parent_state;
	this->_header->hdr.handler   = this->FROMCHILD_AM;
	this->_header->len           = this->_rlen;
	xlpgas_tsp_amsend (_ctxt,
			   dst,
			   &(_header->hdr),
			   (xlpgas_local_addr_t) _rbuf,
			   _rlen,
			   cb_top_senddone,
			   this);
      }
      else {
	//root; run CH
	(*this->_user_cb_received)(&this->_ctxt, this->_cookie, result) ;
      }
    }//if leaf
  }
  else {
    //non return values;
    if (this->_children == 0 && this->_parent != -1) {
      //if leaf run the completion handler here;
      (*this->_user_cb_received)(&this->_ctxt, this->_cookie, result) ;
    }
  }
#endif
}//end kick

/* **************************************************************** */
/*                     check whether we are done                    */
/* **************************************************************** */
template <class AMHeader, class Derived, class T_NI>
bool AMExchange<AMHeader, Derived, T_NI>::isdone () const
{
  if(!_ret_values) return (int)_sndcount >= (int)_children; //done when all messages sent
  else return (_rcvcount >= (int)_children);
}

/* **************************************************************** */
/*               send completion in broadcast                         */
/* **************************************************************** */
template <class AMHeader, class Derived, class T_NI>
void AMExchange<AMHeader,Derived, T_NI>::cb_top_senddone (void * ctxt, void * arg)
{
  derived_type * self = (derived_type*)arg;
  /* LOCK */
  //end of the reduction/gather ; results are sent to parent;
  //invoke user cb and free the state object; locking not necessary as there is only
  //one thread here
  int res=0;
  self->_user_cb_received(&self->_ctxt, self->_cookie, res);
  //delete self;
  self->clear();
  __global.heap_mm->free(self);
  /* UNLOCK */
}

template <class AMHeader, class Derived, class T_NI>
void AMExchange<AMHeader,Derived, T_NI>::cb_toc_senddone (void * ctxt, void * arg)
{
  //run user specific completion handler only after that has been forwarded;
  //the user callback may remove some buffers I use to forward the data
  derived_type * self = (derived_type*)arg;
  /* LOCK */
  self->_sndcount++;
  //printf("L%d :SEND TOC DONE temp_state=%x  [%d | %d]\n", XLPGAS_MYNODE, self, self->_sndcount, self->_children);
  //next call user  call back for non receiving collectives
  if(!self->_ret_values && self->_sndcount == self->_children){
    int result=0;
    (*(self->_user_cb_received))(&self->_ctxt, self->_cookie, result) ;
    if(self->_parent != -1) {
      //delete(self);
      self->clear();
      __global.heap_mm->free(self);
    }
  }
}

/* **************************************************************** */
/*               reception header handler                           */
/* **************************************************************** */
template <class AMHeader, class Derived, class T_NI>
xlpgas_local_addr_t AMExchange<AMHeader,Derived, T_NI>::cb_p_incoming (const struct xlpgas_AMHeader_t * hdr,
             void (** completionHandler)(void *, void *),
             void ** arg)
{
  //here we are if when message from a parent arrives;
  AMHeader * header = (AMHeader *) hdr;
  header->hdr.headerlen &= 0xFFFF;//??? ask George why we need to embed context here in the amsend
  int ctxt = header->dest_ctxt;
  int team_id = header->team_id;

  // we allocate a new instance of the AM Exchange class to hold the
  // state until children return their result; to be freed in the
  // completion handler of AM return child (or equivalen if leaf)
  void* temp_header_space = __global.heap_mm->malloc(header->hdr.headerlen);
  assert(temp_header_space != NULL);
  memcpy(temp_header_space, (void*)hdr, header->hdr.headerlen);

  //alloc a new state with malloc; new is not allowed in the rts because we don't link c++
  //standard library
   derived_type* temp_state = (derived_type *) __global.heap_mm->malloc (sizeof(derived_type));
   assert (temp_state != NULL);
   memset (temp_state, 0, sizeof(derived_type));
   new (temp_state) derived_type (ctxt, team_id, (AMHeader*)temp_header_space, header->senderID);
   //derived_type* temp_state = new derived_type(ctxt, team_id, (AMHeader*)temp_header_space, header->senderID);
  temp_state->set_parent(header->parent_state);
  if (temp_state == NULL)
    xlpgas_fatalerror (-1, "%d: AMExchange/v: reached an invalid state", XLPGAS_MYNODE);
  //printf("L%d :FROM PARENT INcomming Parent=%d  Len=%d\n", XLPGAS_MYNODE, header->senderID, header->len);
  *completionHandler = &derived_type::cb_p_recvcomplete;
  *arg = temp_state;

  //next we invoke the user function for incomming AM the function
  //allocates space for the data; for scatter however we need to
  //allocate some intermediate space; From here we copy into the user
  //buffer
  temp_state->_rlen = header->len;
  void * tbuf = temp_state->parent_incomming();
  temp_state->reset(header->root, tbuf, header->len);//rbuf is send buf for next phase

  assert (temp_state->_rbuf != NULL);
  return (xlpgas_local_addr_t) temp_state->_rbuf;
}

/* **************************************************************** */
/*           active message reception complete                      */
/* **************************************************************** */
template <class AMHeader, class Derived, class T_NI>
void AMExchange<AMHeader,Derived, T_NI>::cb_p_recvcomplete (void * unused, void * arg)
{
  derived_type * self = (derived_type *) arg;
  self->kick();
  TRACE((stderr, "%d: AMExchange: <%d> RECVDONE\n",
         XLPGAS_MYNODE, self->_header.id));
}

template <class AMHeader, class Derived, class T_NI>
xlpgas_local_addr_t AMExchange<AMHeader,Derived, T_NI>::cb_c_incoming (const struct xlpgas_AMHeader_t * hdr,
             void (** completionHandler)(void *, void *),
             void ** arg)
{


  AMHeader * header = (AMHeader *) hdr;
  int ctxt = header->dest_ctxt;
  //retrieve current state from the header
  derived_type* temp_state = (derived_type*)(header->parent_state);
  *arg = temp_state;

  MUTEX_LOCK(&temp_state->_mutex);

  //increment received count; if all children are accounted for then
  //send confirmation to parent
  temp_state->_children_rcv++;
  *completionHandler = &derived_type::cb_c_recvcomplete;
  xlpgas_local_addr_t addr = (xlpgas_local_addr_t)( (char*)(temp_state->_temp) + temp_state->_off_rbuf);
  temp_state->_off_rbuf += header->len;

  assert(addr != NULL);
  //printf("L%d :FROM CHILD incomming _len = %d _temp=%x [%d %d] addr=%x\n", XLPGAS_MYNODE, header->len, temp_state->_temp, temp_state->_children_rcv, temp_state->_children, addr);
  MUTEX_UNLOCK(&temp_state->_mutex);
  return addr;
}

template <class AMHeader, class Derived, class T_NI>
void AMExchange<AMHeader,Derived, T_NI>::cb_c_recvcomplete (void * unused, void * arg)
{
  bool send=false;
  derived_type * self = (derived_type *) arg;
  //printf("L%d CHILD RECV COMPLETE=%d [%d, %d]\n", XLPGAS_MYNODE, self->_rcvcount, self->_children);

  MUTEX_LOCK(&self->_mutex);
  self->_rcvcount++;
  if(self->_rcvcount == self->_children) send = true;
  MUTEX_UNLOCK(&self->_mutex);

  // send confirmation to parent; only one handling thread will succed here
  if(send) {
    //perform reduction function in a critical section
    //when I got all results
    self->merge_data();
    if(self->_parent != -1){
      xlpgas_endpoint_t dst = self->_comm->index2Endpoint (self->_parent);
// #warning, removed this line
//      ((AMHeader&)(self->_header->hdr)).dest_ctxt = dst.ctxt;
      self->_header->parent_state = self->_parent_state;
      self->_header->hdr.handler   = self->FROMCHILD_AM;
      self->_header->len     = self->_rlen;
      xlpgas_tsp_amsend (self->_ctxt,
			 dst,
			 &(self->_header->hdr),
			 (xlpgas_local_addr_t) self->_rbuf,
			 self->_rlen,
			 cb_top_senddone,
			 self);
      }
  }
}
} /* Xlpgas */

#include "algorithms/protocols/tspcoll/AMBcast.h"
#include "algorithms/protocols/tspcoll/AMReduce.h"
#include "algorithms/protocols/tspcoll/AMGather.h"

#endif /* __xlpgas_AMExchange_h__ */
