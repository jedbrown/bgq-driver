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
 * \file components/devices/shmem/shaddr/ShaddrInterface.h
 * \brief ???
 */

#ifndef __components_devices_shmem_shaddr_ShaddrInterface_h__
#define __components_devices_shmem_shaddr_ShaddrInterface_h__

#include <pami.h>

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
      ///
      /// \tparam T ShaddrInterface implementation class
      ///
      template <class T>
      class ShaddrInterface
      {
        public:

          template <class T_Device>
          inline void initialize (T_Device * device);

          inline bool isEnabled ();

          ///
          /// \brief Shared address virtual address operation support
          ///
          /// \attention All shared address interface derived classes \b must
          ///            contain a public static const data member named
          ///            'bool shaddr_va_supported'.
          ///
          /// C++ code using templates to specify the shared address
          /// implementation may statically access the 'shaddr_va_supported'
          /// constant.
          ///
          static bool isVirtualAddressSupported ();

          ///
          /// \brief Shared address memory region operation support
          ///
          /// \attention All shared address interface derived classes \b must
          ///            contain a public static const data member named
          ///            'bool shaddr_mr_supported'.
          ///
          /// C++ code using templates to specify the shared address
          /// implementation may statically access the 'shaddr_mr_supported'
          /// constant.
          ///
          static bool isMemoryRegionSupported ();

          ///
          /// \brief Shared address read operation support
          ///
          /// \attention All shared address interface derived classes \b must
          ///            contain a public static const data member named
          ///            'bool shaddr_read_supported'.
          ///
          /// C++ code using templates to specify the shared address
          /// implementation may statically access the 'shaddr_read_supported'
          /// constant.
          ///
          static bool isReadSupported ();

          ///
          /// \brief Shared address write operation support
          ///
          /// \attention All shared address interface derived classes \b must
          ///            contain a public static const data member named
          ///            'bool shaddr_write_supported'.
          ///
          /// C++ code using templates to specify the shared address
          /// implementation may statically access the 'shaddr_write_supported'
          /// constant.
          ///
          static bool isWriteSupported ();

          ///
          /// \brief Shared address read operation using virtual addresses
          ///
          /// Shared address read from the remote source buffer on the remote
          /// task to the local destination buffer on the local task.
          ///
          /// \param[in] local  Local destination buffer
          /// \param[in] remote Remote source buffer
          /// \param[in] bytes  Number of bytes to transfer
          /// \param[in] task   Remote task id
          ///
          /// \return Number of bytes read from the remote memory region
          ///
          inline size_t read (void   * local,
                              void   * remote,
                              size_t   bytes,
                              size_t   task);

          ///
          /// \brief Shared address write operation using virtual addresses
          ///
          /// Shared address write to the remote destination buffer on the
          /// remote task from the local source buffer on the local task.
          ///
          /// \param[in] remote Remote destination buffer
          /// \param[in] local  Local source buffer
          /// \param[in] bytes  Number of bytes to transfer
          /// \param[in] task   Remote task id
          ///
          /// \return Number of bytes written to the remote memory region
          ///
          inline size_t write (void   * remote,
                               void   * local,
                               size_t   bytes,
                               size_t   task);

          ///
          /// \brief Shared address read operation using memory regions
          ///
          /// Shared address read from the remote memory region and offset to
          /// to the local memory region and offset
          ///
          /// \param[in] local         Local destination memory region
          /// \param[in] local_offset  Destination offset
          /// \param[in] remote        Remote source memory region
          /// \param[in] remote_offset Source offset
          /// \param[in] bytes         Number of bytes to transfer
          ///
          /// \return Number of bytes read from the remote memory region
          ///
          inline size_t read (Memregion * local,
                              size_t      local_offset,
                              Memregion * remote,
                              size_t      remote_offset,
                              size_t      bytes);

          ///
          /// \brief Shared address write operation using memory regions
          ///
          /// Shared address write to the remote memory region and offset from
          /// the local memory region and offset
          ///
          /// \param[in] remote        Remote destination memory region
          /// \param[in] remote_offset Destination offset
          /// \param[in] local         Local source memory region
          /// \param[in] local_offset  Source offset
          /// \param[in] bytes         Number of bytes to transfer
          ///
          /// \return Number of bytes written to the remote memory region
          ///
          inline size_t write (Memregion * remote,
                               size_t      remote_offset,
                               Memregion * local,
                               size_t      local_offset,
                               size_t      bytes);

      };  // PAMI::Device::Shmem::ShaddrInterface::FactoryInterface class
    };    // PAMI::Device::Shmem namespace
  };      // PAMI::Device namespace
};        // PAMI namespace

