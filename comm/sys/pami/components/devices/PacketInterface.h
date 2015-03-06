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
 * \file components/devices/PacketInterface.h
 * \brief ???
 */

#ifndef __components_devices_PacketInterface_h__
#define __components_devices_PacketInterface_h__

#include <sys/uio.h>

#include <pami.h>
#include "util/common.h"

namespace PAMI
{
  namespace Device
  {
    namespace Interface
    {
      ///
      /// \brief Packet device dispatch receive function signature
      ///
      /// \param[in] metadata       Pointer to network header metadata that is
      ///                           not part of the normal packet payload.
      /// \param[in] payload        Pointer to the raw packet payload.
      /// \param[in] bytes          Number of bytes of packet payload.
      /// \param[in] recv_func_parm Registered dispatch clientdata
      /// \param[in] cookie         Device cookie
      ///
      /// \todo Define return value(s)
      ///
      /// \return ???
      ///
      typedef int (*RecvFunction_t) (void   * metadata,
                                     void   * payload,
                                     size_t   bytes,
                                     void   * recv_func_parm,
                                     void   * cookie);

      ///
      /// \brief Packet device interface.
      ///
      /// \todo Need A LOT MORE documentation on this interface and its use
      /// \param T Packet device interface implementation class
      ///
      template <class T>
      class PacketDevice
      {
        public:


          inline void clearConnection (size_t task, size_t offset = 0)
          {
            static_cast<T*>(this)->clearConnection_impl(task, offset);
          };

          inline void * getConnection (size_t task, size_t offset = 0)
          {
            return static_cast<T*>(this)->getConnection_impl(task, offset);
          };

          inline void setConnection (void * value, size_t task, size_t offset = 0)
          {
            static_cast<T*>(this)->setConnection_impl(value, task, offset);
          };


          PacketDevice() {}
          ~PacketDevice() {}

          ///
          /// \brief Read data from a packet device
          ///
          /// \attention All packet device derived classes \b must
          ///            implement the read_impl() method.
          ///
          /// \see RecvFunction_t
          ///
          /// \param[in] dst      Destination buffer
          /// \param[in] bytes    Number of bytes to read
          /// \param[in] cookie   Device cookie
          ///
          /// \todo Define return value(s)
          ///
          /// \return 0 on success, !0 on failure
          ///
          inline int read (void * dst, size_t bytes, void * cookie);
      };

      template <class T>
      inline int PacketDevice<T>::read (void * dst, size_t bytes, void * cookie)
      {
        return static_cast<T*>(this)->read_impl(dst, bytes, cookie);
      }

      ///
      /// \todo Need A LOT MORE documentation on this interface and its use
      /// \param T_Model       Packet model template class
      /// \param T_Device      Packet device template class
      /// \param T_StateBytes  Transfer state object size in bytes
      ///
      /// \see PacketDevice
      ///
      template <class T>
      class PacketModel
      {
        protected:

          /// \brief Packet model interface constructor
          ///
          /// \param[in] device  Packet device reference
          ///
          template <class T_Device>
          PacketModel (T_Device & device)
          {
          };

          ~PacketModel () {};

        public:

          ///
          /// \brief Returns the deterministic network attribute of this model
          ///
          /// A deterministic network "routes" all communication in a fixed,
          /// deterministic, way such that messages are received by the remote
          /// task in the same order as the messages were sent by the local
          /// task.
          ///
          /// \attention All packet model interface derived classes \b must
          ///            contain a public static const data member named
          ///            'bool deterministic_packet_model'.
          ///
          /// C++ code using templates to specify the model may statically
          /// access the 'deterministic_packet_model' constant.
          ///
          static  bool isPacketDeterministic ();

          ///
          /// \brief Returns the reliable network attribute of this model
          ///
          /// A reliable network will not drop packets during the packet
          /// transfer. Protocols written to a reliable packet model
          /// implementation may assume that all packets sent using the
          /// packet interface methods of the model \b will arrive at the
          /// destination task.
          ///
          /// \attention All packet model interface derived classes \b must
          ///            contain a public static const data member named
          ///            'bool reliable_packet_model'.
          ///
          /// C++ code using templates to specify the model may statically
          /// access the 'reliable_packet_model' constant.
          ///
          static bool isPacketReliable ();

