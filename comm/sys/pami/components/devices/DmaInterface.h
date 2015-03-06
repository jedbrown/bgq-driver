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
 * \file components/devices/DmaInterface.h
 * \brief ???
 */

#ifndef __components_devices_DmaInterface_h__
#define __components_devices_DmaInterface_h__

#include <errno.h>
#include "Memregion.h"
#include <pami.h>

namespace PAMI
{
  namespace Device
  {
    namespace Interface
    {
      ///
      /// \todo Need A LOT MORE documentation on this interface and its use
      /// \tparam T Dma model implementation class
      ///
      template <class T>
      class DmaModel
      {
        protected:

          ///
          /// \brief Dma interface model constructor
          ///
          /// The 'protected' constructor prevents direct instantiation of this
          /// interface class.
          ///
          /// \param [in] device  Device class associated with the dma model implementation
          ///
          template <class T_Device>
          inline DmaModel (T_Device & device, pami_result_t & status)
          {
            // This compile time assert verifies that at least one of the two
            // types of dma post interfaces is supported.
            COMPILE_TIME_ASSERT(T::dma_model_va_supported || T::dma_model_mr_supported);

            status = PAMI_SUCCESS;
          };

          inline ~DmaModel () {};

        public:

          /// \see getVirtualAddressSupported()
          static const bool dma_model_va_supported = false;

          /// \see getMemoryRegionSupported()
          static const bool dma_model_mr_supported = false;

          /// \see getFenceSupported()
          static const bool dma_model_fence_supported = false;

          /// \see getDmaTransferStateBytes()
          static const size_t dma_model_state_bytes = 1;

          ///
          /// \brief Returns the virtual address dma attribute of this model.
          ///
          /// \attention All dma model interface derived classes \b must
          ///            contain a public static const data member named
          ///            'bool dma_model_va_supported'.
          ///
          /// C++ code using templates to specify the model may statically
          /// access the 'dma_model_va_supported' constant.
          ///
          static const bool getVirtualAddressSupported ();

          ///
          /// \brief Returns the memory region dma attribute of this model.
          ///
          /// \attention All dma model interface derived classes \b must
          ///            contain a public static const data member named
          ///            'bool dma_model_mr_supported'.
          ///
          /// C++ code using templates to specify the model may statically
          /// access the 'dma_model_mr_supported' constant.
          ///
          static const bool getMemoryRegionSupported ();

          ///
          /// \brief Returns the fence dma attribute of this model.
          ///
          /// \attention All dma model interface derived classes \b must
          ///            contain a public static const data member named
          ///            'bool dma_model_fence_supported'.
          ///
          /// C++ code using templates to specify the model may statically
          /// access the 'dma_model_mr_supported' constant.
          ///
          static const bool getFenceSupported ();

          ///
          /// \brief Returns the transfer state bytes attribute of this model.
          ///
          /// Typically a dma device will require some amount of temporary
          /// storage to be used during the dma operation. This attribute
          /// returns the number of bytes that must be provided to the various
          /// non-blocking dma post* methods.
          ///
          /// \attention All dma model interface derived classes \b must
          ///            contain a public static const data member named
          ///            'size_t dma_model_state_bytes'.
          ///
          /// C++ code using templates to specify the model may statically
          /// access the 'dma_model_state_bytes' constant.
          ///
          static const size_t getDmaTransferStateBytes ();

          ///
          /// \brief Post an immediate virtual address dma put transfer operation
          ///
          /// \attention All dma model interface implementation classes \b must
          ///            implement the postDmaPut_impl() method - even when
          ///            T::dma_model_va_supported == false.
          ///
          /// \param [in] target_task   Global task identifier of the target
          /// \param [in] target_offset Identifier of the destination context
          /// \param [in] bytes         Number of bytes to transfer
          /// \param [in] local         Virtual address of the local buffer to transfer
          /// \param [in] remote        Virtual address of the remote buffer
          ///
          /// \retval true  Put operation completed immediately
          /// \retval false Put operation was unable to be completed immediately
          ///
          inline bool postDmaPut (size_t   target_task,
                                  size_t   target_offset,
                                  size_t   bytes,
                                  void   * local,
                                  void   * remote);

