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
 * \file components/devices/bgq/mu2/msg/work/InjDescWork.h
 * \brief ???
 */

#ifndef __components_devices_bgq_mu2_msg_work_InjDescWork_h__
#define __components_devices_bgq_mu2_msg_work_InjDescWork_h__

#include "components/devices/bgq/mu2/msg/work/MuWork.h"

#undef TRACE_ERR
#define TRACE_ERR(x) // fprintf x

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      class InjDescWork : public MuWork<InjDescWork>
      {
        public:

          ///
          /// \brief Injection fifo work constructor with callback completion
          ///
          /// \param[in] injfifo     Injection fifo to copy the descriptor into
          /// \param[in] done_fn     Event function to invoke upon completion
          /// \param[in] done_cookie Opaque cookie for completion function
          /// \param[in] contextid   Context identifier for completion function
          ///
          inline InjDescWork (MUSPI_InjFifo_t     * injfifo,
                              pami_event_function   done_fn,
                              void                * done_cookie,
                              size_t                contextid) :
              MuWork<InjDescWork> (done_fn, done_cookie, contextid),
              _injfifo (injfifo)
          {
          };

          ///
          /// \brief Injection fifo work constructor with message completion
          ///
          /// \param[in] injfifo Injection fifo to copy the descriptor into
          /// \param[in] msg     Message class to be updated upon completion
          ///
          inline InjDescWork (MUSPI_InjFifo_t         * injfifo,
                              Generic::GenericMessage * msg) :
              MuWork<InjDescWork> (msg),
              _injfifo (injfifo)
          {
          };

          ///
          /// \brief Retrieve the descriptor associated with this work
          ///
          /// \return Pointer to internal work descriptor
          ///
          inline MUSPI_DescriptorBase * getDescriptor () { return &_desc; }

          ///
          /// \brief Retrieve the sequence identifier for the descriptor associated with this work
          ///
          /// \see MUSPI_InjFifoInject
          /// \return Sequence identifier
          ///
          inline uint64_t getSequence () { return _sequence; }

        protected:

          ///
          /// \brief Inject the work descriptor into the injection fifo
          ///
          /// \param[in] context Communication context advancing this work object
          ///
          /// \see MuWork::__advance_cb
          /// \see MuWork::__advance_msg
          ///
          inline pami_result_t advance (pami_context_t context)
          {
            _sequence = MUSPI_InjFifoInject (_injfifo, &_desc);
            if (_sequence != (uint64_t)-1)
            {
              return PAMI_SUCCESS;
            }

            return PAMI_EAGAIN;
          };

          MUSPI_DescriptorBase   _desc;
          MUSPI_InjFifo_t      * _injfifo;
          uint64_t               _sequence;

      };  // PAMI::Device::MU::InjDescWork class
    };    // PAMI::Device::MU namespace
  };      // PAMI::Device namespace
};        // PAMI namespace
#undef TRACE_ERR
#endif // __components_devices_bgq_mu2_msg_work_InjDescWork_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
