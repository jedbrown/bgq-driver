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
 * \file algorithms/protocols/allreduce/ProtocolFactoryT.h
 * \brief ???
 */
#ifndef __algorithms_protocols_allreduce_ProtocolFactoryT_h__
#define __algorithms_protocols_allreduce_ProtocolFactoryT_h__

#include "algorithms/protocols/CollectiveProtocolFactoryT.h"
#include "util/trace.h"

#ifdef CCMI_TRACE_ALL
  #define DO_TRACE_ENTEREXIT 1
  #define DO_TRACE_DEBUG     1
#else
  #define DO_TRACE_ENTEREXIT 0
  #define DO_TRACE_DEBUG     0
#endif


namespace CCMI
{
namespace Adaptor
{
namespace Allreduce
{
template <class T_Composite, MetaDataFn get_metadata, class T_Conn, pami_xfer_type_t T_XFER_TYPE=PAMI_XFER_COUNT>
class ProtocolFactoryT: public CollectiveProtocolFactoryT<T_Composite, get_metadata, T_Conn, T_XFER_TYPE >
{
  T_Conn                           *_bcmgr;
  bool                              _isAsync;

public:
    ProtocolFactoryT (pami_context_t               ctxt,
                      size_t                       ctxt_id,
                      pami_mapidtogeometry_fn      cb_geometry,
                      T_Conn                      *rcmgr,
                      Interfaces::NativeInterface *native,
                      pami_dispatch_multicast_function cb_head = NULL,
		      T_Conn                      *bcmgr=NULL):
      CollectiveProtocolFactoryT<T_Composite, get_metadata, T_Conn, T_XFER_TYPE >(ctxt,ctxt_id,cb_geometry,rcmgr, native, cb_head)
    {
        TRACE_FN_ENTER();
        TRACE_FORMAT("%p", this);

	_bcmgr = bcmgr;
	_isAsync = false;

        TRACE_FN_EXIT();
    }

    Interfaces::NativeInterface * native()
    {
      return CollectiveProtocolFactoryT<T_Composite, get_metadata, T_Conn, T_XFER_TYPE >::_native;
    }
    void setAsync () { _isAsync = true; }

    virtual ~ProtocolFactoryT ()
    {
        TRACE_FN_ENTER();
        TRACE_FORMAT("%p", this);
        TRACE_FN_EXIT();
    }

    /// NOTE: This is required to make "C" programs link successfully with virtual destructors
    void operator delete(void * p)
    {
        CCMI_abort();
    }

    virtual Executor::Composite * generate(pami_geometry_t              g,
                                           void                      * cmd)
    {
        TRACE_FN_ENTER();
        PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *)g;
	
	unsigned iteration = 0;
        size_t contextid = CollectiveProtocolFactoryT<T_Composite, get_metadata, T_Conn, T_XFER_TYPE >::_native->contextid();
	if (_isAsync)
          iteration = geometry->getAllreduceIteration(contextid);

	//fprintf (stderr, "Starting collective on iteration %d\n", iteration);

        CCMI::Executor::Composite * arcomposite = (CCMI::Executor::Composite *) geometry->getAllreduceComposite(contextid,iteration);
        TRACE_FORMAT("%p composite %p", this,arcomposite);

        pami_xfer_t *allreduce = (pami_xfer_t *)cmd;

        void* ptr = NULL;
        if(arcomposite)
          ptr = arcomposite->getAlgorithmFactory();
	
        ///If the allreduce algorithm was created by this factory before, just restart it
        if (arcomposite != NULL && arcomposite->getAlgorithmFactory() == this)
        {
	    //fprintf(stderr, "Call composite %p restart\n", arcomposite);
            pami_result_t status = (pami_result_t)arcomposite->restart(allreduce);

            if (status == PAMI_SUCCESS)
            {
              geometry->setAllreduceComposite(contextid,arcomposite, iteration);
              if (_isAsync)
                  geometry->incrementAllreduceIteration(contextid);

              TRACE_FN_EXIT();
              return NULL;
            }
        }

        if (arcomposite != NULL) // Different factory?  Cleanup old executor.
        {
	  geometry->setAllreduceComposite(contextid,NULL, iteration);
	  arcomposite->cleanup(); //Call destructor
	  CollectiveProtocolFactory *factory = (CollectiveProtocolFactory *) arcomposite->getAlgorithmFactory();
	  factory->freeObject(arcomposite);
        }

        T_Composite* obj = (T_Composite*)CollectiveProtocolFactoryT<T_Composite, get_metadata, T_Conn, T_XFER_TYPE >::_alloc.allocateObject();
        //fprintf(stderr, "%p composite %p\n", this, obj);
        geometry->setAllreduceComposite(contextid,obj, iteration);

        new (obj) T_Composite(this->_context,
                              this->_context_id,
			      CollectiveProtocolFactoryT<T_Composite, get_metadata, T_Conn, T_XFER_TYPE >::_native,  // Native interface
                              CollectiveProtocolFactoryT<T_Composite, get_metadata, T_Conn, T_XFER_TYPE >::_cmgr,    // Connection Manager
			      _bcmgr,
                              this,
                              geometry,          // Geometry Object
                              (pami_xfer_t*) cmd, // Parameters
                              allreduce->cb_done,
                              allreduce->cookie);
        if (_isAsync)
          geometry->incrementAllreduceIteration(contextid);

        obj->start();

        TRACE_FN_EXIT();
        return NULL;
    }

    T_Conn *getBcastConnMgr() { return _bcmgr; }
 };
};
};
}; //CCMI

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
