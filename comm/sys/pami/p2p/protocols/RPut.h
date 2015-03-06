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
/// \file p2p/protocols/RPut.h
/// \brief Defines base class interface for memory region put operations.
///
#ifndef __p2p_protocols_RPut_h__
#define __p2p_protocols_RPut_h__

#include <pami.h>

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf x
#endif

namespace PAMI
{
  namespace Protocol
  {
    namespace Put
    {
      ///
      /// \brief Base class for point-to-point rput implementations.
      ///
      class RPut
      {
        public:

          ///
          /// \brief Base class constructor for point-to-point rput implementations.
          ///
          inline RPut () {};

          ///
          /// \brief Base class destructor.
          ///
          /// \note Any class with virtual functions must define a virtual
          ///       destructor.
          ///
          virtual ~RPut () {};

          ///
          /// \note This is required to make "C" programs link successfully
          ///       with virtual destructors
          ///
          inline void operator delete(void * p)
          {
            PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
          }

          ///
          /// \brief Start a new contiguous rput operation
          ///
          virtual pami_result_t simple (pami_rput_simple_t * parameters) = 0;

          ///
          /// \brief Start a new non-contiguous rput operation
          ///
          virtual pami_result_t typed (pami_rput_typed_t * parameters) = 0;

      }; // PAMI::Protocol::Put::RPut class


      class NoRPut : public RPut
      {
        public:
          template <class T_Allocator>
          static RPut * generate (T_Allocator & allocator)
          {
            TRACE_ERR((stderr, ">> NoRPut::generate()\n"));
            COMPILE_TIME_ASSERT(sizeof(NoRPut) <= T_Allocator::objsize);

            RPut * put = (RPut *) allocator.allocateObject ();
            new ((void *)put) NoRPut ();

            TRACE_ERR((stderr, "<< NoRPut::generate(), put = %p\n", put));
            return put;
          }

          inline NoRPut () :
            RPut ()
          {}

          inline ~NoRPut () {};

          virtual pami_result_t simple (pami_rput_simple_t * parameters)
          {
            return PAMI_ERROR;
          }

          virtual pami_result_t typed (pami_rput_typed_t * parameters)
          {
            return PAMI_ERROR;
          }
      }; // PAMI::Protocol::NoRPut class



      // Forward declaration
      template <class T_Primary, class T_Secondary> class CompositeRPut;

      namespace Factory
      {
        template <class T_Primary, class T_Secondary, class T_Allocator>
        static CompositeRPut<T_Primary, T_Secondary> * generate (T_Primary    * primary,
                                                             T_Secondary  * secondary,
                                                             T_Allocator  & allocator,
                                                             pami_result_t & result)
        {
          TRACE_ERR((stderr, ">> Put::Factory::generate() [CompositeRPut]\n"));
          COMPILE_TIME_ASSERT(sizeof(CompositeRPut<T_Primary, T_Secondary>) <= T_Allocator::objsize);

          void * composite = allocator.allocateObject ();
          new (composite) CompositeRPut<T_Primary, T_Secondary> (primary, secondary, result);

          if (result != PAMI_SUCCESS)
            {
              allocator.returnObject (composite);
              composite = NULL;
            }

          TRACE_ERR((stderr, "<< Put::Factory::generate() [CompositeRPut], composite = %p\n", composite));
          return (CompositeRPut<T_Primary, T_Secondary> *) composite;
        };
      };  // PAMI::Protocol::Put::Factory namespace


      ///
      /// \brief Composite rput protocol class
      ///
      /// The composite rput protocol internally invokes the rput operations on
      /// the primary protocol and, if the primary protocol rput operation does
      /// not succeed the secondary protocol is used for the rput operations.
      ///
      /// \tparam T_Primary    Template rput protocol class
      /// \tparam T_Secondary  Template rput protocol class
      ///
      template <class T_Primary, class T_Secondary>
      class CompositeRPut : public RPut
      {
        public:

          ///
          /// \brief Composite rput protocol constructor.
          ///
          /// \param[in]  primary     Primary rput protocol
          /// \param[in]  secondary   Secondary rput protocol
          /// \param[out] status      Constructor status
          ///
          inline CompositeRPut (T_Primary    * primary,
                                T_Secondary  * secondary,
                                pami_result_t & status) :
              RPut (),
              _primary (primary),
              _secondary (secondary)
          {
            status = PAMI_SUCCESS;
          };

          virtual ~CompositeRPut () {};

          /// \note This is required to make "C" programs link successfully
          ///       with virtual destructors
          inline void operator delete(void * p)
          {
            PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
          }

          ///
          /// \brief Start a new contiguous rput operation
          ///
          virtual pami_result_t simple (pami_rput_simple_t * parameters)
          {
            TRACE_ERR((stderr, ">> CompositeRPut::rput('simple')\n"));
            pami_result_t result = _primary->simple (parameters);
            TRACE_ERR((stderr, "   CompositeRPut::rput('simple'), primary result = %d\n", result));

            if (result != PAMI_SUCCESS)
              {
                result = _secondary->simple (parameters);
                TRACE_ERR((stderr, "   CompositeRPut::rput('simple'), secondary result = %d\n", result));
              }

            TRACE_ERR((stderr, "<< CompositeRPut::rput('simple')\n"));
            return result;
          };

          ///
          /// \brief Start a new non-contiguous rput operation
          ///
          virtual pami_result_t typed (pami_rput_typed_t * parameters)
          {
            TRACE_ERR((stderr, ">> CompositeRPut::rput('typed')\n"));
            pami_result_t result = _primary->typed (parameters);
            TRACE_ERR((stderr, "   CompositeRPut::rput('typed'), primary result = %d\n", result));

            if (result != PAMI_SUCCESS)
              {
                result = _secondary->typed (parameters);
                TRACE_ERR((stderr, "   CompositeRPut::rput('typed'), secondary result = %d\n", result));
              }

            TRACE_ERR((stderr, "<< CompositeRPut::rput('typed')\n"));
            return result;
          }

        protected:

          T_Primary   * _primary;
          T_Secondary * _secondary;

      };  // PAMI::Protocol::Put::CompositeRPut class
    };    // PAMI::Protocol::Put namespace
  };      // PAMI::Protocol namespace
};        // PAMI namespace

#endif // __p2p_protocols_RPut_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
