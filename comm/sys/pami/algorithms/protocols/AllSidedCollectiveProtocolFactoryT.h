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
 * \file algorithms/protocols/AllSidedCollectiveProtocolFactoryT.h
 * \brief ???
 */
#ifndef __algorithms_protocols_AllSidedCollectiveProtocolFactoryT_h__
#define __algorithms_protocols_AllSidedCollectiveProtocolFactoryT_h__

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
    class AllSidedCollectiveProtocolFactoryT: public CollectiveProtocolFactory
    {
      class collObj
      {
      public:
        collObj(pami_context_t                            ctxt,
                size_t                                    ctxt_id,
                Interfaces::NativeInterface             * native,
                T_Conn                                  * cmgr,
                pami_geometry_t                           geometry,
                pami_xfer_t                             * cmd,
                pami_event_function                       fn,
                void                                    * cookie,
                AllSidedCollectiveProtocolFactoryT      * factory):
        _factory(factory),
        _user_done_fn(cmd->cb_done),
        _user_cookie(cmd->cookie),
        _obj(ctxt, ctxt_id, native, cmgr, geometry, cmd, fn, cookie)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>",this);
          DO_DEBUG((templateName<T_Composite>()));
          TRACE_FN_EXIT();
        }

        AllSidedCollectiveProtocolFactoryT * _factory;
        pami_event_function                  _user_done_fn;
        void                               * _user_cookie;
        T_Composite                          _obj;
      };


    public:
      AllSidedCollectiveProtocolFactoryT (pami_context_t ctxt,
					  size_t         ctxt_id,
					  pami_mapidtogeometry_fn     cb_geometry,
					  T_Conn              * cmgr,
                                          Interfaces::NativeInterface * native):
      CollectiveProtocolFactory(ctxt, ctxt_id, cb_geometry),
      _cmgr(cmgr),
      _pnative(native),
      _native(native)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p> native %p",this, native);
        DO_DEBUG((templateName<MetaDataFn>()));
        TRACE_FN_EXIT();
      }
      AllSidedCollectiveProtocolFactoryT (T_Conn              * cmgr,
                                          Interfaces::NativeInterface ** native):
      CollectiveProtocolFactory(),
      _cmgr(cmgr),
      _pnative((Interfaces::NativeInterface *)native), /// \todo fix the NI array ctor's
      _native(NULL)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p> native %p",this, native);
        DO_DEBUG((templateName<MetaDataFn>()));
        TRACE_FN_EXIT();
      }

      virtual ~AllSidedCollectiveProtocolFactoryT ()
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

      static void done_fn(pami_context_t  context,
                          void          * clientdata,
                          pami_result_t   res)
      {
        TRACE_FN_ENTER();
        collObj *cobj = (collObj *)clientdata;
        TRACE_FORMAT("<%p> context %p,result %u) factory %p context %p\n",
                     cobj,context,res, cobj->_factory,cobj->_factory->getContext());
        cobj->_user_done_fn(context?context:cobj->_factory->getContext(), cobj->_user_cookie, res);
        cobj->_factory->_alloc.returnObject(cobj);
        TRACE_FN_EXIT();
      }

      static void cleanup_done_fn(pami_context_t  context,
                                  void           *clientdata,
                                  pami_result_t   res)
      {
        TRACE_FN_ENTER();
        T_Composite *obj = (T_Composite *)clientdata;
        collObj *cobj = (collObj *)obj->getCollObj();

        cobj->~collObj();
        cobj->_factory->_alloc.returnObject(cobj);
        TRACE_FN_EXIT();
      }

      virtual Executor::Composite * generate(pami_geometry_t             geometry,
                                             void                      * cmd)
      {
        TRACE_FN_ENTER();
        collObj *cobj = (collObj*)  _alloc.allocateObject();
        TRACE_FORMAT("<%p> cobj %p",this, cobj);
        new(cobj) collObj(this->_context,
                          this->_context_id,
                          _pnative,          // Native interface
                          _cmgr,            // Connection Manager
                          geometry,         // Geometry Object
                          (pami_xfer_t*)cmd,// Parameters
                          done_fn,          // Intercept function
                          cobj,             // Intercept cookie
                          this);            // Factory

        //We do not override completion callbacks
        //as they must free memory
        TRACE_FN_EXIT();
        return(Executor::Composite *)(&cobj->_obj);
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
      T_Conn                                     * _cmgr;
      Interfaces::NativeInterface                * _pnative;
      Interfaces::NativeInterface                * _native;
      PAMI::MemoryAllocator<sizeof(collObj), 16>   _alloc;
    };//AllSidedCollectiveProtocolFactoryT

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
