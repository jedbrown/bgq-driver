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
 * \file algorithms/geometry/GeometryOptimizer.h
 * \brief ???
 */
#ifndef __algorithms_geometry_GeometryOptimizer_h__
#define __algorithms_geometry_GeometryOptimizer_h__

#include "Geometry.h"
#include "pami.h"

namespace PAMI
{
  namespace Geometry
  {

    template <class T_Geometry>
    class GeometryOptimizer
    {
    public:

      ///
      /// \brief A callback function to be called with the results of the bit-and allreduce.
      /// \param[in] pami context
      /// \param[in] callback clientdata
      /// \param[in] the results of the reduce
      /// \param[in] number of 64bit integers reduced
      /// \param[in] result of operation
      typedef void (*results_function)( size_t          context_id,
                                        void           *cookie,
                                        uint64_t       *reduce_result,
                                        size_t          nelem,
                                        T_Geometry     *geometry,
                                        pami_result_t   result ); 
    protected:

      struct _Reduce_t
      {
        size_t                     context_id;
        results_function           results_fn;
        void                     * cookie;
        size_t                     nelem;
      };

      pami_context_t                     _context;         /// PAMI Context
      Algorithm<T_Geometry>            * _ar_algo;         /// The allreduce algorithm to do the global allreduce
      T_Geometry                       * _geometry;
      std::vector<uint64_t>              _vector_elem;     /// Vector of bit-and reduce elements
      std::vector<_Reduce_t>             _vector_reduce;   /// Vector of corresponding callbacks
      size_t                             _total_nelem;     /// Total elements being reduced
      uint64_t                         * _input;           
      uint64_t                         * _result;
      pami_event_function                _cb_done;
      void                             * _cookie;

    public: 
      GeometryOptimizer<T_Geometry>(pami_context_t                      context,
                                    T_Geometry                         *geometry,
                                    Algorithm<T_Geometry>              *ar_algo,
                                    pami_event_function                 cb_done,
                                    void                               *cookie):  
      _context(context),
      _ar_algo(ar_algo),
      _geometry(geometry),
      _vector_elem(),
      _vector_reduce(),
      _total_nelem(0),
      _input(0),
      _result(0),
      _cb_done(cb_done),
      _cookie(cookie)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>", this);       
        TRACE_FN_EXIT();
      }

      ~GeometryOptimizer<T_Geometry> ()
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p> __input %p, _result %p", this,_input, _result);       
        if (_input)
          __global.heap_mm->free (_input);
        if (_result)
          __global.heap_mm->free (_result);
        TRACE_FN_EXIT();
      }

      T_Geometry *geometry()
      {
        return _geometry;
      }

      void registerWithOptimizer (size_t                    context_id,
                                  uint64_t                * in,
                                  size_t                    nelem,
                                  results_function          results_fn,
                                  void                    * cookie) 
      {
        TRACE_FN_ENTER();
        if (nelem == 0)
          return;

        for (size_t i = 0; i < nelem; ++i)
          _vector_elem.push_back(in[i]);

        _Reduce_t reduce;
        reduce.context_id = context_id;
        reduce.nelem  = nelem;
        reduce.results_fn = results_fn;
        reduce.cookie = cookie;

        _vector_reduce.push_back(reduce);
        _total_nelem += nelem;
        TRACE_FN_EXIT();
      }

