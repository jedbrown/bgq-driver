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
 * \file algorithms/interfaces/CollFactoryInterface.h
 * \brief ???
 */

#ifndef __algorithms_interfaces_CollFactoryInterface_h__
#define __algorithms_interfaces_CollFactoryInterface_h__

#include <pami.h>

namespace PAMI
{
  namespace CollFactory
  {
    template <class T_Collfactory>
    class CollFactory
    {
    public:
      inline CollFactory()
        {
        }
      inline pami_result_t  algorithms_num  (pami_xfer_type_t collective,
                                            int *lists_lengths);
      inline pami_result_t algorithms_info  (pami_xfer_type_t collective,
                                            pami_algorithm_t *alglist,
                                            pami_metadata_t *mdata,
                                            int algorithm_type,
                                            int num);

      inline pami_result_t  collective      (pami_xfer_t           *collective);
      inline pami_result_t  ibroadcast      (pami_broadcast_t      *broadcast);
      inline pami_result_t  iallreduce      (pami_allreduce_t      *allreduce);
      inline pami_result_t  ireduce         (pami_reduce_t         *reduce);
      inline pami_result_t  iallgather      (pami_allgather_t      *allgather);
      inline pami_result_t  iallgatherv     (pami_allgatherv_t     *allgatherv);
      inline pami_result_t  iallgatherv_int (pami_allgatherv_int_t *allgatherv_int);
      inline pami_result_t  iscatter        (pami_scatter_t        *scatter);
      inline pami_result_t  iscatterv       (pami_scatterv_t       *scatterv);
      inline pami_result_t  iscatterv_int   (pami_scatterv_int_t   *scatterv);
      inline pami_result_t  ibarrier        (pami_barrier_t        *barrier);
      inline pami_result_t  ialltoall       (pami_alltoall_t       *alltoall);
      inline pami_result_t  ialltoallv      (pami_alltoallv_t      *alltoallv);
      inline pami_result_t  ialltoallv_int  (pami_alltoallv_int_t  *alltoallv_int);
      inline pami_result_t  iscan           (pami_scan_t           *scan);
      inline pami_result_t  ambroadcast     (pami_ambroadcast_t    *ambroadcast);
      inline pami_result_t  amscatter       (pami_amscatter_t      *amscatter);
      inline pami_result_t  amgather        (pami_amgather_t       *amgather);
      inline pami_result_t  amreduce        (pami_amreduce_t       *amreduce);
    }; // class CollFactory

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::collective(pami_xfer_t *collective)
    {
      return static_cast<T_Collfactory*>(this)->collective_impl(collective);
    }

    template <class T_Collfactory>
    inline pami_result_t CollFactory<T_Collfactory>::algorithms_num(pami_xfer_type_t collective,
                                                                     int *lists_lengths)
    {
      return static_cast<T_Collfactory*>(this)->algorithms_num_impl(collective, lists_lengths);
    }

    template <class T_Collfactory>
    inline pami_result_t CollFactory<T_Collfactory>::algorithms_info(pami_xfer_type_t collective,
                                                                   pami_algorithm_t *alglist,
                                                                   pami_metadata_t *mdata,
                                                                   int algorithm_type,
                                                                   int num)
    {
      return static_cast<T_Collfactory*>(this)->algorithms_info_impl(collective, alglist, mdata, algorithm_type, num);
    }


    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::ibroadcast(pami_broadcast_t *broadcast)
    {
      return static_cast<T_Collfactory*>(this)->ibroadcast_impl(broadcast);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::iallreduce(pami_allreduce_t *allreduce)
    {
      return static_cast<T_Collfactory*>(this)->iallreduce_impl(allreduce);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::ireduce(pami_reduce_t *reduce)
    {
      return static_cast<T_Collfactory*>(this)->ireduce_impl(reduce);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::iallgather(pami_allgather_t *allgather)
    {
      return static_cast<T_Collfactory*>(this)->iallgather_impl(allgather);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::iallgatherv(pami_allgatherv_t *allgatherv)
    {
      return static_cast<T_Collfactory*>(this)->iallgatherv_impl(allgatherv);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::iallgatherv_int(pami_allgatherv_int_t *allgatherv_int)
    {
      return static_cast<T_Collfactory*>(this)->iallgatherv_int_impl(allgatherv_int);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::iscatter(pami_scatter_t *scatter)
    {
      return static_cast<T_Collfactory*>(this)->iscatter_impl(scatter);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::iscatterv(pami_scatterv_t *scatterv)
    {
      return static_cast<T_Collfactory*>(this)->iscatterv_impl(scatterv);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::iscatterv_int(pami_scatterv_int_t *scatterv_int)
    {
      return static_cast<T_Collfactory*>(this)->iscatterv_int_impl(scatterv_int);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::ibarrier(pami_barrier_t *barrier)
    {
      return static_cast<T_Collfactory*>(this)->ibarrier_impl(barrier);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::ialltoall(pami_alltoall_t *alltoall)
    {
      return static_cast<T_Collfactory*>(this)->ialltoall_impl(alltoall);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::ialltoallv(pami_alltoallv_t *alltoallv)
    {
      return static_cast<T_Collfactory*>(this)->ialltoallv_impl(alltoallv);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::ialltoallv_int(pami_alltoallv_int_t *alltoallv_int)
    {
      return static_cast<T_Collfactory*>(this)->ialltoallv_int_impl(alltoallv_int);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::iscan(pami_scan_t *scan)
    {
      return static_cast<T_Collfactory*>(this)->iscan_impl(scan);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::ambroadcast(pami_ambroadcast_t *ambroadcast)
    {
      return static_cast<T_Collfactory*>(this)->ambroadcast_impl(ambroadcast);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::amscatter(pami_amscatter_t *amscatter)
    {
      return static_cast<T_Collfactory*>(this)->amscatter_impl(amscatter);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::amgather(pami_amgather_t *amgather)
    {
      return static_cast<T_Collfactory*>(this)->amgather_impl(amgather);
    }

    template <class T_Collfactory>
    inline pami_result_t
    CollFactory<T_Collfactory>::amreduce(pami_amreduce_t *amreduce)
    {
      return static_cast<T_Collfactory*>(this)->amreduce_impl(amreduce);
    }
  }; // namespace CollFactory
}; // namespace PAMI

#endif
