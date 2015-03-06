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
/*
 * \file common/NativeInterface.h
 * \brief Simple all-sided and active-message NativeInterface on a p2p protocol.
 */

#ifndef __common_NativeInterface_h__
#define __common_NativeInterface_h__

#include "Global.h"
#include "algorithms/interfaces/NativeInterface.h"
#include "util/ccmi_debug.h"
#include "util/ccmi_util.h"

#include "components/devices/MulticastModel.h"
#include "components/devices/MultisyncModel.h"
#include "components/devices/MulticombineModel.h"
#include "p2p/protocols/SendPWQ.h"
#include "p2p/protocols/RecvPWQ.h"
#include "components/memory/MemoryAllocator.h"

#include "util/trace.h"

#undef DO_TRACE_ENTEREXIT 
#undef DO_TRACE_DEBUG     

#ifdef CCMI_TRACE_ALL
 #define DO_TRACE_ENTEREXIT 1
 #define DO_TRACE_DEBUG     1
#else
 #define DO_TRACE_ENTEREXIT 0
 #define DO_TRACE_DEBUG     0
#endif

namespace PAMI
{
  ///
  /// \brief Provide a Native Interface over a point-to-point protocol
  ///
  /// \details See classes below
  /// \see PAMI::NativeInterfaceBase
  /// \see PAMI::NativeInterfaceAllsided
  /// \see PAMI::NativeInterfaceActiveMessage
  /// \see PAMI::Protocol::Send::SendPWQ
  /// \see PAMI::CollRegistration::BGQ:CCMIRegistration
  ///
  /// \par example of usage
  /// \code
  /// // Define a point-to-point protocol (
  ///
  /// typedef Protocol::Send::...<> MyProtocol;
  ///
  /// // Define a SendPWQ over this protocol
  /// typedef PAMI::Protocol::Send::SendPWQ < MyProtocol > MySendPWQ;
  ///
  /// // Construct the Native Interface
  ///
  /// NativeInterfaceActiveMessage *ni = new NativeInterfaceActiveMessage(client, context, context_id, client_id);
  ///
  /// // Generate the protocol with the NI dispatch function and the NI pointer as a cookie
  ///
  /// pami_dispatch_multicast_function dispatch_fn;
  /// dispatch_fn = NativeInterfaceActiveMessage::dispatch_mcast;
  ///
  /// MySendPWQ *protocol = (MySendPWQ*) MySendPWQ::generate(dispatch, dispatch_fn,(void*) ni,  ...);
  ///
  /// // Set the p2p dispatch id and protocol back in the Native Interface
  ///
  /// ni->setMcastProtocol(dispatch, protocol);
  /// \endcode
  ///
  /// This sequence is necessary.  We can't pass the protocol on the NI ctor because it's a
  /// chicken-n-egg problem.  NativeInterface  ctor needs a protocol. Protocol ctor needs a
  /// dispatch function (NI::dispatch_mcast) and cookie (NI*).
  ///
  /// If we had a common ctor/generate and/or setDispatch on P2P protocols, we could do it differently.
  ///

  namespace NativeInterfaceCommon // Common constants
  {
#ifndef  PAMI_PLATFORM_A2A_PACING_WINDOW
    #define PAMI_NI_M2M_MAX_PENDING 1024  /// M2M pacing window default
#else
    #define PAMI_NI_M2M_MAX_PENDING PAMI_PLATFORM_A2A_PACING_WINDOW  /// Platform pacing window
#endif
    /// \brief Enum to enumerate which underlying interface is being used by the NI
    typedef enum
    {
      MULTICAST_ONLY,
      MANYTOMANY_ONLY,
      P2P_ONLY,
      ALL,
    } select_interface;

    /// \brief Construct a P2p Native Interface
    /// \details
    ///
    ///  A native interface is constructed.
    ///
    ///  Then a P2P protocol is constructed from the device and using the
    ///  same dispatch id and using the native interface's dispatch function
    ///  and the native interface as a cookie.
    ///
    ///  Finally, the P2P protocol is set into the native interface.
    ///
    template <class T_Allocator, class T_NativeInterface, class T_Protocol, class T_Device, select_interface T_Select>
    inline pami_result_t constructNativeInterface(T_Allocator        &allocator,
                                                  T_Device           &device,
                                                  T_NativeInterface *&ni,
                                                  pami_client_t       client,
                                                  pami_context_t      context,
                                                  size_t              context_id,
                                                  size_t              client_id,
                                                  int                *dispatch_id)
    {
      TRACE_FN_ENTER();
      DO_DEBUG((templateName<T_NativeInterface>()));
      DO_DEBUG((templateName<T_Protocol>()));
      DO_DEBUG((templateName<T_Device>()));

      pami_result_t result = PAMI_ERROR;

      COMPILE_TIME_ASSERT(sizeof(T_NativeInterface) <= T_Allocator::objsize);
      TRACE_FORMAT("Allocator:  sizeof(T_NativeInterface) %zu, T_Allocator::objsize %zu",sizeof(T_NativeInterface),T_Allocator::objsize);


      // Construct the protocol(s) using the NI dispatch function and cookie
      ni = (T_NativeInterface*) allocator.allocateObject ();
      new ((void*)ni) T_NativeInterface(client, context, context_id, client_id);

      pami_endpoint_t            origin   = PAMI_ENDPOINT_INIT(client_id, __global.mapping.task(), context_id);

      pami_dispatch_p2p_function fn;
      size_t                     dispatch;
      T_Protocol                *protocol;
      pami_configuration_t       config;
      config.name = PAMI_DISPATCH_SEND_IMMEDIATE_MAX;

      if(T_Select == ALL || T_Select == MULTICAST_ONLY)
      {
        // Construct the mcast protocol using the NI dispatch function and cookie
        fn = T_NativeInterface::dispatch_mcast;
        dispatch = (*dispatch_id)--;

        protocol = (T_Protocol*) T_Protocol::generate(dispatch,
						      fn,
						      (void*) ni,
						      device,
						      origin,
						      context,
						      (pami_dispatch_hint_t){0},
						      __global.heap_mm,
						      result);
        protocol->setPWQAllocator(ni->getSendPWQAllocator());

        ni->setMcastProtocol(dispatch, protocol);
        protocol->getAttributes(&config, 1);
      }

      if(T_Select == ALL || T_Select == MANYTOMANY_ONLY)
      {
        // Construct the m2m protocol using the NI dispatch function and cookie
        dispatch = (*dispatch_id)--;
        fn        = T_NativeInterface::dispatch_m2m;
        protocol  = (T_Protocol*) T_Protocol::generate(dispatch,
                                                       fn,
                                                       (void*) ni,
                                                       device,
                                                       origin,
                                                       context,
                                                       (pami_dispatch_hint_t){0},
                                                       __global.heap_mm,
                                                       result);
        protocol->setPWQAllocator(ni->getSendPWQAllocator());

        ni->setM2mProtocol(dispatch, protocol);
        protocol->getAttributes(&config, 1);
      }

      if(T_Select == ALL || T_Select == P2P_ONLY)
      {
        // Construct the p2p protocol using the NI dispatch function and cookie
        dispatch = (*dispatch_id)--;
        fn        = T_NativeInterface::dispatch_send;
        protocol  = (T_Protocol*) T_Protocol::generate(dispatch,
                                                       fn,
                                                       (void*) ni,
                                                       device,
                                                       origin,
                                                       context,
                                                       (pami_dispatch_hint_t){0},
                                                       __global.heap_mm,
                                                       result);
        protocol->setPWQAllocator(ni->getSendPWQAllocator());

        ni->setSendProtocol(dispatch, protocol);

        // Construct the p2p pwq protocol using the NI dispatch function and cookie
        dispatch = (*dispatch_id)--;
        fn        = T_NativeInterface::dispatch_send_pwq;
        protocol  = (T_Protocol*) T_Protocol::generate(dispatch,
                                                       fn,
                                                       (void*) ni,
                                                       device,
                                                       origin,
                                                       context,
                                                       (pami_dispatch_hint_t){0},
                                                       __global.heap_mm,
                                                       result);
        protocol->setPWQAllocator(ni->getSendPWQAllocator());

        ni->setSendPWQProtocol(dispatch, protocol);
        protocol->getAttributes(&config, 1);
      }


      // Workaround:  This gets rid of an unused warning with gcc
//      if (0)
//        getNextDispatch();
      Protocol::Send::SendPWQ<Protocol::Send::Send>::setImmSendSize(config.value.intval);
      // Return
      TRACE_FN_EXIT();
      return result;
    }

    /// \brief Construct a P2p Native Interface
    /// \details
    ///
    ///  A native interface is constructed.
    ///
    ///  Two P2P protocols are constructed from the device and using the
    ///  same dispatch id and using the native interface's dispatch function
    ///  and the native interface as a cookie.
    ///
    /// Then a composite protocol is constructed from the 2 protocols.
    ///
    ///  Finally, the composite protocol is set into the native interface.
    ///
    template <class T_Allocator, class T_NativeInterface, class T_Protocol1, class T_Device1, class T_Protocol2, class T_Device2, select_interface T_Select>
    inline pami_result_t constructNativeInterface(T_Allocator        &allocator,
                                                  T_Device1           &device1,
                                                  T_Device2           &device2,
                                                  T_NativeInterface *&ni,
                                                  pami_client_t       client,
                                                  pami_context_t      context,
                                                  size_t              context_id,
                                                  size_t              client_id,
                                                  int                *dispatch_id)
    {
      TRACE_FN_ENTER();
      DO_DEBUG((templateName<T_NativeInterface>()));
      DO_DEBUG((templateName<T_Protocol1>()));
      DO_DEBUG((templateName<T_Protocol2>()));
      DO_DEBUG((templateName<T_Device1>()));
      DO_DEBUG((templateName<T_Device2>()));

      pami_result_t result = PAMI_ERROR;

      COMPILE_TIME_ASSERT(sizeof(T_NativeInterface) <= T_Allocator::objsize);
      TRACE_FORMAT("Allocator:  sizeof(T_NativeInterface) %zu, T_Allocator::objsize %zu",sizeof(T_NativeInterface),T_Allocator::objsize);

      // Get storage for the NI and construct it.
      ni = (T_NativeInterface*) allocator.allocateObject ();
      new ((void*)ni) T_NativeInterface(client, context, context_id, client_id);
      pami_endpoint_t origin = PAMI_ENDPOINT_INIT(client_id, __global.mapping.task(), context_id);

      // Construct the protocols using the NI dispatch function and cookie
      pami_dispatch_p2p_function fn;
      size_t                     dispatch;
      T_Protocol1               *protocol1;
      T_Protocol2               *protocol2;
      Protocol::Send::SendPWQ<Protocol::Send::Send>* composite;
      pami_configuration_t       config;
      config.name = PAMI_DISPATCH_SEND_IMMEDIATE_MAX;

      if(T_Select == ALL || T_Select == MULTICAST_ONLY)
      {
        // Construct the mcast protocol using the NI dispatch function and cookie
        fn = T_NativeInterface::dispatch_mcast;
        dispatch = (*dispatch_id)--;
        protocol1 = (T_Protocol1*) T_Protocol1::generate(dispatch,
                                              fn,
                                              (void*) ni,
                                              device1,
                                              origin,
                                              context,
                                              (pami_dispatch_hint_t){0},
                                              __global.heap_mm,
                                              result);
        protocol1->setPWQAllocator(ni->getSendPWQAllocator());
        protocol2 = (T_Protocol2*) T_Protocol2::generate(dispatch,
                                              fn,
                                              (void*) ni,
                                              device2,
                                              origin,
                                              context,
                                              (pami_dispatch_hint_t){0},
                                              __global.heap_mm,
                                              result);
        protocol2->setPWQAllocator(ni->getSendPWQAllocator());
        // Construct the composite from the two protocols
        composite = (Protocol::Send::SendPWQ<Protocol::Send::Send>*)
          Protocol::Send::Factory::generate(protocol1,
                                            protocol2,
                                            __global.heap_mm,
                                            result);

        composite->setPWQAllocator(ni->getSendPWQAllocator());
        // Set the composite protocol into the NI
        ni->setMcastProtocol(dispatch, composite);
        composite->getAttributes(&config, 1);

      }

      if(T_Select == ALL || T_Select == MANYTOMANY_ONLY)
      {
        // Construct the m2m protocol using the NI dispatch function and cookie
        dispatch = (*dispatch_id)--;
        fn        = T_NativeInterface::dispatch_m2m;
        protocol1 = (T_Protocol1*) T_Protocol1::generate(dispatch,
							 fn,
							 (void*) ni,
							 device1,
							 origin,
							 context,
							 (pami_dispatch_hint_t){0},
							 __global.heap_mm,
							 result);
        protocol1->setPWQAllocator(ni->getSendPWQAllocator());
        protocol2 = (T_Protocol2*) T_Protocol2::generate(dispatch,
							 fn,
							 (void*) ni,
							 device2,
							 origin,
							 context,
							 (pami_dispatch_hint_t){0},
							 __global.heap_mm,
							 result);
        protocol2->setPWQAllocator(ni->getSendPWQAllocator());
        // Construct the composite from the two protocols
        composite = (Protocol::Send::SendPWQ<Protocol::Send::Send>*)
          Protocol::Send::Factory::generate(protocol1,
                                            protocol2,
                                            __global.heap_mm,
                                            result);

        composite->setPWQAllocator(ni->getSendPWQAllocator());
        // Set the composite protocol into the NI
        ni->setM2mProtocol(dispatch, composite);
        composite->getAttributes(&config, 1);

      }

      if(T_Select == ALL || T_Select == P2P_ONLY)
      {
        // Construct the p2p protocol using the NI dispatch function and cookie
        dispatch = (*dispatch_id)--;
        fn        = T_NativeInterface::dispatch_send;
        protocol1 = (T_Protocol1*) T_Protocol1::generate(dispatch,
                                                                                    fn,
                                                                                    (void*) ni,
                                                                                    device1,
                                                                                    origin,
                                                                                    context,
                                                                                    (pami_dispatch_hint_t){0},
                                                                                    __global.heap_mm,
                                                                                    result);
        protocol1->setPWQAllocator(ni->getSendPWQAllocator());
        protocol2 = (T_Protocol2*) T_Protocol2::generate(dispatch,
                                                                                    fn,
                                                                                    (void*) ni,
                                                                                    device2,
                                                                                    origin,
                                                                                    context,
                                                                                    (pami_dispatch_hint_t){0},
                                                                                    __global.heap_mm,
                                                                                    result);
        protocol2->setPWQAllocator(ni->getSendPWQAllocator());
        // Construct the composite from the two protocols
        composite = (Protocol::Send::SendPWQ<Protocol::Send::Send>*)
          Protocol::Send::Factory::generate(protocol1,
                                            protocol2,
                                            __global.heap_mm,
                                            result);

        composite->setPWQAllocator(ni->getSendPWQAllocator());
        // Set the composite protocol into the NI
        ni->setSendProtocol(dispatch, composite);

        // Construct the p2p pwq protocol using the NI dispatch function and cookie
        dispatch = (*dispatch_id)--;
        fn        = T_NativeInterface::dispatch_send_pwq;
        protocol1 = (T_Protocol1*) T_Protocol1::generate(dispatch,
                                                                                    fn,
                                                                                    (void*) ni,
                                                                                    device1,
                                                                                    origin,
                                                                                    context,
                                                                                    (pami_dispatch_hint_t){0},
                                                                                    __global.heap_mm,
                                                                                    result);
        protocol1->setPWQAllocator(ni->getSendPWQAllocator());
        protocol2 = (T_Protocol2*) T_Protocol2::generate(dispatch,
                                                                                    fn,
                                                                                    (void*) ni,
                                                                                    device2,
                                                                                    origin,
                                                                                    context,
                                                                                    (pami_dispatch_hint_t){0},
                                                                                    __global.heap_mm,
                                                                                    result);
        protocol2->setPWQAllocator(ni->getSendPWQAllocator());
        // Construct the composite from the two protocols
        composite = (Protocol::Send::SendPWQ<Protocol::Send::Send>*)
          Protocol::Send::Factory::generate(protocol1,
                                            protocol2,
                                            __global.heap_mm,
                                            result);

        composite->setPWQAllocator(ni->getSendPWQAllocator());
        // Set the composite protocol into the NI
        ni->setSendPWQProtocol(dispatch, composite);
        composite->getAttributes(&config, 1);

      }
      Protocol::Send::SendPWQ<Protocol::Send::Send>::setImmSendSize(config.value.intval);
      // Return
      TRACE_FN_EXIT();
      return result;
    }

  }


