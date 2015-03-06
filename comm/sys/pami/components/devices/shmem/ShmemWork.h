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
 * \file components/devices/shmem/ShmemWork.h
 * \brief ???
 */

#ifndef __components_devices_shmem_ShmemWork_h__
#define __components_devices_shmem_ShmemWork_h__

#include <sys/uio.h>

#include <pami.h>

#include "util/common.h"
#include "components/devices/generic/AdvanceThread.h"
#include "components/devices/generic/Message.h"

#undef TRACE_ERR
#ifndef TRACE_ERR
#define TRACE_ERR(x) // fprintf x
#endif

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
      class Work : public PAMI::Device::Generic::GenericThread
      {
        public:

          inline Work (pami_work_function   work_func,
                       void               * work_cookie) :
            PAMI::Device::Generic::GenericThread (work_func, work_cookie)
          {
          };
      };  // PAMI::Device::Shmem::Work class

      template <class T_Fifo>
      class RecPacketWork : public Work
      {
        protected:

          static pami_result_t __advance_with_callback (pami_context_t context, void * cookie)
          {
            RecPacketWork * work = (RecPacketWork *) cookie;
            return work->advance_with_callback (context);
          };

          inline pami_result_t advance_with_callback (pami_context_t context)
          {
            TRACE_ERR((stderr, ">> RecPacketWork::advance_with_callback(%p), _sequence = %zd, _fifo.lastPacketConsumed() = %zu\n", context, _sequence, _fifo.lastPacketConsumed()));
            if (_sequence <= _fifo.lastPacketConsumed())
            {
              // Invoke the work completion callback
              _done_fn (context, _done_cookie, PAMI_SUCCESS);

              // return 'success' which will remove the work object from the work queue.
              TRACE_ERR((stderr, "<< RecPacketWork::advance_with_callback(%p), return PAMI_SUCCESS\n", context));
              return PAMI_SUCCESS;
            }

            TRACE_ERR((stderr, "<< RecPacketWork::advance_with_callback(%p), return PAMI_EAGAIN\n", context));
            return PAMI_EAGAIN;
          };

          static pami_result_t __advance_with_status (pami_context_t context, void * cookie)
          {
            RecPacketWork * work = (RecPacketWork *) cookie;
            return work->advance_with_status (context);
          };

          inline pami_result_t advance_with_status (pami_context_t context)
          {
            TRACE_ERR((stderr, ">> RecPacketWork::advance_with_status(%p), _sequence = %zd, _fifo.lastPacketConsumed() = %zu\n", context, _sequence, _fifo.lastPacketConsumed()));
            if (_sequence <= _fifo.lastPacketConsumed())
            {
              // Set the associated message status to "done". This will remove
              // the message from the head of the message queue and invoke any
              // callback
              _msg->setStatus (PAMI::Device::Done);

              // Return 'success' which will remove the work object from the work queue.
              TRACE_ERR((stderr, "<< RecPacketWork::advance_with_status(%p), return PAMI_SUCCESS\n", context));
              return PAMI_SUCCESS;
            }

            TRACE_ERR((stderr, "<< RecPacketWork::advance_with_status(%p), return PAMI_EAGAIN\n", context));
            return PAMI_EAGAIN;
          };


        public:

          inline RecPacketWork (T_Fifo              & fifo,
                                pami_event_function   fn,
                                void                * cookie,
                                ssize_t               sequence) :
            Work (__advance_with_callback, this),
            _fifo (fifo),
            _sequence (sequence),
            _done_fn (fn),
            _done_cookie (cookie)
          {
          };

        protected:

          T_Fifo                  & _fifo;
          ssize_t                   _sequence;
          pami_event_function       _done_fn;
          void                    * _done_cookie;
          Generic::GenericMessage * _msg;

      };  // PAMI::Device::Shmem::RecPacketWork class
    };    // PAMI::Device::Shmem namespace
  };      // PAMI::Device namespace
};        // PAMI namespace
#undef TRACE_ERR
#endif // __components_devices_shmem_ShmemWork_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