          ///
          /// \brief Post a non-blocking virtual address contiguous dma put operation
          ///
          /// The post dma put operation interface allows the dma model
          /// and dma device implementations to optimize for performance by
          /// avoiding the overhead to construct a dma message object. If
          /// the dma device has resources immediately available then the
          /// message may be directly posted, otherwise an internal dma message
          /// object may be constructed in the state array to maintain the
          /// transfer state until the callback is invoked.
          ///
          /// \attention All dma model interface implementation classes \b must
          ///            implement the postDmaPut_impl() method - even when
          ///            T::dma_model_va_supported == false.
          ///
          /// \tparam     T_StateBytes     Number of bytes in the dma transfer
          ///                              state array - must be at least
          ///                              T::dma_model_state_bytes
          ///
          /// \param [in] state            Location to store the internal dma transfer state
          /// \param [in] local_fn         Callback to invoke when the operation is complete
          /// \param [in] cookie           Completion callback opaque application data
          /// \param [in] target_task      Global task identifier of the target
          /// \param [in] target_offset    Identifier of the destination context
          /// \param [in] bytes            Number of bytes to transfer
          /// \param [in] local            Virtual address of the local buffer to transfer
          /// \param [in] remote           Virtual address of the remote buffer
          ///
          /// \retval true  Dma operation completed and the completion
          ///               callback was invoked
          ///
          /// \retval false Dma operation did not complete and the dma
          ///               device must be advanced until the completion
          ///               callback is invoked
          ///
          template <unsigned T_StateBytes>
          inline bool postDmaPut (uint8_t               (&state)[T_StateBytes],
                                  pami_event_function   local_fn,
                                  void                * cookie,
                                  size_t                target_task,
                                  size_t                target_offset,
                                  size_t                bytes,
                                  void                * local,
                                  void                * remote);

          ///
          /// \brief Post an immediate dma memory region put transfer operation
          ///
          /// The post dma put operation interface allows the dma model
          /// and dma device implementations to optimize for performance by
          /// avoiding the overhead to construct a dma message object. If
          /// the dma device has resources immediately available then the
          /// message may be directly posted, otherwise a dma message object
          /// may be constructed to maintain the send state until the callback
          /// is invoked.
          ///
          /// \attention All dma model interface implementation classes \b must
          ///            implement the postDmaPut_impl() method - even when
          ///            T::dma_model_mr_supported == false.
          ///
          /// \param [in] target_task      Global task identifier of the target
          /// \param [in] target_offset    Identifier of the destination context
          /// \param [in] bytes            Number of bytes to transfer
          /// \param [in] local_memregion  Local data memory region
          /// \param [in] local_offset     Offset of data buffer in the local memory region
          /// \param [in] remote_memregion Remote data memory region
          /// \param [in] remote_offset    Offset of data buffer in the remote memory region
          ///
          /// \retval true  Put operation completed immediately
          /// \retval false Put operation was unable to be completed immediately
          ///
          inline bool postDmaPut (size_t                target_task,
                                  size_t                target_offset,
                                  size_t                bytes,
                                  Memregion           * local_memregion,
                                  size_t                local_offset,
                                  Memregion           * remote_memregion,
                                  size_t                remote_offset);

