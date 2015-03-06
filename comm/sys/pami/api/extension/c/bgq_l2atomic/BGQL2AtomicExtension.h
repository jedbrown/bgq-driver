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
 * \file api/extension/c/bgq_l2atomic/BGQL2AtomicExtension.h
 * \brief PAMI extension "BGQ L2 atomic" interface
 */
#ifndef __api_extension_c_bgq_l2atomic_BGQL2AtomicExtension_h__
#define __api_extension_c_bgq_l2atomic_BGQL2AtomicExtension_h__

#include <pami.h>

namespace PAMI
{
  class BGQL2AtomicExtension
  {

    public:

      BGQL2AtomicExtension();

      ///
      /// \brief
      ///
      /// \see PAMI::Memory""MemoryManager::memalign
      /// \todo Do we expose the full memalign interface?
      ///
      static pami_result_t node_memalign(void **memptr, size_t alignment, size_t bytes,
				const char *key);
      ///
      /// \brief
      ///
      /// \see PAMI::Memory""MemoryManager::free
      ///
      static void node_free(void *mem);

      ///
      /// \brief
      ///
      /// 'key' is not really needed.
      ///
      /// \see PAMI::Memory""MemoryManager::memalign
      /// \todo Do we expose the full memalign interface?
      ///
      static pami_result_t proc_memalign(void **memptr, size_t alignment, size_t bytes,
				const char *key);
      ///
      /// \brief
      ///
      /// \see PAMI::Memory""MemoryManager::free
      ///
      static void proc_free(void *mem);

    protected:
	void *dummy;

  };
};

#endif // __api_extension_c_bgq_l2atomic_BGQL2AtomicExtension_h__
