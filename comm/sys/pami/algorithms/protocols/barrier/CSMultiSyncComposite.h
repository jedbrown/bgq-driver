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
 * \file algorithms/protocols/barrier/CSMultiSyncComposite.h
 * \brief Simple composite based on multisync
 */
#ifndef __algorithms_protocols_barrier_CSMultiSyncComposite_h__
#define __algorithms_protocols_barrier_CSMultiSyncComposite_h__

#include "algorithms/interfaces/Schedule.h"
#include "algorithms/executor/Barrier.h"
#include "algorithms/composite/Composite.h"

namespace CCMI
{
  namespace Adaptor
  {
    namespace Barrier
    {

      class CSMultiSyncComposite : public CCMI::Executor::Composite
      {
        protected:
          PAMI_GEOMETRY_CLASS                * _geometry;
          pami_multisync_t                     _minfo;
          Interfaces::NativeInterface        * _native;

        public:
          CSMultiSyncComposite (pami_geometry_t                       g,
                                void                                 * cmd,
                                pami_event_function                     fn,
                                void                                 * cookie) :
              Composite(), _geometry((PAMI_GEOMETRY_CLASS*)g)
          {
            TRACE_ADAPTOR((stderr, "%s\n", __PRETTY_FUNCTION__));

            // setDoneCallback(fn, cookie);

            _native = (Interfaces::NativeInterface *)_geometry->getKey(PAMI::Geometry::GKEY_GEOMETRYCSNI);

            _minfo.cb_done.function   = fn;
            _minfo.cb_done.clientdata = cookie;
            _minfo.connection_id      = 0;
            _minfo.roles              = -1U;
            _minfo.participants       = _geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);

          }

          virtual void start()
          {
            TRACE_ADAPTOR((stderr, "%s\n", __PRETTY_FUNCTION__));
            // _minfo.cb_done.function   = _cb_done;
            // _minfo.cb_done.clientdata = _clientdata;
            _native->multisync(&_minfo);
          }
      };
    };
  };
};

#endif
