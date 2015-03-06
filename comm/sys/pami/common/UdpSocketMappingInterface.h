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
/// \file common/UdpSocketMappingInterface.h
/// \brief ???
///
#ifndef __common_UdpSocketMappingInterface_h__
#define __common_UdpSocketMappingInterface_h__

#include <pami.h>
#include <netinet/in.h>

namespace PAMI
{
  namespace Interface
  {
    namespace Mapping
    {
      ///
      /// \param T_Mapping Socket mapping template class
      ///
      template <class T_Mapping>
      class UdpSocket
      {
        public:

          ///
          /// \brief Get the socket address for the local task
          ///
          /// \param[out]
          /// \param[out]
          ///
          inline void udpAddr (int & rcv_fd);

          ///
          /// \brief Get the socket address for a specific task
          ///
          /// \param[in]  task    Global task identifier
          /// \param[out]
          /// \param[out]
          ///
          inline pami_result_t task2udp (size_t task, int & send_fd, sockaddr * send_addr, int & send_addr_len);

          ///
          /// \brief Get the task associated with a specific socket address
          ///
          /// \param[in]
          /// \param[in]
          /// \param[out] task    Global task identifier
          ///
          inline pami_result_t udp2task (int send_fd, sockaddr * send_addr, int send_addr_len, size_t & task);
      }; // class UdpSocket

      template <class T_Mapping>
      inline void UdpSocket<T_Mapping>::udpAddr (int & rcv_fd)
      {
        return static_cast<T_Mapping*>(this)->udpAddr_impl (rcv_fd);
      }

      template <class T_Mapping>
      inline pami_result_t UdpSocket<T_Mapping>::task2udp (size_t task, int & send_fd, sockaddr * send_addr, int & send_addr_len)
      {
        return static_cast<T_Mapping*>(this)->task2udp_impl (task, send_fd, send_addr, send_addr_len);
      }

      template <class T_Mapping>
      inline pami_result_t UdpSocket<T_Mapping>::udp2task (int send_fd, sockaddr * send_addr, int send_addr_len, size_t & task)
      {
        return static_cast<T_Mapping*>(this)->udp2task_impl (send_fd, send_addr, send_addr_len, task);
      }
    };	// namespace Mapping
  };	// namespace Interface
};	// namespace PAMI
#endif // __common_UdpSocketMappingInterface_h__
