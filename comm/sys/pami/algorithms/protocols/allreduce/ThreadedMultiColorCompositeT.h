/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* (C) Copyright IBM Corp.  2011, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#ifndef __algorithms_protocols_allreduce_ThreadedMultiColorCompositeT_h__
#define __algorithms_protocols_allreduce_ThreadedMultiColorCompositeT_h__

#include "algorithms/executor/Barrier.h"
#include "math/math_coremath.h"
#include "algorithms/executor/AllreduceBaseExec.h"
#include "algorithms/protocols/allreduce/MultiColorCompositeT.h"

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
      //-- ThreadedMultiColorCompositeT
      /// \brief The Composite for the Allreduce (and reduce)
      /// kernel executor.
      ///
      /// Extends MultiColorCompositeT to enable multiple executors to
      /// execute on multiple threads
      ///
      template <int NUMCOLORS, class T_Exec, class T_Sched, class T_Conn, Executor::GetColorsFn pwcfn, class T_Mutex, int T_PostReceives = 0, PAMI::Geometry::topologyIndex_t T_Geometry_Index = PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX>
	class ThreadedMultiColorCompositeT : public MultiColorCompositeT<NUMCOLORS, T_Exec, T_Sched, T_Conn, pwcfn, 0, T_Geometry_Index, false>
	{
	protected:	
	struct MultiColorState {
	  int                                _color;
	  int                                _ncolors;
	  T_Exec                           * _executor;
	  ThreadedMultiColorCompositeT     * _composite;
	};	  
	
	//char                    _started[NUMCOLORS];
	MultiColorState           _mstate[NUMCOLORS+1];
	pami_work_t               _twork[NUMCOLORS+1];
	
	public:
	ThreadedMultiColorCompositeT ()
	{
	  CCMI_abort();
	}
	
	ThreadedMultiColorCompositeT (pami_context_t                             context,
					size_t                                     ctxt_id,
					Interfaces::NativeInterface              * mf,
					T_Conn                                   * rcmgr,
					pami_geometry_t                             g,
					void                                     * cmd,
					pami_event_function                         fn,
					void                                     * cookie)
	{
	  CCMI_abort();
	}
	
	ThreadedMultiColorCompositeT (pami_context_t                             context,
                                size_t                                     ctxt_id,
                                Interfaces::NativeInterface              * mf,
				T_Conn                                   * rcmgr,
				T_Conn                                   * bcmgr,
                                void                                     * algorithmFactory,
                                pami_geometry_t                             g,
                                void                                     * cmd,
                                pami_event_function                         fn,
				void                                     * cookie):
	MultiColorCompositeT<NUMCOLORS, T_Exec, T_Sched, T_Conn, pwcfn, 0, T_Geometry_Index, false>
	(context,
	 ctxt_id,
	 mf,	 
	 rcmgr,
	 bcmgr,
	 algorithmFactory, 
	 g,
	 cmd,
	 fn,
	 cookie)
	{
	  TRACE_FN_ENTER();	
	  //memset(_started, 0, sizeof(_started));
	  for (unsigned c = 0; c < this->_numColors; c++) {
	    T_Exec *allreduce = this->getExecutor(c);
	    //override completion callbacks
	    allreduce->setDoneCallback (cb_mutexed_composite_done, this);  
	  }
  
	  if (T_PostReceives) 
	    this->postReceives();	     
	  
	  PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *)g;
	  CCMI::Executor::Composite  *barrier =  (CCMI::Executor::Composite *) geometry->getKey(ctxt_id, PAMI::Geometry::CKEY_OPTIMIZEDBARRIERCOMPOSITE);
	  
	  this->addBarrier(barrier);
	  barrier->setDoneCallback(cb_mutexed_barrier_done, this);
	  barrier->start();
	  TRACE_FN_EXIT();
	}

	/// Default Destructor
	virtual ~ThreadedMultiColorCompositeT()
	{
	  TRACE_FN_ENTER();
	  TRACE_FORMAT( "<%p>", this);
	  TRACE_FN_EXIT();
	}
	
	void operator delete (void *p)
	{
	  CCMI_abort();
	}
	
	///
	/// \brief Restart this multicolor collective
	///
	virtual unsigned restart   ( void *cmd )
	{
	  TRACE_FN_ENTER();
	  TRACE_FORMAT( "<%p>", this);	    	    
	  
	  MultiColorCompositeT<NUMCOLORS, T_Exec, T_Sched, T_Conn, pwcfn, 0, T_Geometry_Index, false>::restart(cmd);

		this->setDoneCallback(((pami_xfer_t *)cmd)->cb_done,
													((pami_xfer_t *)cmd)->cookie);
					
	  for (unsigned c = 0; c <  this->_numColors; c++)
	  {
	    T_Exec *allreduce = this->getExecutor(c);
	    allreduce->setDoneCallback (cb_mutexed_composite_done, this); 
	  }

	  if (T_PostReceives) 
	    this->postReceives();	     
	  
	  CCMI::Executor::Composite  *barrier =  this->_barrier; 	  
	  barrier->setDoneCallback(cb_mutexed_barrier_done, this);
	  barrier->start();
	  
	  return PAMI_SUCCESS;
	}

	void postWork () 
	{
	  unsigned c = 0;
	  for (c = 0; c <  this->_numColors; c++)
	  { 
	    _mstate[c]._color     = c;
	    _mstate[c]._ncolors   = this->_numColors;
	    _mstate[c]._composite = this;
	    _mstate[c]._executor  = this->getExecutor(c);
	    this->_native->postWork(this->_context, c+1, &_twork[c], threaded_advance, &_mstate[c]);	  
	  }
	}

	void postCompletion() {
	  unsigned c = this->_numColors;
	  _mstate[c]._ncolors   = this->_numColors;
	  _mstate[c]._composite = this;	  
	  pami_result_t rc = completion_advance(this->_context, 
	  				&_mstate[c]);
	  //post completion advance to context color 0
	  if (rc != PAMI_SUCCESS) {
	    this->_native->postWork(this->_context, 0, &_twork[c], completion_advance, &_mstate[c]); 
	  }
	}

	static pami_result_t completion_advance(pami_context_t    context,
						void            * clientdata)
	{
	  MultiColorState *mstate = (MultiColorState *) clientdata;	  
	  //if (mstate->_composite->_doneCount!=mstate->_composite->_nComplete) 
	  //return PAMI_EAGAIN;

	  int c = 0;
	  //Wait for all other colors (possibly in other threads) to complete
	  for (c = 0; c < mstate->_ncolors; ++c)
	    if (!mstate->_composite->_isComplete[c])
	      return PAMI_EAGAIN;
	  
	  // call users done function
	  mstate->_composite->_cb_done(context, mstate->_composite->_clientdata, PAMI_SUCCESS); 
	  return PAMI_SUCCESS;
	}
	
	static pami_result_t threaded_advance (pami_context_t     context,
					       void             * clientdata)
	{
	  //printf("Calling threaded_advance on context %p\n", context);

	  pami_result_t rc = PAMI_SUCCESS;
	  MultiColorState *mstate = (MultiColorState *) clientdata;	  

	  int c = mstate->_color;
	  int is_complete = mstate->_composite->_isComplete[c];
	  
	  if(!is_complete) {
	    rc = mstate->_executor->advance();
	    if (rc == PAMI_SUCCESS) 
	      is_complete = 1;	      
	  }		

	  mstate->_composite->_isComplete[c] = is_complete;	  
	  return rc;
	}

	///
	/// \brief Callback to be called when barrier finishes
	///
	static void cb_mutexed_barrier_done(pami_context_t context, void *me, pami_result_t err)
	{
	  TRACE_FN_ENTER();

	  ThreadedMultiColorCompositeT *composite = (ThreadedMultiColorCompositeT *) me;
	  CCMI_assert (composite != NULL);
	  for (unsigned i = 0; i < composite->_numColors; ++i)
	    composite->getExecutor(i)->start();

	  composite->postWork();	  
	  //composite->postCompletion();
	  CCMI_assert (composite->_doneCount <  composite->_nComplete);

	  ++composite->_doneCount;

	  if (composite->_doneCount == composite->_nComplete) 
	    composite->postCompletion();

	  TRACE_FN_EXIT();
	}

	static void cb_mutexed_composite_done(pami_context_t context, void *me, pami_result_t err)
	{
	  TRACE_FN_ENTER();
	  ThreadedMultiColorCompositeT *composite = 
	  (ThreadedMultiColorCompositeT *) me;

	  CCMI_assert (composite != NULL);
	  CCMI_assert (composite->_doneCount <  composite->_nComplete);
	  
	  ++composite->_doneCount;	  

	  if (composite->_doneCount == composite->_nComplete) 
	    composite->postCompletion();
	  
	  TRACE_FN_EXIT();
	}

      };  //-- ThreadedMultiColorCompositeT
    };
  };
}; // namespace CCMI::Adaptor::Allreduce

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
