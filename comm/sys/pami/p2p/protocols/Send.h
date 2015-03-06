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
/// \file p2p/protocols/Send.h
/// \brief Defines base class interface for sends.
///
#ifndef __p2p_protocols_Send_h__
#define __p2p_protocols_Send_h__

#include <pami.h>

#include "util/trace.h"

#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Protocol
  {
    namespace Send
    {
      typedef enum
      {
        DEFAULT                 = 0x00,

        LONG_HEADER_DISABLE     = 0x01,

        RECV_IMMEDIATE_FORCEON  = 0x01 << 2,
        RECV_IMMEDIATE_FORCEOFF = 0x01 << 3,

        QUEUE_IMMEDIATE_DISABLE = 0x01 << 4
      } configuration_t;

      ///
      /// \brief Base class for point-to-point send implementations.
      ///
      class Send
      {
        public:

          ///
          /// \brief Base class constructor for point-to-point sends.
          ///
          inline Send () {};

          ///
          /// \brief Base class destructor.
          ///
          /// \note Any class with virtual functions must define a virtual
          ///       destructor.
          ///
          virtual ~Send () {};

          /// \note This is required to make "C" programs link successfully with virtual destructors
          inline void operator delete(void * p)
          {
            (void)p;
            PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
          }

          ///
          /// \brief Set the pointer to the mem allocator used by PWQ
          ///
          /// SendPWQ uses this memory allocator. Since SendPWQ class
          /// can't be constructed, we construct the allocator in NI and
          /// pass it to SendPWQ class through the composite.
          ///
          virtual void setPWQAllocator(void * allocator) = 0;

          ///
          /// \brief Return a pointer to the mem allocator used by PWQ
          ///
          virtual void * getPWQAllocator() = 0;

          ///
          /// \brief Query the value of one or more attributes
          ///
          /// \param [in,out] configuration  The configuration attribute(s) of interest
          /// \param [in]     num_configs    The number of configuration elements
          ///
          /// \see PAMI_Dispatch_query()
          ///
          /// \retval PAMI_SUCCESS  The query has completed successfully.
          /// \retval PAMI_INVAL    The query has failed due to invalid parameters.
          ///
          virtual pami_result_t getAttributes (pami_configuration_t  configuration[],
                                               size_t                num_configs) = 0;

          ///
          /// \brief Start a new immediate send message.
          ///
          /// \param[in] parameters Immediate send parameter structure
          ///
          virtual pami_result_t immediate (pami_send_immediate_t * parameters) = 0;

          ///
          /// \brief Start a new simple send message.
          ///
          /// \param[in] parameters Simple send parameter structure
          ///
          virtual pami_result_t simple (pami_send_t * parameters) = 0;

          ///
          /// \brief Start a new typed send message.
          ///
          /// \param[in] parameters Typed send parameter structure
          ///
          virtual pami_result_t typed (pami_send_typed_t * parameters) = 0;

      }; // PAMI::Protocol::Send::Send class

      class Error : public Send
      {
        public:
          inline Error () {};
          virtual ~Error () {};
          virtual void setPWQAllocator(void * allocator)
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT( "return PAMI_ERROR (%d)", PAMI_ERROR);
            TRACE_FN_EXIT();
          };

          virtual void * getPWQAllocator()
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT( "return PAMI_ERROR (%d)", PAMI_ERROR);
            TRACE_FN_EXIT();
            return NULL;
          };

          virtual pami_result_t getAttributes (pami_configuration_t  configuration[],
                                               size_t                num_configs)
          {
            (void)configuration;(void)num_configs;
            TRACE_FN_ENTER();
            TRACE_FORMAT( "return PAMI_ERROR (%d)", PAMI_ERROR);
            TRACE_FN_EXIT();
            return PAMI_INVAL;
          };

          virtual pami_result_t immediate (pami_send_immediate_t * parameters)
          {
            (void)parameters;
            TRACE_FN_ENTER();
            TRACE_FORMAT( "return PAMI_ERROR (%d)", PAMI_ERROR);
            TRACE_FN_EXIT();
            return PAMI_ERROR;
          };

          virtual pami_result_t simple (pami_send_t * parameters)
          {
            (void)parameters;
            TRACE_FN_ENTER();
            TRACE_FORMAT( "return PAMI_ERROR (%d)", PAMI_ERROR);
            TRACE_FN_EXIT();
            return PAMI_ERROR;
          };

          virtual pami_result_t typed (pami_send_typed_t * parameters)
          {
            (void)parameters;
            TRACE_FN_ENTER();
            TRACE_FORMAT( "return PAMI_ERROR (%d)", PAMI_ERROR);
            TRACE_FN_EXIT();
            return PAMI_ERROR;
          };

      }; // PAMI::Protocol::Send::Error class
    };   // PAMI::Protocol::Send namespace
  };     // PAMI::Protocol namespace
};       // PAMI namespace
#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif // __pami_p2p_protocols_send_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
