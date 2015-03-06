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
 * \file common/MemregionInterface.h
 * \brief ???
 */

#ifndef __common_MemregionInterface_h__
#define __common_MemregionInterface_h__

#include <pami.h>

namespace PAMI
{
  namespace Interface
  {
    ///
    /// \brief memory regions.
    ///
    /// \param T Memregion template class
    ///
    template <class T>
    class Memregion
    {
      public:
        ///
        /// \brief memory region constructor.
        ///
        inline  Memregion () {};
        inline ~Memregion () {};

        ///
        /// \brief Create a memory region.
        ///
        /// \attention All memory region derived classes \b must
        ///            implement the createMemregion_impl() method.
        ///
        /// \param[out] bytes_out Actual number of bytes pinned
        /// \param[in]  bytes_in  Requested number of bytes to be pinned
        /// \param[in]  base      Requested base virtual address
        /// \param[in]  options   ???
        ///
        /// \retval PAMI_SUCCESS The entire memory region, or a portion of
        ///                      the memory region was pinned. The actual
        ///                      number of bytes pinned from the start of the
        ///                      buffer is returned in the \c bytes_out
        ///                      field. The memory region must be free'd with
        ///                      with destroyMemregion().
        ///
        /// \retval PAMI_EAGAIN  The memory region was not pinned due to an
        ///                      unavailable resource. The memory region does
        ///                      not need to be freed with destroyMemregion().
        ///
        /// \retval PAMI_INVAL   An invalid parameter value was specified.
        ///
        /// \retval PAMI_ERROR   The memory region was not pinned and does not need to
        ///                      be freed with destroyMemregion().
        ///
        /// \see destroyMemregion
        /// \see PAMI_Memregion_create()
        ///
        inline pami_result_t createMemregion (size_t   * bytes_out,
                                              size_t     bytes_in,
                                              void     * base,
                                              uint64_t   options);

        ///
        /// \attention All memory region derived classes \b must
        ///            implement the destroyMemregion_impl() method.
        ///
        inline pami_result_t destroyMemregion ();
    };

    template <class T>
    inline pami_result_t Memregion<T>::createMemregion(size_t  * bytes_out,
                                                       size_t    bytes_in,
                                                       void    * base,
                                                       uint64_t  options)
    {
      return static_cast<T*>(this)->createMemregion_impl(bytes_out,
                                                         bytes_in,
                                                         base,
                                                         options);
    }

    template <class T>
    inline pami_result_t Memregion<T>::destroyMemregion()
    {
      return static_cast<T*>(this)->destroyMemregion_impl();
    }
  };
};
#endif /* __common_MemregionInterface_h__ */

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
