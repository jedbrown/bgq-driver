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
 * \file algorithms/protocols/tspcoll/AMBcast.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/AMExchange.h" // this includes AMBcast.h

template<class T_NI>
void xlpgas::AMBcast<T_NI>::reset (int root, const void * s, unsigned l)
{
  this->_header->root   = root;
  this->_root           = root;
  this->_rcvcount       = 0;
  this->_sndcount       = 0;
  this->_sbuf           = s;
  this->_rbuf = const_cast<void*>(s);
  this->_slen           = l;
  this->_rlen           = l;
  this->_ret_values      = false;
}
template<class T_NI>
void xlpgas::AMBcast<T_NI>::root(void){
// #warning "Lookup User Fcn"
#if 0
  size_t res_size;

  (*(xlpgas_tsp_amcoll_lookup(_header->user_cb).ambcast))(_ctxt,_team_id, _root, _rlen, NULL, 0, &_rbuf, NULL, &res_size,&_user_cb_received, &_cookie);
  //printf("L%d ROOT=[%x]\n", XLPGAS_MYNODE, _cookie);
  memcpy (this->_rbuf, this->_sbuf, this->_rlen);
#endif
}
template<class T_NI>
void* xlpgas::AMBcast<T_NI>::parent_incomming(void){
// #warning "Lookup User Fcn"
#if 0

  size_t res_size;
  void * tbuf;
  (*(xlpgas_tsp_amcoll_lookup(_header->user_cb).ambcast))(_ctxt, _team_id, _root, _rlen, NULL, 0, &tbuf, NULL, &res_size, &_user_cb_received, &_cookie);
  //printf("L%d PI= [%x]\n", XLPGAS_MYNODE, _cookie);
  return tbuf;
#else
  return NULL;
#endif
}

////////////////////////////////////////////////////////////
//     BCAST Implemented using AMBcast<T_NI>
////////////////////////////////////////////////////////////
#if 0
void *_xlpgas_internal_buf;
bool  _xlpgas_internal_bcast_done;

void _xlpgas_internal_cb_ambcast_done (void *context, void * clientdata, pami_result_t err)
{
  _xlpgas_internal_bcast_done=true;
}

void _xlpgas_internal_cb_bcast_rcv  (int                   context,
				     int                   teamid,
				     size_t                root,
				     const size_t          sndlen,
				     const void          * user_header,
				     const size_t          headerlen,
				     void               ** rcvbuf,
				     void*               * rtype,
				     size_t              * rtypecount,
				     xlpgas_event_function * const cb_info,
				     void                ** cookie)
{
  //printf("L%d buffer to write [%x]\n", XLPGAS_MYNODE, _xlpgas_internal_buf);
  *rcvbuf                        = _xlpgas_internal_buf;
  *rtypecount                    = sndlen;
  *cb_info                       = _xlpgas_internal_cb_ambcast_done;
  *cookie                        = (void*) * rcvbuf;
}

void xlpgas::bcast_tree_collective_reg(){
  //for bcast implemented using ambcast;
// #warning "Fix Registration"
#if 0
  xlpgas_dispatch_function dfb;
  dfb.ambcast=_xlpgas_internal_cb_bcast_rcv;
  xlpgas_tsp_amcoll_reg(XLPGAS_TSP_BCAST_TREE, dfb);
#endif
}

#endif
template<class T_NI>
void xlpgas::BcastTree<T_NI>::reset (int root, const void * sbuf, void* rbuf, unsigned nbytes)
{

  //printf("L%d RESET buffer to write [%x]\n", XLPGAS_MYNODE, rbuf);
  //todo, fix me
  //  _xlpgas_internal_bcast_done=false;
  //  _xlpgas_internal_buf = rbuf;
// #warning "Do Barrier Here?"
#if 0
  xlpgas_tspcoll_barrier(_ctxt, _comm->commID());
#endif
  if(root == (int)this->_comm->ordinal()){
    //only the root need to explicitly build/free this; the rest are handled by AMExchange
    if(a!=NULL) {
      a->clear();
      __global.heap_mm->free(a);
    }
    a = (xlpgas::AMBcast<T_NI> *) __global.heap_mm->malloc (sizeof(xlpgas::AMBcast<T_NI>));
    assert (a != NULL);
    memset (a, 0, sizeof(xlpgas::AMBcast<T_NI>));
    header.hdr.headerlen = sizeof(AMHeader_bcast);
    header.user_cb = XLPGAS_TSP_BCAST_TREE;
    //temporary commented out untill reenabled; this method not
    //available on the topo class passed as argument
    //new (a) xlpgas::AMBcast<T_NI> (this->_ctxt,this->_comm->comm(), &header);
    a->reset (root, sbuf, nbytes);
  }
  else //non root
    a=NULL;
}
template<class T_NI>
void xlpgas::BcastTree<T_NI>::kick()
{
  //only the root start sending;
  if(a != NULL) a->kick();
}

template<class T_NI>
bool xlpgas::BcastTree<T_NI>::isdone (void) const
{
  // #warning Fix me for PAMI
  //  return _xlpgas_internal_bcast_done;
  return false;
}