  //
  // \class NativeInterfaceBase
  // \brief The common base for both active message and all sided NI.
  // \details
  //   - model_available_buffers_only: does not fully
  //        support PipeWorkQueue's
  //
  //      - send side pwq is fully supported.
  //      - receive side pwq in not fully supported.  We must have enough buffer
  //        in the receive pwq when dispatched to give the p2p protocol the
  //        necessary receive buffer.  We do not allocate receive buffers.
  //
  // \todo Possible enhancement - fully support receive pwq with an allocator? or
  //  write a receivePWQ p2p protocol extension like sendPWQ.
  //

  template <class T_Protocol, int T_Max_Msgcount>
  class NativeInterfaceBase : public CCMI::Interfaces::NativeInterface
  {
    public:
      // Model-specific interfaces
      static const size_t multicast_sizeof_msg     = 1024;;///\todo T_Mcast::sizeof_msg - arbitrary - figure it out from data structures/protocol
      static const size_t multisync_sizeof_msg     = 1024;;///\todo T_Msync::sizeof_msg - arbitrary - figure it out from data structures/protocol
      static const size_t multicombine_sizeof_msg  = 1024;;///\todo T_Mcomb::sizeof_msg - arbitrary - figure it out from data structures/protocol
      static const size_t manytomany_sizeof_msg    = 1024;;///\todo T_M2many::sizeof_msg - arbitrary - figure it out from data structures/protocol

      static const size_t model_msgcount_max       = T_Max_Msgcount;
      static const size_t model_bytes_max          = (uint32_t) - 1; /// \todo arbitrary - figure it out from protocol max header
      static const size_t model_connection_id_max  = (uint32_t) - 1; /// \todo arbitrary - figure it out from protocol max header

      inline NativeInterfaceBase(size_t          context_id,
                                 pami_endpoint_t endpoint):
      CCMI::Interfaces::NativeInterface(context_id, endpoint)
      {
        TRACE_FN_ENTER();
        DO_DEBUG((templateName<T_Protocol>()));
        TRACE_FN_EXIT();
      };

      // State (request) implementation.
      class p2p_multicast_statedata_t : public Queue::Element
      {
          inline p2p_multicast_statedata_t() :
              Queue::Element ()
          {
          };
        public:
          // Metadata (ours + user's) passed in the header
          typedef struct __attribute__((__packed__)) _metadata
          {
            uint32_t              connection_id;  ///< Collective connection id \todo expand to 64 bit?
            uint32_t              root;           ///< Root of the collective \todo redundant on p2p
            uint32_t              sndlen;         ///< Number of bytes of application data \todo redundant on p2p
            // Structure used to pass user's msgdata (consider alignment).
            uint32_t              msgcount;       ///< User's msgcount
            pami_quad_t           msginfo[model_msgcount_max];  ///< User's msginfo
        } metadata_t __attribute__ ((__aligned__ (16)));

          unsigned               connection_id;
          size_t                 bytes;
          PAMI::PipeWorkQueue   *rcvpwq;
          unsigned               doneCountDown;
          pami_callback_t        cb_done;
          metadata_t             meta;
          typename T_Protocol::sendpwq_t        sendpwq;
      } ;
      class p2p_manytomany_send_statedata_t : public Queue::Element
      {
          inline p2p_manytomany_send_statedata_t() :
              Queue::Element ()
          {
          };
        public:
          // Metadata (ours + user's) passed in the header
          typedef struct __attribute__((__packed__)) _metadata
          {
            uint32_t              connection_id;  ///< Collective connection id \todo expand to 64 bit?
            uint32_t              msgcount;       ///< User's msgcount
            uint32_t              unused[2];      ///<  msginfo alignment
            pami_quad_t           msginfo[model_msgcount_max];  ///< User's msginfo
          } metadata_t __attribute__ ((__aligned__ (16)));

          unsigned               doneCountDown;   /// number of destinations to send total count down
          unsigned               pacingCountDown; /// number of destinations to send in one window count down
          unsigned               pacingWindow;    /// number of destinations in a window
          unsigned               postInProgress:1;
          unsigned               nextPostPending:1;
          size_t                 nextDestIndex;   /// next destination index
          pami_callback_t        cb_done;
          metadata_t             meta;
          size_t                 msgsize;
          pami_manytomanybuf_t   send;            ///< send data parameters 
          pami_send_t            parameters;
          void                 * m2m_protocol;

      } ;

      class p2p_manytomany_recv_statedata_t : public Queue::Element
      {
        inline p2p_manytomany_recv_statedata_t() :
          Queue::Element ()
          {
          };
        public:
          unsigned               connection_id;
          pami_manytomanybuf_t  *recv;
          unsigned               doneCountDown;
          pami_callback_t        cb_done;
      } ;

      /// Allocation object to store state and user's callback
      class allocObj
      {
        public:
          enum
          {
            MULTICAST, MULTICOMBINE, MULTISYNC, MANYTOMANY_SEND, MANYTOMANY_RECV
          }  _type;
          union
          {
            uint8_t             _mcast[multicast_sizeof_msg]; // p2p_multicast_statedata_t
            uint8_t             _msync[multisync_sizeof_msg];
            uint8_t             _mcomb[multicombine_sizeof_msg];
            uint8_t             _m2m[manytomany_sizeof_msg];
          } _state;
          NativeInterfaceBase  *_ni;
          pami_callback_t       _user_callback;
      };

  }; // NativeInterfaceBase

  //
  // \todo NativeInterfaceAllsided and NativeInterfaceActiveMessage duplicate
  // a lot of code.  Move it to NativeInterfaceBase or templatize it?
  //
  // \todo Short protocol?  Use immediate() instead of simple() based on length
  // and/or template parms.  SendPWQ already supports immediatePWQ().
  //
  //

  //
  // \class NativeInterfaceAllsided
  // \brief A pseudo all-sided native interface based on a p2p protocol
  // \details
  //   - 'all sided' multicast because it does not call a user receive callback,
  // however it does require the user to ensure the 'receive' multicasts are
  // called before the 'send'/root multicast so that data pwq's are available.
  //
  //

  template < class T_Protocol, int T_Max_Msgcount = 1 >
  class NativeInterfaceAllsided : public NativeInterfaceBase<T_Protocol, T_Max_Msgcount>
  {
    public:

      /// \brief ctor
      inline NativeInterfaceAllsided(pami_client_t client, pami_context_t context, size_t context_id, size_t client_id);

      ~NativeInterfaceAllsided();
      /// Virtual interfaces (from base \see CCMI::Interfaces::NativeInterface)
      virtual inline pami_result_t destroy      ( );
      virtual inline pami_result_t multicast    (pami_multicast_t    *, void *devinfo = NULL);
      virtual inline pami_result_t multisync    (pami_multisync_t    *, void *devinfo = NULL);
      virtual inline pami_result_t multicombine (pami_multicombine_t *, void *devinfo = NULL);
      virtual inline pami_result_t manytomany   (pami_manytomany_t   *, void *devinfo = NULL)
      {
        PAMI_abort();
        return PAMI_ERROR;
      }

      virtual inline pami_result_t send (pami_send_t * parameters);
      virtual inline pami_result_t sendPWQ(pami_context_t       context,
                                           pami_endpoint_t      dest,
                                           unsigned             connection_Id,
                                           size_t               header_length,
                                           void                *header,
                                           size_t               length,
                                           PAMI::PipeWorkQueue *pwq,
                                           pami_send_event_t   *events);

      /// \brief initial to set the mcast dispatch
      virtual inline pami_result_t setMulticastDispatch (pami_dispatch_multicast_function fn, void *cookie);
      virtual inline pami_result_t setManytomanyDispatch(pami_dispatch_manytomany_function fn, void *cookie)
      {
        PAMI_abort();
        return PAMI_ERROR;
      }
      virtual inline pami_result_t setSendDispatch(pami_dispatch_p2p_function fn,
                                                   void                      *cookie);
      virtual inline pami_result_t setSendPWQDispatch(pami_dispatch_pwq_function fn,
                                                      void                      *cookie);

      /// \brief Multicast model constants/attributes
      static const bool   model_active_message          = false;
      static const bool   model_available_buffers_only  = true;

    protected:

      /// \brief NativeInterfaceAllsided done function - free allocation and call client's done
      static void ni_client_done(pami_context_t  context,
                                 void          *rdata,
                                 pami_result_t   res);

    public:
      ///
      /// \brief Received a p2p dispatch from another root (static function).
      ///
      /// This function must be passed to the P2P protocol constructor
      ///
      /// \todo If we had a protocol::setDispatch after ctor, we could do this internally.
      ///
      static void dispatch_mcast(pami_context_t       context_hdl,
                                 void               * cookie,
                                 const void         * header,
                                 size_t               header_size,
                                 const void         * data,
                                 size_t               data_size,
                                 pami_endpoint_t      origin,
                                 pami_recv_t        * recv);
      static void dispatch_m2m(  pami_context_t       context_hdl,
                                 void               * cookie,
                                 const void         * header,
                                 size_t               header_size,
                                 const void         * data,
                                 size_t               data_size,
                                 pami_endpoint_t      origin,
                                 pami_recv_t        * recv)
      {
        PAMI_abort();
      }
      static void dispatch_send(pami_context_t       context_hdl,
                                void               * cookie,
                                const void         * header,
                                size_t               header_size,
                                const void         * data,
                                size_t               data_size,
                                pami_endpoint_t      origin,
                                pami_recv_t        * recv);
      static void dispatch_send_pwq(pami_context_t       context_hdl,
                                void               * cookie,
                                const void         * header,
                                size_t               header_size,
                                const void         * data,
                                size_t               data_size,
                                pami_endpoint_t      origin,
                                pami_recv_t        * recv);


