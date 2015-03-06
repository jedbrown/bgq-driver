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
 * \file components/devices/bgq/mu2/msg/work/DescCompletionWork.h
 * \brief ???
 */

#ifndef __components_devices_bgq_mu2_msg_work_DescCompletionWork_h__
#define __components_devices_bgq_mu2_msg_work_DescCompletionWork_h__

#include <hwi/include/bqc/MU_PacketHeader.h>
#include <spi/include/mu/DescriptorBaseXX.h>
#include <spi/include/mu/InjFifo.h>

#include "components/devices/bgq/mu2/msg/work/MuWork.h"

#undef TRACE_ERR
#define TRACE_ERR(x) // fprintf x

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      class DescCompletionWork : public MuWork<DescCompletionWork>
      {
        public:

          ///
          /// \brief Injection fifo work constructor with callback completion
          ///
          /// \param[in] injfifo     Injection fifo to check for descriptor completion
          /// \param[in] sequence    Descriptor sequence identifier to check
          /// \param[in] done_fn     Event function to invoke upon completion
          /// \param[in] done_cookie Opaque cookie for completion function
          /// \param[in] contextid   Context identifier for completion function
          ///
          inline DescCompletionWork (MUSPI_InjFifo_t     * injfifo,
                                     uint64_t              sequence,
                                     pami_event_function   done_fn,
                                     void                * done_cookie,
                                     size_t                contextid) :
              MuWork<DescCompletionWork> (done_fn, done_cookie, contextid),
              _injfifo (injfifo),
              _sequence (sequence)
          {
          };

          ///
          /// \brief Injection fifo work constructor with message completion
          ///
          /// \param[in] injfifo  Injection fifo to check for descriptor completion
          /// \param[in] sequence Descriptor sequence identifier to check
          /// \param[in] msg      Message class to be updated upon completion
          ///
          inline DescCompletionWork (MUSPI_InjFifo_t         * injfifo,
                                     uint64_t                  sequence,
                                     Generic::GenericMessage * msg) :
              MuWork<DescCompletionWork> (msg),
              _injfifo (injfifo),
              _sequence (sequence)
          {
          };

        protected:

          ///
          /// \brief Injection fifo work constructor with callback completion
          ///
          /// \note The descriptor sequence identifier is uninitialized.
          ///
          /// \param[in] injfifo     Injection fifo to check for descriptor completion
          /// \param[in] done_fn     Event function to invoke upon completion
          /// \param[in] done_cookie Opaque cookie for completion function
          /// \param[in] contextid   Context identifier for completion function
          ///
          template <class T>
          inline DescCompletionWork (MUSPI_InjFifo_t     * injfifo,
                                     pami_event_function   done_fn,
                                     void                * done_cookie,
                                     size_t                contextid) :
              MuWork<DescCompletionWork> (done_fn, done_cookie, contextid),
              _injfifo (injfifo)
          {
          };

          ///
          /// \brief Injection fifo work constructor with message completion
          ///
          /// \note The descriptor sequence identifier is uninitialized.
          ///
          /// \param[in] injfifo  Injection fifo to check for descriptor completion
          /// \param[in] msg      Message class to be updated upon completion
          ///
          inline DescCompletionWork (MUSPI_InjFifo_t         * injfifo,
                                     uint64_t                  sequence,
                                     Generic::GenericMessage * msg) :
              MuWork<DescCompletionWork> (msg),
              _injfifo (injfifo)
          {
          };

          ///
          /// \brief Check the injection fifo for completion of a descriptor
          ///
          /// \param[in] context Communication context advancing this work object
          ///
          /// \see MuWork::__advance_cb
          /// \see MuWork::__advance_msg
          ///
          inline pami_result_t advance (pami_context_t context)
          {
            if (MUSPI_CheckDescComplete (_injfifo, _sequence))
            {
              return PAMI_SUCCESS;
            }

            return PAMI_EAGAIN;
          };

          MUSPI_InjFifo_t * _injfifo;
          uint64_t          _sequence;

      };  // PAMI::Device::MU::DescCompletionWork class
    };    // PAMI::Device::MU namespace
  };      // PAMI::Device namespace
};        // PAMI namespace
#undef TRACE_ERR
#endif // __components_devices_bgq_mu2_msg_work_DescCompletionWork_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