template <class T>
template <class T_Device>
void PAMI::Device::Shmem::ShaddrInterface<T>::initialize (T_Device * device)
{
  static_cast<T*>(this)->initialize_impl (device);
};

template <class T>
bool PAMI::Device::Shmem::ShaddrInterface<T>::isEnabled ()
{
  return static_cast<T*>(this)->isEnabled_impl ();
};

template <class T>
bool PAMI::Device::Shmem::ShaddrInterface<T>::isVirtualAddressSupported ()
{
  // This compile time assert verifies that at least one of the two
  // types of shared address interfaces is supported.
  COMPILE_TIME_ASSERT(T::shaddr_va_supported || T::shaddr_mr_supported);

  return T::shaddr_va_supported;
};

template <class T>
bool PAMI::Device::Shmem::ShaddrInterface<T>::isMemoryRegionSupported ()
{
  // This compile time assert verifies that at least one of the two
  // types of shared address interfaces is supported.
  COMPILE_TIME_ASSERT(T::shaddr_va_supported || T::shaddr_mr_supported);

  return T::shaddr_mr_supported;
};

template <class T>
bool PAMI::Device::Shmem::ShaddrInterface<T>::isReadSupported ()
{
  // This compile time assert verifies that at least one of the two
  // types of shared address operations is supported.
  COMPILE_TIME_ASSERT(T::shaddr_read_supported || T::shaddr_write_supported);

  return T::shaddr_read_supported;
};

template <class T>
bool PAMI::Device::Shmem::ShaddrInterface<T>::isWriteSupported ()
{
  // This compile time assert verifies that at least one of the two
  // types of shared address operations is supported.
  COMPILE_TIME_ASSERT(T::shaddr_read_supported || T::shaddr_write_supported);

  return T::shaddr_write_supported;
};

template <class T>
size_t PAMI::Device::Shmem::ShaddrInterface<T>::read (void   * local,
                                                      void   * remote,
                                                      size_t   bytes,
                                                      size_t   task)
{
  return static_cast<T*>(this)->read_impl (local, remote, bytes, task);
};

template <class T>
size_t PAMI::Device::Shmem::ShaddrInterface<T>::write (void   * remote,
                                                       void   * local,
                                                       size_t   bytes,
                                                       size_t   task)
{
  return static_cast<T*>(this)->write_impl (remote, local, bytes, task);
};

template <class T>
size_t PAMI::Device::Shmem::ShaddrInterface<T>::read (Memregion * local,
                                                      size_t      local_offset,
                                                      Memregion * remote,
                                                      size_t      remote_offset,
                                                      size_t      bytes)
{
  return static_cast<T*>(this)->read_impl (local, local_offset,
                                           remote, remote_offset,
                                           bytes);
};

template <class T>
size_t PAMI::Device::Shmem::ShaddrInterface<T>::write (Memregion * remote,
                                                       size_t      remote_offset,
                                                       Memregion * local,
                                                       size_t      local_offset,
                                                       size_t      bytes)
{
  return static_cast<T*>(this)->write_impl (remote, remote_offset,
                                            local, local_offset,
                                            bytes);
};

#endif /* __components_devices_shmem_ShaddrInterface_h__ */