      /// \brief set the protocol pointer.    We can't pass the protocol on the NI ctor because it's a
      /// chicken-n-egg problem.  NI ctor needs a protocol. Protocol ctor needs a dispatch fn (NI::dispatch_mcast) and cookie (NI*).
      ///
      /// I choose to ctor the NI, ctor the protocol, then manually set it with NI::setMcastProtocol().
      ///
      inline void setMcastProtocol(size_t dispatch, T_Protocol* protocol)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "<%p> %zu, %p", this, dispatch, protocol);
        _mcast_dispatch = dispatch;
        _mcast_protocol = protocol;
        TRACE_FN_EXIT();
      }

      inline void setM2mProtocol(size_t dispatch, T_Protocol* protocol)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "<%p> %zu, %p", this, dispatch, protocol);
        _m2m_dispatch = dispatch;
        _m2m_protocol = protocol;
        TRACE_FN_EXIT();
      }

      inline void setSendProtocol(size_t dispatch, T_Protocol* protocol)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "<%p> %zu, %p", this, dispatch, protocol);
        _send_dispatch = dispatch;
        _send_protocol = protocol;
        TRACE_FN_EXIT();
      }

      inline void setSendPWQProtocol(size_t dispatch, T_Protocol* protocol)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "<%p> %zu, %p", this, dispatch, protocol);
        _send_pwq_dispatch = dispatch;
        _send_pwq_protocol = protocol;
        TRACE_FN_EXIT();
      }

      inline void *getSendPWQAllocator()
      {
        return &_send_pwq_allocator;
      }

    private:
      ///
      /// \brief common internal impl of postMulticast over p2p
      ///
      pami_result_t postMulticast_impl(uint8_t (&state)[NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::multicast_sizeof_msg],
				       size_t            client,
				       size_t            context,
                                       pami_multicast_t *mcast,
                                       void             *devinfo = NULL);
      pami_result_t postManytomany_impl(uint8_t (&state)[NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::manytomany_sizeof_msg],
                                        pami_manytomany_t *m2m,
                                        void             *devinfo = NULL)
      {
        PAMI_abort();
        return PAMI_ERROR;
      }

      virtual void postWork (pami_context_t         context,
			     int                    color,
			     pami_work_t          * work, 
			     pami_work_function    fn,
			     void                 * clientdata) 
      {
	PAMI_Context_post(context, work, fn, clientdata);
      }


      ///
      /// \brief Received a p2p dispatch from another root (member function).
      ///
      void handle_mcast(pami_context_t       context_hdl,
                        const void         * header,
                        size_t               header_size,
                        const void         * data,
                        size_t               data_size,
                        pami_endpoint_t      origin,
                        pami_pwq_recv_t    * recv);

      void handle_m2m  (pami_context_t       context_hdl,
                        const void         * header,
                        size_t               header_size,
                        const void         * data,
                        size_t               data_size,
                        pami_endpoint_t      origin,
                        pami_recv_t        * recv)
      {
        PAMI_abort();
// void function cannot return a value
      }

      ///
      /// \brief Called as each p2p send/recv is done.
      ///
      static void sendMcastDone ( pami_context_t   context,
                                  void          *  cookie,
                                  pami_result_t    result );

      static void sendM2mDone ( pami_context_t   context,
                                void          *  cookie,
                                pami_result_t    result )
      {
        PAMI_abort();
// void function cannot return a value
      }

      static void recvM2mDone ( pami_context_t   context,
                                void          *  cookie,
                                pami_result_t    result )
      {
        PAMI_abort();
// void function cannot return a value
      }



    protected:

      PAMI::MemoryAllocator < sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj), 16 > _allocator; // Allocator
      Protocol::Send::SendPWQ<Protocol::Send::Send>::sendpwqAllocators_t                                       _send_pwq_allocator;

      T_Protocol                                   * _mcast_protocol;
      pami_dispatch_multicast_function               _mcast_dispatch_function;
      void                                         * _mcast_dispatch_arg;
      size_t                                         _mcast_dispatch;

      T_Protocol                                   * _m2m_protocol;
      pami_dispatch_manytomany_function              _m2m_dispatch_function;
      void                                         * _m2m_dispatch_arg;
      size_t                                         _m2m_dispatch;

      T_Protocol                                   * _send_protocol;
      pami_dispatch_p2p_function                     _send_dispatch_function;
      void                                         * _send_dispatch_arg;
      size_t                                         _send_dispatch;

      T_Protocol                                   * _send_pwq_protocol;
      pami_dispatch_pwq_function                     _send_pwq_dispatch_function;
      void                                         * _send_pwq_dispatch_arg;
      size_t                                         _send_pwq_dispatch;
      //A map of maps of dispatch IDs per rank. A map of
      //dispatch IDs hold MatchQueues of PWQMessages that
      //match based on connection IDs.
      std::map<pami_endpoint_t, std::map<size_t, MatchQueue<unsigned long long> *> * >       _pwq_ranks_map;
      PAMI::MemoryAllocator<sizeof(PWQMessage),16>                                           _pwq_msg_allocator;
      PAMI::MemoryAllocator<sizeof(std::map<size_t, MatchQueue<unsigned long long> *>), 16>  _pwq_dispatch_map_allocator;
      PAMI::MemoryAllocator<sizeof(MatchQueue<unsigned long long>), 16>                      _pwq_matchqueue_allocator;

      Queue                                          _mcastQ;
      Queue                                          _m2mSendQ;
      Queue                                          _m2mRecvQ;

      pami_client_t                                  _client;
      pami_context_t                                 _context;
      size_t                                         _contextid;
      size_t                                         _clientid;
  }; // class NativeInterfaceAllsided

  //
  // \class NativeInterfaceActiveMessage
  // \brief An active-message native interface based on a p2p protocol
  // \details
  //   - inherits from all-sided and adds dispatch/active message capability
  //   - active message model
  //
  //
  template < class T_Protocol, int T_Max_Msgcount = 1 >
  class NativeInterfaceActiveMessage : public NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>
  {
    public:

      /// \brief ctor
      inline NativeInterfaceActiveMessage(pami_client_t client, pami_context_t context, size_t context_id, size_t client_id);

      ~NativeInterfaceActiveMessage();
      /// Virtual interfaces (from base \see CCMI::Interfaces::NativeInterfaceActiveMessage)
      virtual inline pami_result_t destroy      ( );
      virtual inline pami_result_t multicast    (pami_multicast_t    *, void *devinfo = NULL);
      virtual inline pami_result_t multisync    (pami_multisync_t    *, void *devinfo = NULL);
      virtual inline pami_result_t multicombine (pami_multicombine_t *, void *devinfo = NULL);
      virtual inline pami_result_t manytomany   (pami_manytomany_t   *, void *devinfo = NULL);

      virtual inline pami_result_t send (pami_send_t * parameters);
      virtual inline pami_result_t sendPWQ(pami_context_t       context,
                                           pami_endpoint_t      dest,
                                           unsigned             connection_Id,
                                           size_t               header_length,
                                           void                *header,
                                           size_t               length,
                                           PAMI::PipeWorkQueue *pwq,
                                           pami_send_event_t   *events);


      /// \brief initialize to set the mcast dispatch
      virtual inline pami_result_t setMulticastDispatch (pami_dispatch_multicast_function fn,
                                                         void                       *cookie);
      virtual inline pami_result_t setManytomanyDispatch(pami_dispatch_manytomany_function fn,
                                                         void                        *cookie);
      virtual inline pami_result_t setSendDispatch(pami_dispatch_p2p_function fn,
                                                   void                 *cookie);
      virtual inline pami_result_t setSendPWQDispatch(pami_dispatch_pwq_function fn,
                                                      void                 *cookie);

      /// \brief Multicast model constants/attributes
      static const bool   model_active_message          = true;
      static const bool   model_available_buffers_only  = true;


    protected:
      unsigned                              _m2m_pacing_window;

      /// \brief NativeInterfaceActiveMessage done function - free allocation and call client's done
      static void ni_client_done(pami_context_t  context,
                                 void          *rdata,
                                 pami_result_t   res);

    public:
      ///
      /// \brief Received a p2p dispatch from another root (static function).
      ///
      /// This function must be passed to the P2P protocol constructor
      ///
      /// \todo If we had a protocol::setDispatch after ctor, we could do this internally.
      ///
      static void dispatch_mcast(pami_context_t     context_hdl,
                                 void               * cookie,
                                 const void         * header,
                                 size_t               header_size,
                                 const void         * data,
                                 size_t               data_size,
                                 pami_endpoint_t      origin,
                                 pami_recv_t        * recv);
      static void dispatch_m2m(  pami_context_t     context_hdl,
                                 void               * cookie,
                                 const void         * header,
                                 size_t               header_size,
                                 const void         * data,
                                 size_t               data_size,
                                 pami_endpoint_t      origin,
                                 pami_recv_t        * recv);

      static void dispatch_send(pami_context_t      context_hdl,
                                void               * cookie,
                                const void         * header,
                                size_t               header_size,
                                const void         * data,
                                size_t               data_size,
                                pami_endpoint_t      origin,
                                pami_recv_t        * recv);
      static void dispatch_send_pwq(pami_context_t      context_hdl,
                                void               * cookie,
                                const void         * header,
                                size_t               header_size,
                                const void         * data,
                                size_t               data_size,
                                pami_endpoint_t      origin,
                                pami_recv_t        * recv);

      /// \brief set the protocol pointer.    We can't pass the protocol on the NI ctor because it's a
      /// chicken-n-egg problem.  NI ctor needs a protocol. Protocol ctor needs a dispatch fn (NI::dispatch_mcast) and cookie (NI*).
      ///
      /// I choose to ctor the NI, ctor the protocol, then manually set it with NI::setMcastProtocol().
      ///
      inline void setMcastProtocol(size_t dispatch, T_Protocol* protocol)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "<%p> %p dispatch %zu ", this, protocol, dispatch);
        this->_mcast_dispatch = dispatch;
        this->_mcast_protocol = protocol;
        TRACE_FN_EXIT();
      }

      inline void setM2mProtocol(size_t dispatch, T_Protocol* protocol)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "<%p> %p dispatch %zu ", this, protocol, dispatch);
        this->_m2m_dispatch = dispatch;
        this->_m2m_protocol = protocol;
        TRACE_FN_EXIT();
      }

      inline void setSendProtocol(size_t dispatch, T_Protocol* protocol)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "<%p> %p dispatch %zu ", this, protocol, dispatch);
        this->_send_dispatch = dispatch;
        this->_send_protocol = protocol;
        TRACE_FN_EXIT();
      }

      inline void setSendPWQProtocol(size_t dispatch, T_Protocol* protocol)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "<%p> %p dispatch %zu ", this, protocol, dispatch);
        this->_send_pwq_dispatch = dispatch;
        this->_send_pwq_protocol = protocol;
        TRACE_FN_EXIT();
      }

    private:
      ///
      /// \brief common internal impl of postMulticast over p2p
      ///
      pami_result_t postMulticast_impl(uint8_t (&state)[NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::multicast_sizeof_msg],
                                       size_t            client,
                                       size_t            context,
                                       pami_multicast_t *mcast,
                                       void             *devinfo = NULL);
      pami_result_t postManytomany_impl(uint8_t (&state)[NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::manytomany_sizeof_msg],
                                        pami_manytomany_t *m2m,
                                        void             *devinfo = NULL);

      /// \brief When destinations exceed window size, this is the post 'next' function when a window is done.
      static pami_result_t postNextManytomany_impl(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t *);
      ///
      /// \brief Received a p2p dispatch from another root (member function).
      ///
      void handle_mcast(pami_context_t       context_hdl,
                        const void         * header,
                        size_t               header_size,
                        const void         * data,
                        size_t               data_size,
                        pami_endpoint_t      origin,
                        pami_pwq_recv_t    * recv);
      void handle_m2m  (pami_context_t       context_hdl,
                        const void         * header,
                        size_t               header_size,
                        const void         * data,
                        size_t               data_size,
                        pami_endpoint_t      origin,
                        pami_recv_t        * recv);

      ///
      /// \brief Called as each p2p send/recv is done.
      ///
      static void sendMcastDone ( pami_context_t   context,
                                  void          *  cookie,
                                  pami_result_t    result );
      static void sendM2mDone ( pami_context_t   context,
                                void          *  cookie,
                                pami_result_t    result );
      static void recvM2mDone ( pami_context_t   context,
                                void          *  cookie,
                                pami_result_t    result );


  }; // class NativeInterfaceActiveMessage



  //
  // Inline implementations
  //
  template <class T_Protocol, int T_Max_Msgcount>
  inline NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::NativeInterfaceAllsided(pami_client_t  client,
                                                                                      pami_context_t context,
                                                                                      size_t         context_id,
                                                                                      size_t         client_id):
      NativeInterfaceBase<T_Protocol, T_Max_Msgcount>(context_id,PAMI_ENDPOINT_INIT(client,__global.mapping.task(),context_id)),
      _allocator(),
      _mcast_protocol(NULL), /// must be set by setMcastProtocol() before using the NI.
      _send_protocol(NULL), /// must be set by setMcastProtocol() before using the NI.
      _send_pwq_protocol(NULL),/// must be set by setMcastProtocol() before using the NI.
      _client(client),
      _context(context),
      _contextid(context_id),
      _clientid(client_id)
  {
    TRACE_FN_ENTER();
    DO_DEBUG((templateName<T_Protocol>()));
    TRACE_FN_EXIT();
  };

  template <class T_Protocol, int T_Max_Msgcount>
  inline NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::~NativeInterfaceAllsided()
  {
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline void NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::ni_client_done(pami_context_t  context,
      void          *rdata,
      pami_result_t   res)
  {
    TRACE_FN_ENTER();
    typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj             *obj = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj*)rdata;
    NativeInterfaceAllsided *ni   = (NativeInterfaceAllsided *)obj->_ni;

    TRACE_FORMAT( "<%p> %p, %p, %d calling %p(%p)",
               ni, context, rdata, res,
               obj->_user_callback.function, obj->_user_callback.clientdata);

    if (obj->_type == NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj::MULTICAST)
      {
        // Punch the produce button on the rcvpwq
        typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t *state = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t*)obj->_state._mcast;
        TRACE_FORMAT( "<%p> <%p>->produce(%zu)",
                   ni, state->rcvpwq, state->bytes);

        if (state->rcvpwq) state->rcvpwq->produceBytes(state->bytes); /// \todo ? is this always the right byte count?

        if (state->sendpwq.pwq) state->sendpwq.pwq->consumeBytes(state->bytes); /// \todo ? is this always the right byte count?
      }

    if (obj->_type == NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj::MANYTOMANY_SEND){ ; } /// ?

    if (obj->_type == NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj::MANYTOMANY_SEND){ ; }/// ?

    if (obj->_user_callback.function)
      obj->_user_callback.function(context,
                                   obj->_user_callback.clientdata,
                                   res);

