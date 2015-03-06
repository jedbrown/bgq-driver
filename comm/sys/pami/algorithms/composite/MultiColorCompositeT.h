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
 * \file algorithms/composite/MultiColorCompositeT.h
 * \brief ???
 */
#ifndef __algorithms_composite_MultiColorCompositeT_h__
#define __algorithms_composite_MultiColorCompositeT_h__

#include "algorithms/composite/Composite.h"

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
  namespace Executor
  {
    ///
    /// \brief Get optimal colors based on bytes and schedule
    ///
    typedef void      (*GetColorsFn) (PAMI::Topology             * t,
                                      unsigned                    bytes,
                                      unsigned                  * colors,
                                      unsigned                  & ncolors);

    ///
    ///  \brief Base class for synchronous broadcasts
    ///
    template <int NUMCOLORS, class T_Bar, class T_Exec, class T_Sched, class T_Conn, GetColorsFn pwcfn>
    class MultiColorCompositeT : public CompositeT<NUMCOLORS, T_Bar, T_Exec>
    {
    protected:
      ///
      /// \brief number of done callbacks called
      ///
      unsigned                 _doneCount;
      unsigned                 _nComplete;
      unsigned                 _numColors;
      unsigned                 _numColorsMax;
      unsigned                 _root;
      unsigned                 _bytes;

      ///
      ///  \brief Application callback to call when the broadcast has finished
      ///
      Interfaces::NativeInterface                 * _native;  //native interface
      T_Conn                                      * _cmgr;
      uint8_t                                       _executors [NUMCOLORS][sizeof(T_Exec)] __attribute__((__aligned__(16)));
      uint8_t                                       _schedules [NUMCOLORS][sizeof(T_Sched)] __attribute__((__aligned__(16)));
      unsigned                                      _colors [NUMCOLORS];
      unsigned                                      _bytecounts [NUMCOLORS];
      unsigned                                      _strides [NUMCOLORS];

      static const uint32_t alignment = 0x200; //512 byte aligment
      static const uint32_t alignment_mask = ~(alignment-1); //the mask for sizes
    public:
      MultiColorCompositeT () : CompositeT<NUMCOLORS, T_Bar, T_Exec>(), 
        _doneCount(0), _nComplete(0)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>", this);
        TRACE_FN_EXIT();
      }

      virtual ~MultiColorCompositeT ()
      {
      }

      ///
      /// \brief Receive the broadcast message and notify the executor
      ///
      static void staticRecvFn(pami_context_t context, void *executor, pami_result_t err)
      {
        TRACE_FN_ENTER();
        pami_quad_t *info = NULL;
        T_Exec *exe = (T_Exec *) executor;
        TRACE_FORMAT( "<%p>", (int)exe);
        exe->notifyRecv ((unsigned) - 1, *info, NULL, exe->getPwidth());

        TRACE_FN_EXIT();
      }

      ///
      /// \brief The Composite Constructor
      ///
      MultiColorCompositeT (T_Conn                                * cmgr,
                            pami_event_function                     cb_done,
                            void                                  * clientdata,
                            Interfaces::NativeInterface           * mf,
                            unsigned                                ncolors = NUMCOLORS):
      CCMI::Executor::CompositeT<NUMCOLORS, T_Bar, T_Exec>(), 
        _doneCount(0), _numColors(ncolors), _numColorsMax(ncolors), _native(mf), _cmgr(cmgr)
      {
        TRACE_FN_ENTER();
        _nComplete     = _numColors + 1;
        this->setDoneCallback (cb_done, clientdata);
        TRACE_FORMAT("<%p> numcolors %u, complete %u", this,_numColors,_nComplete);
        TRACE_FN_EXIT();
      }

      uint32_t getNumColors ()
      {
        return _numColors;
      }

      void getColors (PAMI::Topology     * topology,
		      unsigned             bytes,
		      unsigned           * colors, 
		      unsigned           & ncolors) 
      {
	pwcfn (topology, bytes, colors, ncolors);
      }

      void initialize (unsigned                                comm,
                       PAMI::Topology                        * topology,
                       unsigned                                root,
                       unsigned                                stypecount,
                       PAMI::Type::TypeCode                  * stype,
                       unsigned                                rtypecount,
                       PAMI::Type::TypeCode                  * rtype,
                       char                                  * src,
                       char                                  * dst)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p> root %u, bytes %lu", this,root, (unsigned long)stypecount * stype->GetDataSize());
        pwcfn (topology, stypecount * stype->GetDataSize(), _colors, _numColors);
        //printf ("Using %d colors, %d\n", _numColors, _colors[0]);
        if (_numColors > NUMCOLORS)
          _numColors = NUMCOLORS;
        _nComplete     = _numColors + 1;
        TRACE_FORMAT("<%p> numcolors %u, complete %u", this,_numColors,_nComplete);
        if (src == PAMI_IN_PLACE)
        {
          src = dst;
          stype = rtype;
        }

        _root  = root;
        _bytes = stypecount * stype->GetDataSize();

        _bytecounts[0] = stypecount * stype->GetDataSize();
        _strides   [0] = stypecount * stype->GetExtent();
        unsigned aligned_bytes  = 0;
        unsigned aligned_stride = 0;

        if (_numColors > 1)
        {
          aligned_bytes  = ((stypecount*stype->GetDataSize())/_numColors+alignment) & alignment_mask;
          aligned_stride = ((stypecount*stype->GetExtent())/_numColors+alignment) & alignment_mask;
          _bytecounts[0] =  aligned_bytes;
          _strides   [0] =  aligned_stride;

          for (unsigned c = 1; c < _numColors; ++c)
          {
            _bytecounts[c] = aligned_bytes;
            _strides   [c] = aligned_stride;
            TRACE_FORMAT("<%p> bytecounts[%u] %u", this,c,_bytecounts[c]);
          }

          _bytecounts[_numColors-1]  = (stypecount * stype->GetDataSize()) - (aligned_bytes  * (_numColors - 1));
          _strides   [_numColors-1]  = (stypecount * stype->GetExtent  ()) - (aligned_stride * (_numColors - 1));
          TRACE_FORMAT("<%p> bytecounts[%u] %u", this,_numColors-1,_bytecounts[_numColors-1]);
        }

        unsigned c = 0;
        for (c = 0; c < _numColors; c++)
        {
          CCMI_assert (c < NUMCOLORS);

          T_Exec *exec  =
	    new (& _executors[c]) T_Exec (_native,
					  _cmgr,
					  comm);

          exec->setRoot (root);

          exec->setBuffers (src + aligned_stride*c,
                            dst + aligned_stride*c,
                            _bytecounts[c], _strides[c], stype, rtype);//SSS: In the case of broadcast, stype and rtype are the same. In case of allreduce they will be different
          exec->setDoneCallback (cb_composite_done, this);

          this->addExecutor(exec);
        }

        for (c = 0; c < _numColors; c++) 
        {
          T_Sched *schedule = new (&_schedules[c]) T_Sched(_native->endpoint(), topology, _colors[c]);
          T_Exec *exec  = (T_Exec *) (&_executors[c]);
          exec->setSchedule (schedule, _colors[c]);
        }

        TRACE_FN_EXIT();	
      }

      
      void reset (unsigned                                comm,
                  PAMI::Topology                        * topology,
                  unsigned                                root,
                  unsigned                                stypecount,
                  PAMI::Type::TypeCode                  * stype,
                  unsigned                                rtypecount,
                  PAMI::Type::TypeCode                  * rtype,
                  char                                  * src,
                  char                                  * dst)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p> root %u, bytes %lu", this,root, (unsigned long)stypecount * stype->GetDataSize());
        _doneCount = 0;
	
        if (src == PAMI_IN_PLACE)
        {
          src = dst;
          stype = rtype;
        }
        if (_bytes != (stypecount * stype->GetDataSize()) || _root != root) 
        {
          T_Bar *barrier = CompositeT<NUMCOLORS, T_Bar, T_Exec>::_barrier;
          //Free memory allocated by executors
          for (unsigned count = 0; count < this->getNumExecutors(); count++)
            if (getExecutor(count))
              getExecutor(count)->~T_Exec();
	  
          CCMI::Executor::CompositeT<NUMCOLORS, T_Bar, T_Exec>::reset();
          addBarrier(barrier);
          _numColors = _numColorsMax;
          initialize (comm, topology, root, stypecount, stype, rtypecount, rtype, src, dst);
          return;
        }
	
        unsigned c = 0;
        for (c = 0; c < _numColors; c++)
        {
          T_Exec *exec  = (T_Exec *) (&_executors[c]);
          exec->setBuffers (src + _strides[0]*c,
                            dst + _strides[0]*c,
                            _bytecounts[c], _strides[c], stype, rtype);
          exec->setDoneCallback (cb_composite_done, this);
        }
	
        TRACE_FN_EXIT();	
      }

      //The Command will be interpretted in the child class
      virtual unsigned restart(void *cmd)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>", this);
        if (CompositeT<NUMCOLORS, T_Bar, T_Exec>::_barrier != NULL)
        {
          // reset barrier since it may be been used between calls
          TRACE_FORMAT("<%p> reset barrier(%p)", this, (CompositeT<NUMCOLORS, T_Bar, T_Exec>::_barrier));

          T_Bar  *barrier =  CompositeT<NUMCOLORS, T_Bar, T_Exec>::_barrier;
          barrier->setDoneCallback(cb_barrier_done, this);
          barrier->start();
        }

        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }

      void addBarrier (T_Bar *exec)
      {
        TRACE_FN_ENTER();
        CompositeT<NUMCOLORS, T_Bar, T_Exec>::addBarrier(exec);
        TRACE_FORMAT("<%p> numcolors %u, donecount %u, complete count %u", this,_numColors,_doneCount,_nComplete);
        TRACE_FN_EXIT();
      }

      T_Exec * getExecutor (int idx)
      {
        return 	CompositeT<NUMCOLORS, T_Bar, T_Exec>::getExecutor(idx);
      }

      ///
      /// \brief For sync broadcasts, the done call back to be called
      ///        when barrier finishes
      ///
      static void cb_barrier_done(pami_context_t context, void *me, pami_result_t err)
      {
        TRACE_FN_ENTER();
        MultiColorCompositeT * composite = (MultiColorCompositeT *) me;
        TRACE_FORMAT("<%p> %d: numcolors %u, donecount %u, complete count %u bytes %d\n", me, 
                     composite->_native->endpoint(), composite->_numColors,
                     composite->_doneCount, composite->_nComplete, composite->_bytes);
        CCMI_assert (composite != NULL);

        //printf ("In cb_barrier_done donec=%d\n",
        for (unsigned i = 0; i < composite->_numColors; ++i)
        {
          composite->getExecutor(i)->start();
        }

        CCMI_assert (composite->_doneCount <  composite->_nComplete);
        ++composite->_doneCount;

        if (composite->_doneCount == composite->_nComplete) // call users done function
        {
          TRACE_FORMAT ("%d: Composite Done from barrier bytes %d\n", composite->_native->endpoint(), composite->_bytes);
          composite->_cb_done(composite->_context, composite->_clientdata, PAMI_SUCCESS);
        }
        TRACE_FN_EXIT();
      }

      static void cb_composite_done(pami_context_t context, void *me, pami_result_t err)
      {
        TRACE_FN_ENTER();
        MultiColorCompositeT * composite = (MultiColorCompositeT *) me;
        TRACE_FORMAT("<%p> numcolors %u, donecount %u, complete count %u", me,composite->_numColors,composite->_doneCount,composite->_nComplete);
        CCMI_assert (composite != NULL);

        CCMI_assert (composite->_doneCount <  composite->_nComplete);
        ++composite->_doneCount;

        if (composite->_doneCount == composite->_nComplete) // call users done function
        {
          TRACE_FORMAT ("%d: Composite Done from collective, bytes %d context %p/%p\n", composite->_native->endpoint(), composite->_bytes, context,composite->_context);
          composite->_cb_done(context?context:composite->_context, composite->_clientdata, PAMI_SUCCESS);
        }
        TRACE_FN_EXIT();
      }
    };  //-- MultiColorCompositeT

  };  //- end namespace Executor
};  //- end CCMI

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
