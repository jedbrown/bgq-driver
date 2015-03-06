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
 * \file components/devices/CollectivePacketInterface.h
 * \brief Define a collective packet interface
 */

#ifndef __components_devices_CollectivePacketInterface_h__
#define __components_devices_CollectivePacketInterface_h__

#include "components/devices/PacketInterface.h"

namespace PAMI
{
  namespace Device
  {
    namespace Interface
    {

      ///
      /// \todo Need A LOT MORE documentation on this interface and its use
      /// \param T_Model       Collective Packet model template class
      /// \param T_Device      Collective Packet device template class
      /// \param T_StateBytes  Transfer state object size in bytes
      /// \param T_Route       Class representing the collective "route"
      ///
      /// \brief The Collective Packe Model inherits most of the Packet
      /// Model interface and adds an interface for posting collective
      /// packets to a collective "route" (without a specific destination).
      ///
      /// \see PacketModel
      ///
      template <class T_Model, class T_Device, unsigned T_StateBytes, class T_Route, class T_Op, class T_Dt>
      class CollectivePacketModel
      {
      public:

        /// \brief Collective Packet model interface constructor
        ///
        /// \param[in] device  Collective Packet device reference
        ///
        CollectivePacketModel (T_Device & device)
        {
          // This compile time assert verifies that the specific packet model
          // class, T_Model, has correctly specified the same value for the
          // 'transfer state bytes' template parameter and constant.
          COMPILE_TIME_ASSERT(T_Model::packet_model_state_bytes == T_StateBytes);
        };

        ~CollectivePacketModel ()
        {
        };

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
        static const bool isPacketDeterministic ();

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
        static const bool isPacketReliable ();

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
        static const size_t getPacketMetadataBytes ();

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
        static const size_t getPacketMultiMetadataBytes ();

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
        static const size_t getPacketPayloadBytes ();

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
        static const size_t getPacketTransferStateBytes ();

        ///
        /// \brief Base packet model initializer
        ///
        /// The packet device implementation will use the appropriate receive
        /// function depending on the packet device "requires read" attribute.
        ///
        /// \todo remove the 'read' from collective packets?
        ///
        /// \see PAMI::Device::Interface::PacketDevice::readData()
        ///
        /// \param[in] dispatch              Dispatch set identifier
        /// \param[in] direct_recv_func      Receive function for direct-access packet devices
        /// \param[in] direct_recv_func_parm Receive function clientdata for direct-access packet devices
        /// \param[in] read_recv_func        Receive function for read-access packet devices
        /// \param[in] read_recv_func_parm   Receive function clientdata for read-access packet devices
        ///
        pami_result_t init (size_t           dispatch,
                            RecvFunction_t   direct_recv_func,
                            void           * direct_recv_func_parm,
                            RecvFunction_t   read_recv_func,
                            void           * read_recv_func_parm);

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
        /// \param[in] route           Global identifier of the collective route
        /// \param[in] root            Reduce root task (optional)
        /// \param[in] op              Reduce operation (optional)
        /// \param[in] dt              Reduce datatype (optional)
        /// \param[in] metadata        Virtual address of metadata buffer
        /// \param[in] metasize        Number of metadata bytes
        /// \param[in] iov             Iovec array to specify the data to be copied into the packet payload
        /// \param[in] niov            Number of iovec array elements
        ///
        /// \retval true  Collective Packet transfer operation completed and the
        ///               completion event function was invoked
        ///
        /// \retval false Collective Packet transfer operation did not complete and the
        ///               packet device must be advanced until the completion
        ///               event function is invoked
        ///
        inline bool postCollectivePacket (uint8_t              (&state)[T_StateBytes],
                                          pami_event_function   fn,
                                          void               * cookie,
                                          T_Route              route,
                                          void               * metadata,
                                          size_t               metasize,
                                          struct iovec       * iov,
                                          size_t               niov);
        inline bool postCollectivePacket (uint8_t              (&state)[T_StateBytes],
                                          pami_event_function   fn,
                                          void               * cookie,
                                          T_Route              route,
                                          size_t               root,
                                          T_Op                 op,
                                          T_Dt                 dt,
                                          void               * metadata,
                                          size_t               metasize,
                                          struct iovec       * iov,
                                          size_t               niov);