//#ifdef CCMI_DEBUG
//    typename NativeInterfaceBase<T_Protocol,T_Max_Msgcount>::p2p_multicast_statedata_t* receive_state = (typename NativeInterfaceBase<T_Protocol,T_Max_Msgcount>::p2p_multicast_statedata_t*)_mcastQ.peekHead();
//    while (receive_state && receive_state->connection_id != connection_id)
//      receive_state = (typename NativeInterfaceBase<T_Protocol,T_Max_Msgcount>::p2p_multicast_statedata_t*)_mcastQ.nextElem(receive_state);
//    PAMI_assert(receive_state!=obj->_state._mcast); // all-sided and sync'd by MU so this shouldn't still be queued
//#endif

    ni->_allocator.returnObject(obj);
    TRACE_FN_EXIT();
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::setMulticastDispatch (pami_dispatch_multicast_function fn, void *cookie)
  {

    TRACE_FN_ENTER();
    TRACE_FORMAT( "<%p> %p, %p",
               this, fn,  cookie);

    TRACE_FN_EXIT();
    if (fn == NULL) // we allow a no-op dispatch on allsided.
      return PAMI_SUCCESS;

    TRACE_FN_EXIT();
    PAMI_abortf("<%p>%p, %p", this, fn,  cookie);
    return PAMI_ERROR;
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::setSendDispatch (pami_dispatch_p2p_function fn, void *cookie)
  {
    TRACE_FN_ENTER();
    this->_send_dispatch_arg      = cookie;
    this->_send_dispatch_function = fn;
    TRACE_FORMAT( "<%p> dispatch fn %p, arg %p ", this, this->_send_dispatch_function, this->_send_dispatch_arg);
    TRACE_FN_EXIT();
    return PAMI_ERROR;
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::setSendPWQDispatch (pami_dispatch_pwq_function fn, void *cookie)
  {
    TRACE_FN_ENTER();
    this->_send_pwq_dispatch_arg      = cookie;
    this->_send_pwq_dispatch_function = fn;
    TRACE_FORMAT( "<%p> dispatch fn %p, arg %p ", this, this->_send_pwq_dispatch_function, this->_send_pwq_dispatch_arg);
    TRACE_FN_EXIT();
    return PAMI_ERROR;
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::destroy ( )
  {
    this->~NativeInterfaceAllsided();
    return PAMI_SUCCESS;
  }


  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::multicast (pami_multicast_t *mcast, void *devinfo)
  {
    TRACE_FN_ENTER();
    PAMI_assert(sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj) <= _allocator.objsize);
    typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *req          = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *)_allocator.allocateObject();
    req->_type              = NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj::MULTICAST;
    req->_ni               = this;
    req->_user_callback    = mcast->cb_done;
    TRACE_FORMAT( "<%p> %p/%p connection id %u, msgcount %u, bytes %zu", this, req, mcast, mcast->connection_id, mcast->msgcount, mcast->bytes);
    DO_DEBUG((templateName<T_Protocol>()));

    //  \todo:  this copy will cause a latency hit, maybe we need to change postMultisync
    //          interface so we don't need to copy
    pami_multicast_t  m     = *mcast;

    m.dispatch =  _mcast_dispatch; // \todo ? Not really used in C++ objects?
    //    m.client   =  _clientid;   // \todo ? Why doesn't caller set this?
    //    m.context  =  _contextid;// \todo ? Why doesn't caller set this?

    m.cb_done.function     =  ni_client_done;
    m.cb_done.clientdata   =  req;

    postMulticast_impl(req->_state._mcast, this->_clientid, this->_contextid, &m);
    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }


//template <class T_Protocol, int T_Max_Msgcount>
//inline pami_result_t NativeInterfaceAllsided<T_Protocol,T_Max_Msgcount>::manytomany (pami_manytomany_t *m2m, void *devinfo)
//{
//  typename NativeInterfaceBase<T_Protocol,T_Max_Msgcount>::allocObj *req          = (typename NativeInterfaceBase<T_Protocol,T_Max_Msgcount>::allocObj *)_allocator.allocateObject();
//  req->_type              = NativeInterfaceBase<T_Protocol,T_Max_Msgcount>::allocObj::MANYTOMANY_SEND;
//  req->_ni               = this;
//  req->_user_callback    = m2m->cb_done;
//  TRACE_FORMAT( "<%p> %p/%p connection id %u, msgcount %u", this, req, m2m, m2m->connection_id, m2m->metacount);
//  DO_DEBUG((templateName<T_Protocol>()));
//
//  //  \todo:  this copy will cause a latency hit, maybe we need to change postManytomany
//  //          interface so we don't need to copy
//  pami_manytomany_t  m     = *m2m;
//
//  m.client   =  _clientid;   // \todo ? Why doesn't caller set this?
//  m.context  =  _contextid;// \todo ? Why doesn't caller set this?
//
//  m.cb_done.function     =  ni_client_done;
//  m.cb_done.clientdata   =  req;
//
//  postManytomany_impl(req->_state._m2m, &m);
//  return PAMI_SUCCESS;
//}


  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::multisync(pami_multisync_t *msync, void *devinfo)
  {
    TRACE_FN_ENTER();
    PAMI_assert(sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj) <= _allocator.objsize);
    typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *req          = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *)_allocator.allocateObject();
    req->_type              = NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj::MULTISYNC;
    req->_ni               = this;
    req->_user_callback    = msync->cb_done;
    TRACE_FORMAT( "<%p> %p/%p connection id %u", this, msync, req, msync->connection_id);
    DO_DEBUG((templateName<T_Protocol>()));

    pami_multisync_t  m     = *msync;

    //m.client   =  _clientid;
    //m.context  =  _contextid;

    m.cb_done.function     =  ni_client_done;
    m.cb_done.clientdata   =  req;
    //_msync.postMultisync(req->_state._msync, &m);
    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }


  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::multicombine (pami_multicombine_t *mcomb, void *devinfo)
  {
    TRACE_FN_ENTER();
    PAMI_assert(sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj) <= _allocator.objsize);
    typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *req          = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *)_allocator.allocateObject();
    req->_type              = NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj::MULTICOMBINE;
    req->_ni               = this;
    req->_user_callback    = mcomb->cb_done;
    TRACE_FORMAT( "<%p> %p/%p connection id %u, count %zu, dt %#X, op %#X", this, mcomb, req, mcomb->connection_id, mcomb->count, mcomb->dtype, mcomb->optor);
    DO_DEBUG((templateName<T_Protocol>()));

    pami_multicombine_t  m     = *mcomb;
    //m.client   =  _clientid;
    //m.context  =  _contextid;

    m.cb_done.function     =  ni_client_done;
    m.cb_done.clientdata   =  req;

    //_mcomb.postMulticombine(req->_state._mcomb, this->_clientid, this->_contextid, &m);
    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::send (pami_send_t * parameters)
  {
    PAMI_abort();
    return PAMI_ERROR;
  }
  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::sendPWQ(
      pami_context_t       context,
      pami_endpoint_t      dest,
      unsigned             connection_Id,
      size_t               header_length,
      void                *header,
      size_t               length,
      PAMI::PipeWorkQueue *pwq,
      pami_send_event_t   *events
      )
  {
    PAMI_abort();
    return PAMI_ERROR;
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::postMulticast_impl(uint8_t (&state)[NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::multicast_sizeof_msg],
											       size_t            client,
											       size_t            context,
      pami_multicast_t *mcast,
      void             *devinfo)
  {
    TRACE_FN_ENTER();
    pami_result_t result;
    COMPILE_TIME_ASSERT((sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t) <= NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::multicast_sizeof_msg));

//      PAMI_assert(!model_all_sided || mcast->src != ... not sure what I was trying to assert but think about it...
    typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t *state_data = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t*) & state;

    // Get the source data buffer/length and validate (assert) inputs
    size_t length = mcast->bytes;
    PAMI::PipeWorkQueue *pwq = (PAMI::PipeWorkQueue *)mcast->src;

    TRACE_FORMAT( "<%p> dispatch %zu, connection_id %#X, msgcount %d/%p, bytes %zu/%p/%p",
               this, mcast->dispatch, mcast->connection_id,
               mcast->msgcount, mcast->msginfo,
               mcast->bytes, pwq, pwq ? pwq->bufferToConsume() : NULL);

    TRACE_FORMAT( "<%p> dispatch %zu, connection_id %#X, msgcount %d/%p, bytes %zu/%p/%zu",
               this, mcast->dispatch, mcast->connection_id,
               mcast->msgcount, mcast->msginfo,
               mcast->bytes, pwq, pwq ? pwq->bytesAvailableToConsume() : NULL);

    TRACE_FORMAT( "<%p> dispatch %zu, connection_id %#X, msgcount %d/%p, bytes %zu/%p/%zu",
               this, mcast->dispatch, mcast->connection_id,
               mcast->msgcount, mcast->msginfo,
               mcast->bytes, pwq, pwq ? pwq->bytesAvailableToProduce() : NULL);

    state_data->connection_id = mcast->connection_id;
    state_data->rcvpwq        = (PAMI::PipeWorkQueue*)mcast->dst;
    state_data->sendpwq.pwq   = (PAMI::PipeWorkQueue*)mcast->src;
    state_data->bytes         = length;
    // Save the user's done callback
    state_data->cb_done = mcast->cb_done;

    // Get the msginfo buffer/length and validate (assert) inputs
    void* msgdata = (void*)mcast->msginfo;

    PAMI_assert(T_Max_Msgcount >= mcast->msgcount);

    state_data->meta.connection_id = mcast->connection_id;
    state_data->meta.root = this->endpoint();
    state_data->meta.sndlen = length;
    state_data->meta.msgcount = mcast->msgcount;

    if (mcast->msgcount) memcpy(state_data->meta.msginfo, msgdata, mcast->msgcount * sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t().msginfo));

    TRACE_FORMAT( "<%p> state %p/%p", this, state, state_data);

    if (state_data->rcvpwq) _mcastQ.pushTail(state_data); // only use recvQ if this mcast expects to receives data.

    // Destinations may return now and wait for data to be dispatched
    /// \todo Handle metadata only?
    /// \todo handle both pwq's null? Barrier?
    if (!pwq) 
    {
      TRACE_FN_EXIT();
      return PAMI_SUCCESS;
    }

    void* payload = NULL;

    if (length)
      payload = (void*)pwq->bufferToConsume();

    // calc how big our msginfo needs to be
    size_t msgsize =  sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t().connection_id) + sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t().root) + sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t().sndlen) +
                      sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t().msgcount) + (mcast->msgcount * sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t().msginfo));

    // Send the multicast to each destination
    state_data->sendpwq.dst_participants = *((PAMI::Topology*)mcast->dst_participants);

    state_data->doneCountDown = state_data->sendpwq.dst_participants.size();
    state_data->sendpwq.send.simple.send.hints = (pami_send_hint_t)
    {
      0
    };
    state_data->sendpwq.send.simple.send.data.iov_base = payload;
    state_data->sendpwq.send.simple.send.data.iov_len = length;
    state_data->sendpwq.send.simple.send.header.iov_base = &state_data->meta;
    state_data->sendpwq.send.simple.send.header.iov_len = msgsize;
    state_data->sendpwq.send.simple.send.dispatch = mcast->dispatch;
    state_data->sendpwq.send.simple.events.cookie = state_data;
    state_data->sendpwq.send.simple.events.local_fn = sendMcastDone;
    state_data->sendpwq.send.simple.events.remote_fn = NULL;

    state_data->sendpwq.client        = _client;
    state_data->sendpwq.connection_id = state_data->meta.connection_id;
    PAMI_assert(state_data->sendpwq.connection_id != -1U);
    state_data->sendpwq.contextid     = _contextid;
    state_data->sendpwq.userEvents    = state_data->sendpwq.send.simple.events;
    state_data->sendpwq.tmpbuf        = NULL;
    state_data->sendpwq.allocatedHdr  = 0;
    state_data->sendpwq.totalSndln    = length;
    state_data->sendpwq.work_posted   = 0;
    state_data->sendpwq.work_counter  = 0;

    TRACE_FORMAT( "<%p> %p data %zu, header %zu", this, _mcast_protocol, state_data->sendpwq.send.simple.send.data.iov_len, state_data->sendpwq.send.simple.send.header.iov_len);

    result = _mcast_protocol->simplePWQ(&state_data->sendpwq, _context);

    TRACE_FORMAT( "<%p> dispatch %zu, connection_id %#X exit",
               this, mcast->dispatch, mcast->connection_id);

    TRACE_FN_EXIT();
    return PAMI_SUCCESS;

  }; // NativeInterfaceAllsided<T_Protocol,T_Max_Msgcount>::postMulticast_impl

  ///
  /// \brief Received a p2p dispatch from another root (static function).
  /// Call the member function on the protocol.
  ///
  template <class T_Protocol, int T_Max_Msgcount>
  inline void NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::dispatch_mcast(pami_context_t       context_hdl, /**< IN:  communication context handle */
      void               * cookie,       /**< IN:  dispatch cookie (pointer to protocol object)*/
      const void         * header,       /**< IN:  header address  */
      size_t               header_size,  /**< IN:  header size     */
      const void         * data,         /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
      size_t               data_size,    /**< IN:  number of byts of message data, valid regarldless of message type */
      pami_endpoint_t      origin,       /**< IN:  Endpoint that originated the transfer */
      pami_recv_t        * recv)         /**< OUT: receive message structure, only needed if addr is non-NULL */
  {
    TRACE_FN_ENTER();
    TRACE_FORMAT( "<%p> context %p, header/size %p/%zu, data/size %p/%zu, origin %u, recv %p", cookie, context_hdl, header, header_size, data, data_size, origin, recv);
    NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount> *p = (NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount> *)cookie;
    //SSS: Getting pwqHdr info and extracting usrHdr
    pami_pwq_recv_t       local_recv;
    unsigned              connectionId;
    unsigned              msgId;
    unsigned              seqNo;
    unsigned              contig;//If contig data, we avoid lengthy code path of non-contig

    char             * pHdr = (char*)header;
    connectionId   = *((unsigned *)pHdr);
    pHdr          += sizeof(unsigned);
    msgId          = *((unsigned *)pHdr);
    pHdr          += sizeof(unsigned);
    contig         = (*((unsigned *)pHdr)) & 0xF0000000;
    seqNo          = (*((unsigned *)pHdr)) & 0x0FFFFFFF;
    pHdr          += sizeof(unsigned);

    local_recv.connectionId = ((((unsigned long long) connectionId) << (sizeof(unsigned))) | (((unsigned long long) msgId) & 0x00000000FFFFFFFF));

    TRACE_FORMAT( "<%p> contig %#X, seqNo %#X, connectionId %u, msgId %u", cookie, contig, seqNo, connectionId, msgId);

    if(likely(contig))
    {
      p->handle_mcast(context_hdl,
                      (void *) pHdr,
                      header_size,
                      data,
                      data_size,
                      origin,
                      &local_recv);

      PipeWorkQueue  * rcvpwq = local_recv.rcvpwq;
      // SSS: Since we are in the contig case, the total I am expecting should be equal to what I am getting here.. assert
      PAMI_assert_debugf(local_recv.totalRcvln == data_size, "bytes %zu == %zu data_size", local_recv.totalRcvln, data_size);
      /******************************************************/
      /* SSS: Recving a contig msg into a non-contig buffer (another special handling :(( ) */
      TRACE_FORMAT( "<%p> rcvpwq->bytesAvailableToProduce() %zu data_size %zu", cookie, rcvpwq->bytesAvailableToProduce(), data_size);
      if(rcvpwq && rcvpwq->bytesAvailableToProduce() < data_size)
      {

        PWQMessage * msg = (PWQMessage*)p->_pwq_msg_allocator.allocateObject();
        new(msg) PWQMessage(rcvpwq, local_recv.totalRcvln, local_recv.cb_done,
                            NULL, &p->_pwq_msg_allocator, NULL,
                            NULL, p->_mcast_dispatch, origin, local_recv.connectionId);

        if(data)
        {
          char *tmpbuf = NULL;
          posix_memalign((void**)&tmpbuf, 128, data_size);
          memcpy(tmpbuf, data, data_size);
          msg->enqueuePacket(data_size, 0, seqNo, tmpbuf);
          PWQMessage::RecvPWQ(context_hdl, msg, PAMI_SUCCESS);
        }
        else if(recv)
        {
          char *tmpbuf = NULL;
          posix_memalign((void**)&tmpbuf, 128, data_size);
          msg->enqueuePacket(data_size, 0, seqNo, tmpbuf);
          recv->cookie        = msg;
          recv->local_fn      = PWQMessage::RecvPWQ;
          recv->addr          = (char*)tmpbuf;
          recv->type          = PAMI_TYPE_BYTE;
          recv->offset        = 0;
          recv->data_fn       = PAMI_DATA_COPY;
          recv->data_cookie   = (void*)NULL;
        }
        TRACE_FN_EXIT();
        return;
      }
      /******************************************************/
      /* SSS: Now a contig message into a contig buffer */
      PAMI_assert_debugf(local_recv.totalRcvln == data_size, "bytes %zu == %zu data_size", local_recv.totalRcvln, data_size);

      if(likely(data_size && data))
      {
        PAMI_assertf(rcvpwq->bytesAvailableToProduce() >= data_size, "dst %zu >= data_size %zu\n", rcvpwq->bytesAvailableToProduce(), data_size);
        memcpy(rcvpwq->bufferToProduce(), data, data_size);
        if (local_recv.cb_done.function)
          (local_recv.cb_done.function)(context_hdl, local_recv.cb_done.clientdata, PAMI_SUCCESS);
      }
      else if(recv)
      {
        PAMI_assertf(rcvpwq->bytesAvailableToProduce() >= data_size, "dst %zu >= data_size %zu\n", rcvpwq->bytesAvailableToProduce(), data_size);
        recv->cookie        = local_recv.cb_done.clientdata;
        recv->local_fn      = local_recv.cb_done.function;
        recv->addr          = (char*)rcvpwq->bufferToProduce();
        recv->type          = PAMI_TYPE_BYTE;
        recv->offset        = 0;
        recv->data_fn       = PAMI_DATA_COPY;
        recv->data_cookie   = (void*)NULL;
      }
      else //SSS: Handling the 0 byte message case
      {
        if (local_recv.cb_done.function)
          (local_recv.cb_done.function)(context_hdl, local_recv.cb_done.clientdata, PAMI_SUCCESS);
      }
    }
    else
    {
      MatchQueue<unsigned long long> * pwq_msg_queue;
      std::map<size_t, MatchQueue<unsigned long long> *>            * dispatch_map;
      std::map<size_t, MatchQueue<unsigned long long> *>::iterator    dispatch_map_it;
      std::map<pami_endpoint_t, std::map<size_t, MatchQueue<unsigned long long> *> *>::iterator ranks_map_it;
      ranks_map_it = p->_pwq_ranks_map.find(origin);
      if(ranks_map_it == p->_pwq_ranks_map.end())//We don't have a map for this rank
      {
        dispatch_map = (std::map<size_t, MatchQueue<unsigned long long> *> *) p->_pwq_dispatch_map_allocator.allocateObject();
        new (dispatch_map)std::map<size_t, MatchQueue<unsigned long long> *>();
        pwq_msg_queue = (MatchQueue<unsigned long long> *) p->_pwq_matchqueue_allocator.allocateObject();
        new (pwq_msg_queue) MatchQueue<unsigned long long>();
        (*dispatch_map)[p->_mcast_dispatch] = pwq_msg_queue;
        p->_pwq_ranks_map[origin] = dispatch_map;
      }
      else
      {
        dispatch_map = ranks_map_it->second;
        dispatch_map_it = dispatch_map->find(p->_mcast_dispatch);
        if(dispatch_map_it == dispatch_map->end())
        {
          pwq_msg_queue = (MatchQueue<unsigned long long> *) p->_pwq_matchqueue_allocator.allocateObject();
          new (pwq_msg_queue) MatchQueue<unsigned long long>();
          (*dispatch_map)[p->_mcast_dispatch] = pwq_msg_queue;
        }
        else
        {
          pwq_msg_queue = dispatch_map_it->second;
        }
      }
      PWQMessage * msg = (PWQMessage*)pwq_msg_queue->find(local_recv.connectionId);

      if(!msg)
      {
        p->handle_mcast(context_hdl,
                       (void *) pHdr,
                       header_size,
                       data,
                       data_size,
                       origin,
                       &local_recv);
        PipeWorkQueue  * rcvpwq = local_recv.rcvpwq;
        msg = (PWQMessage*)p->_pwq_msg_allocator.allocateObject();
        new(msg) PWQMessage(rcvpwq, local_recv.totalRcvln, local_recv.cb_done,
                            &p->_pwq_ranks_map, &p->_pwq_msg_allocator, &p->_pwq_dispatch_map_allocator,
                            &p->_pwq_matchqueue_allocator, p->_mcast_dispatch, origin, local_recv.connectionId);

        pwq_msg_queue->pushTail((MatchQueueElem<unsigned long long>*)msg);
      }


      if(data)
      {
        char *tmpbuf = NULL;
        posix_memalign((void**)&tmpbuf, 128, data_size);
        memcpy(tmpbuf, data, data_size);
        msg->enqueuePacket(data_size, 0, seqNo, tmpbuf);
        PWQMessage::RecvPWQ(context_hdl, msg, PAMI_SUCCESS);
      }
      else if(recv)
      {
        char *tmpbuf = NULL;
        posix_memalign((void**)&tmpbuf, 128, data_size);
        msg->enqueuePacket(data_size, 0, seqNo, tmpbuf);
        recv->cookie        = msg;
        recv->local_fn      = PWQMessage::RecvPWQ;
        recv->addr          = (char*)tmpbuf;
        recv->type          = PAMI_TYPE_BYTE;
        recv->offset        = 0;
        recv->data_fn       = PAMI_DATA_COPY;
        recv->data_cookie   = (void*)NULL;
      }
      else //SSS: Handling the 0 byte message case .. Should never happen in non-contig though
      {
        PWQMessage::RecvPWQ(context_hdl, msg, PAMI_SUCCESS);
      }
    }
    TRACE_FN_EXIT();
  }