          ///
          /// \brief Returns the 'read() required' attribute of this model
          ///
          /// \attention All packet model interface derived classes \b must
          ///            contain a public static const data member named
          ///            'bool read_is_required_packet_model'.
          ///
          /// C++ code using templates to specify the model may statically
          /// access the 'read_is_required_packet_model' constant.
          ///
          /// \retval true  The packet device 'read()' method must be used to
          ///               access the packet payload from within the packet
          ///               model dispatch function.
          /// \retval false The packet payload data may be directly accessed
          ///               from within the packet model dispatch function.
          ///
          static bool isReadRequired ();

          ///
          /// \brief Returns the maximum metadata bytes attribute of this model for single-packet transfers.
          ///
          /// Certain packet-based hardware may provide a contiguous area in
          /// packet network header that may be initialized and transfered with
          /// the packet to its destination. This attribute specifies the
          /// maximum number of bytes that may be sent in the packet metadata
          /// using any of the post* methods of the packet model interface.
          ///
          /// A packet model implementation may return zero as the number of
          /// single-packet metadata bytes supported.
          ///
          /// \attention All packet model interface derived classes \b must
          ///            contain a public static const data member named
          ///            'size_t packet_model_metadata_bytes'.
          ///
          /// C++ code using templates to specify the model may statically
          /// access the 'packet_model_metadata_bytes' constant.
          ///
          static size_t getPacketMetadataBytes ();

          ///
          /// \brief Returns the maximum metadata bytes attribute of this model for multi-packet transfers.
          ///
          /// Certain packet-based hardware may provide a contiguous area in
          /// packet network header that may be initialized and transfered with
          /// each packet of a multi-packet message to its destination. This
          /// attribute specifies the maximum number of bytes that may be sent
          /// in the packet metadata using any of the post* methods of the
          /// packet model interface.
          ///
          /// A packet model implementation may return zero as the number of
          /// multi-packet metadata bytes supported.
          ///
          /// \attention All packet model interface derived classes \b must
          ///            contain a public static const data member named
          ///            'size_t packet_model_multi_metadata_bytes'.
          ///
          /// C++ code using templates to specify the model may statically
          /// access the 'packet_model_metadata_bytes' constant.
          ///
          static size_t getPacketMultiMetadataBytes ();

          ///
          /// \brief Returns the maximum payload bytes attribute of this model.
          ///
          /// Packet-based network hardware provides a contiguous payload area
          /// within each packet transfered to the destination task. This
          /// attribute specifies the maximum number of bytes that may be sent
          /// in the packet payload using any of the post* methods of the
          /// packet model interface.
          ///
          /// A packet model implementation may return zero as the number of
          /// packet payload bytes supported.
          ///
          /// \attention All packet model interface derived classes \b must
          ///            contain a public static const data member named
          ///            'size_t packet_model_payload_bytes'.
          ///
          /// C++ code using templates to specify the model may statically
          /// access the 'packet_model_payload_bytes' constant.
          ///
          static size_t getPacketPayloadBytes ();

          ///
          /// \brief Returns the maximum immediate payload bytes attribute of this model.
          ///
          /// Packet-based network hardware may provide the capability to
          /// inject a packet immediately if network resources are available.
          /// This attribute specifies the maximum number of bytes that may be
          /// immediately sent in the packet payload using the 'immediate'
          /// postPacket method, which does not have a callback, of the
          /// packet model interface.
          ///
          /// A packet model implementation may return zero as the number of
          /// packet immediate payload bytes supported.
          ///
          /// \attention All packet model interface derived classes \b must
          ///            contain a public static const data member named
          ///            'size_t packet_model_immediate_bytes'.
          ///
          /// C++ code using templates to specify the model may statically
          /// access the 'packet_model_immediate_bytes' constant.
          ///
          /// \note The \c packet_model_immediate_bytes value may be different
          ///       than the \c packet_model_payload_bytes value
          ///
          static size_t getPacketImmediateBytes ();