          ///
          /// \brief Post a non-blocking dma memory region put transfer operation
          ///
          /// The post dma put operation interface allows the dma model
          /// and dma device implementations to optimize for performance by
          /// avoiding the overhead to construct a dma message object. If
          /// the dma device has resources immediately available then the
          /// message may be directly posted, otherwise a dma message object
          /// may be constructed to maintain the send state until the callback
          /// is invoked.
          ///
          /// \attention All dma model interface implementation classes \b must
          ///            implement the postDmaPut_impl() method - even when
          ///            T::dma_model_mr_supported == false.
          ///
          /// \tparam     T_StateBytes     Number of bytes in the dma transfer
          ///                              state array - must be at least
          ///                              T::dma_model_state_bytes
          ///
          /// \param [in] state            Location to store the internal dma transfer state
          /// \param [in] local_fn         Callback to invoke when the operation is complete
          /// \param [in] cookie           Completion callback opaque application data
          /// \param [in] target_task      Global task identifier of the target
          /// \param [in] target_offset    Identifier of the destination context
          /// \param [in] bytes            Number of bytes to transfer
          /// \param [in] local_memregion  Local data memory region
          /// \param [in] local_offset     Offset of data buffer in the local memory region
          /// \param [in] remote_memregion Remote data memory region
          /// \param [in] remote_offset    Offset of data buffer in the remote memory region
          ///
          /// \retval true  Dma operation completed and the completion
          ///               callback was invoked
          ///
          /// \retval false Dma operation did not complete and the dma
          ///               device must be advanced until the completion
          ///               callback is invoked
          ///
          template <unsigned T_StateBytes>
          inline bool postDmaPut (uint8_t               (&state)[T_StateBytes],
                                  pami_event_function   local_fn,
                                  void                * cookie,
                                  size_t                target_task,
                                  size_t                target_offset,
                                  size_t                bytes,
                                  Memregion           * local_memregion,
                                  size_t                local_offset,
                                  Memregion           * remote_memregion,
                                  size_t                remote_offset);

          ///
          /// \brief Post an immediate virtual address dma get transfer operation
          ///
          /// \attention All dma model interface implementation classes \b must
          ///            implement the postDmaGet_impl() method - even when
          ///            T::dma_model_va_supported == false.
          ///
          /// \param [in] target_task   Global task identifier of the target
          /// \param [in] target_offset Identifier of the destination context
          /// \param [in] bytes         Number of bytes to transfer
          /// \param [in] local         Virtual address of the local buffer to transfer
          /// \param [in] remote        Virtual address of the remote buffer
          ///
          /// \retval true  Get operation completed immediately
          /// \retval false Get operation was unable to be completed immediately
          ///
          inline bool postDmaGet (size_t   target_task,
                                  size_t   target_offset,
                                  size_t   bytes,
                                  void   * local,
                                  void   * remote);

          ///
          /// \brief Post a non-blocking virtual address dma get transfer operation
          ///
          /// The post dma get operation interface allows the dma model
          /// and dma device implementations to optimize for performance by
          /// avoiding the overhead to construct a dma message object. If
          /// the dma device has resources immediately available then the
          /// message may be directly posted, otherwise an internal dma message
          /// object may be constructed in the state array to maintain the
          /// transfer state until the callback is invoked.
          ///
          /// \attention All dma model interface implementation classes \b must
          ///            implement the postDmaGet_impl() method - even when
          ///            T::dma_model_va_supported == false.
          ///
          /// \tparam     T_StateBytes     Number of bytes in the dma transfer
          ///                              state array - must be at least
          ///                              T::dma_model_state_bytes
          ///
          /// \param [in] state            Location to store the internal dma transfer state
          /// \param [in] local_fn         Callback to invoke when the operation is complete
          /// \param [in] cookie           Completion callback opaque application data
          /// \param [in] target_task      Global task identifier of the target
          /// \param [in] target_offset    Identifier of the destination context
          /// \param [in] bytes            Number of bytes to transfer
          /// \param [in] local            Virtual address of the local buffer to transfer
          /// \param [in] remote           Virtual address of the remote buffer
          ///
          /// \retval true  Dma operation completed and the completion
          ///               callback was invoked
          ///
          /// \retval false Dma operation did not complete and the dma
          ///               device must be advanced until the completion
          ///               callback is invoked
          ///
          template <unsigned T_StateBytes>
          inline bool postDmaGet (uint8_t               (&state)[T_StateBytes],
                                  pami_event_function   local_fn,
                                  void                * cookie,
                                  size_t                target_task,
                                  size_t                target_offset,
                                  size_t                bytes,
                                  void                * local,
                                  void                * remote);

