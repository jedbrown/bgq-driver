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
 * \file components/devices/shmem/shaddr/SystemShaddr.h
 * \brief ???
 */

#ifndef __components_devices_shmem_shaddr_SystemShaddr_h__
#define __components_devices_shmem_shaddr_SystemShaddr_h__

#include <pami.h>

#ifndef TRACE_ERR
#define TRACE_ERR(x) // fprintf x
#endif

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
      namespace Shaddr
      {
        class SystemShaddrInfo
        {
          public:
            inline SystemShaddrInfo (Memregion * origin_mr,
                                     Memregion * target_mr,
                                     size_t      origin_offset,
                                     size_t      target_offset,
                                     size_t      bytes) :
                _origin_mr (*origin_mr),
                _target_mr (*target_mr),
                _origin_offset (origin_offset),
                _target_offset (target_offset),
                _bytes (bytes)
            {
            }

            ///
            /// \see PAMI::Device::Interface::RecvFunction_t
            ///
            template <class T_Device>
            static int system_shaddr_read (void   * metadata,
                                           void   * payload,
                                           size_t   bytes,
                                           void   * recv_func_parm,
                                           void   * cookie)
            {
              TRACE_ERR((stderr, ">> SystemShaddrInfo::system_shaddr_read():%d .. recv_func_parm = %p\n", __LINE__, recv_func_parm));

              T_Device * device = (T_Device *) recv_func_parm;

              SystemShaddrInfo * info = (SystemShaddrInfo *) payload;
              size_t total_bytes = info->_bytes;
              size_t bytes_copied = 0;
              size_t target_offset = info->_target_offset;
              size_t origin_offset = info->_origin_offset;

              TRACE_ERR((stderr, ">> SystemShaddrInfo::system_shaddr_read():%d .. total_bytes = %zu\n", __LINE__, total_bytes));

              while (bytes_copied < total_bytes)
                {
                  bytes_copied += device->shaddr.read (&(info->_target_mr),
                                                       target_offset + bytes_copied,
                                                       &(info->_origin_mr),
                                                       origin_offset + bytes_copied,
                                                       total_bytes - bytes_copied);
                }

              TRACE_ERR((stderr, "<< SystemShaddrInfo::system_shaddr_read():%d\n", __LINE__));
              return 0;
            }

            ///
            /// \see PAMI::Device::Interface::RecvFunction_t
            ///
            template <class T_Device>
            static int system_shaddr_write (void   * metadata,
                                            void   * payload,
                                            size_t   bytes,
                                            void   * recv_func_parm,
                                            void   * cookie)
            {
              TRACE_ERR((stderr, ">> SystemShaddrInfo::system_shaddr_write():%d\n", __LINE__));

              T_Device * device = (T_Device *) recv_func_parm;

              SystemShaddrInfo * info = (SystemShaddrInfo *) payload;
              size_t total_bytes = info->_bytes;
              size_t bytes_copied = 0;
              size_t target_offset = info->_target_offset;
              size_t origin_offset = info->_origin_offset;

              while (bytes_copied < total_bytes)
                {
                  bytes_copied += device->shaddr.read (&(info->_origin_mr),
                                                       origin_offset + bytes_copied,
                                                       &(info->_target_mr),
                                                       target_offset + bytes_copied,
                                                       total_bytes - bytes_copied);
                }

              TRACE_ERR((stderr, "<< SystemShaddrInfo::system_shaddr_write():%d\n", __LINE__));
              return 0;
            }

            Memregion _origin_mr;
            Memregion _target_mr;
            size_t    _origin_offset;
            size_t    _target_offset;
            size_t    _bytes;
        };

        ///
        /// \tparam T_Shaddr ShaddrInterface implementation class
        ///
        template <class T_Shaddr>
        class System : public T_Shaddr
        {
          public:


            template <class T_Device>
            System (T_Device * device) :
                T_Shaddr ()
            {
              device->registerSystemRecvFunction (SystemShaddrInfo::system_shaddr_read<T_Device>,
                                                  this, system_ro_put_dispatch);

              this->initialize (device);

            }

            uint16_t system_ro_put_dispatch;

        };// PAMI::Device::Shmem::Shaddr::System class
      };  // PAMI::Device::Shmem::Shaddr namespace
    };    // PAMI::Device::Shmem namespace
  };      // PAMI::Device namespace
};        // PAMI namespace

#endif /* __components_devices_shmem_shaddr_SystemShaddr_h__ */