          ///
          /// \brief Returns the transfer state bytes attribute of this model.
          ///
          /// Typically a packet device will require some amount of temporary
          /// storage to be used during the transfer of the packet. This
          /// attribute returns the number of bytes that must be provided to
          /// the various packet post* methods.
          ///
          /// \attention All packet model interface derived classes \b must
          ///            contain a public static const data member named
          ///            'size_t packet_model_state_bytes'.
          ///
          /// C++ code using templates to specify the model may statically
          /// access the 'packet_model_state_bytes' constant.
          ///
          static size_t getPacketTransferStateBytes ();

          ///
          /// \brief Initialize the packet model
          ///
          /// \param [in] dispatch       Dispatch set identifier
          /// \param [in] recv_func      Receive function to register in the dispatch set
          /// \param [in] recv_func_parm Receive function clientdata to register in the dispatch set
          ///
          pami_result_t init (size_t           dispatch,
                              RecvFunction_t   recv_func,
                              void           * recv_func_parm);

          ///
          /// \brief Immediate post of a single packet transfer operation.
          ///
          /// The post packet immediate interface allows the packet model
          /// and packet device implementations to optimize for performance by
          /// avoiding the overhead to construct a packet message object. If
          /// the packet device has resources immediately available then the
          /// packet may be directly posted.
          ///
          /// The post will return \c true if, and only if, the packet was
          /// entirely processed by the packet device and the data from \b all
          /// pointers in the parameter list have been copied or otherwise
          /// consumed by the packet device. After this method returns it is
          /// safe to touch the memory pointed to by the metadata and iov
          /// parameters.
          ///
          /// The post will return \c false if the packet device was unable to
          /// immediately complete the transfer operation. This can be as a
          /// result of any number of conditions specific to each packet device
          /// implementation. For example, a "fifo full" condition would result
          /// in an inability to copy the packet into the fifo. In this case
          /// the packet device would immediately cease processing the packet
          /// operation and would return \c false to the caller. The caller
          /// is expected to handle this condition and perhaps post the packet
          /// information again using one of the asynchronous packet post
          /// methods.
          ///
          /// It is important to understand the difference between the various
          /// packet event types for a packet post operation. These events
          /// include:
          /// - post begin
          /// - post complete
          /// - injection begin
          /// - injection complete
          /// - reception
          ///
          /// When the post immediate returns \c true this is the same as
          /// reporting that the "post begin" \b and the "post complete" packet
          /// events have occured. When the post immediate returns \c false
          /// this is the same as reporting the \b no packet events have
          /// occured.
          ///
          /// A successful post does not imply that any other events have
          /// occured. For example, it is \b unknown if:
          /// - the data has been injected into the network hardware resources
          /// - the network hardware resources have completed sending the data
          /// - the destination has received the data
          ///
          /// \note The packet device will only transfer a single packet. The
          ///       payload data will be truncated if more bytes than a single
          ///       packet can transfer is specified as the payload.
          ///
          /// \see PAMI::Device::Interface::PacketDevice::getPacketPayloadSize()
          ///
          /// \note The size of the metadata to be copied into the packet
          ///       is an attribute of the specific packet device associated
          ///       with this packet model. The metaload data will be truncated
          ///       if more metadat bytes than a single packet can transfer
          ///       is specified.
          ///
          /// \see PAMI::Device::Interface::PacketDevice::getPacketMetadataSize()
          ///
          /// \param[in] target_task     Global identifier of the packet destination task
          /// \param[in] target_offset  Identifier of the packet destination context
          /// \param[in] metadata        Virtual address of metadata buffer
          /// \param[in] metasize        Number of metadata bytes
          /// \param[in] iov             Array of iovec elements to transfer
          ///
          /// \retval true  All data has been posted
          /// \retval false No data has been posted
          ///
          template <unsigned T_Niov>
          inline bool postPacket (size_t         target_task,
                                  size_t         target_offset,
                                  void         * metadata,
                                  size_t         metasize,
                                  struct iovec   (&iov)[T_Niov]);

