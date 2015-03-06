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
/// \file p2p/protocols/send/adaptive/Adaptive.h
/// \brief Adaptive send protocol for reliable devices.
///
/// The Adaptive class defined in this file uses C++ templates and
/// the device "message" interface - which also uses C++ templates.
///
/// C++ templates require all source code to be #include'd from a header file.
///
#ifndef __p2p_protocols_send_adaptive_Adaptive_h__
#define __p2p_protocols_send_adaptive_Adaptive_h__

#include "p2p/protocols/Send.h"
#include "p2p/protocols/send/adaptive/AdaptiveImmediate.h"
#include "p2p/protocols/send/adaptive/AdaptiveSimple.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf x
#endif

namespace PAMI
{
  namespace Protocol
  {
    namespace Send
    {

      ///
      /// \brief Adaptive simple send protocol class for reliable network devices.
      ///
      /// \tparam T_Model   Template packet model class
      /// \tparam T_Device  Template packet device class
      ///
      /// \see PAMI::Device::Interface::PacketModel
      /// \see PAMI::Device::Interface::PacketDevice
      ///
      template < class T_Model, class T_Device, bool T_LongHeader = true >
      class Adaptive : public PAMI::Protocol::Send::Send,
          public AdaptiveImmediate<T_Model, T_Device>,
          public AdaptiveSimple<T_Model, T_Device, T_LongHeader>
      {
        public:

          ///
          /// \brief Adaptive send protocol constructor.
          ///
          /// \param[in]  dispatch     Dispatch identifier
          /// \param[in]  dispatch_fn  Dispatch callback function
          /// \param[in]  cookie       Opaque application dispatch data
          /// \param[in]  device       Device that implements the message interface
          /// \param[in]  origin_task  Origin task identifier
          /// \param[in]  context      Communication context
          /// \param[out] status       Constructor status
          ///
          inline Adaptive (size_t                     dispatch,
                           pami_dispatch_callback_function dispatch_fn,
                           void                     * cookie,
                           T_Device                 & device,
                           size_t                     origin_task,
                           pami_context_t              context,
                           size_t                     contextid,
                           pami_result_t             & status) :
              PAMI::Protocol::Send::Send (),
              AdaptiveImmediate<T_Model, T_Device> (dispatch,
                                                    dispatch_fn,
                                                    cookie,
                                                    device,
                                                    origin_task,
                                                    context,
                                                    contextid,
                                                    status),
              AdaptiveSimple<T_Model, T_Device, T_LongHeader> (dispatch,
                                                               dispatch_fn,
                                                               cookie,
                                                               device,
                                                               origin_task,
                                                               context,
                                                               contextid,
                                                               status)
          {
          };

          virtual ~Adaptive () {};

          ///
          /// \brief Start a new immediate send operation.
          ///
          /// \see PAMI::Protocol::Send::immediate
          ///
          virtual pami_result_t immediate (pami_send_immediate_t * parameters)
          {
            TRACE_ERR((stderr, ">> Adaptive::immediate()\n"));
            pami_result_t result = this->immediate_impl (parameters);
            TRACE_ERR((stderr, "<< Adaptive::immediate()\n"));
            return result;
          };

          ///
          /// \brief Start a new simple send operation.
          ///
          /// \see PAMI::Protocol::Send::simple
          ///
          virtual pami_result_t simple (pami_send_t * parameters)
          {
            TRACE_ERR((stderr, ">> Adaptive::simple()\n"));
            pami_result_t result = this->simple_impl (parameters);
            TRACE_ERR((stderr, "<< Adaptive::simple()\n"));
            return result;
          };

          ///
          /// \brief Start a new typed send operation.
          ///
          /// \see PAMI::Protocol::Send::typed
          ///
          virtual pami_result_t typed (pami_send_typed_t * parameters)
          {
            return PAMI_UNIMPL;
          };

      };  // PAMI::Protocol::Send::Adaptive class
    };    // PAMI::Protocol::Send namespace
  };      // PAMI::Protocol namespace
};        // PAMI namespace
#undef TRACE_ERR
#endif // __pami_p2p_protocol_send_adaptive_adaptive_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
