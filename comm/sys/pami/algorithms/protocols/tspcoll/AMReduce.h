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
 * \file algorithms/protocols/tspcoll/AMReduce.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_AMReduce_h__
#define __algorithms_protocols_tspcoll_AMReduce_h__

namespace xlpgas
{
 template <class T_NI>
 class AMReduce : public AMExchange<AMHeader_reduce, AMReduce<T_NI>, T_NI>
  {
    typedef AMExchange<AMHeader_reduce, AMReduce<T_NI>, T_NI> base_type;
  public:
    void * operator new (size_t, void * addr) { return addr; }
    AMReduce (int ctxt, int team_id, AMHeader_reduce* ucb, int parent=-1)  :
      base_type (ctxt,team_id,ucb, parent) {
	this->_ret_values = true;
	this->_header->hdr.handler = XLPGAS_TSP_AMREDUCE_PREQ;
	this->FROMCHILD_AM         = XLPGAS_TSP_AMREDUCE_CREQ;
    }

    virtual void reset (int root, const void * sbuf, unsigned nbytes);
    virtual void root(void);
    virtual void* parent_incomming(void);
    virtual void allocate_space_result(void);
    virtual void merge_data (void);
  }; /* AMReduce */
} /* Xlpgas */

#endif /* __xlpgas_AMReduce_h__ */
