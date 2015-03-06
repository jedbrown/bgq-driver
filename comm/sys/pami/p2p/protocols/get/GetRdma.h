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
/// \file p2p/protocols/get/GetRdma.h
/// \brief Get protocol factory for CDI devices that implement the 'dma' interface.
///
/// The GetProtocolFactory class defined in this file uses C++ templates
/// and the CDI "dma" interface which also uses C++ templates.
///
/// C++ templates require all source code to be #include'd from a header file
/// which can result in a single very large file. The \b get
/// \b protocol for CDI dma devices defined here is split into \b three files
/// for readability.
///
#ifndef __p2p_protocols_get_GetRdma_h__
#define __p2p_protocols_get_GetRdma_h__

#include <string.h>

#include "Memregion.h"

namespace PAMI
{
  namespace Protocol
  {
    namespace Get
    {
      template <class T_Model, class T_Device>
      class Get
      {

        protected:
          typedef uint8_t msg_t[T_Model::packet_model_state_bytes];

          typedef struct get_state
          {
            msg_t                       msg;
            pami_event_function          local_fn;
            void                      * cookie;    ///< Application callback cookie
            Get < T_Model, T_Device > * get;    ///< get protocol object
          } get_state_t;


        public:
          inline Get( T_Device                 & device,
                      pami_result_t             & status) :
              _get_model (device),
              _device (device),
              _context (device.getContext())
          {
            status = PAMI_SUCCESS;
          }

          ///
          /// \brief Generate a new CDI get message from the factory registration
          ///
          /// \see Get::generate
          ///
          inline pami_result_t getimpl ( pami_event_function   local_fn,
                                        void               * cookie,
                                        pami_endpoint_t       dest,
                                        size_t               bytes,
                                        Memregion          * src_memregion,
                                        Memregion          * dst_memregion,
                                        size_t               src_offset,
                                        size_t               dst_offset)
          {

            // Allocate memory to maintain the state of the send.
            get_state_t * state = allocateGetState ();

            state->cookie   = cookie;
            state->local_fn = local_fn;
            state->get      = this;

            pami_task_t task;
            size_t offset;
            PAMI_ENDPOINT_INFO(dest,task,offset);

            _get_model.postDmaGet (state->msg,
                                   get_complete,
                                   (void*)state,
                                   task,
                                   dst_memregion,
                                   dst_offset,
                                   src_memregion,
                                   src_offset,
                                   bytes);

            return PAMI_SUCCESS;

          };

        protected:
          MemoryAllocator < sizeof(get_state), 16 > _get_allocator;

          T_Model                    _get_model;
          T_Device                 & _device;
          pami_context_t              _context;

          inline get_state_t * allocateGetState ()
          {
            return (get_state_t *) _get_allocator.allocateObject();
          }

          inline void freeGetState (get_state_t * object)
          {
            _get_allocator.returnObject ((void *) object);
          }

          ///
          /// \brief Local get completion event callback.
          ///
          /// This callback will invoke the application local completion
          /// callback function and, if notification of remote receive
          /// completion is not required, free the send state memory.
          ///
          static void get_complete (pami_context_t context,
                                    void          * cookie,
                                    pami_result_t    result)
          {
            get_state_t * state = (get_state_t *) cookie;

            Get<T_Model, T_Device> *get  =
              (Get<T_Model, T_Device> *) state->get;

            if (state->local_fn != NULL)
              {
                state->local_fn (get->_context, state->cookie, PAMI_SUCCESS);
              }

            get->freeGetState(state);

            return;
          }

      };
    };
  };
};

#endif /* __pami_protocol_get_cdi_factory_h__ */

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