        template <unsigned T_Niov>
        inline bool postCollectivePacket (uint8_t              (&state)[T_StateBytes],
                                          pami_event_function   fn,
                                          void               * cookie,
                                          T_Route              route,
                                          void               * metadata,
                                          size_t               metasize,
                                          struct iovec         (&iov)[T_Niov]);
        template <unsigned T_Niov>
        inline bool postCollectivePacket (uint8_t              (&state)[T_StateBytes],
                                          pami_event_function   fn,
                                          void               * cookie,
                                          T_Route              route,
                                          size_t               root,
                                          T_Op                 op,
                                          T_Dt                 dt,
                                          void               * metadata,
                                          size_t               metasize,
                                          struct iovec         (&iov)[T_Niov]);

        ///
        /// often, only a single contiguous buffer is sent in the packet
        /// payload.  Create a special-case interface for this which removes
        /// the iovec management responsibilities...
        ///
        inline bool postCollectivePacket (uint8_t              (&state)[T_StateBytes],
                                          pami_event_function   fn,
                                          void               * cookie,
                                          T_Route              route,
                                          void               * metadata,
                                          size_t               metasize,
                                          void               * payload,
                                          size_t               length);
        inline bool postCollectivePacket (uint8_t              (&state)[T_StateBytes],
                                          pami_event_function   fn,
                                          void               * cookie,
                                          T_Route              route,
                                          size_t               root,
                                          T_Op                 op,
                                          T_Dt                 dt,
                                          void               * metadata,
                                          size_t               metasize,
                                          void               * payload,
                                          size_t               length);

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
        /// \note The size of the metadata to be copied into the packet
        ///       is an attribute of the specific packet device associated
        ///       with this packet model.
        ///
        /// \see PAMI::Device::Interface::PacketDevice::getMultiPacketMetadataSize()
        ///
        /// \param[in] state           Location to store the transfer object
        /// \param[in] fn              Event function to invoke when the operation completes
        /// \param[in] cookie          Opaque data to provide as the cookie parameter of the event function.
        /// \param[in] route           Global identifier of the collective route
        /// \param[in] root            Reduce root task (optional)
        /// \param[in] op              Reduce operation (optional)
        /// \param[in] dt              Reduce datatype (optional)
        /// \param[in] metadata        Virtual address of metadata buffer
        /// \param[in] metasize        Number of metadata bytes
        /// \param[in] iov             Array of iovec elements to transfer
        ///
        /// \retval true  Transfer operation completed and the completion
        ///               callback was invoked
        ///
        /// \retval false Transfer operation did not complete and the
        ///               device must be advanced until the completion
        ///               callback is invoked
        ///
        inline bool postMultiCollectivePacket (uint8_t              (&state)[T_StateBytes],
                                               pami_event_function   fn,
                                               void               * cookie,
                                               T_Route              route,
                                               void               * metadata,
                                               size_t               metasize,
                                               void               * payload,
                                               size_t               length);
        inline bool postMultiCollectivePacket (uint8_t              (&state)[T_StateBytes],
                                               pami_event_function   fn,
                                               void               * cookie,
                                               T_Route              route,
                                               size_t               root,
                                               T_Op                 op,
                                               T_Dt                 dt,
                                               void               * metadata,
                                               size_t               metasize,
                                               void               * payload,
                                               size_t               length);
      };