          ///
          /// \brief Post an immediate dma memory region get transfer operation
          ///
          /// The post dma get operation interface allows the dma model
          /// and dma device implementations to optimize for performance by
          /// avoiding the overhead to construct a dma message object. If
          /// the dma device has resources immediately available then the
          /// message may be directly posted, otherwise a dma message object
          /// may be constructed to maintain the transfer state until the callback
          /// is invoked.
          ///
          /// \attention All dma model interface implementation classes \b must
          ///            implement the postDmaGet_impl() method - even when
          ///            T::dma_model_mr_supported == false.
          ///
          /// \param [in] target_task      Global task identifier of the target
          /// \param [in] target_offset    Identifier of the destination context
          /// \param [in] bytes            Number of bytes to transfer
          /// \param [in] local_memregion  Local data memory region
          /// \param [in] local_offset     Offset of data buffer in the local memory region
          /// \param [in] remote_memregion Remote data memory region
          /// \param [in] remote_offset    Offset of data buffer in the remote memory region
          ///
          /// \retval true  Oeration completed immediately
          /// \retval false Operation was unable to be completed immediately
          ///
          inline bool postDmaGet (size_t                target_task,
                                  size_t                target_offset,
                                  size_t                bytes,
                                  Memregion           * local_memregion,
                                  size_t                local_offset,
                                  Memregion           * remote_memregion,
                                  size_t                remote_offset);

          ///
          /// \brief Post a non-blocking dma memory region get transfer operation
          ///
          /// The post dma get operation interface allows the dma model
          /// and dma device implementations to optimize for performance by
          /// avoiding the overhead to construct a dma message object. If
          /// the dma device has resources immediately available then the
          /// message may be directly posted, otherwise a dma message object
          /// may be constructed to maintain the transfer state until the callback
          /// is invoked.
          ///
          /// \attention All dma model interface implementation classes \b must
          ///            implement the postDmaGet_impl() method - even when
          ///            T::dma_model_mr_supported == false.
          ///
          /// \tparam     T_StateBytes     Number of bytes in the dma transfer
          ///                              state array - must be at least
          ///                              T::dma_model_state_bytes
          ///
          /// \param [in] state            Location to store the internal dma transfer state
          /// \param [in] local_fn         Callback to invoke when the operation is complete
          /// \param [in] cookie           Completion callback opaque application data
          /// \param [in] target_task      Global task identifier of the target
          /// \param [in] target_offset    Identifier of the destination context
          /// \param [in] bytes            Number of bytes to transfer
          /// \param [in] local_memregion  Local data memory region
          /// \param [in] local_offset     Offset of data buffer in the local memory region
          /// \param [in] remote_memregion Remote data memory region
          /// \param [in] remote_offset    Offset of data buffer in the remote memory region
          ///
          /// \retval true  Dma operation completed and the completion
          ///               callback was invoked
          ///
          /// \retval false Dma operation did not complete and the dma
          ///               device must be advanced until the completion
          ///               callback is invoked
          ///
          template <unsigned T_StateBytes>
          inline bool postDmaGet (uint8_t              (&state)[T_StateBytes],
                                  pami_event_function   local_fn,
                                  void                * cookie,
                                  size_t                target_task,
                                  size_t                target_offset,
                                  size_t                bytes,
                                  Memregion           * local_memregion,
                                  size_t                local_offset,
                                  Memregion           * remote_memregion,
                                  size_t                remote_offset);

