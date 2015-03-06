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
 * \file algorithms/protocols/CollectiveProtocolFactory.h
 * \brief ???
 */

#ifndef __algorithms_protocols_CollectiveProtocolFactory_h__
#define __algorithms_protocols_CollectiveProtocolFactory_h__

#include  "algorithms/interfaces/Executor.h"
#include  "algorithms/composite/Composite.h"
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
    class CollectiveProtocolFactory
    {
    public:
      CollectiveProtocolFactory ():
      _cb_geometry(NULL),
      _context(NULL),
      _context_id(-1UL),
      _generation_id(-1),
      _cached_commid((unsigned)-1),
      _cached_geometry(NULL)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>",this);
        TRACE_FN_EXIT();
      }
      static void cleanup_done_fn(pami_context_t  context,
                                  void           *clientdata,
                                  pami_result_t   res)
      {
        (void)context;(void)clientdata;(void)res;
        PAMI_abort();
      }


      inline CollectiveProtocolFactory (pami_context_t          ctxt,
                                        size_t                  ctxt_id,
                                        pami_mapidtogeometry_fn cb_geometry):
      _cb_geometry(cb_geometry),
      _context(ctxt),
      _context_id(ctxt_id),
      _cached_commid((unsigned) -1),
      _cached_geometry(NULL)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>",this);
        TRACE_FN_EXIT();
      }

      pami_geometry_t getGeometry(pami_context_t ctxt,
                                  unsigned       id)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p> ctxt %p, id %u, cached id %u, cached geometry %p",this,ctxt,id,_cached_commid,_cached_geometry);

        if(id != _cached_commid)
        {
          _cached_commid = id;
          CCMI_assert (_cb_geometry != NULL);
          _cached_geometry =  _cb_geometry (ctxt, id);
          if(_cached_geometry == NULL) // likely parentless and early/unexpected, so clear the cache
            _cached_commid = (unsigned)-1;
        }
        TRACE_FORMAT("<%p> geometry %p",this,_cached_geometry);
        TRACE_FN_EXIT();
        return _cached_geometry;
      }
      virtual void clearCache()
      {
        _cached_commid = (unsigned)-1;
        _cached_geometry = NULL;
      }

      virtual ~CollectiveProtocolFactory ()
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>",this);
        TRACE_FN_EXIT();
      }
      /// NOTE: This is required to make "C" programs link successfully with virtual destructors
      void operator delete(void * p)
      {
        (void)p;
        CCMI_abort();
      }

      virtual Executor::Composite * generate(pami_geometry_t              geometry,
                                             void                      * cmd) = 0;

      virtual void metadata(pami_metadata_t *mdata, pami_geometry_t geometry = PAMI_GEOMETRY_NULL)=0;
      virtual uint32_t nameHash(int generation_id=-1, pami_geometry_t geometry = PAMI_GEOMETRY_NULL)
      {
        uint32_t hash =0;          
        pami_metadata_t md;
        metadata(&md, geometry);
        size_t len = strlen(md.name);
        init_hash(&hash);
        update_hash(&hash,md.name,len);
        finalize_hash(&hash);
        if(generation_id != -1)
          _generation_id = generation_id;
        return hash;          
      }
      virtual int getGenerationId()
      {
        return _generation_id;
      }
      virtual void setAsyncInfo (bool                          is_buffered,
                                 pami_dispatch_callback_function cb_async,
                                 pami_mapidtogeometry_fn        cb_geometry)
      {
        (void)is_buffered;(void)cb_async;(void)cb_geometry;
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>",this);
        TRACE_FN_EXIT();
        PAMI_abort();
      };

      virtual void freeObject(void *object) {
	//This method is overridden by sub-class factories
	CCMI_abort();
      }

      inline pami_context_t getContext()
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p> _context %p",this,_context);
        TRACE_FN_EXIT();
        return _context;
      }
      inline size_t getContextId()
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p> _context_id %zu",this,_context_id);
        TRACE_FN_EXIT();
        return _context_id;
      }
    protected:
      pami_mapidtogeometry_fn              _cb_geometry;
      pami_context_t                       _context;
      size_t                               _context_id;
      int                                  _generation_id;
      unsigned                             _cached_commid;
      pami_geometry_t                      _cached_geometry;
    };
  };
};


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
