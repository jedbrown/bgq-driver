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
/// \file p2p/protocols/Put.h
/// \brief Defines base class interface for virtual address put operations.
///
#ifndef __p2p_protocols_Put_h__
#define __p2p_protocols_Put_h__

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
      /// \brief Base class for point-to-point put implementations.
      ///
      class Put
      {
        public:

          ///
          /// \brief Base class constructor for point-to-point put implementations.
          ///
          inline Put () {};

          ///
          /// \brief Base class destructor.
          ///
          /// \note Any class with virtual functions must define a virtual
          ///       destructor.
          ///
          virtual ~Put () {};

          ///
          /// \note This is required to make "C" programs link successfully
          ///       with virtual destructors
          ///
          inline void operator delete(void * p)
          {
            PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
          }

          ///
          /// \brief Start a new contiguous put operation
          ///
          virtual pami_result_t simple (pami_put_simple_t * parameters) = 0;

          ///
          /// \brief Start a new non-contiguous put operation
          ///
          virtual pami_result_t typed (pami_put_typed_t * parameters) = 0;

      }; // PAMI::Protocol::Put::Put class


      class NoPut : public Put
      {
        public:
          template <class T_Allocator>
          static Put * generate (pami_context_t context, T_Allocator & allocator)
          {
            TRACE_ERR((stderr, ">> NoPut::generate()\n"));
            COMPILE_TIME_ASSERT(sizeof(NoPut) <= T_Allocator::objsize);

            Put * put = (Put *) allocator.allocateObject ();
            new ((void *)put) NoPut (context);

            TRACE_ERR((stderr, "<< NoPut::generate(), put = %p\n", put));
            return put;
          }

          inline NoPut (pami_context_t context) :
            Put (),
            _context (context)
          {}

          inline ~NoPut () {};

          virtual pami_result_t simple (pami_put_simple_t * parameters)
          {
            //if (parameters->rma.done_fn)
              //parameters->rma.done_fn (_context, parameters->rma.cookie, PAMI_ERROR);

            return PAMI_ERROR;
          }

          virtual pami_result_t typed (pami_put_typed_t * parameters)
          {
            //if (parameters->rma.done_fn)
              //parameters->rma.done_fn (_context, parameters->rma.cookie, PAMI_ERROR);

            return PAMI_ERROR;
          }

        protected:

          pami_context_t _context;

      }; // PAMI::Protocol::NoPut class
    };   // PAMI::Protocol::Put namespace
  };     // PAMI::Protocol namespace
};       // PAMI namespace

#endif // __p2p_protocols_Put_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