          ///
          /// \brief Post a non-blocking dma fence operation
          ///
          /// The post dma fence operation interface allows the dma model
          /// and dma device implementations to optimize for performance by
          /// avoiding the overhead to construct a dma message object. If
          /// the dma device has resources immediately available then the
          /// message may be directly posted, otherwise a dma message object
          /// may be constructed to maintain the transfer state until the callback
          /// is invoked.
          ///
          /// A fence operation will invoke the callback only after all
          /// previous put and get operations to the target task and offset
          /// have completed.
          ///
          /// \attention All dma model interface implementation classes \b must
          ///            implement the postDmaFence_impl() method - even when
          ///            T::dma_model_fence_supported == false.
          ///
          /// \tparam     T_StateBytes     Number of bytes in the dma transfer
          ///                              state array - must be at least
          ///                              T::dma_model_state_bytes
          ///
          /// \param [in] state            Location to store the internal dma transfer state
          /// \param [in] local_fn         Callback to invoke when the operation is complete
          /// \param [in] cookie           Completion callback opaque application data
          /// \param [in] target_task      Global task identifier of the target
          /// \param [in] target_offset    Identifier of the destination context
          ///
          /// \retval true  Dma operation completed and the completion
          ///               callback was invoked
          ///
          /// \retval false Dma operation did not complete and the dma
          ///               device must be advanced until the completion
          ///               callback is invoked
          ///
          template <unsigned T_StateBytes>
          inline bool postDmaFence (uint8_t              (&state)[T_StateBytes],
                                    pami_event_function   local_fn,
                                    void                * cookie,
                                    size_t                target_task,
                                    size_t                target_offset);
      };

      template <class T>
      const bool DmaModel<T>::getVirtualAddressSupported ()
      {
        return T::dma_model_va_supported;
      }

      template <class T>
      const bool DmaModel<T>::getMemoryRegionSupported ()
      {
        return T::dma_model_mr_supported;
      }

      template <class T>
      const bool DmaModel<T>::getFenceSupported ()
      {
        return T::dma_model_fence_supported;
      }

      template <class T>
      const size_t DmaModel<T>::getDmaTransferStateBytes ()
      {
        return T::dma_model_state_bytes;
      }

      template <class T>
      inline bool DmaModel<T>::postDmaPut (
        size_t   target_task,
        size_t   target_offset,
        size_t   bytes,
        void   * local,
        void   * remote)
      {
        return static_cast<T*>(this)->postDmaPut_impl (target_task,
                                                       target_offset,
                                                       bytes,
                                                       local,
                                                       remote);
      }

      template <class T>
      template <unsigned T_StateBytes>
      inline bool DmaModel<T>::postDmaPut (
        uint8_t               (&state)[T_StateBytes],
        pami_event_function   local_fn,
        void                * cookie,
        size_t                target_task,
        size_t                target_offset,
        size_t                bytes,
        void                * local,
        void                * remote)
      {
        // This compile time assert verifies that sufficient memory was provided
        // to maintain the state of the operation.
        COMPILE_TIME_ASSERT(T::dma_model_state_bytes <= T_StateBytes);

        return static_cast<T*>(this)->postDmaPut_impl (state,
                                                       local_fn,
                                                       cookie,
                                                       target_task,
                                                       target_offset,
                                                       bytes,
                                                       local,
                                                       remote);
      }

      template <class T>
      inline bool DmaModel<T>::postDmaPut (
        size_t                target_task,
        size_t                target_offset,
        size_t                bytes,
        Memregion           * local_memregion,
        size_t                local_offset,
        Memregion           * remote_memregion,
        size_t                remote_offset)
      {
        return static_cast<T*>(this)->postDmaPut_impl (target_task,
                                                       target_offset,
                                                       bytes,
                                                       local_memregion,
                                                       local_offset,
                                                       remote_memregion,
                                                       remote_offset);
      }

