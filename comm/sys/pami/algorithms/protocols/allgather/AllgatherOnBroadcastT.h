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
/// \file algorithms/protocols/allgather/AllgatherOnBroadcastT.h
/// \brief ???
///
#ifndef __algorithms_protocols_allgather_AllgatherOnBroadcastT_h__
#define __algorithms_protocols_allgather_AllgatherOnBroadcastT_h__

#include "algorithms/ccmi.h"
#include "algorithms/protocols/CollectiveProtocolFactoryT.h"
#include "math/Memcpy.x.h"

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
    namespace Allgather
    {
      static const void* ALLGV_IN_PLACE = (void *) (-1);

      ///
      /// \brief Asyc Allgather Composite.
      ///
      template <int NBCAST, int NCOLORS, class T_Bcast, class T_Conn,  PAMI::Geometry::topologyIndex_t T_Geometry_Index, bool T_Allgv, bool T_Int>
      class AllgatherOnBroadcastT: public Executor::Composite
      {
      protected:
        uint32_t                      _ncomplete;
        uint32_t                      _nranks;
        uint32_t                      _cur_nbcast;
        uint32_t                      _nIterDone;
        pami_event_function           _fn;
        void                        * _cookie;
        PAMI_GEOMETRY_CLASS         * _geometry;
        Interfaces::NativeInterface * _native;
        T_Conn                        _cmgr[NBCAST];
        pami_xfer_t                   _cmd;
        T_Bcast                       _bcast[NBCAST];
        pami_context_t                _context;
        pami_work_t                   _work;

      public:
        ///
        /// \brief Constructor
        ///
        AllgatherOnBroadcastT ()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>", this);
          TRACE_FN_EXIT();
        };

        AllgatherOnBroadcastT (pami_context_t               ctxt,
                               size_t                       ctxt_id,
                               Interfaces::NativeInterface              * native,
                               T_Conn                                   * cmgr,
                               pami_geometry_t                             g,
                               void                                     * cmd,
                               pami_event_function                         fn,
                               void                                     * cookie):
        Executor::Composite(),
        _ncomplete(0), 
	  _nranks(((PAMI::Topology*)((PAMI_GEOMETRY_CLASS*)g)->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX))->size()), 
        _cur_nbcast(0),
        _fn (fn),
        _cookie (cookie),
        _geometry((PAMI_GEOMETRY_CLASS *)g),
        _native(native),
        _cmd (*(pami_xfer_t *)cmd),
        _context(ctxt)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p> _nranks %u, geometry %p, native %p", this, _nranks, g, native);
          // allgatherv requires a ranklist, so make sure it's been created.
	  for (unsigned i = 0; i < _nranks; i++)
	  {
	    if (((PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX))->index2Endpoint(i) == _native->endpoint())
	    {
	      size_t rtypecounts;
	      PAMI::Type::TypeCode * rtype;
	      PAMI::Type::TypeCode * stype;
	      getAllgatherInfo (&stype);
	      char *src, *dst;
	      getAllgatherInfo (i, rtypecounts, &dst, &rtype);
	      src = getSrcBuf();
	      
	      unsigned bytes = rtypecounts * rtype->GetDataSize();
	      if (src && (src != (char *)ALLGV_IN_PLACE))
		//Core_memcpy(dst, src, bytes);
		PAMI_Type_transform_data((void*)src,
					 stype, 0,
					 dst,
					 rtype, 0,
					 bytes,
					 PAMI_DATA_COPY, NULL);
	      break;
	    }	      
	  }

          nextStep();
          TRACE_FN_EXIT();
        }

        char *getSrcBuf () 
        {
          if (T_Allgv && !T_Int) {
            return _cmd.cmd.xfer_allgatherv.sndbuf;
          } 
          else if (T_Allgv) {
            return _cmd.cmd.xfer_allgatherv_int.sndbuf;
          }
          else {
            return _cmd.cmd.xfer_allgather.sndbuf;
          }
        }

        void getAllgatherInfo ( size_t      index,
				size_t    & rtypecounts,  
				char     ** dst,
				PAMI::Type::TypeCode ** rtype ) 
        {
          if (T_Allgv && !T_Int) {
            rtypecounts = _cmd.cmd.xfer_allgatherv.rtypecounts[index];
            *dst        = _cmd.cmd.xfer_allgatherv.rcvbuf + 
                          _cmd.cmd.xfer_allgatherv.rdispls[index] * 
                          ((PAMI::Type::TypeCode *)(_cmd.cmd.xfer_allgatherv.rtype))->GetDataSize();
	    //For non-inplace allgvs the src has been copied to dst on root
            *rtype      = (PAMI::Type::TypeCode *)_cmd.cmd.xfer_allgatherv.rtype;
          }
          else if (T_Allgv) {
            rtypecounts = _cmd.cmd.xfer_allgatherv_int.rtypecounts[index];
            *rtype    = (PAMI::Type::TypeCode *)_cmd.cmd.xfer_allgatherv_int.rtype;
            *dst        = _cmd.cmd.xfer_allgatherv_int.rcvbuf + _cmd.cmd.xfer_allgatherv_int.rdispls[index] * ((*rtype)->GetDataSize());
          }
          else {
	    rtypecounts = _cmd.cmd.xfer_allgather.rtypecount;
            *rtype    = (PAMI::Type::TypeCode *)_cmd.cmd.xfer_allgather.rtype;
            *dst      = _cmd.cmd.xfer_allgather.rcvbuf + rtypecounts * index * ((*rtype)->GetDataSize());
          }
        }

        void getAllgatherInfo (PAMI::Type::TypeCode ** stype )
        {
          if (T_Allgv && !T_Int) {
            *stype      = (PAMI::Type::TypeCode *)_cmd.cmd.xfer_allgatherv.stype;
          }
          else if (T_Allgv) {
            *stype    = (PAMI::Type::TypeCode *)_cmd.cmd.xfer_allgatherv_int.stype;
          }
          else {
            *stype    = (PAMI::Type::TypeCode *)_cmd.cmd.xfer_allgather.stype;
          }
        }

        void nextStep ()
        {
          TRACE_FN_ENTER();
          unsigned i = 0, nc = 0;
          unsigned ncomplete = _ncomplete;
          unsigned connids[NCOLORS];
          unsigned colors[NCOLORS];

          for (i = 0; i < NCOLORS; ++i)
            connids[i] = i;

          //fprintf (stderr, "Start next step\n");

          //Allow each bcast to have atleast 3 colors
          for (i = 0; (i < NBCAST) && (ncomplete < _nranks) && nc < NCOLORS; ncomplete++)
          {
            size_t root, rtypecounts;
            char *dst;
            PAMI::Type::TypeCode * rtype;
            getAllgatherInfo (ncomplete, rtypecounts, &dst, &rtype);
	    
	    if (rtypecounts == 0) //nothing to do skip
	      continue;

            root        = ((PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX))->index2Endpoint(ncomplete);
            //fprintf(stderr, "Starting collective on color %d\n", nc);
	    
#if 0
           //Since executors are not mallicious and only use specified colors
           //We dont need to reset the colormap
            _cmgr[i].reset();
#endif
            new (&_bcast[i]) T_Bcast(_native,&_cmgr[i],_geometry, done, this);
            unsigned c = NCOLORS - nc;
            _bcast[i].getColors
            ((PAMI::Topology*)_geometry->getTopology(T_Geometry_Index), 
            rtypecounts * rtype->GetDataSize(),
            colors,
            c);
            _cmgr[i].setConnections(colors, &connids[nc], c);
	    
	    //printf("index %d rtypecounts %d dst %p c %d\n",
	    //   (int)ncomplete, (int)rtypecounts, dst, (int)NCOLORS-nc);
	    
            int rc = _bcast[i].initialize (root, rtypecounts, rtype, dst, 
					   dst, NCOLORS-nc);
            if (rc != 0)
              break;
            
            c = _bcast[i].getNumColors();
            nc += c;	    
	    
	    i ++; //only increment if a bcast was scheduled

            TRACE_FORMAT("<%p> nextStep _cmgr[%u]=%p, _bcast[%u]=%p numcolors %d cur colors %d\n", this,i,&_cmgr[i],i,&_bcast[i], nc, c);	    
          }

	  if (i > 0) { //We scheduled a few bcasts
	    _cur_nbcast = i;
	    _nIterDone  = i;

	    int nb = ncomplete - _ncomplete;
	    _ncomplete += nb - i; //skipped bcasts
	    
	    //if (T_BARRIER) {
	    CCMI::Executor::Composite  *barrier =  
	    /// \todo does NOT support multicontext
	    (CCMI::Executor::Composite *) _geometry->getKey((size_t)0, 
							    PAMI::Geometry::CKEY_OPTIMIZEDBARRIERCOMPOSITE);
	    barrier->setDoneCallback(cb_barrier_done, this);
	    barrier->start();
	    //}
	    //else {
	    //for (i = 0; i < _cur_nbcast; i++) 
	    //_bcast[i].start();
	    //}
	  }
	  else {
	    _ncomplete = ncomplete; 
	    if (_ncomplete == _nranks) 
	      _fn (_context, _cookie, PAMI_SUCCESS);	      
	    else
	      nextStep();
	  }
          TRACE_FN_EXIT();
        }

        void reset ()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>", this);
          _ncomplete = 0;
          TRACE_FN_EXIT();
        }

        void restart ()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>", this);
          reset ();
          nextStep();
          TRACE_FN_EXIT();
        }
      
        static void cb_barrier_done (pami_context_t   context,
                                     void           * cookie,
                                     pami_result_t    result) 
        {
          TRACE_FN_ENTER();
          AllgatherOnBroadcastT<NBCAST, NCOLORS, T_Bcast, T_Conn, T_Geometry_Index, T_Allgv, T_Int> *allg = (AllgatherOnBroadcastT<NBCAST, NCOLORS, T_Bcast, T_Conn, T_Geometry_Index, T_Allgv, T_Int> *) cookie;
          //fprintf (stderr, "cb_barrier_done\n");
          for (unsigned i = 0; i < allg->_cur_nbcast; i++)
          //Calls cb_barrier_done in the composite
          //via external barrier path
            allg->_bcast[i].start(); 
	  
          TRACE_FN_EXIT();
        }

        static void done (pami_context_t   context,
                          void           * cookie,
                          pami_result_t    result) 
        {
          TRACE_FN_ENTER();
          AllgatherOnBroadcastT<NBCAST, NCOLORS, T_Bcast, T_Conn, T_Geometry_Index, T_Allgv, T_Int> *allg = (AllgatherOnBroadcastT<NBCAST, NCOLORS, T_Bcast, T_Conn, T_Geometry_Index, T_Allgv, T_Int> *) cookie;
          //fprintf(stderr, "<%p> ncomplete %d, nranks %d _fn %p\n", allg, allg->_ncomplete, allg->_nranks, allg->_fn);

          ++allg->_ncomplete;
          --allg->_nIterDone;

          if (allg->_nIterDone > 0)
            return;

          if (allg->_ncomplete < allg->_nranks) 
          {
            //allg->nextStep();
            allg->_native->postWork(allg->_context, 0, &allg->_work,
				    advanceNext, allg);
          }
          else
            allg->_fn (context, allg->_cookie, PAMI_SUCCESS);
          TRACE_FN_EXIT();
        }

        static pami_result_t advanceNext (pami_context_t     context,
					  void             * cookie) 
        {
          AllgatherOnBroadcastT<NBCAST, NCOLORS, T_Bcast, T_Conn, T_Geometry_Index, T_Allgv, T_Int> *allg = (AllgatherOnBroadcastT<NBCAST, NCOLORS, T_Bcast, T_Conn, T_Geometry_Index, T_Allgv, T_Int> *) cookie;
	  
          allg->nextStep();
          return PAMI_SUCCESS;
        }
      };
    };
  };
};

#endif
