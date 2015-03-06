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
 * \file algorithms/interfaces/NativeInterface.h
 * \brief ???
 */

#ifndef __algorithms_interfaces_NativeInterface_h__
#define __algorithms_interfaces_NativeInterface_h__

#include <pami.h>
#include "util/common.h"
#include "PipeWorkQueue.h"
#include "components/devices/MulticastModel.h"
#include "components/devices/MultisyncModel.h"
#include "components/devices/MulticombineModel.h"
#include "components/devices/ManytomanyModel.h"

#include "util/trace.h"

typedef struct
{
  PAMI::PipeWorkQueue * rcvpwq;
  size_t                totalRcvln;   /**< Total Msg ln  */
  pami_callback_t       cb_done;      /**< User Callback            */
  unsigned long         connectionId; /**< Unique connection_Id. The user passes in a
                                           connection ID and sendpwq appends a message ID
                                           to that for a unique 64 bit connection Id*/
  pami_recv_t         * recv;
} pami_pwq_recv_t;

typedef void (*pami_dispatch_pwq_function) (pami_context_t          context,
                                            void                  * cookie,
                                            const void            * header_addr,
                                            size_t                  header_size,
                                            const void            * pipe_addr,
                                            size_t                  data_size,
                                            pami_endpoint_t         origin,
                                            pami_pwq_recv_t       * recv);



#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#ifdef CCMI_TRACE_ALL
#define DO_TRACE_ENTEREXIT 1
#define DO_TRACE_DEBUG     1
#else
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0
#endif

namespace CCMI{

  namespace Interfaces{

    class NativeInterface
    {
      public:
        NativeInterface(size_t          context_id,
                  pami_endpoint_t endpoint):
          _endpoint(endpoint),
          _context_id(context_id),
          _status(PAMI_SUCCESS)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "<%p> context %zu, endpoint %u",this,_context_id, _endpoint);
          TRACE_FN_EXIT();
        }
        ///
        /// \brief Virtual destructors make compilers happy.
        ///
        virtual inline ~NativeInterface() {};
        inline pami_endpoint_t endpoint()    { return _endpoint;     }
        inline size_t          contextid()   { return _context_id;   }
        inline pami_result_t   status()      { return _status;       }

        /// \brief this call is called when an active message native interface is initialized and
        /// is not supported on all sided native interfaces
        virtual pami_result_t setMulticastDispatch(pami_dispatch_multicast_function fn,
                                             void *cookie)
        {
          (void)fn;(void)cookie;
          //PAMI_abort();
          return PAMI_ERROR;
        }
        virtual pami_result_t setManytomanyDispatch(pami_dispatch_manytomany_function fn,
                                              void *cookie)
        {
          (void)fn;(void)cookie;
          //PAMI_abort();
          return PAMI_ERROR;
        }
        virtual pami_result_t setSendDispatch(pami_dispatch_p2p_function fn,
                                        void *cookie)
        {
          (void)fn;(void)cookie;
          //PAMI_abort();
          return PAMI_ERROR;
        }
        virtual pami_result_t setSendPWQDispatch(pami_dispatch_pwq_function fn,
                                           void *cookie)
        {
          (void)fn;(void)cookie;
          //PAMI_abort();
          return PAMI_ERROR;
        }
        virtual pami_result_t destroy () = 0;
        virtual pami_result_t multicast(pami_multicast_t *mcast, void *devinfo = NULL) = 0;
        virtual pami_result_t multisync(pami_multisync_t *msync, void *devinfo = NULL) = 0;
        virtual pami_result_t multicombine(pami_multicombine_t *mcombine, void *devinfo = NULL) = 0;
        virtual pami_result_t manytomany(pami_manytomany_t *m2minfo, void *devinfo = NULL) = 0;
        virtual pami_result_t send (pami_send_t * parameters)
        {
          (void)parameters;
          PAMI_abort();
          return PAMI_ERROR;
        }
        virtual pami_result_t sendPWQ(pami_context_t       context,
                                pami_endpoint_t      dest,
                                unsigned             connection_Id,
                                size_t               header_length,
                                void                *header,
                                size_t               length,
                                PAMI::PipeWorkQueue *pwq,
                                pami_send_event_t   *events)
        {
          (void)context;(void)dest;(void)header_length;(void)header;
          (void)length;(void)pwq;(void)events;
          PAMI_abort();
          return PAMI_ERROR;
        }
        ///
        /// \brief NI hook to override metadata for collective
        ///
        virtual void metadata(pami_metadata_t *m, pami_xfer_type_t t) 
        {
          (void)m;(void)t;
          /* no override */
        };

        ///
        /// \brief post a work function to be executed on the
        /// communication thread. The color identifier can choose the
        /// commuication thread relative to the context parameter.
        ///
        virtual void postWork (pami_context_t         context,
                         int                    color,
                         pami_work_t          * work,
                         pami_work_function     fn,
                         void                 * clientdata)
        {
          (void)context;(void)color;(void)work;(void)fn;
          (void)clientdata;
          CCMI_abort();
        }

        virtual void *getSendPWQAllocator()
        {
          PAMI_abort();
          return NULL;
        }

      protected:
        pami_endpoint_t  _endpoint;
        size_t           _context_id;
        pami_result_t    _status;
    };

  };
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