      template <class T>
      template <unsigned T_StateBytes>
      inline bool DmaModel<T>::postDmaPut (
        uint8_t               (&state)[T_StateBytes],
        pami_event_function   local_fn,
        void                * cookie,
        size_t                target_task,
        size_t                target_offset,
        size_t                bytes,
        Memregion           * local_memregion,
        size_t                local_offset,
        Memregion           * remote_memregion,
        size_t                remote_offset)
      {
        // This compile time assert verifies that sufficient memory was provided
        // to maintain the state of the operation.
        COMPILE_TIME_ASSERT(T::dma_model_state_bytes <= T_StateBytes);

        return static_cast<T*>(this)->postDmaPut_impl (state,
                                                       local_fn,
                                                       cookie,
                                                       target_task,
                                                       target_offset,
                                                       bytes,
                                                       local_memregion,
                                                       local_offset,
                                                       remote_memregion,
                                                       remote_offset);
      }


      template <class T>
      inline bool DmaModel<T>::postDmaGet (
        size_t   target_task,
        size_t   target_offset,
        size_t   bytes,
        void   * local,
        void   * remote)
      {
        return static_cast<T*>(this)->postDmaGet_impl (target_task,
                                                       target_offset,
                                                       bytes,
                                                       local,
                                                       remote);
      }

      template <class T>
      template <unsigned T_StateBytes>
      inline bool DmaModel<T>::postDmaGet (
        uint8_t               (&state)[T_StateBytes],
        pami_event_function   local_fn,
        void                * cookie,
        size_t                target_task,
        size_t                target_offset,
        size_t                bytes,
        void                * local,
        void                * remote)
      {
        // This compile time assert verifies that sufficient memory was provided
        // to maintain the state of the operation.
        COMPILE_TIME_ASSERT(T::dma_model_state_bytes <= T_StateBytes);

        return static_cast<T*>(this)->postDmaGet_impl (state,
                                                       local_fn,
                                                       cookie,
                                                       target_task,
                                                       target_offset,
                                                       bytes,
                                                       local,
                                                       remote);
      }

      template <class T>
      inline bool DmaModel<T>::postDmaGet (
        size_t                target_task,
        size_t                target_offset,
        size_t                bytes,
        Memregion           * local_memregion,
        size_t                local_offset,
        Memregion           * remote_memregion,
        size_t                remote_offset)
      {
        return static_cast<T*>(this)->postDmaGet_impl (target_task,
                                                       target_offset,
                                                       bytes,
                                                       local_memregion,
                                                       local_offset,
                                                       remote_memregion,
                                                       remote_offset);
      }

      template <class T>
      template <unsigned T_StateBytes>
      inline bool DmaModel<T>::postDmaGet (
        uint8_t               (&state)[T_StateBytes],
        pami_event_function   local_fn,
        void                * cookie,
        size_t                target_task,
        size_t                target_offset,
        size_t                bytes,
        Memregion           * local_memregion,
        size_t                local_offset,
        Memregion           * remote_memregion,
        size_t                remote_offset)
      {
        // This compile time assert verifies that sufficient memory was provided
        // to maintain the state of the operation.
        COMPILE_TIME_ASSERT(T::dma_model_state_bytes <= T_StateBytes);

        return static_cast<T*>(this)->postDmaGet_impl (state,
                                                       local_fn,
                                                       cookie,
                                                       target_task,
                                                       target_offset,
                                                       bytes,
                                                       local_memregion,
                                                       local_offset,
                                                       remote_memregion,
                                                       remote_offset);
      }

      template <class T>
      template <unsigned T_StateBytes>
      inline bool DmaModel<T>::postDmaFence (
        uint8_t               (&state)[T_StateBytes],
        pami_event_function   local_fn,
        void                * cookie,
        size_t                target_task,
        size_t                target_offset)
      {
        // This compile time assert verifies that sufficient memory was provided
        // to maintain the state of the operation.
        COMPILE_TIME_ASSERT(T::dma_model_state_bytes <= T_StateBytes);

        return static_cast<T*>(this)->postDmaFence_impl (state,
                                                         local_fn,
                                                         cookie,
                                                         target_task,
                                                         target_offset);
      }
    };
  };
};
#endif // __componenets_devices_DmaInterface_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