      template <class T_Model, class T_Device, unsigned T_StateBytes, class T_Route, class T_Op, class T_Dt>
      inline pami_result_t CollectivePacketModel<T_Model, T_Device, T_StateBytes, T_Route, T_Op, T_Dt>::init (size_t           dispatch,
                                                                                                              RecvFunction_t   direct_recv_func,
                                                                                                              void           * direct_recv_func_parm,
                                                                                                              RecvFunction_t   read_recv_func,
                                                                                                              void           * read_recv_func_parm)
      {
        return static_cast<T_Model*>(this)->init_impl (dispatch,
                                                       direct_recv_func,
                                                       direct_recv_func_parm,
                                                       read_recv_func,
                                                       read_recv_func_parm);
      }

      template <class T_Model, class T_Device, unsigned T_StateBytes, class T_Route, class T_Op, class T_Dt>
      inline bool CollectivePacketModel<T_Model, T_Device, T_StateBytes, T_Route, T_Op, T_Dt>::postCollectivePacket (uint8_t              (&state)[T_StateBytes],
                                                                                                                     pami_event_function   fn,
                                                                                                                     void               * cookie,
                                                                                                                     T_Route              route,
                                                                                                                     void               * metadata,
                                                                                                                     size_t               metasize,
                                                                                                                     struct iovec       * iov,
                                                                                                                     size_t               niov)
      {
        return static_cast<T_Model*>(this)->postCollectivePacket_impl (state, fn, cookie,
                                                                       route,
                                                                       metadata, metasize,
                                                                       iov, niov);
      }

      template <class T_Model, class T_Device, unsigned T_StateBytes, class T_Route, class T_Op, class T_Dt>
      inline bool CollectivePacketModel<T_Model, T_Device, T_StateBytes, T_Route, T_Op, T_Dt>::postCollectivePacket (uint8_t              (&state)[T_StateBytes],
                                                                                                                     pami_event_function   fn,
                                                                                                                     void               * cookie,
                                                                                                                     T_Route              route,
                                                                                                                     size_t               root,
                                                                                                                     T_Op                 op,
                                                                                                                     T_Dt                 dt,
                                                                                                                     void               * metadata,
                                                                                                                     size_t               metasize,
                                                                                                                     struct iovec       * iov,
                                                                                                                     size_t               niov)
      {
        return static_cast<T_Model*>(this)->postCollectivePacket_impl (state, fn, cookie,
                                                                       route, root,
                                                                       op, dt,
                                                                       metadata, metasize,
                                                                       iov, niov);
      }

      template <class T_Model, class T_Device, unsigned T_StateBytes, class T_Route, class T_Op, class T_Dt>
      template <unsigned T_Niov>
      inline bool CollectivePacketModel<T_Model, T_Device, T_StateBytes, T_Route, T_Op, T_Dt>::postCollectivePacket (uint8_t              (&state)[T_StateBytes],
                                                                                                                     pami_event_function   fn,
                                                                                                                     void               * cookie,
                                                                                                                     T_Route              route,
                                                                                                                     void               * metadata,
                                                                                                                     size_t               metasize,
                                                                                                                     struct iovec         (&iov)[T_Niov])
      {
        return static_cast<T_Model*>(this)->postCollectivePacket_impl (state, fn, cookie,
                                                                       route,
                                                                       metadata, metasize, iov);
      }

      template <class T_Model, class T_Device, unsigned T_StateBytes, class T_Route, class T_Op, class T_Dt>
      template <unsigned T_Niov>
      inline bool CollectivePacketModel<T_Model, T_Device, T_StateBytes, T_Route, T_Op, T_Dt>::postCollectivePacket (uint8_t              (&state)[T_StateBytes],
                                                                                                                     pami_event_function   fn,
                                                                                                                     void               * cookie,
                                                                                                                     T_Route              route,
                                                                                                                     size_t               root,
                                                                                                                     T_Op                 op,
                                                                                                                     T_Dt                 dt,
                                                                                                                     void               * metadata,
                                                                                                                     size_t               metasize,
                                                                                                                     struct iovec         (&iov)[T_Niov])
      {
        return static_cast<T_Model*>(this)->postCollectivePacket_impl (state, fn, cookie,
                                                                       route, root,
                                                                       op, dt,
                                                                       metadata, metasize, iov);
      }