      static void optimizer_start  (pami_context_t      context,
                                    void              * cookie,
                                    pami_result_t       result) 
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("context %p, cookie %p, result %u",context, cookie,result);
        GeometryOptimizer<T_Geometry> *go = (GeometryOptimizer<T_Geometry> *)cookie;
        go->start();
        TRACE_FN_EXIT();
      }

      void start()
      {
        TRACE_FN_ENTER();

        if (_vector_elem.size() > 0)
        {
          int rc  = 0;
          rc = __global.heap_mm->memalign((void **)&_result, 16, _total_nelem *sizeof(uint64_t));
          PAMI_assertf(rc == PAMI_SUCCESS, "alloc failed for _result %zd", _total_nelem*sizeof(*_result));
          memset(_result, 0, _total_nelem * sizeof(uint64_t));
          rc = __global.heap_mm->memalign((void **)&_input, 16, _total_nelem *sizeof(uint64_t));
          PAMI_assertf(rc == PAMI_SUCCESS, "alloc failed for _input %zd", _total_nelem*sizeof(*_result));
          TRACE_FORMAT("<%p> _result %p, _input %p", this,_result,_input);       

          if (this->geometry()->size() == 1)
          {
            for (size_t i = 0; i < _vector_elem.size(); ++i)
              _result[i] = _vector_elem[i];
            TRACE_FORMAT("<%p> notify", this);       

            optimizer_notify(_context, (void *)this, PAMI_SUCCESS);
          }
          else
          {
            TRACE_FORMAT("<%p> generate", this);       
            for (size_t i = 0; i < _vector_elem.size(); ++i)
              _input[i] = _vector_elem[i];

            pami_xfer_t          ar; // allreduce        
            ar.cb_done           = optimizer_notify;
            ar.cookie            = this;
            // algorithm not needed here       
            memset(&ar.options,0,sizeof(ar.options));
            ar.cmd.xfer_allreduce.sndbuf            = (char*) _input;
            ar.cmd.xfer_allreduce.stype             = PAMI_TYPE_UNSIGNED_LONG_LONG;
            ar.cmd.xfer_allreduce.stypecount        = _total_nelem;
            ar.cmd.xfer_allreduce.rcvbuf            = (char*) _result;
            ar.cmd.xfer_allreduce.rtype             = PAMI_TYPE_UNSIGNED_LONG_LONG;
            ar.cmd.xfer_allreduce.rtypecount        = _total_nelem;
            ar.cmd.xfer_allreduce.op                = PAMI_DATA_BAND;
            _ar_algo->generate(&ar);    
          }
        }
        else if (_cb_done)
        {  
          TRACE_FORMAT("<%p> cb_done", this);       
          _cb_done (_context, _cookie, PAMI_SUCCESS);
        }
        TRACE_FN_EXIT();
      }

      static void optimizer_notify (pami_context_t    context,
                                    void            * cookie,
                                    pami_result_t     result) 
      {
        TRACE_FN_ENTER();
        GeometryOptimizer<T_Geometry> *optimizer = (GeometryOptimizer<T_Geometry> *) cookie;

        int nelem = 0;
        size_t i  = 0;
        for (i = 0; i < optimizer->_vector_reduce.size(); i++)
        {
          TRACE_FORMAT("notify i %zu, optimizer %p",i,optimizer);
          TRACE_FORMAT("notify i %zu, optimizer->_vector_reduce %p, size %zu",i,&optimizer->_vector_reduce,optimizer->_vector_reduce.size());
          TRACE_FORMAT("notify i %zu, optimizer->result %p",i,optimizer->_result);
          TRACE_FORMAT("notify i %zu, "
                       "optimizer->_vector_reduce[i].context_id %zu,"
                       "optimizer->_vector_reduce[i].cookie %p,     "
                       "optimizer->_result[nelem] %zu,              "
                       "optimizer->_vector_reduce[i].nelem %zu,     "
                       "optimizer->_geometry %p,                    "
                       "result %u                                   ",
                       i,
                       optimizer->_vector_reduce[i].context_id, 
                       optimizer->_vector_reduce[i].cookie,     
                       optimizer->_result[nelem],              
                       optimizer->_vector_reduce[i].nelem,      
                       optimizer->_geometry,                    
                       result);                                 
          if (optimizer->_vector_reduce[i].results_fn)
            optimizer->_vector_reduce[i].results_fn (optimizer->_vector_reduce[i].context_id,
                                                     optimizer->_vector_reduce[i].cookie,
                                                     &optimizer->_result[nelem], 
                                                     optimizer->_vector_reduce[i].nelem,
                                                     optimizer->_geometry,
                                                     result);
          nelem += optimizer->_vector_reduce[i].nelem;
        }   

        //The callee can analyze protocols now
        if (optimizer->_cb_done)
          optimizer->_cb_done (context, optimizer->_cookie, result);
        TRACE_FN_EXIT();
      }
    };
  };
};


#endif