          ///
          /// \brief Asynchronous single packet transfer operation with completion event notification
          ///
          /// The post packet with notification interfaces are used to
          /// asynchronously inject a packet to the network resources managed
          /// by the packet device. The packet device will invoke the
          /// completion event function when the packet has been completely
          /// processed.
          ///
          /// The asychronous interfaces allow the packet model and packet
          /// implementations to optimize for performance by avoiding the
          /// overhead to construct an internal packet state object if the
          /// packet device network resources are immediately available.
          /// In this situation, the completion event function will be
          /// immediately invoked and the post method will return \c true.
          ///
          /// The post will return \c false if the packet device was unable to
          /// immediately complete the transfer operation. This can be as a
          /// result of any number of conditions specific to each packet device
          /// implementation. For example, a "fifo full" condition would result
          /// in an inability to copy the packet into the fifo, or a "pending post"
          /// queue may be not empty and the current post packet would need to
          /// be enqueued. In these cases the packet device would immediately
          /// cease processing the packet and instead may instantiate a class
          /// into the \c state array provided as an input parameter. This
          /// device-specific internal state object may be used to track the
          /// state of the packet transfer operation until all of the packet
          /// data has been copied or otherwise processed. When the packet
          /// transfer is complete the completion event function will be
          /// invoked.
          ///
          /// \attention The metadata pointer, iovec array pointer, and the
          ///            base address pointers in all of the iovec elements
          ///            \b must continue to reference valid information until
          ///            the completion event function is invoked. In other
          ///            words, do not put metadata structures, iovec arrays
          ///            or data buffers on the stack.
          ///
          /// \note The packet device will only transfer a single packet. The
          ///       payload data will be truncated if more bytes than a single
          ///       packet can transfer is specified as the payload.
          ///
          /// \see PAMI::Device::Interface::PacketDevice::getPacketPayloadSize()
          ///
          /// \note The size of the metadata to be copied into the packet
          ///       is an attribute of the specific packet device associated
          ///       with this packet model. The metaload data will be truncated
          ///       if more metadat bytes than a single packet can transfer
          ///       is specified.
          ///
          /// \see PAMI::Device::Interface::PacketDevice::getPacketMetadataSize()
          ///
          /// \param[in] state           Byte array for the packet transfer state
          /// \param[in] fn              Event function to invoke when the operation completes
          /// \param[in] cookie          Event function cookie
          /// \param[in] target_task     Global identifier of the packet destination task
          /// \param[in] target_offset  Identifier of the packet destination context
          /// \param[in] metadata        Virtual address of metadata buffer
          /// \param[in] metasize        Number of metadata bytes
          /// \param[in] iov             Iovec array to specify the data to be copied into the packet payload
          /// \param[in] niov            Number of iovec array elements
          ///
          /// \retval true  Packet transfer operation completed and the
          ///               completion event function was invoked
          ///
          /// \retval false Packet transfer operation did not complete and the
          ///               packet device must be advanced until the completion
          ///               event function is invoked
          ///
          template <unsigned T_StateBytes>
          inline bool postPacket (uint8_t               (&state)[T_StateBytes],
                                  pami_event_function   fn,
                                  void                * cookie,
                                  size_t                target_task,
                                  size_t                target_offset,
                                  void                * metadata,
                                  size_t                metasize,
                                  struct iovec        * iov,
                                  size_t                niov);

          template <unsigned T_StateBytes, unsigned T_Niov>
          inline bool postPacket (uint8_t               (&state)[T_StateBytes],
                                  pami_event_function   fn,
                                  void                * cookie,
                                  size_t                target_task,
                                  size_t                target_offset,
                                  void                * metadata,
                                  size_t                metasize,
                                  struct iovec          (&iov)[T_Niov]);

          ///
          /// often, only a single contiguous buffer is sent in the packet
          /// payload.  Create a special-case interface for this which removes
          /// the iovec management responsibilities...
          ///
          template <unsigned T_StateBytes>
          inline bool postPacket (uint8_t               (&state)[T_StateBytes],
                                  pami_event_function   fn,
                                  void                * cookie,
                                  size_t                target_task,
                                  size_t                target_offset,
                                  void                * metadata,
                                  size_t                metasize,
                                  void                * payload,
                                  size_t                length);