      template <class T_Model, class T_Device, unsigned T_StateBytes, class T_Route, class T_Op, class T_Dt>
      inline bool CollectivePacketModel<T_Model, T_Device, T_StateBytes, T_Route, T_Op, T_Dt>::postCollectivePacket (uint8_t              (&state)[T_StateBytes],
                                                                                                                     pami_event_function   fn,
                                                                                                                     void               * cookie,
                                                                                                                     T_Route              route,
                                                                                                                     void               * metadata,
                                                                                                                     size_t               metasize,
                                                                                                                     void               * payload,
                                                                                                                     size_t               length)
      {
        return static_cast<T_Model*>(this)->postCollectivePacket_impl (state, fn, cookie,
                                                                       route,
                                                                       metadata, metasize,
                                                                       payload, length);
      }

      template <class T_Model, class T_Device, unsigned T_StateBytes, class T_Route, class T_Op, class T_Dt>
      inline bool CollectivePacketModel<T_Model, T_Device, T_StateBytes, T_Route, T_Op, T_Dt>::postCollectivePacket (uint8_t              (&state)[T_StateBytes],
                                                                                                                     pami_event_function   fn,
                                                                                                                     void               * cookie,
                                                                                                                     T_Route              route,
                                                                                                                     size_t               root,
                                                                                                                     T_Op                 op,
                                                                                                                     T_Dt                 dt,
                                                                                                                     void               * metadata,
                                                                                                                     size_t               metasize,
                                                                                                                     void               * payload,
                                                                                                                     size_t               length)
      {
        return static_cast<T_Model*>(this)->postCollectivePacket_impl (state, fn, cookie,
                                                                       route, root,
                                                                       op, dt,
                                                                       metadata, metasize,
                                                                       payload, length);
      }

      template <class T_Model, class T_Device, unsigned T_StateBytes, class T_Route, class T_Op, class T_Dt>
      inline bool CollectivePacketModel<T_Model, T_Device, T_StateBytes, T_Route, T_Op, T_Dt>::postMultiCollectivePacket (uint8_t              (&state)[T_StateBytes],
                                                                                                                          pami_event_function   fn,
                                                                                                                          void               * cookie,
                                                                                                                          T_Route              route,
                                                                                                                          void               * metadata,
                                                                                                                          size_t               metasize,
                                                                                                                          void               * payload,
                                                                                                                          size_t               length)
      {
        return static_cast<T_Model*>(this)->postMultiCollectivePacket_impl (state, fn, cookie,
                                                                            route,
                                                                            metadata, metasize,
                                                                            payload, length);
      }
      template <class T_Model, class T_Device, unsigned T_StateBytes, class T_Route, class T_Op, class T_Dt>
      inline bool CollectivePacketModel<T_Model, T_Device, T_StateBytes, T_Route, T_Op, T_Dt>::postMultiCollectivePacket (uint8_t              (&state)[T_StateBytes],
                                                                                                                          pami_event_function   fn,
                                                                                                                          void               * cookie,
                                                                                                                          T_Route              route,
                                                                                                                          size_t               root,
                                                                                                                          T_Op                 op,
                                                                                                                          T_Dt                 dt,
                                                                                                                          void               * metadata,
                                                                                                                          size_t               metasize,
                                                                                                                          void               * payload,
                                                                                                                          size_t               length)
      {
        return static_cast<T_Model*>(this)->postMultiCollectivePacket_impl (state, fn, cookie,
                                                                            route, root,
                                                                            op, dt,
                                                                            metadata, metasize,
                                                                            payload, length);
      }
    };
  };
};
#endif // __components_devices_CollectivePacketInterface_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
