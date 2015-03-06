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
 * \file algorithms/geometry/UnexpBarrierQueueElement.h
 * \brief ???
 */

#ifndef __algorithms_geometry_UnexpBarrierQueueElement_h__
#define __algorithms_geometry_UnexpBarrierQueueElement_h__

#include "util/queue/MatchQueue.h"

namespace PAMI
{
  namespace Geometry
  {
    class UnexpBarrierQueueElement : public PAMI::MatchQueueElem<>
    {
    public:
      UnexpBarrierQueueElement (unsigned     comm,
                                size_t       context_id,
                                pami_quad_t &info,
                                unsigned     src,
                                unsigned     algorithm):
        PAMI::MatchQueueElem<>(comm),
        _comm (comm),
        _context_id(context_id),
        _info(info),
        _srcrank(src),
        _algorithm(algorithm)
        {
        }
      unsigned       getComm()      { return _comm;      }
      unsigned       getContextId() { return _context_id;}
      pami_quad_t  & getInfo()      { return _info;      }
      unsigned       getSrcRank()   { return _srcrank;   }
      unsigned       getAlgorithm() { return _algorithm; }
    private:
      unsigned          _comm;
      size_t            _context_id;
      pami_quad_t       _info;
      unsigned          _srcrank;
      unsigned          _algorithm;
    };
  };
};

#endif