          ///
          /// \brief Post a multiple packet transfer operation
          ///
          /// Transfer a contiguous source buffer to the destination
          /// task as a stream of individual packets.  The metadata is copied
          /// into each packet header and is unchanged during the duration of
          /// the transfer.
          ///
          /// The post multi-packet interface allows the packet model and
          /// device implementations to optimize for performance by avoiding
          /// the overhead to construct a transfer state object. If
          /// the device has resources immediately available then the
          /// transfer operation may be directly posted, otherwise a transfer state object
          /// may be constructed at the location specified to maintain the
          /// operation state until the completion callback is invoked.
          ///
          /// \attention It is \em illegal to send zero bytes of data with the
          ///            postMultiPacket() interface. Zero byte transfers must
          ///            use one of the postPacket() interfaces for single-packet
          ///            transfers.
          ///
          /// \note The size of the metadata to be copied into the packet
          ///       is an attribute of the specific packet device associated
          ///       with this packet model.
          ///
          /// \see PAMI::Device::Interface::PacketDevice::getMultiPacketMetadataSize()
          ///
          /// \param[in] state           Location to store the transfer object
          /// \param[in] fn              Event function to invoke when the operation completes
          /// \param[in] cookie          Opaque data to provide as the cookie parameter of the event function.
          /// \param[in] target_task     Global identifier of the packet destination task
          /// \param[in] target_offset   Identifier of the packet destination context
          /// \param[in] metadata        Virtual address of metadata buffer to transfer
          /// \param[in] metasize        Number of metadata bytes to transfer
          /// \param[in] payload         Virtual address of data buffer to transfer
          /// \param[in] length          Number of data bytes to transfer
          ///
          /// \retval true  Transfer operation completed and the completion
          ///               callback was invoked
          ///
          /// \retval false Transfer operation did not complete and the
          ///               device must be advanced until the completion
          ///               callback is invoked
          ///
          template <unsigned T_StateBytes>
          inline bool postMultiPacket (uint8_t               (&state)[T_StateBytes],
                                       pami_event_function   fn,
                                       void                * cookie,
                                       size_t                target_task,
                                       size_t                target_offset,
                                       void                * metadata,
                                       size_t                metasize,
                                       void                * payload,
                                       size_t                length);
      };

      template <class T>
      bool PacketModel<T>::isPacketDeterministic ()
      {
        return T::deterministic_packet_model;
      }

      template <class T>
      bool PacketModel<T>::isPacketReliable ()
      {
        return T::reliable_packet_model;
      }

      template <class T>
      bool PacketModel<T>::isReadRequired ()
      {
        return T::read_is_required_packet_model;
      }

      template <class T>
      size_t PacketModel<T>::getPacketMetadataBytes ()
      {
        return T::packet_model_metadata_bytes;
      }

      template <class T>
      size_t PacketModel<T>::getPacketMultiMetadataBytes ()
      {
        return T::packet_model_multi_metadata_bytes;
      }

      template <class T>
      size_t PacketModel<T>::getPacketPayloadBytes ()
      {
        return T::packet_model_payload_bytes;
      }

      template <class T>
      size_t PacketModel<T>::getPacketImmediateBytes ()
      {
        return T::packet_model_immediate_bytes;
      }

      template <class T>
      size_t PacketModel<T>::getPacketTransferStateBytes ()
      {
        return T::packet_model_state_bytes;
      }

      template <class T>
      pami_result_t PacketModel<T>::init (size_t           dispatch,
                                          RecvFunction_t   recv_func,
                                          void           * recv_func_parm)
      {
        return static_cast<T*>(this)->init_impl (dispatch,
                                                 recv_func,
                                                 recv_func_parm);
      }

      template <class T>
      template <unsigned T_Niov>
      inline bool PacketModel<T>::postPacket (size_t         target_task,
                                              size_t         target_offset,
                                              void         * metadata,
                                              size_t         metasize,
                                              struct iovec   (&iov)[T_Niov])
      {
#ifdef ERROR_CHECKS
        {
          size_t i, bytes = 0;

          for (i = 0; i < T_Niov; i++) bytes += iov[i].iov_len;

          PAMI_assert(bytes <= T::packet_model_payload_bytes);
        }
#endif
        return static_cast<T*>(this)->postPacket_impl (target_task, target_offset, metadata, metasize, iov);
      }

