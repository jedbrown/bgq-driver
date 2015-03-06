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
/// \file common/BaseMappingInterface.h
/// \brief ???
///
#ifndef __common_BaseMappingInterface_h__
#define __common_BaseMappingInterface_h__

#include <pami.h>
#include "common/MultisendInterface.h"

namespace PAMI
{
  namespace Interface
  {
    namespace Mapping
    {
      ///
      /// \param T_Mapping Base mapping template class
      ///
      template <class T_Mapping>
      class Base
      {
        public:

          ///
          /// \brief Returns the global task id for this process
          ///
          inline size_t task ();

          ///
          /// \brief Returns the number of global tasks
          ///
          inline size_t size ();


          inline pami_result_t network2task (const pami_coord_t  * addr,
                                           pami_task_t          * rank,
                                           pami_network         * type);

          inline pami_result_t task2network (pami_task_t           rank,
                                           pami_coord_t         * addr,
                                           pami_network           type);
          inline size_t globalDims();
      };  // class Base

      template <class T_Mapping>
      inline size_t Base<T_Mapping>::task ()
      {
        return static_cast<T_Mapping*>(this)->task_impl ();
      }

      template <class T_Mapping>
      inline size_t Base<T_Mapping>::size ()
      {
        return static_cast<T_Mapping*>(this)->size_impl ();
      }
      template <class T_Mapping>
      inline pami_result_t Base<T_Mapping>::network2task (const pami_coord_t  * addr,
                                                         pami_task_t         * rank,
                                                         pami_network        * type)
      {
        return static_cast<T_Mapping*>(this)->network2task_impl (addr, rank, type);
      }

      template <class T_Mapping>
      inline pami_result_t Base<T_Mapping>::task2network (pami_task_t    rank,
                                                         pami_coord_t * addr,
                                                         pami_network   type)
      {
        return static_cast<T_Mapping*>(this)->task2network_impl (rank, addr, type);
      }

      template <class T_Mapping>
      inline size_t Base<T_Mapping>::globalDims ()
      {
        return static_cast<T_Mapping*>(this)->globalDims_impl ();
      }
    };  // namespace Mapping
  };  // namespace Interface
};  // namespace PAMI
#endif // __components_mapping_basemapping_h__
