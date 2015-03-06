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
 * \file algorithms/geometry/ClassRouteId.h
 * \brief ???
 */

#ifndef __algorithms_geometry_ClassRouteId_h__
#define __algorithms_geometry_ClassRouteId_h__

#include "algorithms/geometry/Algorithm.h"

namespace PAMI
{
  namespace Geometry
  {
    template <class T_Geometry>
    class ClassRouteId
    {
    public:
      typedef void (*cr_event_function)(pami_context_t  context,
                                        void           *cookie,
                                        uint64_t       *reduce_result,
                                        T_Geometry     *geometry,
                                        pami_result_t   result );


    public:
      ClassRouteId<T_Geometry>(Algorithm<T_Geometry>    *ar_algo,
                               T_Geometry               *geometry,
                               uint64_t                 *bitmask,
                               size_t                    count,
                               cr_event_function         result_cb_done,
                               void                     *result_cookie,
                               pami_event_function       user_cb_done,
                               void                     *user_cookie):
        _ar_algo(ar_algo),
        _geometry(geometry),
        _bitmask(bitmask),
        _count(count),
        _result_cb_done(result_cb_done),
        _result_cookie(result_cookie),
        _user_cb_done(user_cb_done),
        _user_cookie(user_cookie)
        {
        }
      inline void setCallback(cr_event_function  f,
                              void             * cookie)
      {
        _result_cb_done = f;
        _result_cookie  = cookie;
      }
      void startAllreduce(pami_context_t       context,
                          pami_event_function  cb_done,
                          void                *cookie)
        {
          pami_xfer_t                             ar;
          ar.cb_done                              = cb_done;
          ar.cookie                               = cookie;
          memset(&ar.options,0,sizeof(ar.options));
          ar.cmd.xfer_allreduce.sndbuf            = (char*)_bitmask;
          ar.cmd.xfer_allreduce.stype             = PAMI_TYPE_UNSIGNED_LONG_LONG;
          ar.cmd.xfer_allreduce.stypecount        = _count;
          ar.cmd.xfer_allreduce.rcvbuf            = (char*)_bitmask;
          ar.cmd.xfer_allreduce.rtype             = PAMI_TYPE_BYTE;
          ar.cmd.xfer_allreduce.rtypecount        = 8*_count;
          ar.cmd.xfer_allreduce.op                = PAMI_DATA_BAND;
          _ar_algo->generate(&ar);
        }
    protected:
      Algorithm<T_Geometry>  *_ar_algo;
      T_Geometry             *_geometry;
      uint64_t               *_bitmask;
      size_t                  _count;
      cr_event_function       _result_cb_done;
      void                   *_result_cookie;
      pami_event_function     _user_cb_done;
      void                   *_user_cookie;
    };

  };
};
#endif