      template <class T>
      template <unsigned T_StateBytes>
      inline bool PacketModel<T>::postPacket (uint8_t               (&state)[T_StateBytes],
                                              pami_event_function   fn,
                                              void                * cookie,
                                              size_t                target_task,
                                              size_t                target_offset,
                                              void                * metadata,
                                              size_t                metasize,
                                              struct iovec        * iov,
                                              size_t                niov)
      {
#ifdef ERROR_CHECKS
        {
          size_t i, bytes = 0;

          for (i = 0; i < niov; i++) bytes += iov[i].iov_len;

          PAMI_assert(bytes <= T::packet_model_payload_bytes);
        }
#endif
        // This compile time assert verifies that sufficient memory was provided
        // to maintain the state of the post operation.
        COMPILE_TIME_ASSERT(T::packet_model_state_bytes <= T_StateBytes);

        return static_cast<T*>(this)->postPacket_impl (state, fn, cookie,
                                                       target_task, target_offset,
                                                       metadata, metasize,
                                                       iov, niov);
      }

      template <class T>
      template <unsigned T_StateBytes, unsigned T_Niov>
      inline bool PacketModel<T>::postPacket (uint8_t               (&state)[T_StateBytes],
                                              pami_event_function   fn,
                                              void                * cookie,
                                              size_t                target_task,
                                              size_t                target_offset,
                                              void                * metadata,
                                              size_t                metasize,
                                              struct iovec          (&iov)[T_Niov])
      {
#ifdef ERROR_CHECKS
        {
          size_t i, bytes = 0;

          for (i = 0; i < T_Niov; i++) bytes += iov[i].iov_len;

          PAMI_assert(bytes <= T::packet_model_payload_bytes);
        }
#endif
        // This compile time assert verifies that sufficient memory was provided
        // to maintain the state of the post operation.
        COMPILE_TIME_ASSERT(T::packet_model_state_bytes <= T_StateBytes);

        return static_cast<T*>(this)->postPacket_impl (state, fn, cookie,
                                                       target_task, target_offset,
                                                       metadata, metasize, iov);
      }

      template <class T>
      template <unsigned T_StateBytes>
      inline bool PacketModel<T>::postPacket (uint8_t               (&state)[T_StateBytes],
                                              pami_event_function   fn,
                                              void                * cookie,
                                              size_t                target_task,
                                              size_t                target_offset,
                                              void                * metadata,
                                              size_t                metasize,
                                              void                * payload,
                                              size_t                length)
      {
        // This compile time assert verifies that sufficient memory was provided
        // to maintain the state of the post operation.
        COMPILE_TIME_ASSERT(T::packet_model_state_bytes <= T_StateBytes);

        return static_cast<T*>(this)->postPacket_impl (state, fn, cookie,
                                                       target_task, target_offset,
                                                       metadata, metasize,
                                                       payload, length);
      }

      template <class T>
      template <unsigned T_StateBytes>
      inline bool PacketModel<T>::postMultiPacket (uint8_t               (&state)[T_StateBytes],
                                                   pami_event_function   fn,
                                                   void                * cookie,
                                                   size_t                target_task,
                                                   size_t                target_offset,
                                                   void                * metadata,
                                                   size_t                metasize,
                                                   void                * payload,
                                                   size_t                length)
      {
        // This compile time assert verifies that sufficient memory was provided
        // to maintain the state of the post operation.
        COMPILE_TIME_ASSERT(T::packet_model_state_bytes <= T_StateBytes);

        PAMI_assert_debugf(payload!=NULL, "payload must not be NULL for PacketModel<T>::postMultiPacket()\n");
        PAMI_assert_debugf(length>0, "length must not be zero for PacketModel<T>::postMultiPacket()\n");

        return static_cast<T*>(this)->postMultiPacket_impl (state, fn, cookie,
                                                            target_task, target_offset,
                                                            metadata, metasize,
                                                            payload, length);
      }
    };
  };
};
#endif // __components_devices_PacketInterface_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
