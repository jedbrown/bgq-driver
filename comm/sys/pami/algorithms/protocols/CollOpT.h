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
 * \file algorithms/protocols/CollOpT.h
 * \brief ???
 */

#ifndef __algorithms_protocols_CollOpT_h__
#define __algorithms_protocols_CollOpT_h__

#include "util/queue/MatchQueue.h"
#include "util/ccmi_util.h"
#include "algorithms/ccmi.h"
#include "algorithms/composite/Composite.h"

#define MAX_NUM_PREALLOCATED        64

namespace CCMI
{
  namespace Adaptor
  {
    class EADescriptor : public PAMI::Queue::Element
    {
    public:

      CollHeaderData cdata;          //copy of the header metadata
      char           *buf;           //pointer to early arrival buffer
      unsigned       bytes;          //size of message in early arrival buffer
      unsigned       flag;           //flags
    };

    enum {EANODATA = 0, EASTARTED, EACOMPLETED};
    enum {LocalPosted = 1, EarlyArrival};
    enum {TS_INPROGRESS = 1};

    template <typename T_xfer, typename T_composite>
      class CollOpT : public PAMI::MatchQueueElem<>
      {
      protected:

        T_composite                 _composite __attribute__((__aligned__(16)));
        T_xfer                      _xfer;   ///copy of calling parameters
        unsigned                    _ntokens;/// number of tokens required
        unsigned                    _flags;  ///TokenBlocked,EarlyArrival,LocalPosted, OpCompleted
                                             ///ActiveMessage, NonBlockingCC etc.
        void                        *_collfac;///pointer to collective registration/factory
        PAMI_GEOMETRY_CLASS         *_geometry ;///pointer to geometry
        PAMI::Queue                  _eaq;    ///early arrival queue, should be matchqueue to support multiple early arrivals

      public:

        ///
        /// \brief Default constructor
        ///
        ~CollOpT()
        {
        }
        CollOpT(unsigned key) : PAMI::MatchQueueElem<> (key),
        _composite(),
        _ntokens(0),
        _flags(0),
        _collfac(NULL),
        _geometry(NULL),
        _eaq()
        {
        }

        void *getFactory()
        {
          return _collfac;
        }

        void setFactory(void *factoryp)
        {
          _collfac = factoryp;
        }

        PAMI_GEOMETRY_CLASS *getGeometry ()
        {
          return _geometry;
        }

        void setGeometry (PAMI_GEOMETRY_CLASS *geometry)
        {
          _geometry = geometry;
        }

        T_composite *getComposite()
        {
          return &_composite;
        }

        T_xfer *getXfer()
        {
          return &_xfer;
        }

        void setXfer(T_xfer *xfer)
        {
          memcpy(&_xfer, xfer, sizeof(T_xfer));
        }

        void setFlag (unsigned flag)
        {
          _flags |= flag ;
        }

        void unsetFlag (unsigned flag)
        {
          _flags &= (~flag);
        }

        unsigned getFlags ()
        {
          return _flags;
        }

/*
        void enqueueEA(EADescriptor *ea)
        {
          _eaq.pushHead(ea);
        }

*/

        PAMI::Queue *getEAQ()
        {
          return &(_eaq);
        }

        static void common_cleanup()
        {

           PAMI_assert(0);

        }


      }; //- CollOpT


      template <typename T_xfer, typename T_composite>
       class CollOpPoolT : public PAMI::Queue
       {

        public:

          ///
          /// \brief Constructor
          ///
          CollOpPoolT () :
          PAMI::Queue()
          {
          }

          /// \brief free the element of the pool
          ~CollOpPoolT()
          {
             while(! isEmpty())
             {
               delete (popHead());
             }
          }

          ///
          /// \brief Keep a pool of 64 buffers
          ///  to optimize malloc/free overheads
          ///
          CollOpT<T_xfer, T_composite> * allocate (unsigned connid)
          {
            if(! isEmpty())
            {
              CollOpT<T_xfer, T_composite> *co = (CollOpT<T_xfer, T_composite> *) popHead();
              return ( new (co) CollOpT<T_xfer, T_composite>(connid) );
            } else {
              CollOpT<T_xfer, T_composite>* co = NULL;
              pami_result_t rc;
	      rc = __global.heap_mm->memalign((void **)&co, 0, sizeof(CollOpT<T_xfer, T_composite>));
              PAMI_assertf(rc == PAMI_SUCCESS, "Failed to allocate composite storage\n");
              return( new (co) CollOpT<T_xfer, T_composite>(connid) );
            }
          }

          ///
          /// \brief Free the buffer allocated
          /// Enqueue to pool if size is less than
          /// MAX_PREALLOCATED_BYTES
          ///
          void free (CollOpT<T_xfer, T_composite> *co)
          {
            co->~CollOpT<T_xfer, T_composite>();
            if (size() < MAX_NUM_PREALLOCATED)
              pushHead (co);
            else
            {  
              __global.heap_mm->free(co);
            }
             
          }

        };

    };  //- Adaptor
};  //- CCMI

#endif /* __algorithms_protocols_CollOpT_h__ */
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