//template <class T_Protocol, int T_Max_Msgcount>
//inline void NativeInterfaceAllsided<T_Protocol,T_Max_Msgcount>::dispatch_m2m(  pami_context_t       context_hdl,  /**< IN:  communication context handle */
//                                                                void               * cookie,       /**< IN:  dispatch cookie (pointer to protocol object)*/
//                                                                const void         * header,       /**< IN:  header address  */
//                                                                size_t               header_size,  /**< IN:  header size     */
//                                                                const void         * data,         /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
//                                                                size_t               data_size,    /**< IN:  number of byts of message data, valid regarldless of message type */
//                                                                pami_endpoint_t      origin,       /**< IN:  Endpoint that originated the transfer */
//                                                                pami_recv_t        * recv)         /**< OUT: receive message structure, only needed if addr is non-NULL */
//{
//  TRACE_FORMAT( "<%p> header size %zu, data size %zu", cookie, header_size, data_size);
//  NativeInterfaceAllsided<T_Protocol,T_Max_Msgcount> *p = (NativeInterfaceAllsided<T_Protocol,T_Max_Msgcount> *)cookie;
//  p->handle_m2m(context_hdl,
//                  header,
//                  header_size,
//                  data,
//                  data_size,
//                  origin,
//                  recv);
//}

  template <class T_Protocol, int T_Max_Msgcount>
  inline void NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::dispatch_send(pami_context_t       context_hdl, /**< IN:  communication context handle */
                                                                                 void               * cookie,       /**< IN:  dispatch cookie (pointer to protocol object)*/
                                                                                 const void         * header,       /**< IN:  header address  */
                                                                                 size_t               header_size,  /**< IN:  header size     */
                                                                                 const void         * data,         /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
                                                                                 size_t               data_size,    /**< IN:  number of byts of message data, valid regarldless of message type */
                                                                                 pami_endpoint_t      origin,       /**< IN:  Endpoint that originated the transfer */
                                                                                 pami_recv_t        * recv)         /**< OUT: receive message structure, only needed if addr is non-NULL */
  {
    PAMI_abort();
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline void NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::dispatch_send_pwq(pami_context_t       context_hdl, /**< IN:  communication context handle */
                                                                                 void               * cookie,       /**< IN:  dispatch cookie (pointer to protocol object)*/
                                                                                 const void         * header,       /**< IN:  header address  */
                                                                                 size_t               header_size,  /**< IN:  header size     */
                                                                                 const void         * data,         /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
                                                                                 size_t               data_size,    /**< IN:  number of byts of message data, valid regarldless of message type */
                                                                                 pami_endpoint_t      origin,       /**< IN:  Endpoint that originated the transfer */
                                                                                 pami_recv_t        * recv)         /**< OUT: receive message structure, only needed if addr is non-NULL */
  {
    PAMI_abort();
  }


  ///
  /// \brief Received a p2p dispatch from another root (member function).
  /// Call user's dispatch
  /// \see PAMI::Protocol::Send::Send::..?
  ///
  template <class T_Protocol, int T_Max_Msgcount>
  inline void NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::handle_mcast(pami_context_t       context_hdl, /**< IN:  communication context handle */
                                                                                const void         * header,       /**< IN:  header address  */
                                                                                size_t               header_size,  /**< IN:  header size     */
                                                                                const void         * data,         /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
                                                                                size_t               data_size,    /**< IN:  number of byts of message data, valid regarldless of message type */
                                                                                pami_endpoint_t      origin,       /**< IN:  Endpoint that originated the transfer */
                                                                                pami_pwq_recv_t    * recv)         /**< OUT: receive message structure, only needed if addr is non-NULL */
  {
    TRACE_FN_ENTER();
    unsigned connection_id = ((typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t*)header)->connection_id;
    size_t bytes           = ((typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t*)header)->sndlen;
    //size_t root            = ((NativeInterfaceBase<T_Protocol,T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t*)header)->root;

    TRACE_FORMAT( "<%p>  header size %zu, data size %zu/%zu, connection_id %u, root %u, origin %u, recv %p", this, header_size, data_size, bytes, connection_id, ((typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t*)header)->root, origin, recv);

    typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t* receive_state = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t*)_mcastQ.peekHead();

    // probably the head, but (unlikely) search if it isn't
    while (receive_state && receive_state->connection_id != connection_id)
      receive_state = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t*)_mcastQ.nextElem(receive_state);

    PAMI_assert(receive_state); // all-sided and sync'd by MU so this shouldn't be unexpected data

    recv->cb_done           = receive_state->cb_done;
    recv->totalRcvln        = receive_state->bytes;
    recv->rcvpwq            = receive_state->rcvpwq;

    TRACE_FORMAT( "<%p>  header size %zu, data size %zu/%zu, connection_id %u, root %u, recv %p, receive_state %p", this, header_size, data_size, bytes, connection_id, ((typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t*)header)->root, recv, receive_state);

    _mcastQ.deleteElem(receive_state);

    TRACE_FN_EXIT();
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline void NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>::sendMcastDone ( pami_context_t   context,
      void          *  cookie,
      pami_result_t    result )
  {
    TRACE_FN_ENTER();

    typedef typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t p2p_multicast_statedata_t;
    p2p_multicast_statedata_t *state_data = (p2p_multicast_statedata_t*)cookie;
    TRACE_FORMAT( "<%p> countDown %u", cookie, state_data->doneCountDown);

    if (--state_data->doneCountDown == 0)
      {
        pami_callback_t        cb_done;
        cb_done.function   = state_data->cb_done.function;
        cb_done.clientdata = state_data->cb_done.clientdata;

        state_data->~p2p_multicast_statedata_t();
        if (cb_done.function)
          (cb_done.function)(context,cb_done.clientdata, PAMI_SUCCESS);
      }
    TRACE_FN_EXIT();
  }

//  template <class T_Protocol, int T_Max_Msgcount>
//  inline void NativeInterfaceAllsided<T_Protocol,T_Max_Msgcount>::sendM2mDone ( pami_context_t   context,
//                                                              void          *  cookie,
//                                                              pami_result_t    result )
//  {
//
//    typename NativeInterfaceBase<T_Protocol,T_Max_Msgcount>::p2p_manytomany_send_statedata_t *state_data = (typename NativeInterfaceBase<T_Protocol,T_Max_Msgcount>::p2p_manytomany_send_statedata_t*)cookie;
//
//    TRACE_FORMAT( "<%p> countDown %u", cookie, state_data->doneCountDown);
//
//    if (--state_data->doneCountDown == 0)
//      {
//        // call original done
//        if (state_data->cb_done.function)
//          (state_data->cb_done.function)(context,
//                                         state_data->cb_done.clientdata, PAMI_SUCCESS);
//
//        return;
//      }
//  }
//
//template <class T_Protocol, int T_Max_Msgcount>
//inline void NativeInterfaceAllsided<T_Protocol,T_Max_Msgcount>::recvM2mDone ( pami_context_t   context,
//                                                            void          *  cookie,
//                                                            pami_result_t    result )
//{
//
//  typename NativeInterfaceBase<T_Protocol,T_Max_Msgcount>::p2p_manytomany_recv_statedata_t *state_data = (typename NativeInterfaceBase<T_Protocol,T_Max_Msgcount>::p2p_manytomany_recv_statedata_t*)cookie;
//
//  TRACE_FORMAT( "<%p> countDown %u", cookie, state_data->doneCountDown);
//
//  if (--state_data->doneCountDown == 0)
//    {
//      // call original done
//      if (state_data->cb_done.function)
//        (state_data->cb_done.function)(context,
//                                       state_data->cb_done.clientdata, PAMI_SUCCESS);
//
//      return;
//    }
//}

  template <class T_Protocol, int T_Max_Msgcount>
  inline NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::NativeInterfaceActiveMessage(pami_client_t   client,
                                                                                                pami_context_t  context,
                                                                                                size_t          context_id,
                                                                                                size_t          client_id):
    NativeInterfaceAllsided<T_Protocol, T_Max_Msgcount>(client,context,context_id,client_id),
    _m2m_pacing_window(PAMI_NI_M2M_MAX_PENDING)  /// \todo make this configurable?
  {
    TRACE_FN_ENTER();

    DO_DEBUG((templateName<T_Protocol>()));
    TRACE_FN_EXIT();
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::~NativeInterfaceActiveMessage()
  {
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline void NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::ni_client_done(pami_context_t  context,
      void          *rdata,
      pami_result_t   res)
  {
    TRACE_FN_ENTER();
    typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj             *obj = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj*)rdata;
    NativeInterfaceActiveMessage *ni   = (NativeInterfaceActiveMessage *)obj->_ni;

    TRACE_FORMAT( "<%p> %p, %p, %d calling %p(%p)",
               ni, context, rdata, res,
               obj->_user_callback.function, obj->_user_callback.clientdata);

    if (obj->_type == NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj::MULTICAST) // Punch the produce button on the pwq
      {
        typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t *state = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t*)obj->_state._mcast;
        TRACE_FORMAT( "<%p> pwq<%p>->produce(%zu)",
                   ni, state->rcvpwq, state->bytes);

        if (state->rcvpwq) state->rcvpwq->produceBytes(state->bytes); /// \todo ? is this always the right byte count?

        if (state->sendpwq.pwq) state->sendpwq.pwq->consumeBytes(state->bytes); /// \todo ? is this always the right byte count?
      }

    if (obj->_type == NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj::MANYTOMANY_SEND)
      {
      typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t *state = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t*)obj->_state._m2m;
      ni->_m2mSendQ.deleteElem(state);
      }

    if (obj->_type == NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj::MANYTOMANY_RECV)
      {
      typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_recv_statedata_t *state = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_recv_statedata_t*)obj->_state._m2m;
      ni->_m2mRecvQ.deleteElem(state);
      }

    if (obj->_user_callback.function)
      obj->_user_callback.function(context,
                                   obj->_user_callback.clientdata,
                                   res);

    ni->_allocator.returnObject(obj);
    TRACE_FN_EXIT();
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::setManytomanyDispatch (pami_dispatch_manytomany_function fn, void *cookie)
  {
    TRACE_FN_ENTER();

    TRACE_FORMAT( "<%p> %p, %p id=%zu",
               this, fn,  cookie,  this->_m2m_dispatch);
    DO_DEBUG((templateName<T_Protocol>()));

    this->_m2m_dispatch_arg = cookie;
    this->_m2m_dispatch_function = fn;

    /// \todo I would really like to set the p2p protocol dispatch now, but that isn't defined...
    /// so my caller must construct the p2p protocol with my dispatch function and this ptr.
    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::setMulticastDispatch (pami_dispatch_multicast_function fn, void *cookie)
  {
    TRACE_FN_ENTER();

    TRACE_FORMAT( "<%p> %p, %p id=%zu",
               this, fn,  cookie,  this->_mcast_dispatch);
    DO_DEBUG((templateName<T_Protocol>()));

    this->_mcast_dispatch_arg = cookie;
    this->_mcast_dispatch_function = fn;

    /// \todo I would really like to set the p2p protocol dispatch now, but that isn't defined...
    /// so my caller must construct the p2p protocol with my dispatch function and this ptr.
    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::setSendDispatch (pami_dispatch_p2p_function fn, void *cookie)
  {
    TRACE_FN_ENTER();
    this->_send_dispatch_arg      = cookie;
    this->_send_dispatch_function = fn;
    TRACE_FORMAT( "<%p> dispatch fn %p, arg %p ", this, this->_send_dispatch_function, this->_send_dispatch_arg);
    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::setSendPWQDispatch (pami_dispatch_pwq_function fn, void *cookie)
  {
    TRACE_FN_ENTER();
    this->_send_pwq_dispatch_arg      = cookie;
    this->_send_pwq_dispatch_function = fn;
    TRACE_FORMAT( "<%p> dispatch fn %p, arg %p ", this, this->_send_pwq_dispatch_function, this->_send_pwq_dispatch_arg);
    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }


  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::destroy ()
  {
    this->~NativeInterfaceActiveMessage();
	return PAMI_SUCCESS;
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::multicast (pami_multicast_t *mcast, void *devinfo)
  {
    TRACE_FN_ENTER();
    PAMI_assert(sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj) <= this->_allocator.objsize);
    typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *req          = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *)this->_allocator.allocateObject();
    req->_type              = NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj::MULTICAST;
    req->_ni               = this;
    req->_user_callback    = mcast->cb_done;
    TRACE_FORMAT( "<%p> %p/%p connection id %u, msgcount %u, bytes %zu", this, req, mcast, mcast->connection_id, mcast->msgcount, mcast->bytes);
    DO_DEBUG((templateName<T_Protocol>()));

    //  \todo:  this copy will cause a latency hit, maybe we need to change postMultisync
    //          interface so we don't need to copy
    pami_multicast_t  m     = *mcast;

    m.dispatch =  this->_mcast_dispatch; // \todo ? Not really used in C++ objects?
    //m.client   =  this->_clientid;   // \todo ? Why doesn't caller set this?
    //m.context  =  this->_contextid;// \todo ? Why doesn't caller set this?

    m.cb_done.function     =  ni_client_done;
    m.cb_done.clientdata   =  req;

    postMulticast_impl(req->_state._mcast, this->_clientid, this->_contextid, &m);
    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::manytomany (pami_manytomany_t *m2m, void *devinfo)
  {
    TRACE_FN_ENTER();
    PAMI_assert(sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj) <= this->_allocator.objsize);
    typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *req = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *)this->_allocator.allocateObject();
    typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t* state_data = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t*)req->_state._m2m;

    this->_m2mSendQ.pushTail(state_data); // queue this m2m state
    req->_type             = NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj::MANYTOMANY_SEND;
    req->_ni               = this;
    req->_user_callback    = m2m->cb_done;

    TRACE_FORMAT( "<%p> %p/%p connection id %u, msgcount %u", this, req, m2m, m2m->connection_id, m2m->msgcount);
    DO_DEBUG((templateName<T_Protocol>()));

    //  \todo:  this copy will cause a latency hit, maybe we need to change postManytomany
    //          interface so we don't need to copy
    pami_manytomany_t  m     = *m2m;

    m.client   =  this->_clientid;   // \todo ? Why doesn't caller set this?
    m.context  =  this->_contextid;// \todo ? Why doesn't caller set this?

    m.cb_done.function     =  ni_client_done;
    m.cb_done.clientdata   =  req;

    postManytomany_impl(req->_state._m2m, &m);
      
    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }


  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::multisync(pami_multisync_t *msync, void *devinfo)
  {
    TRACE_FN_ENTER();
    PAMI_assert(sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj) <= this->_allocator.objsize);
    typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *req          = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *)this->_allocator.allocateObject();
    req->_type              = NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj::MULTISYNC;
    req->_ni               = this;
    req->_user_callback    = msync->cb_done;
    TRACE_FORMAT( "<%p> %p/%p connection id %u", this, msync, req, msync->connection_id);
    DO_DEBUG((templateName<T_Protocol>()));

    pami_multisync_t  m     = *msync;

    //m.client   =  this->_clientid;
    //m.context  =  this->_contextid;

    m.cb_done.function     =  ni_client_done;
    m.cb_done.clientdata   =  req;
    //_msync.postMultisync(req->_state._msync, &m);
    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }


  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::multicombine (pami_multicombine_t *mcomb, void *devinfo)
  {
    TRACE_FN_ENTER();
    PAMI_assert(sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj) <= this->_allocator.objsize);
    typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *req          = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *)this->_allocator.allocateObject();
    req->_type              = NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj::MULTICOMBINE;
    req->_ni               = this;
    req->_user_callback    = mcomb->cb_done;
    TRACE_FORMAT( "<%p> %p/%p connection id %u, count %zu, dt %#X, op %#X", this, mcomb, req, mcomb->connection_id, mcomb->count, mcomb->dtype, mcomb->optor);
    DO_DEBUG((templateName<T_Protocol>()));

    pami_multicombine_t  m     = *mcomb;

    //m.client   =  this->_clientid;
    //m.context  =  this->_contextid;

    m.cb_done.function     =  ni_client_done;
    m.cb_done.clientdata   =  req;

    //_mcomb.postMulticombine(req->_state._mcomb, this->_clientid, this->_contextid, &m);
    TRACE_FN_EXIT();
    return PAMI_SUCCESS;

  }
  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::send (pami_send_t * parameters)
  {
    pami_send_t s   = *parameters;
    s.send.dispatch = this->_send_dispatch;
    return this->_send_protocol->simple(&s);
  }
  template <class T_Protocol, int T_Max_Msgcount>
    inline pami_result_t NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::sendPWQ(
      pami_context_t       context,
      pami_endpoint_t      dest,
      unsigned             connection_Id,
      size_t               header_length,
      void                *header,
      size_t               length,
      PAMI::PipeWorkQueue *pwq,
      pami_send_event_t   *events
      )
  {
    return this->_send_pwq_protocol->simplePWQ(context,dest,connection_Id,header_length,header,length,pwq,events,this->_send_pwq_dispatch);
  }


  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::postMulticast_impl(uint8_t (&state)[NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::multicast_sizeof_msg],
      size_t            client,  
      size_t            context,  
      pami_multicast_t *mcast,
      void             *devinfo)
  {
    TRACE_FN_ENTER();
    pami_result_t result;
    COMPILE_TIME_ASSERT((sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t) <= NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::multicast_sizeof_msg));

