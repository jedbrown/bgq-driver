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
/// \file p2p/protocols/get/Composite.h
/// \brief Composite get protocol composed of multiple individual get protocols
///
/// The Composite class defined in this file uses C++ templates and
/// the device "message" interface - which also uses C++ templates.
///
/// C++ templates require all source code to be #include'd from a header file.
///
#ifndef __p2p_protocols_get_Composite_h__
#define __p2p_protocols_get_Composite_h__

#include "p2p/protocols/Get.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) // fprintf x
#endif

namespace PAMI
{
  namespace Protocol
  {
    namespace Get
    {
      // Forward declaration
      template <class T_Primary, class T_Secondary> class Composite;

      namespace Factory
      {
        template <class T_Primary, class T_Secondary, class T_Allocator>
        static Composite<T_Primary, T_Secondary> * generate (T_Primary     * primary,
                                                             T_Secondary   * secondary,
                                                             T_Allocator   & allocator,
                                                             pami_result_t & result)
        {
          TRACE_ERR((stderr, ">> Get::Factory::generate() [Composite]\n"));
          COMPILE_TIME_ASSERT(sizeof(Composite<T_Primary, T_Secondary>) <= T_Allocator::objsize);

          void * composite = allocator.allocateObject ();
          new (composite) Composite<T_Primary, T_Secondary> (primary, secondary, result);

          if (result != PAMI_SUCCESS)
            {
              allocator.returnObject (composite);
              composite = NULL;
            }

          TRACE_ERR((stderr, "<< Get::Factory::generate() [Composite], composite = %p\n", composite));
          return (Composite<T_Primary, T_Secondary> *) composite;
        };
      };  // PAMI::Protocol::Get::Factory namespace

      ///
      /// \brief Composite get protocol class
      ///
      /// The Composite get protocol internally invokes the get operations on
      /// the primary protocol and, if the primary protocol get operation does
      /// not succeed the secondary protocol is used for the get operations.
      ///
      /// \tparam T_Primary    Template get protocol class
      /// \tparam T_Secondary  Template get protocol class
      ///
      /// \see PAMI::Protocol::Send::Send
      ///
      template <class T_Primary, class T_Secondary>
      class Composite : public PAMI::Protocol::Get::Get
      {
        public:
          ///
          /// \brief Composite get protocol constructor.
          ///
          /// \param[in]  primary     Primary get protocol
          /// \param[in]  secondary   Secondary get protocol
          /// \param[out] status      Constructor status
          ///
          inline Composite (T_Primary    * primary,
                            T_Secondary  * secondary,
                            pami_result_t & status) :
              PAMI::Protocol::Get::Get (),
              _primary (primary),
              _secondary (secondary)
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
          /// \brief Start a new contiguous get operation
          ///
          virtual pami_result_t get (pami_get_simple_t * parameters)
          {
            TRACE_ERR((stderr, ">> Composite::get('simple')\n"));
            pami_result_t result = _primary->get (parameters);
            TRACE_ERR((stderr, "   Composite::get('simple'), primary result = %d\n", result));

            if (result != PAMI_SUCCESS)
              {
                result = _secondary->get (parameters);
                TRACE_ERR((stderr, "   Composite::get('simple'), secondary result = %d\n", result));
              }

            TRACE_ERR((stderr, "<< Composite::get('simple')\n"));
            return result;
          };

        protected:

          T_Primary   * _primary;
          T_Secondary * _secondary;

      };  // PAMI::Protocol::Get::Composite class
    };    // PAMI::Protocol::Get namespace
  };      // PAMI::Protocol namespace
};        // PAMI namespace
#undef TRACE_ERR
#endif // __p2p_protocols_get_CompositeGet_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
