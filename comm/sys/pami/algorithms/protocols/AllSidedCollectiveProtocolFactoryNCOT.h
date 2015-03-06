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
 * \file algorithms/protocols/AllSidedCollectiveProtocolFactoryNCOT.h
 * \brief Allsided - No Collective Object (NCO), No allocator 
 */
#ifndef __algorithms_protocols_AllSidedCollectiveProtocolFactoryNCOT_h__
#define __algorithms_protocols_AllSidedCollectiveProtocolFactoryNCOT_h__

#include "algorithms/protocols/CollectiveProtocolFactory.h"
#include "algorithms/interfaces/NativeInterface.h"
#include "components/memory/MemoryAllocator.h"

#include "util/ccmi_debug.h"
#include "util/ccmi_util.h"

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
    template <class T_Composite, MetaDataFn get_metadata, class T_Conn, pami_xfer_type_t T_XFER_TYPE=PAMI_XFER_COUNT>
    class AllSidedCollectiveProtocolFactoryNCOT: public CollectiveProtocolFactory
    {

    public:
      AllSidedCollectiveProtocolFactoryNCOT (pami_context_t ctxt,
                                             size_t         ctxt_id,
                                             pami_mapidtogeometry_fn     cb_geometry,
                                             T_Conn              * cmgr,
                                             Interfaces::NativeInterface * native):
      CollectiveProtocolFactory(ctxt,ctxt_id,cb_geometry),
      _native(native),
      _cmgr(cmgr),
      _pnative(native)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p> native %p",this, native);
        DO_DEBUG((templateName<MetaDataFn>()));
        TRACE_FN_EXIT();
      }
      AllSidedCollectiveProtocolFactoryNCOT (pami_context_t ctxt,
                                             size_t         ctxt_id,
                                             pami_mapidtogeometry_fn     cb_geometry,
                                             T_Conn              * cmgr,
                                             Interfaces::NativeInterface ** native):
      CollectiveProtocolFactory(ctxt,ctxt_id,cb_geometry),
      _native(NULL),
      _cmgr(cmgr),
      _pnative((Interfaces::NativeInterface *)native) /// \todo fix the NI array ctor's
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p> native %p",this, native);
        DO_DEBUG((templateName<MetaDataFn>()));
        TRACE_FN_EXIT();
      }

      virtual ~AllSidedCollectiveProtocolFactoryNCOT ()
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>",this);
        TRACE_FN_EXIT();
      }

      /// NOTE: This is required to make "C" programs link successfully with virtual destructors
      void operator delete(void * p)
      {
        CCMI_abort();
      }

      static void cleanup_done_fn(pami_context_t  context,
                                  void           *clientdata,
                                  pami_result_t   res)
      {
        TRACE_FN_ENTER();
        T_Composite *obj = (T_Composite *)clientdata;
        obj->~T_Composite();
        __global.heap_mm->free(obj);
        TRACE_FN_EXIT();
      }
      virtual Executor::Composite * generate(pami_geometry_t             geometry,
                                             void                      * cmd)
      {
        TRACE_FN_ENTER();
        T_Composite *p;
        pami_result_t rc;
        rc = __global.heap_mm->memalign((void **)&p,16,sizeof(T_Composite));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _temp_topo");
        TRACE_FORMAT("<%p> composite %p",this, p);
        new(p) T_Composite(_pnative,_cmgr,geometry,(pami_xfer_t*)cmd,((pami_xfer_t*)cmd)->cb_done,((pami_xfer_t*)cmd)->cookie );
        TRACE_FN_EXIT();
        return(Executor::Composite *)(p);
      }

      virtual void metadata(pami_metadata_t *mdata, pami_geometry_t geometry = PAMI_GEOMETRY_NULL)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>",this);
        DO_DEBUG((templateName<MetaDataFn>()));
        get_metadata(mdata);
        if(_native) _native->metadata(mdata,T_XFER_TYPE);
        TRACE_FN_EXIT();
      }
    private:
      Interfaces::NativeInterface                * _native;
      T_Conn                                     * _cmgr;
      Interfaces::NativeInterface                * _pnative;
      PAMI::MemoryAllocator<sizeof(T_Composite), 16>   _alloc;
    };//AllSidedCollectiveProtocolFactoryNCOT

  };//Adaptor
};//CCMI

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