//      PAMI_assert(!model_all_sided || mcast->src != ... not sure what I was trying to assert but think about it...
    typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t *state_data = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t*) & state;

    // Get the source data buffer/length and validate (assert) inputs
    size_t length = mcast->bytes;
    PAMI::PipeWorkQueue *pwq = (PAMI::PipeWorkQueue *)mcast->src;

    TRACE_FORMAT( "<%p> dispatch %zu, connection_id %#X, msgcount %d/%p, bytes %zu/%p/%p",
               this, mcast->dispatch, mcast->connection_id,
               mcast->msgcount, mcast->msginfo,
               mcast->bytes, pwq, pwq ? pwq->bufferToConsume() : NULL);

    void* payload = NULL;

    if (length)
      payload = (void*)pwq->bufferToConsume();

    state_data->connection_id = mcast->connection_id;
    state_data->rcvpwq        = (PAMI::PipeWorkQueue*)mcast->dst;
    state_data->sendpwq.pwq   = (PAMI::PipeWorkQueue*)mcast->src;
    state_data->bytes         = length;
    // Save the user's done callback
    state_data->cb_done = mcast->cb_done;

//    state_data->rcvpwq = NULL;
//    state_data->sendpwq.pwq  = NULL;

    // Get the msginfo buffer/length and validate (assert) inputs
    void* msgdata = (void*)mcast->msginfo;

    PAMI_assert(T_Max_Msgcount >= mcast->msgcount);

    state_data->meta.connection_id = mcast->connection_id;
    state_data->meta.root = this->endpoint();
    state_data->meta.sndlen = length;
    state_data->meta.msgcount = mcast->msgcount;
    memcpy(state_data->meta.msginfo, msgdata, mcast->msgcount * sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t().msginfo));

    // calc how big our msginfo needs to beg
    size_t msgsize =  sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t().connection_id) + sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t().root) + sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t().sndlen) +
                      sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t().msgcount) + (mcast->msgcount * sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t().msginfo));

    // Send the multicast to each destination
    state_data->sendpwq.dst_participants = *((PAMI::Topology*)mcast->dst_participants);

    state_data->doneCountDown = state_data->sendpwq.dst_participants.size();
    state_data->sendpwq.send.simple.send.hints = (pami_send_hint_t)
    {
      0
    };
    state_data->sendpwq.send.simple.send.data.iov_base = payload;
    state_data->sendpwq.send.simple.send.data.iov_len = length;
    state_data->sendpwq.send.simple.send.header.iov_base = &state_data->meta;
    state_data->sendpwq.send.simple.send.header.iov_len = msgsize;
    state_data->sendpwq.send.simple.send.dispatch = mcast->dispatch;
    state_data->sendpwq.send.simple.events.cookie = state_data;
    state_data->sendpwq.send.simple.events.local_fn = sendMcastDone;
    state_data->sendpwq.send.simple.events.remote_fn = NULL;

    state_data->sendpwq.client        = this->_client;
    state_data->sendpwq.connection_id = state_data->meta.connection_id;
    PAMI_assert(state_data->sendpwq.connection_id != -1U);
    state_data->sendpwq.contextid     = this->_contextid;
    state_data->sendpwq.userEvents    = state_data->sendpwq.send.simple.events;
    state_data->sendpwq.tmpbuf        = NULL;
    state_data->sendpwq.allocatedHdr  = 0;
    state_data->sendpwq.totalSndln    = length;
    state_data->sendpwq.work_posted   = 0;
    state_data->sendpwq.work_counter  = 0;

    TRACE_FORMAT( "<%p> %p, data %zu, header %zu", this, this->_mcast_protocol, state_data->sendpwq.send.simple.send.data.iov_len, state_data->sendpwq.send.simple.send.header.iov_len);

    result = this->_mcast_protocol->simplePWQ(&state_data->sendpwq, this->_context);

    TRACE_FORMAT( "<%p> dispatch %zu, connection_id %#X exit",
               this, mcast->dispatch, mcast->connection_id);

    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }; // NativeInterfaceActiveMessage<T_Protocol,T_Max_Msgcount>::postMulticast_impl

  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::postManytomany_impl(uint8_t (&state)[NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::manytomany_sizeof_msg],
      pami_manytomany_t *m2m,
      void             *devinfo)
  {
    TRACE_FN_ENTER();
    COMPILE_TIME_ASSERT((sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t) <= NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::manytomany_sizeof_msg));

    typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t *state_data = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t*) & state;

    state_data->send = m2m->send;

    // Get the source data and participants
    PAMI::M2MPipeWorkQueueT<size_t, 1> *spwq = (PAMI::M2MPipeWorkQueueT<size_t, 1> *)state_data->send.buffer;
    PAMI::M2MPipeWorkQueueT<size_t, 0> *lpwq = (PAMI::M2MPipeWorkQueueT<size_t, 0> *)state_data->send.buffer;
    PAMI::M2MPipeWorkQueueT<int, 0>    *ipwq = (PAMI::M2MPipeWorkQueueT<int, 0> *)state_data->send.buffer;

    TRACE_FORMAT("<%p> dispatch %zu, connection_id %#X, msgcount %d/%p, pwq %p",
		 this, this->_m2m_dispatch, m2m->connection_id,
		 m2m->msgcount, m2m->msginfo,
		 spwq);

    // Get the msginfo buffer/length and validate (assert) inputs
    void* msgdata             = (void*)m2m->msginfo;

    PAMI_assert(T_Max_Msgcount >= m2m->msgcount);

    state_data->cb_done            = m2m->cb_done;
    state_data->meta.connection_id = m2m->connection_id;
    state_data->meta.msgcount      = m2m->msgcount;

    // calc how big our msginfo needs to beg
    state_data->msgsize =  sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t::metadata_t().connection_id) +
                      sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t::metadata_t().msgcount) +
                      sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t::metadata_t().unused) +
                      (m2m->msgcount * sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t::metadata_t().msginfo));

    if (m2m->msgcount)memcpy(state_data->meta.msginfo, msgdata, m2m->msgcount * sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t::metadata_t().msginfo));

    state_data->nextDestIndex = 0;
    state_data->nextPostPending = 0;
    state_data->pacingCountDown = state_data->pacingWindow = this->_m2m_pacing_window;
    state_data->doneCountDown = state_data->send.participants->size();
    
    size_t ndestinations = state_data->doneCountDown > state_data->pacingCountDown ? state_data->pacingCountDown:state_data->doneCountDown; 

    state_data->m2m_protocol = (void*) this->_m2m_protocol;

    state_data->parameters.send.dispatch = this->_m2m_dispatch;
    state_data->parameters.send.hints  = (pami_send_hint_t)
    {
      0
    };

    state_data->parameters.events.cookie = state_data;
    state_data->parameters.events.local_fn = sendM2mDone;
    state_data->parameters.events.remote_fn = NULL;

    state_data->parameters.send.header.iov_base = &state_data->meta;
    state_data->parameters.send.header.iov_len = state_data->msgsize;

    state_data->postInProgress = 1;

    TRACE_FORMAT( "<%p> next destination %zu, ndestinations %zu", state_data,state_data->nextDestIndex, ndestinations);
    // Send the manytomany to each destination
    for (size_t i = 0; i < ndestinations; ++i)
      {
        size_t index = state_data->send.participants->index2PermutedIndex(i);
        pami_endpoint_t endpoint = state_data->send.participants->index2Endpoint(index);

        if (state_data->send.type == M2M_SINGLE) {
          state_data->parameters.send.data.iov_base = spwq->bufferToConsume(index);
          state_data->parameters.send.data.iov_len  = spwq->bytesAvailableToConsume(index);
        }
        else if (state_data->send.type == M2M_VECTOR_INT) {
          state_data->parameters.send.data.iov_base = ipwq->bufferToConsume(index);
          state_data->parameters.send.data.iov_len  = ipwq->bytesAvailableToConsume(index);
        }
        else {
          state_data->parameters.send.data.iov_base = lpwq->bufferToConsume(index);
          state_data->parameters.send.data.iov_len  = lpwq->bytesAvailableToConsume(index);
        }

        pami_result_t result = PAMI_SUCCESS;

        state_data->parameters.send.dest = endpoint;

        TRACE_FORMAT( "<%p> simple(nd(%u(%u,%zu)) length %zd, payload %p", this, state_data->parameters.send.dest, endpoint, this->_contextid, state_data->parameters.send.data.iov_len, state_data->parameters.send.data.iov_base);
        result = this->_m2m_protocol->simple(&(state_data->parameters));
        TRACE_FORMAT( "<%p> simple result %u", this, result);
      }

    state_data->postInProgress = 0;

    if(state_data->nextPostPending)
    {
      TRACE_FORMAT( "<%p> nextPostPending next destination %zu, ndestinations %zu", state_data,state_data->nextDestIndex, ndestinations);
      postNextManytomany_impl(state_data);
    }

    TRACE_FORMAT( "<%p> dispatch %zu, connection_id %#X exit",
               this, this->_m2m_dispatch, m2m->connection_id);
    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }; // NativeInterfaceActiveMessage<T_Protocol,T_Max_Msgcount>::postManytomany_impl

  /// \brief When destinations exceed window size, this is the post 'next' function when a window is done.
  template <class T_Protocol, int T_Max_Msgcount>
  inline pami_result_t NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::postNextManytomany_impl(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t *state_data)
  {
    TRACE_FN_ENTER();
    // Get the source data and participants
    PAMI::M2MPipeWorkQueueT<size_t, 1> *spwq = (PAMI::M2MPipeWorkQueueT<size_t, 1> *)state_data->send.buffer;
    PAMI::M2MPipeWorkQueueT<size_t, 0> *lpwq = (PAMI::M2MPipeWorkQueueT<size_t, 0> *)state_data->send.buffer;
    PAMI::M2MPipeWorkQueueT<int, 0>    *ipwq = (PAMI::M2MPipeWorkQueueT<int, 0> *)state_data->send.buffer;

    T_Protocol* m2m_protocol = (T_Protocol*)(state_data->m2m_protocol);

    state_data->postInProgress = 1;
    do 
    {
      // calc how big our msginfo needs to beg
      size_t ndestinations = state_data->doneCountDown > state_data->pacingCountDown ? state_data->pacingCountDown:state_data->doneCountDown; 

      TRACE_FORMAT( "<%p> nextPostPending %u, next destination %zu, ndestinations %zu",state_data,state_data->nextPostPending, state_data->nextDestIndex, ndestinations);
      state_data->nextPostPending = 0;
      // Send the manytomany to each destination
      for (size_t i = 0; i < ndestinations; ++i)
      {
        size_t index = state_data->send.participants->index2PermutedIndex(i+state_data->nextDestIndex);
        pami_endpoint_t endpoint = state_data->send.participants->index2Endpoint(index);
        
        if (state_data->send.type == M2M_SINGLE) 
        {
          state_data->parameters.send.data.iov_base = spwq->bufferToConsume(index);
          state_data->parameters.send.data.iov_len  = spwq->bytesAvailableToConsume(index);
        }
        else if (state_data->send.type == M2M_VECTOR_INT) 
        {
          state_data->parameters.send.data.iov_base = ipwq->bufferToConsume(index);
          state_data->parameters.send.data.iov_len  = ipwq->bytesAvailableToConsume(index);
        }
        else 
        {
          state_data->parameters.send.data.iov_base = lpwq->bufferToConsume(index);
          state_data->parameters.send.data.iov_len  = lpwq->bytesAvailableToConsume(index);
        }

        pami_result_t result = PAMI_SUCCESS;

        state_data->parameters.send.dest = endpoint;

        TRACE_FORMAT( "<%p> simple(nd(%u) length %zd, payload %p", state_data, state_data->parameters.send.dest, state_data->parameters.send.data.iov_len, state_data->parameters.send.data.iov_base);
        result = m2m_protocol->simple(&(state_data->parameters));
        TRACE_FORMAT( "<%p> simple result %u", state_data, result);
      }

    } while(state_data->nextPostPending);

    state_data->postInProgress = 0;

    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }; // NativeInterfaceActiveMessage<T_Protocol,T_Max_Msgcount>::postNextManytomany_impl

  ///
  /// \brief Received a p2p dispatch from another root (static function).
  /// Call the member function on the protocol.
  ///
  template <class T_Protocol, int T_Max_Msgcount>
  inline void NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::dispatch_mcast(pami_context_t       context_hdl, /**< IN:  communication context handle */
      void               * cookie,       /**< IN:  dispatch cookie (pointer to protocol object)*/
      const void         * header,       /**< IN:  header address  */
      size_t               header_size,  /**< IN:  header size     */
      const void         * data,         /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
      size_t               data_size,    /**< IN:  number of byts of message data, valid regarldless of message type */
      pami_endpoint_t      origin,       /**< IN:  Endpoint that originated the transfer */
      pami_recv_t        * recv)         /**< OUT: receive message structure, only needed if addr is non-NULL */
  {
    TRACE_FN_ENTER();
    TRACE_FORMAT( "<%p> context %p, header/size %p/%zu, data/size %p/%zu, origin %u, recv %p", cookie, context_hdl, header, header_size, data, data_size, origin, recv);
    NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount> *p = (NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount> *)cookie;
    //SSS: Getting pwqHdr info and extracting usrHdr
    pami_pwq_recv_t       local_recv;
    unsigned              connectionId;
    unsigned              msgId;
    unsigned              seqNo;
    unsigned              contig;//If contig data, we avoid lengthy code path of non-contig

    char             * pHdr = (char*)header;
    connectionId   = *((unsigned *)pHdr);
    pHdr          += sizeof(unsigned);
    msgId          = *((unsigned *)pHdr);
    pHdr          += sizeof(unsigned);
    contig         = (*((unsigned *)pHdr)) & 0xF0000000;
    seqNo          = (*((unsigned *)pHdr)) & 0x0FFFFFFF;
    pHdr          += sizeof(unsigned);

    local_recv.connectionId = ((((unsigned long long) connectionId) << (sizeof(unsigned))) | (((unsigned long long) msgId) & 0x00000000FFFFFFFF));

    if(likely(contig))
    {
      p->handle_mcast(context_hdl,
                      (void *) pHdr,
                      header_size,
                      data,
                      data_size,
                      origin,
                      &local_recv);

      PipeWorkQueue  * rcvpwq = local_recv.rcvpwq;

      // SSS: Since we are in the contig case, the total I am expecting should be equal to what I am getting here.. assert
      PAMI_assert_debugf(local_recv.totalRcvln == data_size, "bytes %zu == %zu data_size", local_recv.totalRcvln, data_size);
      /******************************************************/
      /* SSS: Recving a contig msg into a non-contig buffer (another special handling :(( ) */
      if(rcvpwq && rcvpwq->bytesAvailableToProduce() < data_size) // NJ: rcvpwq can be NULL for barrier */
      {

        PWQMessage * msg = (PWQMessage*)p->_pwq_msg_allocator.allocateObject();
        new(msg) PWQMessage(rcvpwq, local_recv.totalRcvln, local_recv.cb_done,
                            NULL, &p->_pwq_msg_allocator, NULL,
                            NULL, p->_mcast_dispatch, origin, local_recv.connectionId);

        if(data)
        {
          char *tmpbuf = NULL;
          posix_memalign((void**)&tmpbuf, 128, data_size);
          memcpy(tmpbuf, data, data_size);
          msg->enqueuePacket(data_size, 0, seqNo, tmpbuf);
          PWQMessage::RecvPWQ(context_hdl, msg, PAMI_SUCCESS);
        }
        else if(recv)
        {
          char *tmpbuf = NULL;
          posix_memalign((void**)&tmpbuf, 128, data_size);
          msg->enqueuePacket(data_size, 0, seqNo, tmpbuf);
          recv->cookie        = msg;
          recv->local_fn      = PWQMessage::RecvPWQ;
          recv->addr          = (char*)tmpbuf;
          recv->type          = PAMI_TYPE_BYTE;
          recv->offset        = 0;
          recv->data_fn       = PAMI_DATA_COPY;
          recv->data_cookie   = (void*)NULL;
        }
        TRACE_FN_EXIT();
        return;
      }
      /******************************************************/
      /* SSS: Now a contig message into a contig buffer */
      if(likely(data_size && data))
      {
        PAMI_assertf(rcvpwq->bytesAvailableToProduce() >= data_size, "dst %zu >= data_size %zu\n", rcvpwq->bytesAvailableToProduce(), data_size);
        memcpy(rcvpwq->bufferToProduce(), data, data_size);
        rcvpwq->produceBytes(data_size);
        if (local_recv.cb_done.function)
          (local_recv.cb_done.function)(context_hdl, local_recv.cb_done.clientdata, PAMI_SUCCESS);
      }
      else if(recv)
      {
        // We have to intercept the callback to punch buttons on the rcv pwq.
        typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *req          = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *)p->_allocator.allocateObject();

        req->_type              = NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj::MULTICAST;
        req->_ni                = p;
        req->_user_callback     = local_recv.cb_done;

        typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t *state = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t*)req->_state._mcast;
        state->bytes       = local_recv.totalRcvln;
        state->rcvpwq      = rcvpwq;
        state->sendpwq.pwq = NULL;

        PAMI_assertf(rcvpwq->bytesAvailableToProduce() >= data_size, "dst %zu >= data_size %zu\n", rcvpwq->bytesAvailableToProduce(), data_size);

        recv->cookie   = req;
        recv->local_fn = ni_client_done;
        recv->addr     = rcvpwq->bufferToProduce();
        recv->type     = PAMI_TYPE_BYTE;
        recv->offset   = 0;
        recv->data_fn  = PAMI_DATA_COPY;
        recv->data_cookie = (void*)NULL;
      }
      else //SSS: Handling the 0 byte message case
      {
        if (local_recv.cb_done.function)
          (local_recv.cb_done.function)(context_hdl, local_recv.cb_done.clientdata, PAMI_SUCCESS);
      }
    }
    /* SSS: The complicated case.. a non-contig message into a non-contig buffer */
    else
    {
      MatchQueue<unsigned long long> * pwq_msg_queue;
      std::map<size_t, MatchQueue<unsigned long long> *>            * dispatch_map;
      std::map<size_t, MatchQueue<unsigned long long> *>::iterator    dispatch_map_it;
      std::map<pami_endpoint_t, std::map<size_t, MatchQueue<unsigned long long> *> *>::iterator ranks_map_it;
      ranks_map_it = p->_pwq_ranks_map.find(origin);
      if(ranks_map_it == p->_pwq_ranks_map.end())//We don't have a map for this rank
      {
        dispatch_map = (std::map<size_t, MatchQueue<unsigned long long> *> *) p->_pwq_dispatch_map_allocator.allocateObject();
        new (dispatch_map)std::map<size_t, MatchQueue<unsigned long long> *>();
        pwq_msg_queue = (MatchQueue<unsigned long long> *) p->_pwq_matchqueue_allocator.allocateObject();
        new (pwq_msg_queue) MatchQueue<unsigned long long>();
        (*dispatch_map)[p->_mcast_dispatch] = pwq_msg_queue;
        p->_pwq_ranks_map[origin] = dispatch_map;
      }
      else
      {
        dispatch_map = ranks_map_it->second;
        dispatch_map_it = dispatch_map->find(p->_mcast_dispatch);
        if(dispatch_map_it == dispatch_map->end())
        {
          pwq_msg_queue = (MatchQueue<unsigned long long> *) p->_pwq_matchqueue_allocator.allocateObject();
          new (pwq_msg_queue) MatchQueue<unsigned long long>();
          (*dispatch_map)[p->_mcast_dispatch] = pwq_msg_queue;
        }
        else
        {
          pwq_msg_queue = dispatch_map_it->second;
        }
      }
      PWQMessage * msg = (PWQMessage*)pwq_msg_queue->find(local_recv.connectionId);

      if(!msg)
      {
        p->handle_mcast(context_hdl,
                       (void *) pHdr,
                       header_size,
                       data,
                       data_size,
                       origin,
                       &local_recv);
        PipeWorkQueue  * rcvpwq = local_recv.rcvpwq;
        msg = (PWQMessage*)p->_pwq_msg_allocator.allocateObject();
        new(msg) PWQMessage(rcvpwq, local_recv.totalRcvln, local_recv.cb_done,
                            &p->_pwq_ranks_map, &p->_pwq_msg_allocator, &p->_pwq_dispatch_map_allocator,
                            &p->_pwq_matchqueue_allocator, p->_mcast_dispatch, origin, local_recv.connectionId);

        pwq_msg_queue->pushTail((MatchQueueElem<unsigned long long>*)msg);
      }


      if(data)
      {
        char *tmpbuf = NULL;
        posix_memalign((void**)&tmpbuf, 128, data_size);
        memcpy(tmpbuf, data, data_size);
        msg->enqueuePacket(data_size, 0, seqNo, tmpbuf);
        PWQMessage::RecvPWQ(context_hdl, msg, PAMI_SUCCESS);
      }
      else if(recv)
      {
        char *tmpbuf = NULL;
        posix_memalign((void**)&tmpbuf, 128, data_size);
        msg->enqueuePacket(data_size, 0, seqNo, tmpbuf);
        recv->cookie        = msg;
        recv->local_fn      = PWQMessage::RecvPWQ;
        recv->addr          = (char*)tmpbuf;
        recv->type          = PAMI_TYPE_BYTE;
        recv->offset        = 0;
        recv->data_fn       = PAMI_DATA_COPY;
        recv->data_cookie   = (void*)NULL;
      }
      else //SSS: Handling the 0 byte message case .. Should never happen in non-contig though
      {
        PWQMessage::RecvPWQ(context_hdl, msg, PAMI_SUCCESS);
      }
    }
    TRACE_FN_EXIT();
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline void NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::dispatch_m2m(pami_context_t       context_hdl, /**< IN:  communication context handle */
      void               * cookie,       /**< IN:  dispatch cookie (pointer to protocol object)*/
      const void         * header,       /**< IN:  header address  */
      size_t               header_size,  /**< IN:  header size     */
      const void         * data,         /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
      size_t               data_size,    /**< IN:  number of byts of message data, valid regarldless of message type */
      pami_endpoint_t      origin,       /**< IN:  Endpoint that originated the transfer */
      pami_recv_t        * recv)         /**< OUT: receive message structure, only needed if addr is non-NULL */
  {
    TRACE_FN_ENTER();
    TRACE_FORMAT( "<%p> context %p, header/size %p/%zu, data/size %p/%zu, origin %u, recv %p", cookie, context_hdl, header, header_size, data, data_size, origin, recv);
    NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount> *p = (NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount> *)cookie;
    p->handle_m2m (  context_hdl,
		     header,
		     header_size,
		     data,
		     data_size,
		     origin,
		     recv);
    TRACE_FN_EXIT();
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline void NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::dispatch_send(pami_context_t       context_hdl, /**< IN:  communication context handle */
      void               * cookie,       /**< IN:  dispatch cookie (pointer to protocol object)*/
      const void         * header,       /**< IN:  header address  */
      size_t               header_size,  /**< IN:  header size     */
      const void         * data,         /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
      size_t               data_size,    /**< IN:  number of byts of message data, valid regarldless of message type */
      pami_endpoint_t      origin,       /**< IN:  Endpoint that originated the transfer */
      pami_recv_t        * recv)         /**< OUT: receive message structure, only needed if addr is non-NULL */
  {
    TRACE_FN_ENTER();
    TRACE_FORMAT( "<%p> context %p, header/size %p/%zu, data/size %p/%zu, origin %u, recv %p", cookie, context_hdl, header, header_size, data, data_size, origin, recv);
    NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount> *p = (NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount> *)cookie;
    p->_send_dispatch_function(context_hdl,
                               p->_send_dispatch_arg,
                               header,
                               header_size,
                               data,
                               data_size,
                               origin,
                               recv);
    TRACE_FN_EXIT();
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline void NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::dispatch_send_pwq(pami_context_t       context_hdl, /**< IN:  communication context handle */
      void               * cookie,       /**< IN:  dispatch cookie (pointer to protocol object)*/
      const void         * header,       /**< IN:  header address  */
      size_t               header_size,  /**< IN:  header size     */
      const void         * data,         /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
      size_t               data_size,    /**< IN:  number of byts of message data, valid regarldless of message type */
      pami_endpoint_t      origin,       /**< IN:  Endpoint that originated the transfer */
      pami_recv_t        * recv)         /**< OUT: receive message structure, only needed if addr is non-NULL */
  {
    TRACE_FN_ENTER();
    TRACE_FORMAT( "<%p> context %p, header/size %p/%zu, data/size %p/%zu, origin %u, recv %p", cookie, context_hdl, header, header_size, data, data_size, origin, recv);
    NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount> *p = (NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount> *)cookie;

    pami_pwq_recv_t       local_recv;
    unsigned              connectionId;
    unsigned              msgId;
    unsigned              seqNo;
    unsigned              contig;//If contig data, we avoid length code path of non-contig


    char                * pHdr = (char*)header;
    connectionId = *((unsigned *)pHdr);
    pHdr += sizeof(unsigned);
    msgId        = *((unsigned *)pHdr);
    pHdr += sizeof(unsigned);
    contig       = (*((unsigned *)pHdr)) & 0xF0000000;
    seqNo        = (*((unsigned *)pHdr)) & 0x0FFFFFFF;
    pHdr += sizeof(unsigned);

    local_recv.connectionId = ((((unsigned long long) connectionId) << (sizeof(unsigned))) | (((unsigned long long) msgId) & 0x00000000FFFFFFFF));

    if(contig)//Take the shorter code path
    {
       p->_send_pwq_dispatch_function(context_hdl,
                             p->_send_pwq_dispatch_arg,
                             (void*)pHdr,
                             header_size - (sizeof(unsigned)*3),
                             data,
                             data_size,
                             origin,
                             &local_recv);
      PipeWorkQueue  * rcvpwq = local_recv.rcvpwq;
      /******************************************************/
      /* Recving a contig msg into a non-contig buffer (another special handling :(( ) */
      if(rcvpwq && rcvpwq->bytesAvailableToProduce() < data_size) 
      {

        PWQMessage * msg = (PWQMessage*)p->_pwq_msg_allocator.allocateObject();
        new(msg) PWQMessage(rcvpwq, local_recv.totalRcvln, local_recv.cb_done,
                            NULL, &p->_pwq_msg_allocator, NULL,
                            NULL, p->_mcast_dispatch, origin, local_recv.connectionId);

        if(data)
        {
          char *tmpbuf = NULL;
          posix_memalign((void**)&tmpbuf, 128, data_size);
          memcpy(tmpbuf, data, data_size);
          msg->enqueuePacket(data_size, 0, seqNo, tmpbuf);
          PWQMessage::RecvPWQ(context_hdl, msg, PAMI_SUCCESS);
        }
        else if(recv)
        {
          char *tmpbuf = NULL;
          posix_memalign((void**)&tmpbuf, 128, data_size);
          msg->enqueuePacket(data_size, 0, seqNo, tmpbuf);
          recv->cookie        = msg;
          recv->local_fn      = PWQMessage::RecvPWQ;
          recv->addr          = (char*)tmpbuf;
          recv->type          = PAMI_TYPE_BYTE;
          recv->offset        = 0;
          recv->data_fn       = PAMI_DATA_COPY;
          recv->data_cookie   = (void*)NULL;
        }
        TRACE_FN_EXIT();
        return;
      }
      /******************************************************/
      /* Now a contig message into a contig buffer */

      if(data_size && data)
      {
        PAMI_assertf(rcvpwq->bytesAvailableToProduce() >= data_size, "dst %zu >= data_size %zu\n", rcvpwq->bytesAvailableToProduce(), data_size);
        memcpy(rcvpwq->bufferToProduce(), data, data_size);
        if (local_recv.cb_done.function)
          (local_recv.cb_done.function)(context_hdl, local_recv.cb_done.clientdata, PAMI_SUCCESS);
      }
      else if(recv)
      {
        PAMI_assertf(rcvpwq->bytesAvailableToProduce() >= data_size, "dst %zu >= data_size %zu\n", rcvpwq->bytesAvailableToProduce(), data_size);
        recv->cookie        = local_recv.cb_done.clientdata;
        recv->local_fn      = local_recv.cb_done.function;
        recv->addr          = (char*)rcvpwq->bufferToProduce();
        recv->type          = PAMI_TYPE_BYTE;
        recv->offset        = 0;
        recv->data_fn       = PAMI_DATA_COPY;
        recv->data_cookie   = (void*)NULL;
      }
      else //SSS: Handling the 0 byte message case
      {
        if (local_recv.cb_done.function)
          (local_recv.cb_done.function)(context_hdl, local_recv.cb_done.clientdata, PAMI_SUCCESS);
      }
    }
    else
    {
      MatchQueue<unsigned long long> * pwq_msg_queue;
      std::map<size_t, MatchQueue<unsigned long long> *>            * dispatch_map;
      std::map<size_t, MatchQueue<unsigned long long> *>::iterator    dispatch_map_it;
      std::map<pami_endpoint_t, std::map<size_t, MatchQueue<unsigned long long> *> *>::iterator ranks_map_it;
      ranks_map_it = p->_pwq_ranks_map.find(origin);

      if(ranks_map_it == p->_pwq_ranks_map.end())//We don't have a map for this rank
      {
        dispatch_map = (std::map<size_t, MatchQueue<unsigned long long> *> *) p->_pwq_dispatch_map_allocator.allocateObject();
        new (dispatch_map)std::map<size_t, MatchQueue<unsigned long long> *>();
        pwq_msg_queue = (MatchQueue<unsigned long long> *) p->_pwq_matchqueue_allocator.allocateObject();
        new (pwq_msg_queue) MatchQueue<unsigned long long>();
        (*dispatch_map)[p->_send_pwq_dispatch] = pwq_msg_queue;
        p->_pwq_ranks_map[origin] = dispatch_map;
      }
      else
      {
        dispatch_map = ranks_map_it->second;
        dispatch_map_it = dispatch_map->find(p->_send_pwq_dispatch);
        if(dispatch_map_it == dispatch_map->end())
        {
          pwq_msg_queue = (MatchQueue<unsigned long long> *) p->_pwq_matchqueue_allocator.allocateObject();
          new (pwq_msg_queue) MatchQueue<unsigned long long>();
          (*dispatch_map)[p->_send_pwq_dispatch] = pwq_msg_queue;
        }
        else
        {
          pwq_msg_queue = dispatch_map_it->second;
        }
      }
      PWQMessage * msg = (PWQMessage*)pwq_msg_queue->find(local_recv.connectionId);

      if(!msg)
      {
        p->_send_pwq_dispatch_function(context_hdl,
                                   p->_send_pwq_dispatch_arg,
                                   (void*)pHdr,
                                   header_size - (sizeof(unsigned)*3),
                                   data,
                                   data_size,
                                   origin,
                                   &local_recv);
        PipeWorkQueue  * rcvpwq = local_recv.rcvpwq;
        msg = (PWQMessage*)p->_pwq_msg_allocator.allocateObject();
        new(msg) PWQMessage(rcvpwq, local_recv.totalRcvln, local_recv.cb_done,
                            &p->_pwq_ranks_map, &p->_pwq_msg_allocator, &p->_pwq_dispatch_map_allocator,
                            &p->_pwq_matchqueue_allocator, p->_send_pwq_dispatch, origin, local_recv.connectionId);

        pwq_msg_queue->pushTail((MatchQueueElem<unsigned long long>*)msg);
      }


      if(data)
      {
        char *tmpbuf = NULL;
        posix_memalign((void**)&tmpbuf, 128, data_size);
        memcpy(tmpbuf, data, data_size);
        msg->enqueuePacket(data_size, 0, seqNo, tmpbuf);
        PWQMessage::RecvPWQ(context_hdl, msg, PAMI_SUCCESS);
      }
      else if(recv)
      {
        char *tmpbuf = NULL;
        posix_memalign((void**)&tmpbuf, 128, data_size);
        msg->enqueuePacket(data_size, 0, seqNo, tmpbuf);
        recv->cookie        = msg;
        recv->local_fn      = PWQMessage::RecvPWQ;
        recv->addr          = (char*)tmpbuf;
        recv->type          = PAMI_TYPE_BYTE;
        recv->offset        = 0;
        recv->data_fn       = PAMI_DATA_COPY;
        recv->data_cookie   = (void*)NULL;
      }
      else //SSS: Handling the 0 byte message case .. Should never happen in non-contig though
      {
        PWQMessage::RecvPWQ(context_hdl, msg, PAMI_SUCCESS);
      }
    }
    TRACE_FN_EXIT();
  }

  ///
  /// \brief Received a p2p dispatch from another root (member function).
  /// Call user's dispatch
  /// \see PAMI::Protocol::Send::Send::..?
  ///
  template <class T_Protocol, int T_Max_Msgcount>
  inline void NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::handle_mcast(pami_context_t       context_hdl, /**< IN:  communication context handle */
      const void         * header,       /**< IN:  header address  */
      size_t               header_size,  /**< IN:  header size     */
      const void         * data,         /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
      size_t               data_size,    /**< IN:  number of byts of message data, valid regarldless of message type */
      pami_endpoint_t      origin,       /**< IN:  Endpoint that originated the transfer */
      pami_pwq_recv_t    * recv)         /**< OUT: receive message structure, only needed if addr is non-NULL */
  {
    TRACE_FN_ENTER();

    // Call user's dispatch to get receive pwq and cb_done.
    unsigned connection_id = ((typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t*)header)->connection_id;
    size_t bytes           = ((typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t*)header)->sndlen;
    size_t root            = ((typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t*)header)->root;
    PAMI::PipeWorkQueue   *rcvpwq;
    pami_callback_t       cb_done = {NULL, NULL};

    TRACE_FORMAT( "<%p> header size %zu, data size %zu/%zu, connection_id %u, root %zu, recv %p", this, header_size, data_size, bytes, connection_id, root, recv);

    // tolerate a null dispatch if there's no data (barrier?)
    PAMI_assertf(((this->_mcast_dispatch_function == NULL) && (data_size == 0)) || (this->_mcast_dispatch_function != NULL), "fn %p, size %zu", this->_mcast_dispatch_function, data_size);

    if (this->_mcast_dispatch_function != NULL)
      this->_mcast_dispatch_function(context_hdl,
        ((typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t*)header)->msginfo, ((typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t::metadata_t*)header)->msgcount,
                                     connection_id, root, bytes, this->_mcast_dispatch_arg, &bytes,
                                     (pami_pipeworkqueue_t**)&rcvpwq, &cb_done);

    TRACE_FORMAT( "<%p> requested bytes %zu", this, bytes);

    recv->cb_done           = cb_done;
    recv->totalRcvln        = bytes;
    recv->rcvpwq            = rcvpwq;

    TRACE_FN_EXIT();
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline void NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::handle_m2m(pami_context_t       context_hdl, /**< IN:  communication context handle */
      const void         * header,       /**< IN:  header address  */
      size_t               header_size,  /**< IN:  header size     */
      const void         * data,         /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
      size_t               data_size,    /**< IN:  number of byts of message data, valid regarldless of message type */
      pami_endpoint_t      origin,       /**< IN:  Endpoint that originated the transfer */
      pami_recv_t        * recv)         /**< OUT: receive message structure, only needed if addr is non-NULL */
  {
    TRACE_FN_ENTER();

    typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t::metadata_t* metadata = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t::metadata_t*)header;
    unsigned             connection_id = metadata->connection_id;

    TRACE_FORMAT( "<%p> header size %zu, data size %zu, connection_id %u, origin %u, recv %p", this, header_size, data_size, connection_id, origin, recv);

    typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_recv_statedata_t* state = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_recv_statedata_t*)this->_m2mRecvQ.peekHead();

    // Find this connection id state if it's in progress
    while (state && state->connection_id != connection_id)
      state = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_recv_statedata_t*)this->_m2mRecvQ.nextElem(state);


    // Not found? Dispatch to user and create a state
    if (!state)
      {

        PAMI_assert(this->_m2m_dispatch_function != NULL);

        PAMI_assert(sizeof(typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj) <= this->_allocator.objsize);
        typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *req = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj *)this->_allocator.allocateObject();
        state = (typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_recv_statedata_t*)req->_state._m2m;


        req->_type             = NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::allocObj::MANYTOMANY_RECV;
        req->_ni               = this;

        // Call user's dispatch on the first recv for this connection id to get receive parms and cb_done.
        this->_m2m_dispatch_function(context_hdl,
                                     this->_m2m_dispatch_arg,
                                     connection_id,
                                     metadata->msginfo, metadata->msgcount,
                                     &state->recv,
                                     &req->_user_callback);

        state->cb_done.function   = ni_client_done;
        state->cb_done.clientdata = req;

        state->connection_id = connection_id;

        state->doneCountDown = state->recv->participants->size();

        this->_m2mRecvQ.pushTail(state); // queue this m2m state
        TRACE_FORMAT( "<%p> connection_id %u, doneCountDown %u", this, state->connection_id, state->doneCountDown);
      }
    TRACE_FORMAT( "<%p> state %p, state->recv %p", this, state, state->recv);
    TRACE_FORMAT( "<%p> state->recv->type %u, state->recv->buffer %p, state->recv->participants %p", this, state->recv->type, state->recv->buffer, state->recv->participants);

    PAMI::M2MPipeWorkQueueT<size_t, 1> *spwq = (PAMI::M2MPipeWorkQueueT<size_t, 1> *)state->recv->buffer;
    PAMI::M2MPipeWorkQueueT<size_t, 0> *lpwq = (PAMI::M2MPipeWorkQueueT<size_t, 0> *)state->recv->buffer;
    PAMI::M2MPipeWorkQueueT<int, 0>    *ipwq = (PAMI::M2MPipeWorkQueueT<int, 0> *)   state->recv->buffer;

    PAMI::Topology           *topology = state->recv->participants;
    size_t                    originIndex  = topology->endpoint2Index(origin);

    size_t                    bytesToProduce;
    char                     *buffer; 

    if (state->recv->type == M2M_VECTOR_LONG) {
      buffer          = lpwq->bufferToProduce(originIndex);                
      bytesToProduce  = lpwq->bytesAvailableToProduce(originIndex);
    }
    else if (state->recv->type == M2M_SINGLE) {
      buffer          = spwq->bufferToProduce(originIndex);
      bytesToProduce  = spwq->bytesAvailableToProduce(originIndex);
    }
    else if (state->recv->type == M2M_VECTOR_INT) 
    {
      buffer          = ipwq->bufferToProduce(originIndex);
      bytesToProduce  = ipwq->bytesAvailableToProduce(originIndex);
    }
    else // avoid uninitialized error
    {
      PAMI_abort();
      buffer          = NULL;
      bytesToProduce  = 0;
    }
    TRACE_FORMAT( "<%p> bytesToProduce %zu, buffer %p", this,bytesToProduce, buffer);

    // Assert they gave us enough buffer for the recv
    PAMI_assertf(data_size == bytesToProduce, "data_size %zu == %zu bytesToProduce(%zu)\n", data_size, bytesToProduce, originIndex);

    // No data or immediate data? We're done.
    if ((data_size == 0) || (recv == NULL) || (data != NULL))
      {
        TRACE_FORMAT( "<%p> immediate", this);

        if (data && data_size)
          {
            /// \todo An assertion probably isn't the best choice...
          TRACE_FORMAT( "<%p>NativeInterfaceActiveMessage<%d>::handle_m2m()  pwq<%p>", this, __LINE__, spwq);
            memcpy(buffer, data, data_size);
            //pwq->produceBytes(originTask,data_size); \todo don't care about pipelining on M2M pwq's
          }

        // call done for this receive
        recvM2mDone(this->_context,
                    state,
                    PAMI_SUCCESS);

        // #warning \todo if it's 0 byte, no recv structure should be delivered 
        if (recv != NULL)
        {  
          memset(recv, 0, sizeof(*recv));
          recv->type = PAMI_TYPE_BYTE;
          recv->data_fn = PAMI_DATA_COPY;
        }
        TRACE_FN_EXIT();
        return;
      }

    TRACE_FORMAT( "<%p> data_size %zu, bytesToProduce(%zu) %zu", this, data_size, originIndex, bytesToProduce);

    recv->cookie   = state;
    recv->local_fn = recvM2mDone;
    recv->addr     = buffer;
    recv->type     = PAMI_TYPE_BYTE;
    recv->offset   = 0;
    recv->data_fn  = PAMI_DATA_COPY;
    recv->data_cookie = (void*)NULL;

    TRACE_FN_EXIT();
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline void NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::sendMcastDone ( pami_context_t   context,
      void          *  cookie,
      pami_result_t    result )
  {
    TRACE_FN_ENTER();

    typedef typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_multicast_statedata_t p2p_multicast_statedata_t;
    p2p_multicast_statedata_t *state_data = (p2p_multicast_statedata_t*)cookie;
    TRACE_FORMAT( "<%p> countDown %u", cookie, state_data->doneCountDown);

    if (--state_data->doneCountDown == 0)
      {
        pami_callback_t        cb_done;
        cb_done.function   = state_data->cb_done.function;
        cb_done.clientdata = state_data->cb_done.clientdata;

        state_data->~p2p_multicast_statedata_t();
        if (cb_done.function)
          (cb_done.function)(context,cb_done.clientdata, PAMI_SUCCESS);

      }
    TRACE_FN_EXIT();
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline void NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::sendM2mDone ( pami_context_t   context,
      void          *  cookie,
      pami_result_t    result )
  {
    TRACE_FN_ENTER();


    typedef typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_send_statedata_t p2p_manytomany_send_statedata_t;
    p2p_manytomany_send_statedata_t *state_data = (p2p_manytomany_send_statedata_t*)cookie;
    TRACE_FORMAT( "<%p> doneCountDown %u, pacingCountDown %u", cookie, state_data->doneCountDown, state_data->pacingCountDown);

    if (--state_data->doneCountDown == 0)
      {
        pami_callback_t        cb_done;
        cb_done.function   = state_data->cb_done.function;
        cb_done.clientdata = state_data->cb_done.clientdata;

        state_data->~p2p_manytomany_send_statedata_t();
        if (cb_done.function)
          (cb_done.function)(context,cb_done.clientdata, PAMI_SUCCESS);
      }
    else if(--state_data->pacingCountDown == 0)
    {
      state_data->pacingCountDown = state_data->pacingWindow;
      state_data->nextDestIndex  += state_data->pacingWindow;
      // Can't next too deep, so don't nest at all.  We're in post? Just mark next post is pending.
      if(state_data->postInProgress == 0) postNextManytomany_impl(state_data);
      else state_data->nextPostPending = 1;
    }
    TRACE_FN_EXIT();
  }

  template <class T_Protocol, int T_Max_Msgcount>
  inline void NativeInterfaceActiveMessage<T_Protocol, T_Max_Msgcount>::recvM2mDone ( pami_context_t   context,
      void          *  cookie,
      pami_result_t    result )
  {
    TRACE_FN_ENTER();

    typedef typename NativeInterfaceBase<T_Protocol, T_Max_Msgcount>::p2p_manytomany_recv_statedata_t p2p_manytomany_recv_statedata_t;
    p2p_manytomany_recv_statedata_t *state_data = (p2p_manytomany_recv_statedata_t*)cookie;
    TRACE_FORMAT( "<%p> countDown %u", cookie, state_data->doneCountDown);

    if (--state_data->doneCountDown == 0)
      {
      // call original done
        pami_callback_t        cb_done;
        cb_done.function   = state_data->cb_done.function;
        cb_done.clientdata = state_data->cb_done.clientdata;

        state_data->~p2p_manytomany_recv_statedata_t();
        if (cb_done.function)
          (cb_done.function)(context,cb_done.clientdata, PAMI_SUCCESS);
      }
    TRACE_FN_EXIT();
  }

};

#undef DO_TRACE_ENTEREXIT 
#undef DO_TRACE_DEBUG     

#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
