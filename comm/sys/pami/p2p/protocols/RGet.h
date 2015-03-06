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
/// \file p2p/protocols/RGet.h
/// \brief Defines base class interface for virtual address get operations.
///
#ifndef __p2p_protocols_RGet_h__
#define __p2p_protocols_RGet_h__

#include <pami.h>

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf x
#endif

namespace PAMI
{
  namespace Protocol
  {
    namespace Get
    {
      ///
      /// \brief Base class for point-to-point rget implementations.
      ///
      class RGet
      {
        public:

          ///
          /// \brief Base class constructor for point-to-point rget implementations.
          ///
          inline RGet () {};

          ///
          /// \brief Base class destructor.
          ///
          /// \note Any class with virtual functions must define a virtual
          ///       destructor.
          ///
          virtual ~RGet () {};

          ///
          /// \note This is required to make "C" programs link successfully
          ///       with virtual destructors
          ///
          inline void operator delete(void * p)
          {
            PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
          }

          ///
          /// \brief Start a new contiguous rget operation
          ///
          virtual pami_result_t simple (pami_rget_simple_t * parameters) = 0;

          ///
          /// \brief Start a new non-contiguous rget operation
          ///
          virtual pami_result_t typed (pami_rget_typed_t * parameters) = 0;

      }; // PAMI::Protocol::Get::RGet class


      class NoRGet : public RGet
      {
        public:
          template <class T_Allocator>
          static RGet * generate (T_Allocator & allocator)
          {
            TRACE_ERR((stderr, ">> NoRGet::generate()\n"));
            COMPILE_TIME_ASSERT(sizeof(NoRGet) <= T_Allocator::objsize);

            RGet * get = (RGet *) allocator.allocateObject ();
            new ((void *)get) NoRGet ();

            TRACE_ERR((stderr, "<< NoRGet::generate(), get = %p\n", get));
            return get;
          }

          inline NoRGet () :
            RGet ()
          {}

          inline ~NoRGet () {};

          pami_result_t simple (pami_rget_simple_t * parameters)
          {
            TRACE_ERR((stderr, "<> NoRGet::simple()\n"));
            return PAMI_ERROR;
          }

          pami_result_t typed (pami_rget_typed_t * parameters)
          {
            TRACE_ERR((stderr, "<> NoRGet::typed()\n"));
            return PAMI_ERROR;
          }
      }; // PAMI::Protocol::NoRGet class



      // Forward declaration
      template <class T_Primary, class T_Secondary> class CompositeRGet;

      namespace Factory
      {
        template <class T_Primary, class T_Secondary, class T_Allocator>
        static CompositeRGet<T_Primary, T_Secondary> * generate (T_Primary    * primary,
                                                             T_Secondary  * secondary,
                                                             T_Allocator  & allocator,
                                                             pami_result_t & result)
        {
          TRACE_ERR((stderr, ">> Get::Factory::generate() [CompositeRGet]\n"));
          COMPILE_TIME_ASSERT(sizeof(CompositeRGet<T_Primary, T_Secondary>) <= T_Allocator::objsize);

          void * composite = allocator.allocateObject ();
          new (composite) CompositeRGet<T_Primary, T_Secondary> (primary, secondary, result);

          if (result != PAMI_SUCCESS)
            {
              allocator.returnObject (composite);
              composite = NULL;
            }

          TRACE_ERR((stderr, "<< Get::Factory::generate() [CompositeRGet], composite = %p\n", composite));
          return (CompositeRGet<T_Primary, T_Secondary> *) composite;
        };
      };  // PAMI::Protocol::Get::Factory namespace


      ///
      /// \brief Composite rget protocol class
      ///
      /// The composite rget protocol internally invokes the rget operations on
      /// the primary protocol and, if the primary protocol rget operation does
      /// not succeed the secondary protocol is used for the rget operations.
      ///
      /// \tparam T_Primary    Template rget protocol class
      /// \tparam T_Secondary  Template rget protocol class
      ///
      template <class T_Primary, class T_Secondary>
      class CompositeRGet : public RGet
      {
        public:
#if 0
          template <class T_Allocator>
          static CompositeRGet * generate (T_Allocator   & allocator,
                                  T_Primary     * primary,
                                  T_Secondary   * secondary,
                                  pami_result_t & status)
          {
            TRACE_ERR((stderr, ">> CompositeRGet::generate()\n"));
            COMPILE_TIME_ASSERT(sizeof(CompositeRGet) <= T_Allocator::objsize);

            CompositeRGet * get = (CompositeRGet *) allocator.allocateObject ();
            new ((void *)get) CompositeRGet (primary, secondary, status);

            TRACE_ERR((stderr, "<< CompositeRGet::generate(), get = %p\n", get));
            return get;
          }
#endif
          ///
          /// \brief Composite rget protocol constructor.
          ///
          /// \param[in]  primary     Primary rget protocol
          /// \param[in]  secondary   Secondary rget protocol
          /// \param[out] status      Constructor status
          ///
          inline CompositeRGet (T_Primary    * primary,
                                T_Secondary  * secondary,
                                pami_result_t & status) :
              RGet (),
              _primary (primary),
              _secondary (secondary)
          {
            status = PAMI_SUCCESS;
          };

          virtual ~CompositeRGet () {};

          /// \note This is required to make "C" programs link successfully
          ///       with virtual destructors
          inline void operator delete(void * p)
          {
            PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
          }

          ///
          /// \brief Start a new contiguous rget operation
          ///
          virtual pami_result_t simple (pami_rget_simple_t * parameters)
          {
            TRACE_ERR((stderr, ">> CompositeRGet::rget('simple')\n"));
            pami_result_t result = _primary->simple (parameters);
            TRACE_ERR((stderr, "   CompositeRGet::rget('simple'), primary result = %d\n", result));

            if (result != PAMI_SUCCESS)
              {
                result = _secondary->simple (parameters);
                TRACE_ERR((stderr, "   CompositeRGet::rget('simple'), secondary result = %d\n", result));
              }

            TRACE_ERR((stderr, "<< CompositeRGet::rget('simple')\n"));
            return result;
          };

          ///
          /// \brief Start a new contiguous rget operation
          ///
          virtual pami_result_t typed (pami_rget_typed_t * parameters)
          {
            TRACE_ERR((stderr, ">> CompositeRGet::rget('typed')\n"));
            pami_result_t result = _primary->typed (parameters);
            TRACE_ERR((stderr, "   CompositeRGet::rget('typed'), primary result = %d\n", result));

            if (result != PAMI_SUCCESS)
              {
                result = _secondary->typed (parameters);
                TRACE_ERR((stderr, "   CompositeRGet::rget('typed'), secondary result = %d\n", result));
              }

            TRACE_ERR((stderr, "<< CompositeRGet::rget('typed')\n"));
            return result;
          };

        protected:

          T_Primary   * _primary;
          T_Secondary * _secondary;

      };  // PAMI::Protocol::Get::CompositeRGet class
    };    // PAMI::Protocol::Get namespace
  };      // PAMI::Protocol namespace
};        // PAMI namespace

#endif // __p2p_protocols_RGet_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
