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
/// \file p2p/protocols/send/composite/Composite.h
/// \brief Composite send protocol composed of multiple individual send protocols
///
/// The Composite class defined in this file uses C++ templates and
/// the device "message" interface - which also uses C++ templates.
///
/// C++ templates require all source code to be #include'd from a header file.
///
#ifndef __p2p_protocols_send_composite_Composite_h__
#define __p2p_protocols_send_composite_Composite_h__

#include "p2p/protocols/Send.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) // fprintf x
#endif

namespace PAMI
{
  namespace Protocol
  {
    namespace Send
    {
      // Forward declaration
      template <class T_Primary, class T_Secondary> class Composite;

      namespace Factory
      {
        template <class T_Primary, class T_Secondary, class T_MemoryManager>
        static Composite<T_Primary, T_Secondary> * generate (T_Primary       * primary,
                                                             T_Secondary     * secondary,
                                                             T_MemoryManager * mm,
                                                             pami_result_t   & result)
        {
          TRACE_ERR((stderr, ">> Send::Factory::generate() [Composite]\n"));

          void * composite = NULL;
          result = mm->memalign((void **) & composite, 16, sizeof(Composite<T_Primary, T_Secondary>));
          PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for composite send protocol");
          new (composite) Composite<T_Primary, T_Secondary> (primary, secondary, result);

          if (result != PAMI_SUCCESS)
            {
              mm->free (composite);
              composite = NULL;
            }

          TRACE_ERR((stderr, "<< Send::Factory::generate() [Composite], composite = %p\n", composite));
          return (Composite<T_Primary, T_Secondary> *) composite;
        };
      };  // PAMI::Protocol::Send::Factory namespace

      ///
      /// \brief Composite send protocol class
      ///
      /// The Composite send protocol internally invokes the send operations on
      /// the primary protocol and, if the primary protocol send operation does
      /// not succeed the secondary protocol is used for the send operations.
      ///
      /// \tparam T_Primary    Template send protocol class
      /// \tparam T_Secondary  Template send protocol class
      ///
      /// \see PAMI::Protocol::Send::Send
      ///
      template <class T_Primary, class T_Secondary>
      class Composite : public PAMI::Protocol::Send::Send
      {
        public:
          ///
          /// \brief Composite send protocol constructor.
          ///
          /// \param[in]  primary     Primary send protocol
          /// \param[in]  secondary   Secondary send protocol
          /// \param[out] status      Constructor status
          ///
          inline Composite (T_Primary    * primary,
                            T_Secondary  * secondary,
                            pami_result_t & status) :
              PAMI::Protocol::Send::Send (),
              _primary (primary),
              _secondary (secondary),
              _pwqAllocator (NULL)
          {
            status = PAMI_SUCCESS;
          };

          virtual ~Composite () {};

          /// \note This is required to make "C" programs link successfully
          ///       with virtual destructors
          inline void operator delete(void * p)
          {
            PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
          }

          ///
          /// \brief Set the pointer to the mem allocator used by PWQ
          ///
          /// SendPWQ uses this memory allocator. Since SendPWQ class
          /// can't be constructed, we construct the allocator in NI and
          /// pass it to SendPWQ class through the composite.
          ///
          /// \see PAMI::Protocol::Send::setPWQAllocator
          ///
          virtual void setPWQAllocator(void * allocator)
          {
            _pwqAllocator = allocator;
          }

          ///
          /// \brief Return a pointer to the mem allocator used by PWQ
          ///
          /// \see PAMI::Protocol::Send::getPWQAllocator
          ///
          virtual void * getPWQAllocator()
          {
            return _pwqAllocator;
          }

          ///
          /// \brief Query the value of one or more attributes
          ///
          /// \see PAMI::Protocol::Send::getAttributes
          ///
          virtual pami_result_t getAttributes (pami_configuration_t  configuration[],
                                               size_t                num_configs)
          {
            pami_result_t result;
            size_t tmp, i;

            for (i = 0; i < num_configs; i++)
              {
                switch (configuration[i].name)
                  {
                    case PAMI_DISPATCH_RECV_IMMEDIATE_MAX:
                    case PAMI_DISPATCH_SEND_IMMEDIATE_MAX:
                      result = _primary->getAttributes (&configuration[i], 1);

                      if (result != PAMI_SUCCESS) return result;

                      tmp = configuration[i].value.intval;

                      result = _secondary->getAttributes (&configuration[i], 1);

                      if (result != PAMI_SUCCESS) return result;

                      if (tmp < configuration[i].value.intval)
                        configuration[i].value.intval = tmp;

                      break;
                    default:
                      return PAMI_INVAL;
                      break;
                  };
              };

            return PAMI_SUCCESS;
          };

          ///
          /// \brief Start a new immediate send operation.
          ///
          /// \see PAMI::Protocol::Send::immediate
          ///
          virtual pami_result_t immediate (pami_send_immediate_t * parameters)
          {
            TRACE_ERR((stderr, ">> Composite::immediate()\n"));
            pami_result_t result = _primary->immediate (parameters);
            TRACE_ERR((stderr, "   Composite::immediate(), destination endpoint = 0x%08x, primary result = %d\n", parameters->dest, result));

            if (result != PAMI_SUCCESS)
              {
                result = _secondary->immediate (parameters);
                TRACE_ERR((stderr, "   Composite::immediate(), destination endpoint = 0x%08x, secondary result = %d\n", parameters->dest, result));
              }

            TRACE_ERR((stderr, "<< Composite::immediate()\n"));
            return result;
          };

          ///
          /// \brief Start a new simple send operation.
          ///
          /// \see PAMI::Protocol::Send::simple
          ///
          virtual pami_result_t simple (pami_send_t * parameters)
          {
            TRACE_ERR((stderr, ">> Composite::simple()\n"));
            pami_result_t result = _primary->simple (parameters);
            TRACE_ERR((stderr, "   Composite::simple(), destination endpoint = 0x%08x, primary result = %d\n", parameters->send.dest, result));

            if (result != PAMI_SUCCESS)
              {
                result = _secondary->simple (parameters);
                TRACE_ERR((stderr, "   Composite::simple(), destination endpoint = 0x%08x, secondary result = %d\n", parameters->send.dest, result));
              }

            TRACE_ERR((stderr, "<< Composite::simple()\n"));
            return result;
          };

          ///
          /// \brief Start a new typed send operation.
          ///
          /// \see PAMI::Protocol::Send::typed
          ///
          virtual pami_result_t typed (pami_send_typed_t * parameters)
          {
            TRACE_ERR((stderr, ">> Composite::typed()\n"));
            pami_result_t result = _primary->typed (parameters);
            TRACE_ERR((stderr, "   Composite::typed(), destination endpoint = 0x%08x, primary result = %d\n", parameters->send.dest, result));

            if (result != PAMI_SUCCESS)
              {
                result = _secondary->typed (parameters);
                TRACE_ERR((stderr, "   Composite::typed(), destination endpoint = 0x%08x, secondary result = %d\n", parameters->send.dest, result));
              }

            TRACE_ERR((stderr, "<< Composite::typed()\n"));
            return result;
          };

        protected:

          T_Primary   * _primary;
          T_Secondary * _secondary;
          void        * _pwqAllocator;

      };  // PAMI::Protocol::Send::Composite class
    };    // PAMI::Protocol::Send namespace
  };      // PAMI::Protocol namespace
};        // PAMI namespace
#undef TRACE_ERR
#endif // __p2p_protocols_send_composite_Composite_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
